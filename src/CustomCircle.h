#pragma once

#include "ofxBox2d.h"

class CustomCircle : public ofxBox2dCircle {

public:
	CustomCircle(int num);
	void update();
	void draw();
	float getVolume();

	ofSoundPlayer sound;
	float radius;
	float soundSpeed;

	int num;
	ofTrueTypeFont font;
	string word;
	float age;
	int lifeTime;
	bool dead;

};