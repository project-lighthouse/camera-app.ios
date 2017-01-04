//
//  CaptureDelegate.swift
//  Lighthouse Camera
//
//  Created by David Teller on 04/01/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

import Foundation
import AVFoundation

class CaptureDelegate: NSObject, AVCaptureVideoDataOutputSampleBufferDelegate {
    
    func captureOutput(_ captureOutput: AVCaptureOutput!, didOutputSampleBuffer sampleBuffer: CMSampleBuffer!, from connection: AVCaptureConnection!) {
        // FIXME: This should be called whenever we hit a new frame.
    }
}
