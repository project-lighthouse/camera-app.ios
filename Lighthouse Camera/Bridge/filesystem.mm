//
//  filesystem.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 03/01/2017.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#include "filesystem.hpp"
#import <Foundation/Foundation.h>

NSString *(^getRoot)() = ^() {
    return NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES)[0];
};

/*static*/ std::string
Filesystem::GetResourcePath(const std::string &aName, const std::string &aType) {
    NSString *nsName = [NSString stringWithCString:aName.c_str()
                                          encoding:[NSString defaultCStringEncoding]];
    NSString *nsType = [NSString stringWithCString:aType.c_str()
                                          encoding:[NSString defaultCStringEncoding]];
    NSString *path = [[NSBundle mainBundle] pathForResource:nsName ofType:nsType];
    return [path UTF8String]; // FIXME: What's the ownership of this?
}

// FIXME: Use common private method for all overloads.
std::string Filesystem::GetResourcePath(const std::string &aName, const std::string &aType,
        const std::string &aSubPath) {
    NSString *nsName = [NSString stringWithCString:aName.c_str() encoding:[NSString defaultCStringEncoding]];
    NSString *nsType = [NSString stringWithCString:aType.c_str() encoding:[NSString defaultCStringEncoding]];
    NSString *nsSubPath = [NSString stringWithCString:aSubPath.c_str() encoding:[NSString defaultCStringEncoding]];

    NSString *path = [[NSBundle mainBundle] pathForResource:nsName ofType:nsType inDirectory:nsSubPath];
    return [path UTF8String];
}

std::string Filesystem::GetRoot() {
    return [getRoot() UTF8String];
}

std::vector<std::string> Filesystem::GetSubFolders(const std::string aDirectoryName) {
    NSString *directoryName = [NSString stringWithCString:aDirectoryName.c_str() encoding:NSUTF8StringEncoding];

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSArray *dirs = [fileManager contentsOfDirectoryAtPath:directoryName error:NULL];

    std::vector<std::string> subFolders;
    for (NSString *itemName in dirs) {
        NSString *path = [directoryName stringByAppendingPathComponent:itemName];

        BOOL isDir = NO;
        [fileManager fileExistsAtPath:path isDirectory:(&isDir)];
        if (isDir) {
            subFolders.push_back([path UTF8String]);
        }
    }

    return subFolders;
}

void Filesystem::CreateDirectory(const std::string aDirectoryPath) {
    NSString *directoryPath = [NSString stringWithCString:aDirectoryPath.c_str() encoding:NSUTF8StringEncoding];

    NSError *error;
    if (![[NSFileManager defaultManager] fileExistsAtPath:directoryPath])
        [[NSFileManager defaultManager] createDirectoryAtPath:directoryPath withIntermediateDirectories:YES
                                                   attributes:nil error:&error];
}
