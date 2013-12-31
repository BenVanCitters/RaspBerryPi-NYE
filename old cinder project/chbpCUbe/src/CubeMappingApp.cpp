#include "Resources.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>

#include "cinder/Rand.h"

#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/DisplayList.h"
#include "cinder/gl/Vbo.h"
#include "cinder/Capture.h"
#include "cinder/Text.h"
#include "cinder/Camera.h"
#include "cinder/audio/Input.h"

#include "BlobMesh.h"
#include "CubeMap.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int CUBE_MAP_LOC = 4;
using std::vector;

class CubeMappingApp : public AppBasic {
  public:	
	gl::GlslProg		mShader;
    CameraPersp mCam;	

    static const int VERTICES_X = 720/4, VERTICES_Z = 1280/4;
    static const int WIDTH = 512, HEIGHT = 512;

    BlobMesh mBlobMesh;
	gl::VboMeshRef	mVboMesh;
    Vec2i mMouseLoc;
	//we're gonna have multiple cube maps
	vector<CubeMap>		mMaps;
	vector<CubeMap>::size_type	currentMap;
	float	theta;
    static const int eyeCount = 2;
	Vec2f   eyes[eyeCount];
    Vec2f   eyespd[eyeCount];
	vector<gl::TextureRef>	mNameTextures;
    
    audio::Input mInput;
    audio::PcmBuffer32fRef mPcmBuffer;
    
    
	void prepareSettings( Settings *settings )
	{
		settings->setWindowSize( 1024, 768 );
		settings->setFullScreen( true );
		settings->setResizable( true );
		settings->setFrameRate( 30.0f );
	}

	void setup()
	{
        hideCursor();
        mCam.setPerspective( 60.0f, getWindowAspectRatio(), 1.0f, 9000.0f );
        
        //initialize the audio Input, using the default input device
        mInput = audio::Input();
        
        //tell the input to start capturing audio
        mInput.start();
		
		glEnable( GL_DEPTH_TEST );
		glDepthMask( GL_TRUE );
		
		try {
			mShader = gl::GlslProg( loadResource( "CubeMapping_vert.glsl" ), loadResource( "CubeMapping_frag.glsl" ) );
		}
		catch( ci::gl::GlslProgCompileExc &exc ) {
			std::cout << "Shader compile error: " << std::endl;
			std::cout << exc.what();
		}
		catch( ... ) {
			std::cout << "Unable to load shader" << std::endl;
		}
		currentMap = 0;
		
        for(int i = 0; i < eyeCount; i++)
        {
            float theta = Rand::randFloat( 0.f, M_PI*2.f );
            float vel = 1.f;
            eyes[i] = Vec2f(Rand::randFloat( 0.f, VERTICES_X ),
                            Rand::randFloat( 0.f, VERTICES_Z ));
            eyespd[i] = Vec2f(vel*cos(theta),
                              vel*sin(theta));
        }
        
//		mMaps.push_back( CubeMap( GLsizei(WIDTH), GLsizei(HEIGHT),
//                                 Surface8u( loadImage( loadResource( "grandcanyon_positive_x.jpg" ) ) ),
//                                 Surface8u( loadImage( loadResource( "grandcanyon_positive_y.jpg" ) ) ),
//                                 Surface8u( loadImage( loadResource( "grandcanyon_positive_z.jpg" ) ) ),
//                                 Surface8u( loadImage( loadResource( "grandcanyon_negative_x.jpg" ) ) ),
//                                 Surface8u( loadImage( loadResource( "grandcanyon_negative_y.jpg" ) ) ),
//                                 Surface8u( loadImage( loadResource( "grandcanyon_negative_z.jpg" ) ) )));
		mMaps.push_back( CubeMap( GLsizei(512), GLsizei(512),
                                 Surface8u( loadImage( loadResource( "studioPosX.jpg" ) ) ),
                                 Surface8u( loadImage( loadResource( "studioPosY.jpg" ) ) ),
                                 Surface8u( loadImage( loadResource( "studioPosZ.jpg" ) ) ),
                                 Surface8u( loadImage( loadResource( "studioNegX.jpg" ) ) ),
                                 Surface8u( loadImage( loadResource( "studioNegY.jpg" ) ) ),
                                 Surface8u( loadImage( loadResource( "studioNegZ.jpg" ) ) )));
        //////////////////////
        
        // setup the parameters of the Vbo
        int totalVertices = VERTICES_X * VERTICES_Z;
        int totalTris = totalVertices * 2;//( VERTICES_X - 1 ) * ( VERTICES_Z - 1 );
        
        gl::VboMesh::Layout layout;
        layout.setStaticIndices();
        layout.setDynamicPositions();
        layout.setDynamicNormals();
        mVboMesh = gl::VboMesh::create( totalVertices, totalTris * 3, layout, GL_TRIANGLES );
        
        // buffer our static data - the texcoords and the indices
        vector<uint32_t> indices;
        vector<Vec2f> texCoords;
        for( int x = 0; x < VERTICES_X; ++x )
        {
            for( int y = 0; y < VERTICES_Z; ++y )
            {
                // create a quad for each vertex, except for along the bottom and right edges
                if( ( x + 1 < VERTICES_X ) && ( y + 1 < VERTICES_Z ) )
                {
                    indices.push_back( (x+0) * VERTICES_Z + (y+0) );
                    indices.push_back( (x+1) * VERTICES_Z + (y+0) );
                    indices.push_back( (x) * VERTICES_Z + (y+1) );
                    
                    indices.push_back( (x+0) * VERTICES_Z + (y+1) );
                    indices.push_back( (x+1) * VERTICES_Z + (y+0) );
                    indices.push_back( (x+1) * VERTICES_Z + (y+1) );
                }
                // the texture coordinates are mapped to [0,1.0)
//                texCoords.push_back( Vec2f( x / (float)VERTICES_X, y / (float)VERTICES_Z ) );
            }
        }
        
        mVboMesh->bufferIndices( indices );
//        mVboMesh->bufferTexCoords2d( 0, texCoords );
//        mTexture = gl::Texture::create( loadImage( loadResource( RES_IMAGE ) ) );
        mBlobMesh = BlobMesh(5);
	}
	
	void update()
	{
        mBlobMesh.update();
        // update eyes
        Vec2f flagSz(VERTICES_X,VERTICES_Z);
        float diagDist = flagSz.length();
        for(int i = 0; i < eyeCount; i++)
        {
            eyes[i] += eyespd[i];
            //eyes[i] += flagSz;
            
            if(eyes[i].x > flagSz.x || eyes[i].x < 0)
            {
                eyespd[i].x*=-1;
                eyes[i].x+=eyespd[i].x;
            }
            if(eyes[i].y > flagSz.y || eyes[i].y < 0)
            {
                eyespd[i].y *=-1;
                eyes[i].y += eyespd[i].y;
            }
        }
        
        //audio update
        mPcmBuffer = mInput.getPcmBuffer();
        if( ! mPcmBuffer ) {
            return;
        }
        
//        try (in vain) to show fps
//        TextLayout layout;
//        layout.clear( Color(1.0f, 1.0f, 1.0f) );
//        layout.setFont( Font( "Arial", 24 ) );
//        layout.setColor( Color( 1, 1, 1 ) );
//        layout.addLine( std::string("FPS: %f", getAverageFps()) );
//        mInfoTexture = gl::Texture::create( layout.render( true ) );
        cout << "FrameRate: " << getAverageFps() << endl;

        mMaps[currentMap].update();
        int totalVertices = VERTICES_X * VERTICES_Z;
        const float timeFreq = .30f;
        float offset = getElapsedSeconds() * timeFreq;
        Vec3f points[VERTICES_X][VERTICES_Z];
        
        uint32_t bufferSamples = mPcmBuffer->getSampleCount();
        audio::Buffer32fRef leftBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
//        cout << "buffersamples: " << bufferSamples << endl;
        
        float r = 1.f;
        // dynmaically generate our new positions based on a simple sine wave
        for( int x = 0; x < VERTICES_X; ++x )
        {
            for( int y = 0; y < VERTICES_Z; ++y)
            {
                float t = (x*VERTICES_Z+y)*1.f/totalVertices;
//                Vec3f pos = Vec3f((y-VERTICES_Z/2)*.2,
//                                  (x-VERTICES_X/2.f)*.2,
//                                  leftBuffer->mData[(int)(t*bufferSamples)]*1);
                float rPrime = 1.0f;//r + leftBuffer->mData[(int)(t*bufferSamples)]/21.f;
                Vec3f pos = Vec3f(rPrime*sin(y*M_PI*2.f/(VERTICES_Z-1))*cos(x*M_PI/(VERTICES_X-1)),
                                  rPrime*sin(y*M_PI*2.f/(VERTICES_Z-1))*sin(x*M_PI/(VERTICES_X-1)),
                                  rPrime*cos(y*M_PI*2.f/(VERTICES_Z-1)));
                points[x][y] = pos;
            }
        }
        
        gl::VboMesh::VertexIter iter = mVboMesh->mapVertexBuffer();
        for( int x = 0; x < VERTICES_X; ++x )
        {
            for( int y = 0; y < VERTICES_Z; ++y)
            {
                float curTheta = M_PI + y * M_PI*2/(VERTICES_Z-1);
                iter.setPosition(points[x][y]);
                Vec3f norm;
                int x1 = (x != VERTICES_X-1) ? x+1 : x-1;
                int y1 = (y != VERTICES_Z-1) ? y+1 : y-1;

                //get the actual surface normal
                norm = (points[x1][y]-points[x][y]).cross((points[x][y1]-points[x][y]));
                
//                float dist = diff.length();
                
//                norm.rotate(Vec3f(cos(curTheta),0,sin(curTheta)), sin(offset*5 + x/100.f)/40);
//                norm.y += (sin(offset + x/100.f)+1)/10050.f;
                for(int k = 0; k < eyeCount; k++)
                {
                    Vec2f diff = cinder::toPolar(eyes[k]-Vec2f(x,y));
                    norm.rotate((Vec3f)diff,  .004*cos(diff.x/10+diff.y*6));
                }
//                norm.rotate(Vec3f::yAxis(), .2*cos(dist/6));
//                    norm.rotate(Vec3f::xAxis(), .2*sin(dist/17));
                
                //                norm.rotate(Vec3f::yAxis(), 1.2*sin(-offset*3.1+ x*1.8*2*M_PI/VERTICES_X));
//                norm.rotate(Vec3f::xAxis(), 1.3*sin(offset+y*2.2*2*M_PI/VERTICES_Z));
                
                norm.normalize();
                iter.setNormal(norm);
                ++iter;
            }
        }
//        cout << "theta: " << theta << endl;
        float rrd = 400*sin(offset/6.67);//2000+ 900*(cos(offset/9.f)+1)/2;//mMouseLoc.y*10;
//        float thet = mMouseLoc.x*M_PI*2/getWindowWidth();
        mCam.lookAt( Vec3f(rrd*cos(offset/13.2),rrd*cos(3+offset/8.92),rrd*sin(8+offset/11.2)),//eye
                    Vec3f(sin(4+offset/14.3),sin(4+offset/7.2),sin(9+offset/7.1)),//center
                    Vec3f(sin(offset/5.2),sin(2+offset/7.1),cos(1+offset/3.3)) );//up
    }
	
	void draw()
	{
		if( ! mShader )
			return;
		float tm = getElapsedSeconds();
        glClearColor( (sin(tm/4.21)+1)/2,
                      (sin(2+tm/6.21)+1)/2,
                      (sin(5+tm/1.21)+1)/2,
                      1.0f );
//		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		gl::clear( Color( (sin(tm/4.21)+1)/2,
                         (sin(2+tm/6.21)+1)/2,
                         (sin(5+tm/1.21)+1)/2) );

//        render the framerate
//        gl::enableAlphaBlending();
//        gl::color( Color::black() );
//		gl::draw( mInfoTexture, Vec2f(  10 + 1,  10 + 1 ) );
//		gl::color( Color( 0.5, 0.75, 1 ) );
//		gl::draw( mInfoTexture, Vec2f( 10,  10 ) );
//        gl::disableAlphaBlending();
        
        //	glEnable( GL_BLEND );
		
        gl::setMatrices( mCam );

        
        
        glEnable( GL_MULTISAMPLE_ARB );
		//	glHint (GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
		
		mMaps[currentMap].bindMulti(CUBE_MAP_LOC);
		
		mShader.bind();
		
		mShader.uniform( "LightPos", Vec3f( getWindowWidth()/2 + 350.0f, getWindowHeight()/2 - 150.0f, 150.0f ) );
		mShader.uniform( "MixRatio", 0.5f );
		mShader.uniform( "EnvMap", CUBE_MAP_LOC );
		
		mShader.uniform( "BaseColor", Vec3f( 1.0f, 1.f, 1.f ) );
		mShader.uniform( "MixRatio", 0.4f );
        
        glPushMatrix();
        glTranslated( 0,0,0);//getWindowWidth() * 0.5f, getWindowHeight()* 0.35f, 100.0f );
        gl::scale( Vec3f( 800,800,800 ) );
        gl::draw( mVboMesh );
        glPopMatrix();

        
		glPushMatrix();
		glTranslated( 0.0f, cos( theta )*50.0f, 0.0f );
//		mSphere.draw();
		glPopMatrix();
//
//		mShader.uniform( "BaseColor", Vec3f( 0.15f, 0.75f, 0.6f ) );
//		mShader.uniform( "MixRatio", 0.2f );
//		glPushMatrix();
//		glTranslated( getWindowWidth() * 0.66f, getWindowHeight() * 0.5f, 0.0f );
////		mTorus.draw();
//		glPopMatrix();
		
        mShader.unbind();
		mMaps[currentMap].unbind();

        
        glPushMatrix();
        glTranslated( 0,0,0);//getWindowWidth() * 0.5f, getWindowHeight()* 0.35f, 100.0f );
//        float s = mMouseLoc.x*1.f/getWindowWidth();
        gl::scale( Vec3f( 200,200,200 ) );
        gl::draw( mBlobMesh.mVboMesh );
        glPopMatrix();

    }

    void mouseMove( MouseEvent event ) {
        mMouseLoc = event.getPos();
    }
    
	void mouseDown( app::MouseEvent event )
	{
		if( event.isLeftDown() ) {
			nextMap();
		}
		else if( event.isRightDown() ) {
			hideCursor();
		}
		else if( event.isMiddleDown() ) {
			showCursor();
		}
	}
	
	void keyDown( app::KeyEvent event ) {
		if( event.getCode() == app::KeyEvent::KEY_f ) {
			setFullScreen( true );
		}
		else if( event.getCode() == app::KeyEvent::KEY_ESCAPE ) {
			setFullScreen( false );
		}
		
		if( event.getCode() == app::KeyEvent::KEY_p )
		{
			nextMap();
		}
        
	}
	
	void nextMap(){
		mMaps[currentMap].unbind();
		currentMap++;
		if( currentMap >= mMaps.size() ) currentMap = 0;
	}

};


CINDER_APP_BASIC( CubeMappingApp, RendererGl )