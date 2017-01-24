//
//  recorder.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 05/01/2017.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#include <chrono>
#include <thread>

#include "recorder.hpp"

namespace lighthouse {

bool Recorder::Record(const std::string &aFilePath, const uint64_t aMaxLengthMs) {
  assert(aMaxLengthMs > 0);

  AudioQueueRecorderState state = PrepareState();

  // Create a new recording audio queue. NULL specifies default behavior the run loop on which the callback will be
  // invoked, in which it will be invoked on a thread internal to the audio queue. This is typical use that allows the
  // audio queue to record while your application’s user interface thread waits for user input to stop the recording.
  AudioQueueNewInput(&state.mDataFormat, HandleInputBuffer, &state, NULL, kCFRunLoopCommonModes, 0, &state.mQueue);

  // Get an expected property value size to use when querying the audio queue about its audio data format.
  UInt32 dataFormatSize = sizeof(state.mDataFormat);

  // When the audio queue came is created, it may have filled out the AudioStreamBasicDescription structure more
  // completely than we have, particularly for compressed formats. To obtain the complete format description, let's
  // call the AudioQueueGetProperty. We'll use the complete audio format when we create an audio file to record into.
  AudioQueueGetProperty(state.mQueue, kAudioQueueProperty_StreamDescription, &state.mDataFormat, &dataFormatSize);

  // Now let's create a file we'll be recording to.
  CFURLRef audioFileURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
      (const UInt8 *) aFilePath.c_str(), aFilePath.length(), false);

  AudioFileCreateWithURL(audioFileURL, state.mAudioFileType, &state.mDataFormat, kAudioFileFlags_EraseFile,
      &state.mAudioFile);

  // Sets an appropriate audio queue buffer size. We set `kAudioBufferLengthMs` milliseconds of audio that each audio
  // queue buffer should hold.
  DeriveBufferSize(state.mQueue, state.mDataFormat, kAudioBufferLengthMs, &state.mBufferByteSize,
      &state.mMaxPacketSize);

  // Now we should ask the audio queue to prepare a set of audio queue buffers.
  for (int i = 0; i < kNumberBuffers; ++i) {
    // Allocate audio queue buffer.
    AudioQueueAllocateBuffer(state.mQueue, state.mBufferByteSize, &state.mBuffers[i]);

    // Add an audio queue buffer to the end of a buffer queue.
    AudioQueueEnqueueBuffer(state.mQueue, state.mBuffers[i], 0, NULL);
  }

  // Turn on level metering (will be used later for leading/trailing silence monitoring).
  UInt32 levelMeteringOn = 1;
  AudioQueueSetProperty(state.mQueue, kAudioQueueProperty_EnableLevelMetering, &levelMeteringOn,
      sizeof(levelMeteringOn));

  // And finally let's record the audio.
  // Initialize the packet index to 0 to begin recording at the start of the audio file.
  state.mCurrentPacket = 0;

  // Set a flag in the custom structure to indicate that the audio queue is running. This flag is used by the
  // recording audio queue callback.
  state.mIsRunning = true;

  // Start the audio queue, on its own thread. NULL indicates that the audio queue should start recording immediately.
  AudioQueueStart(state.mQueue, NULL);

  // Wait for `aMaxLengthMs` milliseconds to allow user to record some audio. The max audio length still can be a little
  // longer than that because of time we spend in the main thread to analyze trailing silence, but it's not a problem.
  const uint32_t numberOfSleepCycles = (uint32_t) ceil(aMaxLengthMs / kAudioBufferLengthMs);
  const auto cycleDuration = std::chrono::milliseconds(kAudioBufferLengthMs);
  for (uint32_t i = 0; i < numberOfSleepCycles; i++) {
    std::this_thread::sleep_for(cycleDuration);

    if (state.mTrailingSilenceLength >= kMaxTrailingSilenceLengthMs) {
      fprintf(stderr, "Recorder::Record() stopping recording because of large trailing silence: %d ms. \n",
          state.mTrailingSilenceLength);
      break;
    }
  }

  // Let's stop and reset the recording audio queue.
  AudioQueueStop(state.mQueue, true /* stop immediately */);

  // Set a flag to indicate that the audio queue is not running anymore.
  state.mIsRunning = false;

  // Close audio queue and dispose all related resources.
  AudioQueueDispose(state.mQueue, true /* dispose queue immediately */);

  AudioFileClose(state.mAudioFile);

  // If we record at least something we say that recording completed successfully.
  return state.mCurrentPacket > 0;
}

AudioQueueRecorderState Recorder::PrepareState() {
  AudioQueueRecorderState state;

  // Defines the audio data format type as linear PCM.
  state.mDataFormat.mFormatID = kAudioFormatLinearPCM;
  state.mDataFormat.mSampleRate = 44100.0;
  state.mDataFormat.mChannelsPerFrame = 2;
  state.mDataFormat.mBitsPerChannel = 16;

  // Number of bytes per packet and the number of bytes per frame is 4 (2 channels times 2 bytes per sample).
  state.mDataFormat.mBytesPerPacket = state.mDataFormat.mBytesPerFrame =
      state.mDataFormat.mChannelsPerFrame * sizeof(SInt16);
  state.mDataFormat.mFramesPerPacket = 1;

  // Defines the file type as AIFF.
  state.mAudioFileType = kAudioFileAIFFType;

  // The format flags needed for the specified file type.
  state.mDataFormat.mFormatFlags = kLinearPCMFormatFlagIsBigEndian | kLinearPCMFormatFlagIsSignedInteger
      | kLinearPCMFormatFlagIsPacked;

  state.mTrailingSilenceLength = 0;

  return state;
}

void Recorder::DeriveBufferSize(AudioQueueRef aAudioQueue, AudioStreamBasicDescription &aAudioStreamDescription,
    Float64 aBufferLengthMs, UInt32 *aBufferSize, UInt32 *aMaxPacketSize) {

  // For CBR audio data, get the (constant) packet size from the AudioStreamBasicDescription structure. Use this value
  // as the maximum packet size. This assignment has the side effect of determining if the audio data to be recorded
  // is CBR or VBR. If it is VBR, the audio queue’s AudioStreamBasicDescription structure lists the value of
  // bytes-per-packet as 0.
  UInt32 maxPacketSize = aAudioStreamDescription.mBytesPerPacket;

  // For VBR audio data, query the audio queue to get the estimated maximum packet size.
  if (maxPacketSize == 0) {
    UInt32 maxVBRPacketSize = sizeof(maxPacketSize);
    AudioQueueGetProperty(aAudioQueue, kAudioQueueProperty_MaximumOutputPacketSize, &maxPacketSize,
        &maxVBRPacketSize);
  }

  // Derive the buffer size in bytes.
  Float64 numBytesForTime = aAudioStreamDescription.mSampleRate * maxPacketSize * aBufferLengthMs / 1000;

  // Limit the buffer size, if needed, to the previously set upper bound.
  *aBufferSize = UInt32(numBytesForTime < kMaxBufferSize ? numBytesForTime : kMaxBufferSize);
  *aMaxPacketSize = maxPacketSize;
}

void Recorder::HandleInputBuffer(void *aAudioQueueData, AudioQueueRef aAudioQueue,
    AudioQueueBufferRef aAudioQueueBuffer, const AudioTimeStamp *aStartTime, UInt32 aNumPackets,
    const AudioStreamPacketDescription *aPacketDescriptions) {

  AudioQueueRecorderState *pAqData = (AudioQueueRecorderState *) aAudioQueueData;

  // If the audio queue buffer contains CBR data, calculate the number of packets in the buffer. This number equals
  // the total bytes of data in the buffer divided by the (constant) number of bytes per packet. For VBR data,
  // the audio queue supplies the number of packets in the buffer when it invokes the callback.
  if (aNumPackets == 0 && pAqData->mDataFormat.mBytesPerPacket != 0) {
    aNumPackets = aAudioQueueBuffer->mAudioDataByteSize / pAqData->mDataFormat.mBytesPerPacket;
  }

  AudioQueueLevelMeterState meters[1];
  UInt32 metersLength = sizeof(meters);
  AudioQueueGetProperty(pAqData->mQueue, kAudioQueueProperty_CurrentLevelMeterDB, meters, &metersLength);
  Float32 peakPower = meters[0].mPeakPower;

  fprintf(stderr, "Recorder::HandleInputBuffer() sound level at %f is %f (silence: %s).\n", aStartTime->mSampleTime,
      peakPower, peakPower < kMinSoundPowerLevelThreshold ? "true" : "false");

  // If the sound level is less than the predefined threshold then we consider entire sample as silence and don't write
  // this portion to the file, otherwise let's write the contents of the buffer to the audio data file.
  if (peakPower > kMinSoundPowerLevelThreshold && AudioFileWritePackets(pAqData->mAudioFile, false,
      aAudioQueueBuffer->mAudioDataByteSize, aPacketDescriptions, pAqData->mCurrentPacket, &aNumPackets,
      aAudioQueueBuffer->mAudioData) == noErr) {
    // If successful in writing the audio data, increment the audio data file’s packet index to be ready for writing
    // the next buffer's worth of audio data.
    fprintf(stderr, "Recorder::HandleInputBuffer() packets written to a file: %lu.\n", aNumPackets);
    pAqData->mCurrentPacket += aNumPackets;
    // Zero trailing silence counter since silence has been interrupted.
    pAqData->mTrailingSilenceLength = 0;
  } else if (pAqData->mCurrentPacket != 0) {
    // In case we already recorded something previously let's increase trailing silence counter.
    pAqData->mTrailingSilenceLength += 1000 * aAudioQueueBuffer->mAudioDataByteSize /
        (pAqData->mDataFormat.mSampleRate * pAqData->mMaxPacketSize);
  }

  // If the audio queue has stopped, return.
  if (pAqData->mIsRunning == 0) {
    return;
  }

  // Enqueues the audio queue buffer whose contents have just been written to the audio file.
  AudioQueueEnqueueBuffer(pAqData->mQueue, aAudioQueueBuffer, 0, NULL);
}


} /* recorder_hpp */
