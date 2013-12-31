#pragma once

#include "ofMain.h"
#include "BlobMesh.h"

class chbp12App : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    // helper functions
    void addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c);
    void addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d);
    void addTexCoords(ofMesh& mesh, ofVec2f a, ofVec2f b, ofVec2f c);
    void addTexCoords(ofMesh& mesh, ofVec2f a, ofVec2f b, ofVec2f c, ofVec2f d);
    ofVec3f getVertexFromImg(ofImage& img, int x, int y);
    
    //my stuff
    void buildSphere();
    
    ofEasyCam cam;
    ofMesh mesh;
    ofVboMesh vboMesh;
    ofVbo mReflectiveSphereVbo;
    ofShader mCubeMapper;
    BlobMesh mBlobMesh;
    ofImage img;
};
