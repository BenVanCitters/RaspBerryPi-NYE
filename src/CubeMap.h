/*
 *  CubeMap.h
 *  CubeMapping
 *
 *  Created by David Wicks on 2/27/09.
 *  Copyright 2009 The Barbarian Group. All rights reserved.
 *
 */
 #include <vector>
//#include "cinder/Surface.h"
//#include "cinder/gl/gl.h"
//#include "cinder/ImageIo.h"
//#include "cinder/gl/Texture.h"
//#include "cinder/Capture.h"
//class CaptureRef;
#include "ofMain.h"
//using ci::Surface8u;
class CubeMap
{
    
    GLint mWidth = 512;
    GLint mHeight = 512;
	unsigned int textureObject;
    std::vector<ofVideoGrabber>		mCaptures;
    ofVideoGrabber              mCapture;
    ofTexture          mVideoTexture;
public:
	//this should be overloaded or generalized to allow different types of texture inputs
	CubeMap( GLsizei texWidth,
            GLsizei texHeight,
            const ofTexture &pos_x,
            const ofTexture &pos_y,
            const ofTexture &pos_z,
            const ofTexture &neg_x,
            const ofTexture &neg_y,
            const ofTexture &neg_z );
	void bind();
	void bindMulti( int loc );
	void unbind();
    void update();
	static void enableFixedMapping();
	static void disableFixedMapping();
private:
    void flipSurface(ofTexture & surf);
    void setupCapture();
};