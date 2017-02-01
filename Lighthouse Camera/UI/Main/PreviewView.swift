//
//  PreviewView.swift
//  Lighthouse Camera
//
//  Created by Punam Dahiya on 1/27/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

import UIKit
import AVFoundation

protocol PreviewViewDelegate: class {
  func displayPreviewSession()
}

// Displays what the camera lens sees in UIView, previewView to
// see a live video stream of phone camera's input within previewView

class PreviewView: UIView {
  // Keep weak reference of delegate UIView to break retain cycle and help get
  // it deallocated by ARC - Automatic Reference counting, it only frees up
  // memory for objects when there are zero strong references to them
  weak var delegate: PreviewViewDelegate?

  required init?(coder aDecoder:NSCoder) {
    super.init(coder: aDecoder)
    // call method to setup and start live preview session
    self.startPreviewSession()
  }

  var videoPreviewLayer: AVCaptureVideoPreviewLayer {
    return layer as! AVCaptureVideoPreviewLayer
  }

  // Session associated with the AVCaptureVideoPreviewLayer
  var session: AVCaptureSession? {
    get {
      return videoPreviewLayer.session
    }
    set {
      videoPreviewLayer.session = newValue
    }
  }

  // override default layer inside PreviewView to AVCaptureViedoPreviewLayer
  override class var layerClass: AnyClass {
    return AVCaptureVideoPreviewLayer.self
  }

  // override to make previewView element Accessibile
  override var isAccessibilityElement: Bool {
    get {
      return true
    }
    set {
      self.isAccessibilityElement = true
    }
  }

  // override Accessibility label
  override var accessibilityLabel: String? {
    get {
      return NSLocalizedString("Preview Screen", comment: "")
    }
    set {
      self.accessibilityLabel = NSLocalizedString("Preview Screen", comment: "")
    }
  }

  // Set up and start live preview session
  func startPreviewSession() {
    // Session will coordinate the input and output data from the devices camera
    let captureSession = AVCaptureSession()
    self.session = captureSession

    // create AVCapturePhotoOutput to attach the output to the session
    let sessionOutput = AVCapturePhotoOutput()

    // Select input device as rear camera which is the default device for media capture
    let backCamera = AVCaptureDevice.defaultDevice(withMediaType: AVMediaTypeVideo)

    var error: NSError?
    var input: AVCaptureDeviceInput!

    // Prepare input device, there is a chance that the input device might not be
    // available, set up a try catch to handle any potential errors
    do {
      // make a new AVCaptureDeviceInput and associate it with backCamera input device
      input = try AVCaptureDeviceInput(device: backCamera)
    } catch let error1 as NSError {
      error = error1
      input = nil
    }

    // The AVCaptureDeviceInput serve as middle layer to attach
    // the input device, backCamera to the session. If there are no errors
    // from previous step and the session is able to accept input,
    // then go ahead and add input to the Session.
    if error == nil && captureSession.canAddInput(input) {
      captureSession.addInput(input)

      if captureSession.canAddOutput(sessionOutput) {
        // If the session is able to accept output, then attach the output to the session
        captureSession.addOutput(sessionOutput)

        // After input and output are hooked up with session, configure
        // videoPreviewLayer to resize and fix the orientation to portrait
        self.videoPreviewLayer.videoGravity = AVLayerVideoGravityResize;
        self.videoPreviewLayer.connection.videoOrientation = AVCaptureVideoOrientation.portrait;

        // start the session
        captureSession.startRunning()
      }
    }

    // call displayPreviewSession method of PreviewViewDelegate
    self.delegate?.displayPreviewSession()
  }

}
