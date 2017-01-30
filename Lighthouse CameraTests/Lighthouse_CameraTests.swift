//
//  Lighthouse_CameraTests.swift
//  Lighthouse CameraTests
//
//  Created by David Teller on 16/12/16.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

import XCTest
@testable import Lighthouse_Camera

class Lighthouse_CameraTests: XCTestCase {
  var bridge: Bridge!

  override func setUp() {
    super.setUp()
    // Put setup code here. This method is called before the invocation of each test method in the class.
    self.bridge = Bridge()
  }
    
  override func tearDown() {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    super.tearDown()
  }
    
  func testExample() {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
   XCTAssert(true, "THIS SHOULD SUCCEED");
  }

  func testTrivialCallbacks() {
    // Test that we can receive callbacks from the C++ event loop.
    // Each call to `doNoop` should callback `onProgress` to the delegate exactly once.
    class Callback: LighthouseDelegate {
      let value: UInt32
      var received: Bool
      let expectation: XCTestExpectation
      required init(_ value: UInt32, _ expectation: XCTestExpectation) {
        print("Creating a callback", value);
        self.value = value;
        self.received = false;
        self.expectation = expectation;
      }
      override func onProgress(_ code: NSNumber) {
        print("Callback.OnProgress", code);
        XCTAssert(!self.received, "Have we already received a call to `onProgress`?");
        XCTAssertEqual(self.value, code.uint32Value);
        self.received = true;
        self.expectation.fulfill();
      }
      override func onError(_ error: Error) {
        XCTAssert(false, "Did we call `onError`?");
      }
    }
    let callbacks = (1 ... 10).map {
      Callback($0, expectation(description: "callback"))
    }
    print("Callbacks", callbacks.count);
    for cb in callbacks {
      self.bridge.doNoop(cb, withResult:cb.value);
    }
    self.waitForExpectations(timeout:10 /*seconds*/);
  }
}
