//
//  BlobMesh.h
//  CubeMapping
//
//  Created by Ben Van Citters on 7/26/13.
//
//

#ifndef __CubeMapping__BlobMesh__
#define __CubeMapping__BlobMesh__

#include <iostream>
#include <vector>
#include "weight.h"
#include "cinder/gl/Vbo.h"

class BlobMesh
{
private:
    static const int mMeshWidth = 180, mMeshHeight = 180;
    vector<Weight> mWeights;
    float mVertexVals[mMeshWidth][mMeshHeight];
    void init(int weightCount);
public:
    BlobMesh();
    BlobMesh(int weightCount);
    void update();
    void constructVBO();
    gl::VboMeshRef	mVboMesh;
};


#endif /* defined(__CubeMapping__BlobMesh__) */
