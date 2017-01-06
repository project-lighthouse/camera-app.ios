//
//  player.hpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 06/01/2017.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#ifndef player_hpp
#define player_hpp

#include <stdio.h>
#include <string>

namespace lighthouse {
class Player {
public:
    void Play(const std::string aFilePath);
};
} // namespace lighthouse

#endif /* player_hpp */
