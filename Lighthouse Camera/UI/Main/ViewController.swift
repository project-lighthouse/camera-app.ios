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
    @IBAction func onButtonClick(_ sender: UIButton) {
        let image = UIImage(named: "lighthouse")

        self.imageView.image = bridge.drawKeypoints(image)
        bridge.saveDescription(image)
    }

    let queue = DispatchQueue(label: "com.mozilla.cd.lighthouse")

    // Invoked when the user has clicked on "record".
    @IBAction func onRecordClick(_ sender: Any) {
        let captureSession = AVCaptureSession()
        captureSession.beginConfiguration()
        
        
        // Locate a camera.
        let discoverySession = AVCaptureDeviceDiscoverySession(deviceTypes: [AVCaptureDeviceType.builtInWideAngleCamera], mediaType: AVMediaTypeVideo, position: AVCaptureDevicePosition.front)
        let devices = discoverySession?.devices
        if devices?.count == 0 {
            self.showError(message: "No camera")
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
    }

    // Invoked when the user has clicked on "identify".
    @IBAction func onIdentifyClick(_ sender: Any) {
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

