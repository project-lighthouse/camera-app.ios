bootstrap:
	echo "Installing dependencies" && \
  unzip 3rdparty/opencv-ios.zip -d Lib

test:
	xcodebuild build-for-testing -enableCodeCoverage NO -project "Lighthouse Camera.xcodeproj" -scheme "Lighthouse Camera" -sdk iphonesimulator10.2 | xcpretty -f `xcpretty-travis-formatter` && \
	3rdparty/xctool/xctool.sh run-tests -test-sdk iphonesimulator10.2 -scheme "Lighthouse Camera"
