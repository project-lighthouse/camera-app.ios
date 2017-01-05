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
    
    //MARK: Properties
    @IBOutlet weak var button: UIButton!
    @IBOutlet weak var imageView: UIImageView!

    //MARK: Actions
    let queue = DispatchQueue(label: "com.mozilla.cd.lighthouse")

    // Invoked when the user has clicked on "record".
    @IBAction func onRecordClick(_ sender: Any) {
        bridge.onRecordObject();
/*
        let captureSession = AVCaptureSession()
        captureSession.beginConfiguration()
        
        
        // Locate a camera.
        let discoverySession = AVCaptureDeviceDiscoverySession(deviceTypes: [AVCaptureDeviceType.builtInWideAngleCamera], mediaType: AVMediaTypeVideo, position: AVCaptureDevicePosition.front)
        let devices = discoverySession?.devices
        if devices?.count == 0 {
            self.showError(message: "No camera")
            // FIXME: Check that we're on the simulator.
            return
        }

        let device = devices![0]
        if device.hasFlash {
            device.flashMode = AVCaptureFlashMode.on
        }

        // Prepare input.
        let input : AVCaptureDeviceInput
        do {
            input = try AVCaptureDeviceInput(device: device)
        } catch _ {
            self.showError(message: "Cannot open camera")
            return
        }

        captureSession.addInput(input)
        // FIXME: use preferredVideoStabilizationMode

        // Prepare an output to examine frame-by-frame.
        let dataOutput = AVCaptureVideoDataOutput()
        dataOutput.alwaysDiscardsLateVideoFrames = true

        // FIXME: Also show a preview?
        captureSession.addOutput(dataOutput)
        captureSession.commitConfiguration()
    
        let delegate = CaptureDelegate()
        dataOutput.setSampleBufferDelegate(delegate, queue: self.queue)

        // FIXME: When do we stop?
*/
 }

    // Invoked when the user has clicked on "identify".
    @IBAction func onIdentifyClick(_ sender: Any) {
        let matches: Array = bridge.match(self.imageView.image)

        let alert = UIAlertController(title: "Match Result", message: "Matches found: \(matches.count)",
                preferredStyle: UIAlertControllerStyle.alert)

        // Define alert actions.
        let defaultAction = UIAlertAction(title: "OK", style: UIAlertActionStyle.default)

        let rememberAction = UIAlertAction(title: "Remember", style: UIAlertActionStyle.default, handler: {
            action in
            self.bridge.saveDescription(self.imageView.image)
        })

        let showKeypointsAction = UIAlertAction(title: "Show Keypoints", style: UIAlertActionStyle.default, handler: {
            action in
            self.imageView.image = self.bridge.drawKeypoints(self.imageView.image)
        })

        alert.addAction(defaultAction)
        alert.addAction(rememberAction)
        alert.addAction(showKeypointsAction)

        self.present(alert, animated: true, completion: nil)
    }

    private func showError(message: String) {
        let alert = UIAlertView()
        alert.title = "Error"
        alert.message = message
        alert.addButton(withTitle: "Ok")
        alert.show()
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()

        bridge = Bridge()

        self.registerSettingsBundle()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }

    func registerSettingsBundle(){
        let defaults = UserDefaults.standard

        defaults.set(500, forKey: "Matching:NumberOfFeatures")
    }
}

