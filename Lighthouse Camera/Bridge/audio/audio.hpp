//
//  audio.hpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 06/01/2017.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#ifndef audio_h
#define audio_h

#include <stdio.h>
#include <string>
#include <CoreAudio/CoreAudioTypes.h>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioToolbox/AudioToolbox.h>

namespace lighthouse {

// The number of audio queue buffers to use.
static const uint8_t kNumberBuffers = 3;

// An upper bound for the audio queue buffer size in bytes (320 KB). This corresponds to approximately five seconds
// of stereo, 24 bit audio at a sample rate of 96 kHz.
static const uint32_t kMaxBufferSize = 0x50000;

// A lower bound for the audio queue buffer size, in bytes (16 KB).
static const uint32_t kMinBufferSize = 0x4000;

struct AudioQueueState {
  // Represents the audio data format to write/read to/from disk. This format gets used by the audio queue
  // specified in the mQueue field.
  AudioStreamBasicDescription mDataFormat;
  // The recording/playback audio queue.
  AudioQueueRef mQueue;
  // An array holding pointers to the audio queue buffers managed by the audio queue.
  AudioQueueBufferRef mBuffers[kNumberBuffers];
  // An audio file object representing the file into which we'll record audio data or from which we'll be playing
  // audio.
  AudioFileID mAudioFile;
  // Describes type of the audio file (eg. AAIF).
  AudioFileTypeID mAudioFileType;
  // The size, in bytes, for each audio queue buffer.
  UInt32 mBufferByteSize;
  // The packet index for the first packet to be written from the current audio queue buffer.
  SInt64 mCurrentPacket;
  // Describes the maximum packet size.
  UInt32 mMaxPacketSize;
  // A Boolean value indicating whether or not the audio queue is running.
  bool mIsRunning;
};
} // namespace lighthouse
#endif /* audio_h */
