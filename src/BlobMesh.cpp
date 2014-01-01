//
//  BlobMesh.cpp
//  CubeMapping
//
//  Created by Ben Van Citters on 7/26/13.
//
//

#include "BlobMesh.h"
//#include "cinder/Rand.h"
//#include "cinder/app/App.h"

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
        mWeights.push_back(Weight(ofVec2f(ofRandom( 0.f, mMeshWidth ),
                                        ofRandom( 0.f, mMeshHeight )),
                                  ofVec2f(0,
                                        ofRandom( -0.5f, -2 ))));
    }
}

void BlobMesh::constructVBO()
{
    int totalVertices = mMeshWidth * mMeshHeight;
    int totalTris = totalVertices * 2;
//    gl::VboMesh::Layout layout;
//    layout.setDynamicIndices();
//    layout.setStaticIndices();
//    layout.setStaticPositions();
    //        layout.setStaticTexCoords2d();
  
//        layout.setDynamicColorsRGBA();
//    layout.setStaticColorsRGBA();
//    layout.setDynamicNormals();
//    mVboMesh = gl::VboMesh::create( totalVertices, totalTris*3, layout, GL_TRIANGLES );
    
    // buffer our static data - the texcoords and the indices
    vector<uint32_t> indices;
    vector<ofVec3f> positions;
//    vector<ColorA> colors;
    float r  = 1.f;
    cout << "mMeshHeight: " <<  mMeshHeight  << "mMeshWidth: " << mMeshWidth << endl;
    
    for( int y = 0; y < mMeshHeight; ++y )
    {
        for( int x = 0; x < mMeshWidth; ++x )
        {
            int curIndex =x+mMeshWidth*y;
            ofVec3f pos(r*sin(y*M_PI*2.f/(mMeshHeight-1))*cos(x*M_PI/(mMeshWidth-1)),
                        r*sin(y*M_PI*2.f/(mMeshHeight-1))*sin(x*M_PI/(mMeshWidth-1)),
                        r*cos(y*M_PI*2.f/(mMeshHeight-1)));
//            Vec3f(x*1.f/mMeshWidth - .5,
//                  y*1.f/mMeshHeight -.5,
//                  0)
            mVerts[curIndex] = pos;
                        // the texture coordinates are mapped to [0,1.0)
            //                texCoords.push_back( Vec2f( x / (float)VERTICES_X, y / (float)VERTICES_Z ) );
            
        }
    }
    
    int curIIndex = 0;
    for( int y = 0; y < mMeshHeight-1; ++y )
    {
        for( int x = 0; x < mMeshWidth-1; ++x )
        {
//            if( ( x + 1 < mMeshWidth ) && ( y + 1 < mMeshHeight ) )
            {
                int index =curIIndex*6;
                mIndicies[index+0] =(y+0) * mMeshWidth + (x+0);
                mIndicies[index+1] =(y+1) * mMeshWidth + (x+0);
                mIndicies[index+2] =(y) * mMeshWidth + (x+1);
                
                mIndicies[index+3] =(y+0) * mMeshWidth + (x+1);
                mIndicies[index+4] =(y+1) * mMeshWidth + (x+0);
                mIndicies[index+5] =(y+1) * mMeshWidth + (x+1);
                
                //dynamic
                //                mIndicies[index+0] =x*mMeshHeight+y;
                //                mIndicies[index+1] =(x+1)*mMeshHeight+y+1;
                //                mIndicies[index+2] =(x+1)*mMeshHeight+y;
                //
                //                mIndicies[index+3] =x*mMeshHeight+y;
                //                mIndicies[index+4] =x*mMeshHeight+y+1;
                //                mIndicies[index+5] =(x+1)*mMeshHeight+y+1;
                curIIndex++;
            }
        }
    }

    mVboMesh.setColorData(mVertColors, mMeshWidth*mMeshHeight, GL_DYNAMIC_DRAW);
//    mVboMesh->bufferIndices( indices );
//    mVboMesh->bufferColorsRGBA(colors);
    mVboMesh.setVertexData(mVerts, mMeshWidth*mMeshHeight, GL_STATIC_DRAW);
    mVboMesh.setIndexData(mIndicies, mMeshWidth*mMeshHeight*6, GL_STATIC_DRAW);
//    mVboMesh->bufferPositions( positions );
    //        mVboMesh->bufferTexCoords2d( 0, texCoords );
    //////
}

void BlobMesh::update()
{
    float curSeconds =  ofGetElapsedTimef();
    //move the blobs
    for( vector<Weight>::iterator cIt = mWeights.begin(); cIt != mWeights.end(); ++cIt )
    {
        cIt->move(ofVec2f(mMeshWidth,
                        mMeshHeight));
    }
    
//    gl::VboMesh::VertexIter iter = mVboMesh->mapVertexBuffer();
    vector<uint32_t> indices;

    for( int x = 0; x < mMeshWidth; ++x )
    {
        for( int y = 0; y < mMeshHeight; ++y )
        {
            mVertexVals[x][y] *= .5;
            for( vector<Weight>::iterator cIt = mWeights.begin(); cIt != mWeights.end(); ++cIt )
            {
                float val = (cIt->pos - ofVec2f(x,y)).length();
                val = 10/(val*val);
                mVertexVals[x][y] += val;
            }
            mVertColors[x+y*mMeshWidth] = ofFloatColor((sin(curSeconds*8+sqrt(mVertexVals[x][y])*100)+1)/2,
                                                       (sin(curSeconds + x/100.f)+1)/2,
                                                       (sin(curSeconds - y/8.f)+1)/2,
                                                       (sin((mVertexVals[x][y])*100)+1)/2);
//            ++iter;
        }
    }
    mVboMesh.updateColorData(mVertColors, mMeshWidth*mMeshHeight);
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
//                    //indices.push_back(y*mMeshWidth+x);
//                    //indices.push_back(y*mMeshWidth+x+1);
//                    //indices.push_back((y+1)*mMeshWidth+x+1);
//                    indices.push_back(x*mMeshHeight+y);
//                    indices.push_back((x+1)*mMeshHeight+y+1);
//                    indices.push_back((x+1)*mMeshHeight+y);
                }
                if(mVertexVals[x][y] > threshhold && mVertexVals[x+1][y+1] > threshhold && mVertexVals[x][y+1] > threshhold)
                {
//                    indices.push_back(x*mMeshHeight+y);
//                    indices.push_back(x*mMeshHeight+y+1);
//                    indices.push_back((x+1)*mMeshHeight+y+1);
//                    //indices.push_back(y*mMeshWidth+x);
//                    //indices.push_back((y+1)*mMeshWidth+x+1);
//                    //indices.push_back((y+1)*mMeshWidth+x);
                }
            }
            // the texture coordinates are mapped to [0,1.0)
            //                texCoords.push_back( Vec2f( x / (float)VERTICES_X, y / (float)VERTICES_Z ) );
        }
    }
//    mVboMesh->bufferIndices(indices);
}

void BlobMesh::draw()
{
    ofPushMatrix();
//    glTranslated( 0,0,0);//getWindowWidth() * 0.5f, getWindowHeight()* 0.35f, 100.0f );
    //        float s = mMouseLoc.x*1.f/getWindowWidth();

    ofScale(200,200,200);
    glPointSize(10.f);
    mVboMesh.draw(GL_TRIANGLES, 0, mMeshWidth*mMeshHeight*6);
//    gl::draw( GL_TRIANGLES, mBlobMesh.mVboMesh );
    glPopMatrix();

}