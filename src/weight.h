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
#include "ofMain.h"

//using namespace ci;
using std::vector;
using namespace std;

class Weight
{
public:
    Weight(ofVec2f position, ofVec2f vel);
    ofVec2f vel;
    ofVec2f pos;
    
    void move(ofVec2f rect);
    
};

#endif /* defined(__CubeMapping__weight__) */
