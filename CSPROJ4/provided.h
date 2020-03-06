//
//  provided.h
//  CSPROJ4
//
//  Created by Sam Shen on 3/4/20.
//  Copyright © 2020 CS Class. All rights reserved.
//

#ifndef provided_h
#define provided_h

#include <iostream>
#include <functional>
unsigned long hash(const std::string& g)
{
    std::hash<std::string> str_hash;
    unsigned long num = str_hash(g);
    return num;
}


#endif /* provided_h */
