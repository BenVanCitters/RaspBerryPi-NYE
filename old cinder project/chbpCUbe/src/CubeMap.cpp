/*
 *  CubeMap.cpp
 *  CubeMapping
 *
 *  Created by David Wicks on 2/27/09.
 *  Copyright 2009 The Barbarian Group. All rights reserved.
 *
 */

#ifndef	CUBE_MAP_H
#define CUBE_MAP_H
#include "CubeMap.h"
#include "cinder/app/AppBasic.h"
#include <vector>
#include "cinder/Capture.h"
#include <iostream>

using namespace ci;
using namespace ci::app;
using namespace std;

using ci::Surface8u;
using boost::shared_ptr;

CubeMap::CubeMap( GLsizei texWidth, GLsizei texHeight, const Surface8u &pos_x, const Surface8u &pos_y, const Surface8u &pos_z, const Surface8u &neg_x, const Surface8u &neg_y, const Surface8u &neg_z )
{
    mWidth = texWidth;
    mHeight = texHeight;
    setupCapture();
//    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS)​;
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	//create a texture object
	glGenTextures(1, &textureObject);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, textureObject);
	//assign the images to positions
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pos_x.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, neg_x.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pos_y.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, neg_y.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pos_z.getData());
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, neg_z.getData());
	//set filtering modes for scaling up and down
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void CubeMap::bindMulti( int pos )
{
	glActiveTexture(GL_TEXTURE0 + pos );
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, textureObject);
}

void CubeMap::update()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, textureObject);
    int i = 0;
	for( vector<CaptureRef>::iterator cIt = mCaptures.begin(); cIt != mCaptures.end(); ++cIt )
    {
        int GLCubeDir = -1;
        if( (*cIt)->checkNewFrame() )
        {
            Surface8u surf = (*cIt)->getSurface();
            switch(i)
            {
                case 0:
                    GLCubeDir = GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
                    
                    glTexImage2D(GLCubeDir, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, surf.getData());
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, surf.getData());
                    flipSurface(surf);
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, surf.getData());
                    break;
                case 1:
                    GLCubeDir = GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
//                    Surface8u surf = (*cIt)->getSurface();
                    glTexImage2D(GLCubeDir, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, surf.getData());
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, surf.getData());
                    flipSurface(surf);
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, surf.getData());
                    break;
//                case 2:
//                    GLCubeDir = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
//                    break;
                default:
                    GLCubeDir = GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
                    break;                
            }
            

		}
        i++;
    }
}

void CubeMap::flipSurface(Surface8u & surf)
{
    unsigned char* data = surf.getData();
    
    int w = surf.getRowBytes();
    int h = surf.getHeight();
    char r,g,b,a=0;
//    for(int i = 0 ; i < w*h*100; i++)
//        data[i] = 10;

    int pixInc = surf.getPixelInc();
    for(int j = 0; j < h; j++)
    {
        for(int i = 0; i < 256; i++)
        {
            int index =pixInc*i+j*w;
            int nIndex = (j+1)*w-(i)*pixInc;
            r = data[index];
            g = data[index+1];
            b = data[index+2];
            a = data[index+3];

            data[index]   = data[nIndex];
            data[index+1] = data[nIndex+1];
            data[index+2] = data[nIndex+2];
            data[index+3] = data[nIndex+3];
            
             data[nIndex]   = r;
             data[nIndex+1] = g;
             data[nIndex+2] = b;
             data[nIndex+3] = a;

        }
        
    }
}

void CubeMap::setupCapture()
{
    vector<Capture::DeviceRef> devices( Capture::getDevices() );
	for( vector<Capture::DeviceRef>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
		Capture::DeviceRef device = *deviceIt;
		console() << "Found Device " << device->getName() << " ID: " << device->getUniqueId() << std::endl;
		try {
			if( device->checkAvailable() ) {
				mCaptures.push_back( Capture::create( mWidth, mHeight, device ) );
				mCaptures.back()->start();                
			}
			else
				console() << "device is NOT available" << std::endl;
		}
		catch( CaptureExc & ) {
			console() << "Unable to initialize device: " << device->getName() << endl;
		}
	}

    
    
//    vector<Capture::DeviceRef> devices( Capture::getDevices() );
//    for( vector<Capture::DeviceRef>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt )
//    {
//        Capture::DeviceRef device = *deviceIt;
//        console() << "Found Device " << device->getName() << " ID: " << device->getUniqueId() << std::endl;
//        try
//        {
//            if( device->checkAvailable() )
//            {
//                mCapture =Capture::create( texWidth, texHeight, device );
//                mCapture->start();
//                
//                // placeholder text
//                //                    mTextures.push_back( gl::TextureRef() );
//                mVideoTexture =gl::TextureRef();
//
//            }
//            else
//                console() << "device is NOT available" << std::endl;
//        }
//        catch( CaptureExc & )
//        {
//            console() << "Unable to initialize device: " << device->getName() << endl;
//        }
//    }
    
}
void CubeMap::bind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, textureObject);
}

void CubeMap::unbind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0 );
}

void CubeMap::enableFixedMapping()
{
	glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP );
	glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP );
	glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP );
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable( GL_TEXTURE_CUBE_MAP );
}

void CubeMap::disableFixedMapping()
{
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable( GL_TEXTURE_CUBE_MAP );
}


#endif
