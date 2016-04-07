#pragma once

#include "ofxBox2d.h"

class CustomJoint : public ofxBox2dJoint {

public:
	void draw(float _soundVolume);
	float lineWidth;
};

