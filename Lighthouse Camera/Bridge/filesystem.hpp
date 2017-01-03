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
    std::string GetRoot();
    std::vector<std::string> GetSubFolders(std::string aDirectoryName);
    void CreateDirectory(std::string aDirectoryPath);
};

#endif /* filesystem_hpp */
