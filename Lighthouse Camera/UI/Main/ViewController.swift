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

    // Invoked when the user has clicked on "record".
    @IBAction func onRecordClick(_ sender: Any) {
        bridge.onRecordObject();
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
    
    @objc(showFrame:)
    public dynamic func showFrame(frame: UIImage) {
        NSLog("ViewController.showFrame %f x %f\n", frame.size.width * frame.scale, frame.size.height * frame.scale);
        self.imageView.image = frame;
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()

        bridge = Bridge()
        sViewController = self;

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

