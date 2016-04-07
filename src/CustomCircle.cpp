#include "CustomCircle.h"

CustomCircle::CustomCircle(int _num) {
	num = _num;
	radius = 0;
	age = 0;
	font.loadFont("arial.ttf", 20);
	//char words[] = { 'H', 'e', 'l', 'o', 'W', 'r', 'd' };
	if (num == 0) {
		word = "-";
	}
	else if (num == 1) {
		word = "+";
	}
	else {
		word = " ";
		//word = words[((int)ofRandom(0, 6))];
	}
	if (num % 2 == 0) {
		lifeTime = 30 * 5; //30FPS * 30sec
	}
	else {
		lifeTime = 30 * 10; //30FPS * 60sec
	}
	dead = false;

	if (num > 0) {
		//sound notes
		float notes[] = { 0.5, 0.75, 1, 1.5, 2.0, 3.0 };
		soundSpeed = notes[((int)ofRandom(0, 5))];
		sound.loadSound("ambientSound.wav");
		sound.setLoop(true);
		sound.setSpeed(soundSpeed);
		//Change the volume by the length of the nodes
		float dist = ofDist(getPosition().x, getPosition().y, ofGetWidth() / 2, ofGetHeight() / 2);
		float volume = (1.0 - dist / 300.0) * 1.0;
		if (volume < 0) {
			volume = 0;
		}
		//Play
		sound.setVolume(volume);
		sound.play();
	}
}

void CustomCircle::update() {
	if (num == 0) {
		float *vol = ofSoundGetSpectrum(1);
		radius = vol[0] * 2000;
	}
	else {
		radius = sin(sound.getPosition()*TWO_PI * 20) * 20 + 5;

		//Change the volume & pan by the length of the nodes
		float dist = ofDist(getPosition().x, getPosition().y, ofGetWidth() / 2, ofGetHeight() / 2);
		float volume = (1.0 - dist / 300.0) * 0.5;
		float pan = getPosition().x / (float)ofGetWidth();
		if (volume < 0) {
			volume = 0;
		}
		//Update sound
		sound.setVolume(volume);
		sound.setPan(pan);

		//Î‚ð‚Æ‚é
		if (age > lifeTime) {
			dead = true;
		}
		age++;
	}
}

void CustomCircle::draw() {
	//Outround
	ofFill();
	if (num % 2 == 0) { //red
		ofSetColor(255, 150, 203);
	}
	else { //blue
		ofSetColor(150, 255, 255);
	}
	ofCircle(getPosition().x, getPosition().y, radius);

	//Inrond
	if (num % 2 == 0) { //red
		ofSetColor(255, 150, 203);
	}
	else { //blue
		ofSetColor(150, 255, 255);
	}
	ofCircle(getPosition().x, getPosition().y, 5);
	font.drawString(word, getPosition().x + 10, getPosition().y + 5);
	//ofDrawBitmapString(word, getPosition().x + 10, getPosition().y + 5);
}

float CustomCircle::getVolume() {
	return radius;
}
