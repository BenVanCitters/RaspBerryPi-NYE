//
//  weight.h
//  CubeMapping
//
//  Created by Ben Van Citters on 7/26/13.
//
//

#ifndef __CubeMapping__weight__
#define __CubeMapping__weight__

#include <iostream>
#include "cinder/app/AppBasic.h"

using namespace ci;
using std::vector;
using namespace std;

class Weight
{
public:
    Weight(Vec2f position, Vec2f vel);
    Vec2f vel;
    Vec2f pos;
    
    void move(Vec2f rect);
    
};

#endif /* defined(__CubeMapping__weight__) */
