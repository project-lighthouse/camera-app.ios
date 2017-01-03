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
    return NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES)[0];
};

std::string Filesystem::GetRoot() {
    return [getRoot() UTF8String];
}

std::vector<std::string> Filesystem::GetSubFolders(std::string aDirectoryName) {
    NSString *directoryName = [NSString stringWithCString:aDirectoryName.c_str() encoding:NSUTF8StringEncoding];

    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSArray* dirs = [fileManager contentsOfDirectoryAtPath:directoryName error:NULL];

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

void Filesystem::CreateDirectory(std::string aDirectoryPath) {
    NSString *directoryPath = [NSString stringWithCString:aDirectoryPath.c_str() encoding:NSUTF8StringEncoding];

    NSError *error;
    if (![[NSFileManager defaultManager] fileExistsAtPath:directoryPath])
        [[NSFileManager defaultManager] createDirectoryAtPath:directoryPath withIntermediateDirectories:YES
                                                   attributes:nil error:&error];
}
