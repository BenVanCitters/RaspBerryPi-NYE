//
//  weight.cpp
//  CubeMapping
//
//  Created by Ben Van Citters on 7/26/13.
//
//

#include "weight.h"
#include "cinder/Rand.h"

Weight::Weight(Vec2f position, Vec2f velocity)
{
    pos = position;
    vel = velocity;
}

void Weight::move(Vec2f rect)
{
    pos += vel;
    
//    if(pos.x > rect.x || pos.x < 0)
//    {
//        vel.x *=-1;
//        pos.x+=vel.x;
//    }
//    if(pos.y > rect.y || pos.y < 0)
//    {
//        vel.y *=-1;
//        pos.y += vel.y;
//    }
    float offset = 40;
    if(pos.x > rect.x + offset)
    {
        pos.x-= rect.x;
    }
    if(pos.y < -offset)
    {
        pos.y += rect.y+offset*2;
        pos.x = (Rand::randFloat(-offset+rect.x+offset));
    }
    
}