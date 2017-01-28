#!/usr/bin/env bash

# Install dependencies that are needed by CocoaPods.

# OpenCV needs cmake.
if !which cmake > /dev/null; then
  echo "Lighthouse: Attempting to install cmake."
  if !which port > /dev/null; then
    echo "Lighthouse: Installing cmake with macports."
    sudo port install cmake
  elif !which brew > /dev/null; then
    echo "Lighthouse: Installing cmake with brew."
    sudo brew install cmake
  else
    echo "Lighthouse: Installing brew."
    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

    echo "Lighthouse: Installing cmake with brew."
    sudo brew install cmake
  fi
fi

# Install CocoaPods.
if ! which pod >/dev/null; then
  echo "Lighthouse: Installing CocoaPods."
  sudo gem install cocoapods
fi

# Installing further dependencies through CocoaPods.
echo "Lighthouse: Installing dependencies through CocoaPods."
echo "Lighthouse: Go and fetch a cup of tea, this may take a while."
pod install

echo "Lighthouse: We're all set."
echo "Lighthouse: To open Lighthouse from XCode, please use 'Lighthouse Camera.xcworkspace'"
echo "Lighthouse: ***NOT*** 'Lighthouse Camera.xcodeproj'"
