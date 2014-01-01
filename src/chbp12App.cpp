#include "chbp12App.h"

/*
 This code demonstrates the difference between using an ofMesh and an ofVboMesh.
 The ofMesh is uploaded to the GPU once per frame, while the ofVboMesh is
 uploaded once. This makes it much faster to draw multiple copies of an
 ofVboMesh than multiple copies of an ofMesh.
 */

/*
 These functions are for adding quads and triangles to an ofMesh -- either
 vertices, or texture coordinates.
 */
//--------------------------------------------------------------
void chbp12App::addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c) {
	mesh.addVertex(a);
	mesh.addVertex(b);
	mesh.addVertex(c);
}

//--------------------------------------------------------------
void chbp12App::addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d) {
	addFace(mesh, a, b, c);
	addFace(mesh, a, c, d);
}

//--------------------------------------------------------------
void chbp12App::addTexCoords(ofMesh& mesh, ofVec2f a, ofVec2f b, ofVec2f c) {
	mesh.addTexCoord(a);
	mesh.addTexCoord(b);
	mesh.addTexCoord(c);
}

//--------------------------------------------------------------
void chbp12App::addTexCoords(ofMesh& mesh, ofVec2f a, ofVec2f b, ofVec2f c, ofVec2f d) {
	addTexCoords(mesh, a, b, c);
	addTexCoords(mesh, a, c, d);
}

/*
 The 3d data is stored in an image where alpha represents depth. Here we create
 a 3d point from the current x,y image position.
 */
//--------------------------------------------------------------
ofVec3f chbp12App::getVertexFromImg(ofImage& img, int x, int y) {
	ofColor color = img.getColor(x, y);
	if(color.a > 0) {
		float z = ofMap(color.a, 0, 255, -480, 480);
		return ofVec3f(x - img.getWidth() / 2, y - img.getHeight() / 2, z);
	} else {
		return ofVec3f(0, 0, 0);
	}
}

//--------------------------------------------------------------
void chbp12App::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
    #ifdef TARGET_OPENGLES
    // While this will will work on normal OpenGL as well, it is 
    // required for OpenGL ES because ARB textures are not supported.
    // If this IS set, then we conditionally normalize our 
    // texture coordinates below.
    ofEnableNormalizedTexCoords();
    #endif

    mCubeMapper.load("shaders/CubeMapping_vert.glsl", "shaders/CubeMapping_frag.glsl");
    
	img.loadImage("linzer.png");
	
	// OF_PRIMITIVE_TRIANGLES means every three vertices create a triangle
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);
	int skip = 10;	// this controls the resolution of the mesh
	
	int width = img.getWidth();
	int height = img.getHeight();

	ofVec2f imageSize(width,height);

	ofVec3f zero(0, 0, 0);
	for(int y = 0; y < height - skip; y += skip) {
		for(int x = 0; x < width - skip; x += skip) {
			/*
			 To construct a mesh, we have to build a collection of quads made up of
			 the current pixel, the one to the right, to the bottom right, and
			 beneath. These are called nw, ne, se and sw. To get the texture coords
			 we need to use the actual image indices.
			 */
			ofVec3f nw = getVertexFromImg(img, x, y);
			ofVec3f ne = getVertexFromImg(img, x + skip, y);
			ofVec3f sw = getVertexFromImg(img, x, y + skip);
			ofVec3f se = getVertexFromImg(img, x + skip, y + skip);
			ofVec2f nwi(x, y);
			ofVec2f nei(x + skip, y);
			ofVec2f swi(x, y + skip);
			ofVec2f sei(x + skip, y + skip);
			
			// ignore any zero-data (where there is no depth info)
			if(nw != zero && ne != zero && sw != zero && se != zero) {
				addFace(mesh, nw, ne, se, sw);

				// Normalize our texture coordinates if normalized 
				// texture coordinates are currently enabled.
				if(ofGetUsingNormalizedTexCoords()) {
					nwi /= imageSize;
					nei /= imageSize;
					sei /= imageSize;
					swi /= imageSize;
				}

				addTexCoords(mesh, nwi, nei, sei, swi);
			}
		}
	}
	
	vboMesh = mesh;
    mBlobMesh = BlobMesh(5);
    buildSphere();
}

void chbp12App::buildSphere()
{
//	mReflectiveSphereVbo
}

//--------------------------------------------------------------
void chbp12App::update()
{
	mBlobMesh.update();
}

//--------------------------------------------------------------
void chbp12App::draw() {
	ofBackgroundGradient(ofColor(64), ofColor(0));
	cam.begin();
	ofEnableDepthTest();
	
//	ofRotateY(ofGetElapsedTimef() * 30); // slowly rotate the model
//	
//	ofScale(1, -1, 1); // make y point down
//	ofScale(.5, .5, .5); // make everything a bit smaller
//	
//	img.bind(); // bind the image to begin texture mapping
//	int n = 5; // make a 5x5 grid
//	ofVec2f spacing(img.getWidth(), img.getHeight()); // spacing between meshes
//	ofTranslate(-spacing.x * n / 2, -spacing.y * n / 2, 0); // center the grid
//	for(int i = 0; i < n; i++) { // loop through the rows
//		for(int j = 0; j < n; j++) { // loop through the columns
//			ofPushMatrix();
//			ofTranslate(i * spacing.x, j * spacing.y); // position the current mesh
//			ofTranslate(spacing.x / 2, spacing.y / 2); // center the mesh
//			if(ofGetKeyPressed()) {
//				vboMesh.draw(); // draw a vboMesh (faster) when a key is pressed
//			} else {
//				mesh.draw(); // draw an ofMesh (slower) when no key is pressed
//			}
//			ofPopMatrix();
//		}
//	}
//	img.unbind();
    
	ofPushMatrix();
    ofTranslate(0,0,-500+ofGetMouseX()*1000.f/ofGetWindowWidth() );
    ofRotateX(ofGetElapsedTimef());
    ofRotateY(ofGetElapsedTimef());
    ofRotateZ(ofGetElapsedTimef());
    mBlobMesh.draw();
    ofPopMatrix();
    
	ofDisableDepthTest();
	cam.end();
	
    
    
	// draw the framerate in the top left corner
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()) + " fps", 10, 20);
}

//-----
/*
 //indexed rendering done right???
const ofIndexType Faces[] = {
    2, 1, 0,
    3, 2, 0,
    4, 3, 0,
    5, 4, 0,
    1, 5, 0,
    11, 6,  7,
    11, 7,  8,
    11, 8,  9,
    11, 9,  10,
    11, 10, 6,
    1, 2, 6,
    2, 3, 7,
    3, 4, 8,
    4, 5, 9,
    5, 1, 10,
    2,  7, 6,
    3,  8, 7,
    4,  9, 8,
    5, 10, 9,
    1, 6, 10 };
const float Verts[] = {
    0.000f,  0.000f,  1.000f,
    0.894f,  0.000f,  0.447f,
    0.276f,  0.851f,  0.447f,
    -0.724f,  0.526f,  0.447f,
    -0.724f, -0.526f,  0.447f,
    0.276f, -0.851f,  0.447f,
    0.724f,  0.526f, -0.447f,
    -0.276f,  0.851f, -0.447f,
    -0.894f,  0.000f, -0.447f,
    -0.276f, -0.851f, -0.447f,
    0.724f, -0.526f, -0.447f,
    0.000f,  0.000f, -1.000f };
ofVec3f v[12];
ofVec3f n[12];
ofFloatColor c[12];
ofVbo vbo;
void HelloWorldApp::setup()
{
    int i, j = 0;
    for ( i = 0; i < 12; i++ )
    {
        
        c[i].r = ofRandom(1.0);
        c[i].g = ofRandom(1.0);
        c[i].b = ofRandom(1.0);
        
        v[i][0] = Verts[j] * 100.f;
        j++;
        v[i][1] = Verts[j] * 100.f;
        j++;
        v[i][2] = Verts[j] * 100.f;
        j++;
        
    }
    
    vbo.setVertexData( &v[0], 12, GL_STATIC_DRAW );
    vbo.setColorData( &c[0], 12, GL_STATIC_DRAW );
    vbo.setIndexData( &Faces[0], 60, GL_STATIC_DRAW );
    
    glEnable(GL_DEPTH_TEST);
}
void HelloWorldApp::draw(){
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2, 100);
    ofRotate(ofGetElapsedTimef() * 20.0, 1, 1, 0);
    glPointSize(10.f);
    vbo.drawElements( GL_TRIANGLES, 60);
}

*/
//--------------------------------------------------------------
void chbp12App::keyPressed(int key){
	if(key == ' ') {
		ofToggleFullscreen();
	}
}

//--------------------------------------------------------------
void chbp12App::keyReleased(int key){
	
}

//--------------------------------------------------------------
void chbp12App::mouseMoved(int x, int y){
	
}

//--------------------------------------------------------------
void chbp12App::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void chbp12App::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void chbp12App::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void chbp12App::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void chbp12App::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void chbp12App::dragEvent(ofDragInfo dragInfo){ 
	
}
