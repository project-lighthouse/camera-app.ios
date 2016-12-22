//
//  ViewController.swift
//  Lighthouse Camera
//
//  Created by David Teller on 16/12/16.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
    
    //MARK: Properties
    @IBOutlet weak var button: UIButton!
    @IBOutlet weak var imageView: UIImageView!

    //MARK: Actions
    @IBAction func onButtonClick(_ sender: UIButton) {
        self.imageView.image = Bridge().drawKeypoints(UIImage(named: "lighthouse"))
    }

    override func viewDidLoad() {
        super.viewDidLoad()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
}

