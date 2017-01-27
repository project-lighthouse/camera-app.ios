#!/usr/bin/env bash

# Install dependencies if necessary
if ! which swiftlint >/dev/null; then
    brew install swiftlint
fi

if ! which cmake >/dev/null; then
    brew install cmake
fi

# Run Swiftlint
swiftlint

