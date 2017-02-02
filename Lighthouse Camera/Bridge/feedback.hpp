//
//  feedback.hpp
//  Lighthouse Camera
//
//  Created by David Teller on 06/01/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//
//
// C++ callbacks (implemented in Obj-C) to let C++ code provide visual/auditory feedback.


#ifndef feedback_hpp
#define feedback_hpp

#include <stdio.h>

#include <opencv2/opencv.hpp> // Must be imported before Cocoa


class Feedback {
public:
  static void ShowLabel(const char* info);
  static void ReceivedFrame(const char* info, cv::Mat& frame);
  static void OperationComplete();
  static void OnItemRecorded(const std::string &aItemId);

  static void CameraSnap();

  static void CannotTakePicture();

  static void SetFlashLight(bool isOn); // FIXME: Implement

  static void PlaySoundNamed(const std::string &aSoundName);
  static void PlaySound(const std::string &aSoundPath);
  static void PlaySound(const std::string &aSoundPath, float aVolume);

  // Uses TTS engine to say the text with the specified utterance rate (default is -1.0f that is interpreted as
  // AVSpeechUtteranceDefaultSpeechRate).
  static void Say(const std::string &aText, float aUtteranceRate = -1.0f);

  /**
   * Record that something has happened.
   *
   * At the time of this writing, events are uploaded and processed through Google Analytics.
   * The exact instant at which statistics are uploaded is left for Google Analytics to
   * decide.
   *
   * If you wish to record something, please keep in mind the privacy of users.
   *
   * If you define a new category/action/label, please update the constants in "telemetry.h".
   *
   * @param aCategory A general category for the event, used for sorting. Example
   *    categories: "gui_action", "voice_action", "performance", ...
   *    See the #defines for standard categories.
   * @param aAction The action undertaken by the user or the app, also used for sorting.
   *    Generally speaking, that's the verb of the sentence. Examples: "clicked".
   * @param aLabel More information about the action. Generally speaking, that's
   *    the object of the action, e.g. "button_play", "silence", "duration_ms".
   *    If there is a physical unit involved, it should appear here.
   * @param aMeasure Some value measured by the action. For instance, a duration,
   *    a memory amount, ...
   */
  static void RecordTelemetryEvent(const std::string &aCategory,
                                   const std::string &aAction,
                                   const std::string &aLabel,
                                   const uint32_t aMeasure = 0);
};


#endif /* feedback_hpp */
