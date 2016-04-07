#include "CustomJoint.h"

void CustomJoint::draw(float _soundVolueme) {
	if (!alive) return;

	lineWidth = _soundVolueme;
	//Get the anchor point
	b2Vec2 p1 = joint->GetAnchorA();
	b2Vec2 p2 = joint->GetAnchorB();
	p1 *= OFX_BOX2D_SCALE;
	p2 *= OFX_BOX2D_SCALE;

	float dist = ofDist(p1.x, p1.y, p2.x, p2.y);
	ofEnableSmoothing();
	ofNoFill();
	//ofSetLineWidth(dist / 100.0);
	ofSetLineWidth(lineWidth/5.0);
	ofSetColor(255, 255, 255, 255 - (dist / 10.0));
	ofLine(p1.x, p1.y, p2.x, p2.y);
	ofDisableSmoothing();
}