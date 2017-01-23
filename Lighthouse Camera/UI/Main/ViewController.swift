//
//  ViewController.swift
//  Lighthouse Camera
//
//  Created by David Teller on 16/12/16.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

import UIKit
import AVFoundation

class ViewController: UIViewController {
  var bridge: Bridge!

  var isBusy: Bool

  required init?(coder aCoder: NSCoder) {
    isBusy = false
    super.init(coder: aCoder)
  }

  //MARK: Properties
  @IBOutlet weak var imageView: UIImageView!
  @IBOutlet weak var label: UILabel!

  //MARK: Actions

  // Invoked when the user has clicked on "record".
  @IBAction func onRecordClick(_ sender: Any) {
    if self.isBusy {
      bridge.onStopCapture()
      self.isBusy = false

      return;
    }

    // Ask for microphone permission beforehand so that user is not interrupted when he tries to record voice label
    // for the first time.
    AVAudioSession.sharedInstance().requestRecordPermission({ (granted: Bool) -> Void in
      if granted {
        self.bridge.onRecordObject();
        self.isBusy = true
      } else {
        self.showError(message: "If you want to add or edit voice labels for the images, please, go to " +
            "Settings > Privacy > Microphone and enable microphone permission for the Lighthouse app.")
      }
    })
  }

  // Invoked when the user clicks to stop the ongoing action.
  @IBAction func onEmptyClick(_ sender: Any) {
    bridge.onStopCapture();
  }

  // Invoked when the user has clicked on "identify".
  @IBAction func onIdentifyClick(_ sender: Any) {
    if self.isBusy {
      bridge.onStopCapture()
      self.isBusy = false
    } else {
      bridge.onIdentifyObject();
      self.isBusy = true
    }
  }

  private func showError(message: String) {
    let alert = UIAlertController(title: "Error", message: message,
        preferredStyle: UIAlertControllerStyle.alert)
    alert.addAction(UIAlertAction(title: "OK", style: UIAlertActionStyle.default))

    self.present(alert, animated: true, completion: nil)
  }

  @objc(operationComplete)
  public dynamic func operationComplete() {
    self.isBusy = false
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
    self.isBusy = false;
    sViewController = self;

    self.registerSettingsBundle()

    // Establish audio session.
    // FIXME: We should probably start session only when we really need it and stop when we don't.
    let session = AVAudioSession.sharedInstance()
    try! session.setCategory(AVAudioSessionCategoryPlayAndRecord,
        with: AVAudioSessionCategoryOptions.defaultToSpeaker)
    try! session.setActive(true)
  }

  override func didReceiveMemoryWarning() {
    super.didReceiveMemoryWarning()
  }

  func registerSettingsBundle() {
    let defaults = UserDefaults.standard

    defaults.set(500, forKey: "Matching:NumberOfFeatures")
  }
}

