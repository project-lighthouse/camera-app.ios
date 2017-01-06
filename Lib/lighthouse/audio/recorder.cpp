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

void Recorder::Record(const std::string aFilePath) {
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

    // Sets an appropriate audio queue buffer size. We set 0.5 of seconds of audio that each audio queue buffer should
    // hold.
    DeriveBufferSize(state.mQueue, state.mDataFormat, 0.5, &state.bufferByteSize);

    // Now we should ask the audio queue to prepare a set of audio queue buffers.
    for (int i = 0; i < kNumberBuffers; ++i) {
        // Allocate audio queue buffer.
        AudioQueueAllocateBuffer(state.mQueue, state.bufferByteSize, &state.mBuffers[i]);

        // Add an audio queue buffer to the end of a buffer queue.
        AudioQueueEnqueueBuffer(state.mQueue, state.mBuffers[i], 0, NULL);
    }

    // And finally let's record the audio.
    // Initialize the packet index to 0 to begin recording at the start of the audio file.
    state.mCurrentPacket = 0;

    // Set a flag in the custom structure to indicate that the audio queue is running. This flag is used by the
    // recording audio queue callback.
    state.mIsRunning = true;

    // Start the audio queue, on its own thread. NULL indicates that the audio queue should start recording immediately.
    AudioQueueStart(state.mQueue, NULL);

    // Wait for 3 seconds to allow user to record some audio.
    // FIXME: Here should be definitely something smarter.
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    // Let's stop and reset the recording audio queue. True indicates that we want to stop queue synchronously.
    AudioQueueStop(state.mQueue, true);

    // Set a flag to indicate that the audio queue is not running anymore.
    state.mIsRunning = false;

    // Close audio queue and dispose all related resources. True indicates that we want to dispose queue immediately.
    AudioQueueDispose(state.mQueue, true);

    AudioFileClose(state.mAudioFile);
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

    return state;
}

void Recorder::DeriveBufferSize(AudioQueueRef aAudioQueue, AudioStreamBasicDescription &aAudioStreamDescription,
        Float64 aSeconds, UInt32 *aOutBufferSize) {

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
    Float64 numBytesForTime = aAudioStreamDescription.mSampleRate * maxPacketSize * aSeconds;

    // Limit the buffer size, if needed, to the previously set upper bound.
    *aOutBufferSize = UInt32(numBytesForTime < kMaxBufferSize ? numBytesForTime : kMaxBufferSize);
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

    // Write the contents of the buffer to the audio data file.
    if (AudioFileWritePackets(pAqData->mAudioFile, false, aAudioQueueBuffer->mAudioDataByteSize, aPacketDescriptions,
            pAqData->mCurrentPacket, &aNumPackets, aAudioQueueBuffer->mAudioData) == noErr) {
        // If successful in writing the audio data, increment the audio data file’s packet index to be ready for writing
        // the next buffer's worth of audio data.
        pAqData->mCurrentPacket += aNumPackets;
    }

    // If the audio queue has stopped, return.
    if (pAqData->mIsRunning == 0) {
        return;
    }

    // Enqueues the audio queue buffer whose contents have just been written to the audio file.
    AudioQueueEnqueueBuffer(pAqData->mQueue, aAudioQueueBuffer, 0, NULL);
}


} /* recorder_hpp */
