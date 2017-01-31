//
//  TTSHelper.swift
//  Lighthouse Camera
//
//  Created by Punam Dahiya on 1/27/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

import AVFoundation

extension AVSpeechSynthesizer {

  /// Input strings for TTS
  @objc public enum TextStrings: Int, CustomStringConvertible {
    case AppReady, AppClosed
    public var description: String {
      switch self {
      case .AppReady: return NSLocalizedString("Lighthouse App is Ready", comment: "")
      case .AppClosed: return NSLocalizedString("Lighthouse App is closing", comment: "")
      }
    }
  }

  @discardableResult convenience init(textID: TextStrings, rate: Float) {
    self.init()
    let utterance = AVSpeechUtterance(string: textID.description)
    utterance.rate = rate
    self.speak(utterance)
  }
}
