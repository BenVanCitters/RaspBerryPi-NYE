//
//  BlobMesh.cpp
//  CubeMapping
//
//  Created by Ben Van Citters on 7/26/13.
//
//

#include "BlobMesh.h"
#include "cinder/Rand.h"
#include "cinder/app/App.h"

BlobMesh::BlobMesh(int weightCount)
{
    init(weightCount);
}

BlobMesh::BlobMesh()
{
//    init(4);
}

void BlobMesh::init(int weightCount)
{
    constructVBO();
    for(int i = 0; i < weightCount; i++)
    {
        mWeights.push_back(Weight(Vec2f(Rand::randFloat( 0.f, mMeshWidth ),
                                        Rand::randFloat( 0.f, mMeshHeight )),
                                  Vec2f(0,
                                        Rand::randFloat( -0.5f, -2 ))));
    }
}

void BlobMesh::constructVBO()
{
    int totalVertices = mMeshWidth * mMeshHeight;
    int totalTris = totalVertices * 2;
    gl::VboMesh::Layout layout;
//    layout.setDynamicIndices();
    layout.setStaticIndices();
    layout.setStaticPositions();
    //        layout.setStaticTexCoords2d();
  
        layout.setDynamicColorsRGBA();
//    layout.setStaticColorsRGBA();
    layout.setDynamicNormals();
    mVboMesh = gl::VboMesh::create( totalVertices, totalTris*3, layout, GL_TRIANGLES );
    
    // buffer our static data - the texcoords and the indices
    vector<uint32_t> indices;
    vector<Vec3f> positions;
//    vector<ColorA> colors;
    float r  = 1.f;
    for( int x = 0; x < mMeshWidth; ++x )
    {
        for( int y = 0; y < mMeshHeight; ++y )
        {
            Vec3f pos = Vec3f(r*sin(y*M_PI*2.f/(mMeshHeight-1))*cos(x*M_PI/(mMeshWidth-1)),
                              r*sin(y*M_PI*2.f/(mMeshHeight-1))*sin(x*M_PI/(mMeshWidth-1)),
                              r*cos(y*M_PI*2.f/(mMeshHeight-1)));
//            Vec3f(x*1.f/mMeshWidth - .5,
//                  y*1.f/mMeshHeight -.5,
//                  0)
            
            positions.push_back( pos );
            if( ( x + 1 < mMeshWidth ) && ( y + 1 < mMeshHeight ) )
            {
                indices.push_back( (y+0) * mMeshHeight + (x+0) );
                indices.push_back( (y+1) * mMeshHeight + (x+0) );
                indices.push_back( (y) * mMeshHeight + (x+1) );
                
                indices.push_back( (y+0) * mMeshHeight + (x+1) );
                indices.push_back( (y+1) * mMeshHeight + (x+0) );
                indices.push_back( (y+1) * mMeshHeight + (x+1) );
            }
            // the texture coordinates are mapped to [0,1.0)
            //                texCoords.push_back( Vec2f( x / (float)VERTICES_X, y / (float)VERTICES_Z ) );
        }
    }
    mVboMesh->bufferIndices( indices );
//    mVboMesh->bufferColorsRGBA(colors);
    mVboMesh->bufferPositions( positions );
    //        mVboMesh->bufferTexCoords2d( 0, texCoords );
    //////
}

void BlobMesh::update()
{
    float curSeconds =  ci::app::getElapsedSeconds() *1.f;
    //move the blobs
    for( vector<Weight>::iterator cIt = mWeights.begin(); cIt != mWeights.end(); ++cIt )
    {
        cIt->move(Vec2f(mMeshWidth,
                        mMeshHeight));
    }
    
    gl::VboMesh::VertexIter iter = mVboMesh->mapVertexBuffer();
    vector<uint32_t> indices;

    for( int x = 0; x < mMeshWidth; ++x )
    {
        for( int y = 0; y < mMeshHeight; ++y )
        {
            mVertexVals[x][y] *= .5;
            for( vector<Weight>::iterator cIt = mWeights.begin(); cIt != mWeights.end(); ++cIt )
            {
                float val = (cIt->pos - Vec2f(x,y)).length();
                val = 10/(val*val);
                mVertexVals[x][y] += val;
            }
            iter.setColorRGBA(ColorA((sin(curSeconds*8+sqrt(mVertexVals[x][y])*100)+1)/2,
                                     (sin(curSeconds + x/100.f)+1)/2,//(sin(y/1.3)+1)/2,
                                     (sin(curSeconds - y/8.f)+1)/2,//(sin(x+y/2.3)+1)/2,
                                     (sin((mVertexVals[x][y])*100)+1)/2));//(sin(y+x/2.3)+1)/2));
            ++iter;
        }
    }
    float threshhold = .025f;
    for( int x = 0; x < mMeshWidth; ++x )
    {
        for( int y = 0; y < mMeshHeight; ++y )
        {
            if(cos(curSeconds+x/5.4 + 10*sin(y/12.1) ) > (-1.5*(sin(curSeconds/15.1))-.7))
            if(x+1 < mMeshWidth && y+1 < mMeshHeight )
            {
                if(mVertexVals[x][y] > threshhold && mVertexVals[x+1][y] > threshhold && mVertexVals[x][y+1] > threshhold)
                {
//                    indices.push_back(y*mMeshWidth+x);
//                    indices.push_back(y*mMeshWidth+x+1);
//                    indices.push_back((y+1)*mMeshWidth+x+1);
                    indices.push_back(x*mMeshHeight+y);
                    indices.push_back((x+1)*mMeshHeight+y+1);
                    indices.push_back((x+1)*mMeshHeight+y);
                }
                if(mVertexVals[x][y] > threshhold && mVertexVals[x+1][y+1] > threshhold && mVertexVals[x][y+1] > threshhold)
                {
                    indices.push_back(x*mMeshHeight+y);
                    indices.push_back(x*mMeshHeight+y+1);
                    indices.push_back((x+1)*mMeshHeight+y+1);
//                    indices.push_back(y*mMeshWidth+x);
//                    indices.push_back((y+1)*mMeshWidth+x+1);
//                    indices.push_back((y+1)*mMeshWidth+x);
                }
            }
            // the texture coordinates are mapped to [0,1.0)
            //                texCoords.push_back( Vec2f( x / (float)VERTICES_X, y / (float)VERTICES_Z ) );
        }
    }
    mVboMesh->bufferIndices(indices);
}