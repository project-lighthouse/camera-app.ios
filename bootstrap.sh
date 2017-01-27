#!/usr/bin/env bash

# Installing CocoaPods if necessary
if ! which pod >/dev/null; then
  echo "Lighthouse: Installing CocoaPods"
  sudo gem install cocoapods
fi

# Installing dependencies through CocoaPods
echo "Lighthouse: Installing dependencies through CocoaPods."
echo "Lighthouse: Go and fetch a cup of tea, this may take a while."
pod install

echo "Lighthouse: We're all set."
echo "Lighthouse: To open Lighthouse from XCode, please use 'Lighthouse Camera.xcworkspace'"
echo "Lighthouse: ***NOT*** 'Lighthouse Camera.xcodeproj'"
