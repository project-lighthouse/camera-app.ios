//
//  AlertHelper.swift
//  Lighthouse Camera
//
//  Created by Punam Dahiya on 1/26/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

extension UIAlertController {
  // Customise UIAlertController to handle permission failure alerts

  enum AlertMessageID {
    // List permission failures id used by alerts shown inside lighthouse app.
    // These are used in custom UIAlertController initialization to
    // display respective message e.g. CameraFailed signifies lighthouse app
    // failed to get Camera permissions and cannot access camera
    case CameraFailed, MicrophoneFailed
  }

  convenience init(messageID: AlertMessageID) {
    switch messageID {
    case .CameraFailed:
      let message = NSLocalizedString("To access Camera, please, go to " +
      "Settings > Privacy > Camera and Enable camera permission for the Lighthouse app.", comment: "")
      self.init(title: "Error", message: message, preferredStyle: .alert)

    case .MicrophoneFailed:
      let message = NSLocalizedString("To add or edit voice labels for the images, please, go to " +
      "Settings > Privacy > Microphone and Enable microphone permission for the Lighthouse app.", comment: "")
      self.init(title: "Error", message: message, preferredStyle: .alert)
    }

    let okButton = UIAlertAction(title: "OK", style: .default, handler: nil)
    self.addAction(okButton)
  }
}
