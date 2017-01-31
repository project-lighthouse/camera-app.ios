//
//  PermissionHelper.swift
//  Lighthouse Camera
//
//  Created by Punam Dahiya on 1/26/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

import AVFoundation

extension UIViewController {
  // Check and grant permission for accessing the Camera.

  func checkCameraAuthorization(_ completionHandler: @escaping ((_ authorized: Bool) -> Void)) {
    switch AVCaptureDevice.authorizationStatus(forMediaType: AVMediaTypeVideo) {
    case .authorized:
      //The user has previously granted access to the camera.
      completionHandler(true)
    case .notDetermined:
      // The user has not yet been presented with the option to grant video access so request access.
      AVCaptureDevice.requestAccess(forMediaType: AVMediaTypeVideo, completionHandler: { success in
        if !success {
          self.present(UIAlertController(messageID: .CameraFailed), animated: true, completion: nil)
        }
        completionHandler(success)
      })
    case .denied, .restricted:
      self.present(UIAlertController(messageID: .CameraFailed), animated: true, completion: nil)
      // The user doesn't have the authority to request access e.g. parental restriction.
      completionHandler(false)
    }
  }
  // Check and grant permission for accessing the Microphone.

  func checkMicrophoneAuthorization(_ completionHandler: @escaping ((_ authorized: Bool) -> Void)) {
    AVAudioSession.sharedInstance().requestRecordPermission({ (granted: Bool) -> Void in
      if !granted {
        self.present(UIAlertController(messageID: .MicrophoneFailed), animated: true, completion: nil)
      }
      completionHandler(granted)
    })
  }
}
