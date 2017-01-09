//
//  recorder.hpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 05/01/2017.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#ifndef recorder_hpp
#define recorder_hpp

#include <stdio.h>
#include <string>
#include <CoreAudio/CoreAudioTypes.h>
#include <AudioToolbox/AudioQueue.h>
#include <AudioToolbox/AudioFile.h>

#include "audio.hpp"

namespace lighthouse {

struct AudioQueueRecorderState : AudioQueueState {};

class Recorder {
public:
    void Record(const std::string aFilePath);

private:
    /**
     * Specifies a size for the audio queue buffers. It derives a buffer size large enough to hold a given duration of
     * audio data. The calculation here takes into account the audio data format we’re recording to. The format includes
     * all the factors that might affect buffer size, such as the number of audio channels.
     * @param aAudioQueue The audio queue that owns the buffers whose size we'd like to specify.
     * @param aAudioStreamDescription The AudioStreamBasicDescription structure for the audio queue.
     * @param aSeconds The size for each audio queue buffer in seconds of audio.
     * @param[out] aBufferSize The size for each audio queue buffer in bytes.
     **/
    static void DeriveBufferSize(AudioQueueRef aAudioQueue, AudioStreamBasicDescription &aAudioStreamDescription,
            Float64 aSeconds, UInt32 *aBufferSize);

    /**
     * The Recording Audio Queue Callback.
     * @param aAudioQueueData A custom structure that contains state data for the audio queue.
     * @param aAudioQueue The audio queue that owns this callback.
     * @param aAudioQueueBuffer The audio queue buffer containing the incoming audio data to record.
     * @param aStartTime The sample time of the first sample in the audio queue buffer (not needed for simple
     * recording).
     * @param aNumPackets The number of packet descriptions in the inPacketDesc parameter. A value of 0 indicates CBR
     * data.
     * @param aPacketDescriptions For compressed audio data formats that require packet descriptions, the packet
     * descriptions produced by the encoder for the packets in the buffer.
     **/
    static void HandleInputBuffer(void *aAudioQueueData, AudioQueueRef aAudioQueue,
            AudioQueueBufferRef aAudioQueueBuffer, const AudioTimeStamp *aStartTime, UInt32 aNumPackets,
            const AudioStreamPacketDescription *aPacketDescriptions);

    /**
     * Prepares custom AudioQueueRecorderState structure that will hold all the information for the audio queue (audio
     * file format, sizes of the queue buffer etc.) including the reference to the queue itself.
     * @return AudioQueueRecorderState
     **/
    static AudioQueueRecorderState PrepareState();
};
} // namespace lighthouse

#endif /* recorder_hpp */
