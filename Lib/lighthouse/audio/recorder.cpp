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
    AudioQueueRecorderState recorderState;

    // Defines the audio data format type as linear PCM.
    recorderState.mDataFormat.mFormatID = kAudioFormatLinearPCM;
    recorderState.mDataFormat.mSampleRate = 44100.0;
    recorderState.mDataFormat.mChannelsPerFrame = 2;
    recorderState.mDataFormat.mBitsPerChannel = 16;

    // Number of bytes per packet and the number of bytes per frame is 4 (2 channels times 2 bytes per sample).
    recorderState.mDataFormat.mBytesPerPacket = recorderState.mDataFormat.mBytesPerFrame =
            recorderState.mDataFormat.mChannelsPerFrame * sizeof(SInt16);
    recorderState.mDataFormat.mFramesPerPacket = 1;

    // Defines the file type as AIFF.
    recorderState.mAudioFileType = kAudioFileAIFFType;

    // The format flags needed for the specified file type.
    recorderState.mDataFormat.mFormatFlags = kLinearPCMFormatFlagIsBigEndian | kLinearPCMFormatFlagIsSignedInteger
            | kLinearPCMFormatFlagIsPacked;

    // The run loop on which the callback will be invoked. Using NULL to specify default behavior, in which the callback
    // will be invoked on a thread internal to the audio queue. This is typical use that allows the audio queue to
    // record while your application’s user interface thread waits for user input to stop the recording.
    CFRunLoopRef runLoop = NULL;

    // Create a new recording audio queue.
    AudioQueueNewInput(&recorderState.mDataFormat, HandleInputBuffer, &recorderState, runLoop, kCFRunLoopCommonModes, 0,
            &recorderState.mQueue);

    // Get an expected property value size to use when querying the audio queue about its audio data format.
    UInt32 dataFormatSize = sizeof(recorderState.mDataFormat);

    // When the audio queue came is created, it may have filled out the AudioStreamBasicDescription structure more
    // completely than we have, particularly for compressed formats. To obtain the complete format description, let's
    // call the AudioQueueGetProperty. We'll use the complete audio format when we create an audio file to record into.
    AudioQueueGetProperty(recorderState.mQueue, kAudioQueueProperty_StreamDescription, &recorderState.mDataFormat,
            &dataFormatSize);

    // Now let's create a file we'll be recording to.
    CFURLRef audioFileURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
            (const UInt8 *) aFilePath.c_str(), aFilePath.length(), false);

    AudioFileCreateWithURL(audioFileURL, recorderState.mAudioFileType, &recorderState.mDataFormat,
            kAudioFileFlags_EraseFile, &recorderState.mAudioFile);

    // Sets an appropriate audio queue buffer size. We set 0.5 of seconds of audio that each audio queue buffer should
    // hold.
    DeriveBufferSize(recorderState.mQueue, recorderState.mDataFormat, 0.5, &recorderState.bufferByteSize);

    // Now we should ask the audio queue to prepare a set of audio queue buffers.
    for (int i = 0; i < kNumberBuffers; ++i) {
        // Allocate audio queue buffer.
        AudioQueueAllocateBuffer(recorderState.mQueue, recorderState.bufferByteSize, &recorderState.mBuffers[i]);

        // Add an audio queue buffer to the end of a buffer queue.
        AudioQueueEnqueueBuffer(recorderState.mQueue, recorderState.mBuffers[i], 0, NULL);
    }

    // And finally let's record the audio.
    // Initialize the packet index to 0 to begin recording at the start of the audio file.
    recorderState.mCurrentPacket = 0;

    // Set a flag in the custom structure to indicate that the audio queue is running. This flag is used by the
    // recording audio queue callback.
    recorderState.mIsRunning = true;

    // Start the audio queue, on its own thread. NULL indicates that the audio queue should start recording immediately.
    AudioQueueStart(recorderState.mQueue, NULL);

    // Wait, on user interface thread, until user stops the recording.
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    // Let's stop and reset the recording audio queue. True indicates that we want to stop queue synchronously.
    AudioQueueStop(recorderState.mQueue, true);

    // Set a flag to indicate that the audio queue is not running anymore.
    recorderState.mIsRunning = false;

    // Close audio queue and dispose all related resources. True indicates that we want to dispose queue immediately.
    AudioQueueDispose(recorderState.mQueue, true);

    AudioFileClose(recorderState.mAudioFile);
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
