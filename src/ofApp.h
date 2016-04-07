#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "CustomCircle.h"
#include "CustomJoint.h"
#include "ofxPostProcessing.h"
#include "ofxKinect.h"
#include "ofxGui.h"
#include "ofxCv.h"

class SoundData {
public:
	int	 id;
	int sex;
	bool bHit;
};

class ofApp : public ofBaseApp {

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
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void sceneDead();
	void sceneAlive();

	//Sound
	ofSoundPlayer contactSound;
	float soundSpeed;

	//Box2d 1st world
	ofxBox2d box2d;
	vector<ofPtr<CustomCircle>> circles;
	vector<ofPtr<CustomJoint>> joints;
	//Box2d 2nd world
	ofxBox2d box2d2;
	vector<ofPtr<CustomCircle>> circles2;
	vector<ofPtr<CustomJoint>> joints2;
	//Contact Listener
	void contactStart(ofxBox2dContactArgs &e);
	void contactEnd(ofxBox2dContactArgs &e);
	float contactX;
	float contactY;
	int idCount;
	bool isContact;

	//Display Size
	float scale, offsetX, offsetY;
	//Light
	ofLight light;
	//Shader
	ofxPostProcessing shader;

	//Kinect
	ofxKinect kinect;
	ofImage kinectImg;
	ofImage depthImg;
	ofxCv::ContourFinder contourFinder;
	int worldEndCount;
	bool isBorn;
	//bool isInvade;
	bool isWorldEnd;

	//font
	ofTrueTypeFont bgFont;
	//Gui
	float thresh, minRadius, maxRadius;
	/*ofxPanel gui;
	ofxFloatSlider thresh;
	ofxFloatSlider minRadius;
	ofxFloatSlider maxRadius;*/
};
