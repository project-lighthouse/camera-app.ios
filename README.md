# Lighthouse

Lighthouse iOS App that empowers the visually impaired to efficiently & independently identify household items.

Files are organized as follows:

- `Lib/` contains the C++ code that uses OpenCV APIs.
- `Lighhouse Camera/Bridge` is Objective C code.
  - Bridges UI in Swift with C++ back-end.
- `Lighhouse Camera/UI` and `Lighhouse Camera/Storyboards` consist of UI facing code.
- `Lighhouse Camera/Resources` contains sound and assets used in the app.

## Installation

Prerequisites:

1. Install XCode 8.2.1.
2. Download [OpenCV](http://opencv.org/downloads.html)


## Build Instructions

- Download the code in the repo
- Download, extract and copy [OpenCV iOS framework](http://opencv.org/downloads.html) to Lib folder
- Double click to open Lighthouse%20Camera.xcodeproj. To run on device change below two project level settings and push to device
  - Link your AppleId in XCode (XCode->Preferences->Accounts) 
  - Change App Bundle Id to your unique name
- If you see 'Untrusted Enterprise Devloper' message on device, go to Settings - General - Device Management - Tap on your Profile - Tap on Trust button

## Targeted Device Support

- iphone 5S & later (6, 6S,6+) w/ ios v. 10.x and 9.x

## More Info
Wiki: https://wiki.mozilla.org/Connected_Devices/Projects/Project_Lighthouse

Discourse: https://discourse.mozilla-community.org/c/connected-devices/project-lighthouse
