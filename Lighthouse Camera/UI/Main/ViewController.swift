//
//  ViewController.swift
//  Lighthouse Camera
//
//  Created by David Teller on 16/12/16.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

import UIKit
import AVFoundation

class ViewController: UIViewController, PreviewViewDelegate {
  var bridge: Bridge!
  var isBusy: Bool

  required init?(coder aCoder: NSCoder) {
    isBusy = false
    super.init(coder: aCoder)
  }

  //MARK: Properties
  // Outlet for the captured images
  @IBOutlet weak var imageView: UIImageView!
  // TBD - Outlet for debug info, to be removed eventually
  @IBOutlet weak var label: UILabel!
  // Outlet for the live preview UIView
  @IBOutlet private weak var previewView: PreviewView!

  // Invoked when the user has clicked on "record/capture".
  @IBAction func onCaptureClick(_ sender: Any) {
    // Check for camera authorization before starting match
    checkCameraAuthorization { authorized in
      if authorized {
        if self.isBusy {
            self.bridge.onStopCapture()
            self.isBusy = false
        }
        // Ask for microphone permission beforehand so that user is not interrupted when he tries to record voice label
        // for the first time.
        self.checkMicrophoneAuthorization { authorized in
          if authorized {
            self.bridge.onRecordObject();
            self.isBusy = true
            self.resetView()
          }
        }
      }
    }
  }

  // Invoked when the user clicks to stop the ongoing action.
  @IBAction func onEmptyClick(_ sender: Any) {
    bridge.onStopCapture();
  }

  // Invoked when the user has clicked on "identify/match".
  @IBAction func onMatchClick(_ sender: Any) {
    // Check for camera authorization before starting match
    checkCameraAuthorization { authorized in
      if authorized {
        if self.isBusy {
          self.bridge.onStopCapture()
          self.isBusy = false
        } else {
          self.bridge.onIdentifyObject();
          self.isBusy = true
        }
        self.resetView()
      }
    }
  }

  @objc(operationComplete)
  public dynamic func operationComplete() {
    NSLog("operation complete \n");
    self.isBusy = false
    self.resetView()
  }

  // Used by `Feedback` to show frames currently being recorded
  // from the camera.
  @objc(showFrame:)
  public dynamic func showFrame(frame: UIImage) {
    NSLog("ViewController.showFrame %f x %f\n", frame.size.width * frame.scale, frame.size.height * frame.scale);
    self.imageView.image = frame;
  }

  @objc(showText:)
  public dynamic func showText(text: NSString) {
    NSLog("ViewController.showText %s\n", text.utf8String!);
    self.label.text = text as String;
  }

  /**
   Method is called whenever new item is recorded to perform followup actions (re-record voice label, discard item
   etc.).

   - parameter itemId: String Unique identifier of the item that has been recorded.
  */
  @objc(onItemRecorded:)
  public dynamic func onItemRecorded(itemId: String) {
    print("Item is successfully added: ", itemId)

    // Item has been successfully added, so let's replay the voice label.
    bridge.playVoiceLabel(itemId)
  }

  override func viewDidLoad() {
    super.viewDidLoad()

    bridge = Bridge()
    isBusy = false;
    sViewController = self;

    // Assign the delegate to self, previewView UIView is holding a weak reference to
    // PreviewViewDelegate class and will be deallocated
    previewView.delegate = self

    registerSettingsBundle()

    // Establish audio session.
    // FIXME: We should probably start session only when we really need it and stop when we don't.
    let session = AVAudioSession.sharedInstance()
    try! session.setCategory(AVAudioSessionCategoryPlayAndRecord,
        with: AVAudioSessionCategoryOptions.defaultToSpeaker)
    try! session.setActive(true)

  }

  func displayPreviewSession() {
    // Called from PreviewView when it starts preview session
    // Placeholder for any subsequent logic once preview session displays
  }

  override func didReceiveMemoryWarning() {
    super.didReceiveMemoryWarning()
  }

  override func viewWillAppear(_ animated: Bool) {
    super.viewWillAppear(animated)
  }

  override func viewDidAppear(_ animated: Bool) {
    super.viewDidAppear(animated)
    // Check for camera permission before preview view display
    checkCameraAuthorization { authorized in
      if authorized {
        // Announce Lighthouse App is ready to use
        AVSpeechSynthesizer(textID: .AppReady, rate: 0.55)
      }
    }
  }

  private func resetView() {
    if isBusy {
      imageView.isHidden = false
      previewView.videoPreviewLayer.opacity = 0
    } else {
      imageView.isHidden = true
      previewView.videoPreviewLayer.opacity = 1
    }
  }

  func registerSettingsBundle() {
    let defaults = UserDefaults.standard

    defaults.set(500, forKey: "Matching:NumberOfFeatures")
  }

}

