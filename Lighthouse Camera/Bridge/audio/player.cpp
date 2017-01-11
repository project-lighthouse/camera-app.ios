//
//  player.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 06/01/2017.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#include <cmath>
#include "player.hpp"

namespace lighthouse {

void Player::Play(const std::string aFilePath, const float aVolume) {
  // Let's obtain CFURL object for the audio file we want to play.
  CFURLRef audioFileURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
      (const UInt8 *) aFilePath.c_str(), aFilePath.length(), false /* file, not directory */);

  AudioQueuePlayerState state;

  // FIXME: Analyze result code and throw if needed.
  // Let's open the audio file.
  OSStatus result = AudioFileOpenURL(audioFileURL, AudioFilePermissions::kAudioFileReadPermission,
      0 /* don't use file type hint */, &state.mAudioFile);
  CFRelease(audioFileURL);

  UInt32 dataFormatSize = sizeof(state.mDataFormat);
  AudioFileGetProperty(state.mAudioFile, kAudioFilePropertyDataFormat, &dataFormatSize, &state.mDataFormat);

  // Now it's time to create an output audio queue.
  AudioQueueNewOutput(&state.mDataFormat, HandleOutputBuffer, &state, CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0,
      &state.mQueue);

  // Let's set a size in bytes for each audio queue buffer, and determine the number of packets to read for each
  // invocation of the playback audio queue callback.
  UInt32 propertySize = sizeof(state.mMaxPacketSize);
  AudioFileGetProperty(state.mAudioFile, kAudioFilePropertyPacketSizeUpperBound, &propertySize, &state.mMaxPacketSize);

  UInt64 fileDataSize;
  propertySize = sizeof(fileDataSize);
  AudioFileGetProperty(state.mAudioFile, kAudioFilePropertyAudioDataByteCount, &propertySize, &fileDataSize);

  UInt64 totalPackets;
  AudioFileGetProperty(state.mAudioFile, kAudioFilePropertyAudioDataPacketCount, &propertySize, &totalPackets);

  // This the actual length of the audio (checked with PCM only).
  Float64 totalSeconds = state.mDataFormat.mFramesPerPacket * totalPackets / state.mDataFormat.mSampleRate;

  fprintf(stderr, "Total length of the audio to play in seconds: %f \n", totalSeconds);

  // Sets an appropriate audio queue buffer size. We set 0.5 of seconds of audio that each audio queue buffer should
  // hold.
  DeriveBufferSize(state.mDataFormat, state.mMaxPacketSize, 0.5, &state.mBufferByteSize, &state.mNumPacketsToRead);

  // Determines if the audio file’s data format is VBR or CBR. In VBR data, one or both of the bytes-per-packet or
  // frames-per-packet values is variable, and so will be listed as 0 in the audio queue’s state structure. For an
  // audio file that contains VBR data, we allocate memory for the packet descriptions array based on the number of
  // audio data packets to be read on each invocation of the playback callback.
  bool isFormatVBR = (state.mDataFormat.mBytesPerPacket == 0 || state.mDataFormat.mFramesPerPacket == 0);
  if (isFormatVBR) {
    state.mPacketDescs = (AudioStreamPacketDescription *) malloc(state.mNumPacketsToRead *
        sizeof(AudioStreamPacketDescription));
  } else {
    state.mPacketDescs = NULL;
  }

  // Now let's allocate and prime audio queue buffers for playback.
  state.mCurrentPacket = 0;
  // Start the queue and playback.
  state.mIsRunning = true;

  for (int i = 0; i < kNumberBuffers; ++i) {
    AudioQueueAllocateBuffer(state.mQueue, state.mBufferByteSize, &state.mBuffers[i]);

    HandleOutputBuffer(&state, state.mQueue, state.mBuffers[i]);
  }

  // Before we tell an audio queue to begin playing, we set its volume by way of the audio queue parameter mechanism.
  AudioQueueSetParameter(state.mQueue, kAudioQueueParam_Volume, aVolume);

  AudioQueueStart(state.mQueue, NULL /* start playing immediately */);

  // Round total seconds to a single decimal point to make sure that all audio queue buffers are emptied.
  CFRunLoopRunInMode(kCFRunLoopDefaultMode, std::round(totalSeconds * 10) / 10,
      false /* run loop should continue for the full time specified */);

  // Do the cleanup.
  AudioQueueDispose(state.mQueue, true /* dispose queue immediately */);
  AudioFileClose(state.mAudioFile);
  free(state.mPacketDescs);
}

void Player::HandleOutputBuffer(void *aAudioQueueData, AudioQueueRef aAudioQueue,
    AudioQueueBufferRef aAudioQueueBuffer) {
  AudioQueuePlayerState *pAqData = (AudioQueuePlayerState *) aAudioQueueData;

  // If the audio queue has stopped, return.
  if (!pAqData->mIsRunning) {
    return;
  }

  // The number of packets to read from the file being played.
  UInt32 numPackets = pAqData->mNumPacketsToRead;
  // The number of bytes of audio data that was read from the file being played.
  UInt32 numBytesReadFromFile = numPackets * pAqData->mMaxPacketSize;

  // First let's read data from an audio file and place it in an audio queue buffer.
  AudioFileReadPacketData(pAqData->mAudioFile, false, &numBytesReadFromFile, pAqData->mPacketDescs,
      pAqData->mCurrentPacket, &numPackets, aAudioQueueBuffer->mAudioData);

  // Now that data has been read from an audio file and placed in an audio queue buffer, let's enqueue the buffer (if
  // we have read some data). Once in the buffer queue, the audio data in the buffer is available for the audio queue
  // to send to the output device.
  if (numPackets > 0) {
    aAudioQueueBuffer->mAudioDataByteSize = numBytesReadFromFile;

    AudioQueueEnqueueBuffer(pAqData->mQueue, aAudioQueueBuffer, (pAqData->mPacketDescs ? numPackets : 0 /* CBR */),
        pAqData->mPacketDescs);

    // Increment the packet index according to the number of packets that were read.
    pAqData->mCurrentPacket += numPackets;
  } else {
    AudioQueueStop(pAqData->mQueue, false  /* stops only when all queued buffers have been played */);

    pAqData->mIsRunning = false;
  }
}

void Player::DeriveBufferSize(AudioStreamBasicDescription &aAudioStreamDescription, UInt32 aMaxPacketSize,
    Float64 aSeconds, UInt32 *aBufferSize, UInt32 *aNumPacketsToRead) {
  // For audio data formats that define a fixed number of frames per packet, derive the audio queue buffer size.
  if (aAudioStreamDescription.mFramesPerPacket != 0) {
    Float64 numPacketsForTime = aAudioStreamDescription.mSampleRate / aAudioStreamDescription.mFramesPerPacket *
        aSeconds;
    *aBufferSize = (UInt32) (numPacketsForTime * aMaxPacketSize);
  } else {
    // For audio data formats that do not define a fixed number of frames per packet, derive a reasonable audio
    // queue buffer size based on the maximum packet size and the upper bound we’ve set.
    *aBufferSize = kMaxBufferSize > aMaxPacketSize ? kMaxBufferSize : aMaxPacketSize;
  }

  // If the derived buffer size is above the upper bound we’ve set, adjusts it, taking into account the estimated
  // maximum packet size.
  if (*aBufferSize > kMaxBufferSize && *aBufferSize > aMaxPacketSize) {
    *aBufferSize = kMaxBufferSize;
  } else if (*aBufferSize < kMinBufferSize) {
    // If the derived buffer size is below the lower bound we’ve set, adjusts it to the bound.
    *aBufferSize = kMinBufferSize;
  }

  // The number of packets to read from the audio file on each invocation of the callback.
  *aNumPacketsToRead = *aBufferSize / aMaxPacketSize;
}

} // namespace lighthouse
