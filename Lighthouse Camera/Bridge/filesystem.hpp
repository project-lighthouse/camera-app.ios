//
//  filesystem.hpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 03/01/2017.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#include <string>
#include <vector>

#ifndef filesystem_hpp
#define filesystem_hpp

class Filesystem
{
public:
    static std::string GetResourcePath(const std::string& name, const std::string& type);
    std::string GetRoot();
    std::vector<std::string> GetSubFolders(const std::string aDirectoryName);
    void CreateDirectory(const std::string aDirectoryPath);
};

#endif /* filesystem_hpp */
