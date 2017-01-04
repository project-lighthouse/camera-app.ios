//
//  ViewController.swift
//  Lighthouse Camera
//
//  Created by David Teller on 16/12/16.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

import UIKit

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

