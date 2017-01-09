//
//  player.hpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 06/01/2017.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#ifndef player_hpp
#define player_hpp

#include <stdio.h>
#include <string>

#include "audio.hpp"

namespace lighthouse {

struct AudioQueuePlayerState : AudioQueueState {
    // The number of packets to read on each invocation of the audio queue’s playback callback. It is calculated in the
    // DeriveBufferSize function, after the audio queue is created and before it is started.
    UInt32 mNumPacketsToRead;
    // For VBR audio data, the array of packet descriptions for the file being played. For CBR data, the value of this
    // field is NULL.
    AudioStreamPacketDescription *mPacketDescs;
};

class Player {
public:
    /**
     * Plays the audio file path to which is passed as an argument. Note that this method doesn't support playing of
     * audio formats with "magic number/cookie".
     * @param aFilePath Path to the audio file we'd like to play.
     * @param aVolume Volume to use while playback. THe 0.0 is for silence and 1.0 for the unity gain (default).
     */
    static void Play(const std::string aFilePath, const float aVolume = 1.0);

private:
    /**
     * The Playback Audio Queue Callback.
     * @param aAudioQueueData A custom structure that contains state data for the audio queue.
     * @param aAudioQueue The audio queue that owns this callback.
     * @param aAudioQueueBuffer An audio queue buffer that the callback is to fill with data by reading from an audio
     * file.
     **/
    static void HandleOutputBuffer(void *aAudioQueueData, AudioQueueRef aAudioQueue,
            AudioQueueBufferRef aAudioQueueBuffer);

    /**
     * Specifies a size for the audio queue buffers. It derives a buffer size large enough to hold a given duration of
     * audio data. The calculation here takes into account the audio data format we’re playing. The format includes all
     * the factors that might affect buffer size, such as the number of audio channels.
     * @param aAudioStreamDescription The AudioStreamBasicDescription structure for the audio queue.
     * @param aMaxPacketSize The estimated maximum packet size for the data in the audio file you’re playing.
     * @param aSeconds The size for each audio queue buffer in seconds of audio.
     * @param[out] aBufferSize The size for each audio queue buffer in bytes.
     * @param[out] aNumPacketsToRead The number of packets of audio data to read from the file on each invocation of the
     * playback audio queue callback.
     **/
    static void DeriveBufferSize(AudioStreamBasicDescription &aAudioStreamDescription, UInt32 aMaxPacketSize,
            Float64 aSeconds, UInt32 *aBufferSize, UInt32 *aNumPacketsToRead);
};
} // namespace lighthouse

#endif /* player_hpp */
