#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);
	ofBackground(77, 77, 92, 127);
	ofSetCircleResolution(64);
	ofSetFrameRate(30);
	ofSeedRandom();

	//Sound
	contactSound.loadSound("contact.wav");
	contactSound.setLoop(false);

	//Display Size
	scale = 1.0;
	offsetX = offsetY = 0;

	//font
	bgFont.loadFont("DS-DIGI.TTF", 200);
	//box2d 1st world
	box2d.init();
	box2d.enableEvents();
	box2d.setGravity(0, 0);
	box2d.createBounds(0, 0, ofGetWidth(), ofGetHeight());
	box2d.setFPS(30);
	box2d.registerGrabbing();
	ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
	ofAddListener(box2d.contactEndEvents, this, &ofApp::contactEnd);
	idCount = 0;
	isContact = false;
	//box2d 2nd world
	box2d2.init();
	box2d2.enableEvents();
	box2d2.setGravity(0, 0);
	box2d2.createBounds(0, 0, ofGetWidth(), ofGetHeight());
	box2d2.setFPS(30);
	box2d2.registerGrabbing();

	//Create 1st Circle
	ofPtr<CustomCircle> circle = ofPtr<CustomCircle>(new CustomCircle(circles.size()));
	circle.get()->setPhysics(0.5, 1.0, 1.0);//èdÇ≥ÅEîΩî≠óÕÅEñÄéCóÕ
	circle.get()->setup(box2d.getWorld(), ofGetWidth() / 2, ofGetHeight() / 2, 10);
	circle.get()->setData(new SoundData());
	SoundData * sd = (SoundData*)circle.get()->getData();
	if (idCount % 2 == 0 || idCount == 0) {
		sd->sex = 0;
	}
	else {
		sd->sex = 1;
	}
	sd->id = idCount;
	idCount++;
	sd->bHit = false;
	circles.push_back(circle);
	//Create 2nd Circle
	ofPtr<CustomCircle> circle2 = ofPtr<CustomCircle>(new CustomCircle(circles.size()));
	circle2.get()->setPhysics(1.0, 1.5, 5.0);//èdÇ≥ÅEîΩî≠óÕÅEñÄéCóÕ
	circle2.get()->setup(box2d.getWorld(), ofGetWidth() / 2 + 200, ofGetHeight() / 2 + 100, 10);
	circle2.get()->setVelocity(5.0, 5.0);
	circle2.get()->setData(new SoundData());
	SoundData * sd2 = (SoundData*)circle2.get()->getData();
	if (idCount % 2 == 0) {
		sd2->sex = 0;
	}
	else {
		sd2->sex = 1;
	}
	sd2->id = idCount;
	idCount++;
	sd2->bHit = false;
	circles.push_back(circle2);
	//Create Joints
	for (int i = 0; i < circles.size(); i++) {
		ofPtr<CustomJoint> joint = ofPtr<CustomJoint>(new CustomJoint());
		joint.get()->setup(box2d.getWorld(), circles[i].get()->body, circles[circles.size() - 1].get()->body);
		joint.get()->setFrequency(0.4);
		joint.get()->setDamping(0.1);
		joint.get()->setLength(100);
		joints.push_back(joint);
	}

	//Light
	light.setPosition(1000, 1000, 2000);
	//Shader
	shader.init(ofGetWidth(), ofGetHeight());
	shader.createPass<EdgePass>()->setEnabled(false);
	shader.createPass<BloomPass>()->setEnabled(true);
	shader.createPass<GodRaysPass>()->setEnabled(true);

	//kinect
	kinect.init();
	kinect.open();
	isBorn = 0;
	isWorldEnd = 0;
	worldEndCount = 0;
	//Gui
	thresh = 127;
	minRadius = 10;
	maxRadius = 200;
	/*gui.setup();
	gui.add(thresh.setup("thresh", 127, 0, 255));
	gui.add(minRadius.setup("minRad", 10, 0, 400));
	gui.add(maxRadius.setup("maxRad", 200, 0, 400));*/
}

//--------------------------------------------------------------
//Contact Listener
void ofApp::contactStart(ofxBox2dContactArgs &e) {
	contactX = e.b->GetBody()->GetPosition().x;
	contactY = e.b->GetBody()->GetPosition().y;

	float pan = ofMap(e.b->GetBody()->GetPosition().x, 0.0, 25.0, -1.0, 1.0);
	contactSound.setPan(pan);
	SoundData * data = (SoundData*)e.b->GetBody()->GetUserData();
	if (data) {
		float notes[] = { 0.5, 0.75, 1, 1.5, 2.0, 3.0 };
		soundSpeed = notes[((int)ofRandom(0, 5))];
		contactSound.setSpeed(soundSpeed);
		contactSound.setVolume(0.3);
		contactSound.play();
		data->bHit = true;
		cout << "id: " << data->id << endl;
		if (data->sex == 0 && ((int)contactX == 0 || (int)contactX == 42 || (int)contactY == 0 || (int)contactY == 23)) {
			isContact = true;
		}
	}

	//cout << "id: " << data->num << endl;
}

void ofApp::contactEnd(ofxBox2dContactArgs &e) {

	SoundData * data = (SoundData*)e.a->GetBody()->GetUserData();
	if (data) {
		data->bHit = false;
	}

}

//--------------------------------------------------------------
void ofApp::update() {
	circles[0].get()->update();
	circles[1].get()->update();
	cout << "joint size: " << joints.size() << endl;
	//cout << "circle size: " << circles.size() << endl;

	for (int i = 2; i < circles.size(); i++) {
		circles[i].get()->update();
		//ê∂éÄîªíË
		//cout << "dead: " << circles[i].get()->dead << endl;
		if (circles[i].get()->dead == true && joints.size() > 0) {
			for (int j = 2; j < joints.size(); j++) {
				joints[j].get()->destroy();
				//joints.erase(joints.begin() + j);
			}
			circles[i].get()->destroy();
			circles.erase(circles.begin() + i);
		}
	}

	//êîÇ™ëùÇ¶ÇƒÇ´ÇΩÇÁJointÇífÇøêÿÇÈ
	if (circles.size() > 20) {
		for (int i = 2; i < joints.size(); i++) {
			joints[i].get()->destroy();
			if (joints.size() > 500) {
				joints.erase(joints.begin() + i);
			}
			//joints.erase(joints.begin() + i);
		}
		for (int i = 2; i < circles.size(); i++) {
			circles[i].get()->destroy();
			circles.erase(circles.begin() + i);
		}

		for (int i = 2; i < joints.size(); i++) {
			joints[i].get()->destroy();
			if (joints.size() > 500) {
				joints.erase(joints.begin() + i);
			}
			//joints.erase(joints.begin() + i);
		}
		for (int i = 2; i < circles.size(); i++) {
			circles[i].get()->destroy();
			circles.erase(circles.begin() + i);
		}
	}

	box2d.update();

	for (int i = 0; i < circles2.size(); i++) {
		circles2[i].get()->update();
	}
	box2d2.update();

	//ó÷äsåüèoÇÃä¥ìxê›íË
	contourFinder.setMinAreaRadius(minRadius);
	contourFinder.setMaxAreaRadius(maxRadius);
	//Kinect
	kinect.update();
	if (kinect.isFrameNew()) {
		ofPixels depthPix = kinect.getDepthPixels();
		depthPix.mirror(false, true);
		cv::Mat mat = cv::Mat(kinect.height, kinect.width, CV_8UC1, depthPix, 0);
		contourFinder.setThreshold(thresh);
		contourFinder.findContours(mat);
		
		if (contourFinder.size() > 0) {
			isBorn = 1;
		}
		else {
			isBorn = 0;
		}
		/*else if (isWorldEnd = 1) {
			worldEndCount--;
			if (worldEndCount < 0) {
				sceneAlive();
				worldEndCount = 0;
			}
		}*/
	}

	if (isBorn == 1 && isWorldEnd == 0) {
		for (int i = 0; i < circles.size(); i++) {
			circles[i].get()->sound.setSpeed(5.0);
		}
	}
	else if(isWorldEnd == 0){
		for (int i = 0; i < circles.size(); i++) {
			circles[i].get()->sound.setSpeed(circles[i].get()->soundSpeed);
		}
	}

	if (isBorn == 1) {
		if (worldEndCount < 2045) {
			worldEndCount += 5;
		}
	}
	else if (isWorldEnd == 1) {
		worldEndCount -=10;
	}


	if (worldEndCount > 2040) {
		sceneDead();
	}
	if (worldEndCount < 0) {
		sceneAlive();
		worldEndCount = 0;
	}

	cout << "worldEndCount: " << worldEndCount << endl;
	cout << "isBorn: " << isBorn << endl;
	//cout << "isWorldEnd: " << isWorldEnd << endl;
}

//--------------------------------------------------------------
void ofApp::draw() {
	//Display Size
	ofTranslate(offsetX, offsetY);
	ofScale(scale, scale);

	//light.enable();
	shader.begin();

	//Background color
	if (isBorn == 1) {
		//Background color
		float bgColor = circles[0].get()->getVolume();
		bgColor *= 2;
		cout << bgColor << endl;
		ofBackground(77 - bgColor, 77 - bgColor, 92 - bgColor, 127);
	}

	//font
	if (isBorn == 1 || isWorldEnd == 1) {
		bgFont.drawString(ofToString(worldEndCount), ofGetWidth() / 2, ofGetHeight() / 2 - 100);
	}
	//è’ìÀÇ™Ç†Ç¡ÇΩèÍçáÅEéqãüÇëùÇ‚Ç∑
	if (isContact == true) {
		ofSetLineWidth(2);
		//Create Circle
		ofPtr<CustomCircle> circle = ofPtr<CustomCircle>(new CustomCircle(circles.size()));
		if (circle.get()->num % 2 == 0) {
			circle.get()->setPhysics(0.5, 1.0, 1.0);//èdÇ≥ÅEîΩî≠óÕÅEñÄéCóÕ
		}
		else {
			circle.get()->setPhysics(1.0, 1.5, 5.0);//èdÇ≥ÅEîΩî≠óÕÅEñÄéCóÕ
		}
		circle.get()->setup(box2d.getWorld(), ofRandom(0, ofGetWidth()), ofRandom(0, ofGetHeight()), 10);
		circle.get()->setData(new SoundData());
		SoundData * sd = (SoundData*)circle.get()->getData();
		if (idCount % 2 == 0) {
			sd->sex = 0;
		}
		else {
			sd->sex = 1;
		}
		sd->id = idCount;
		idCount++;
		sd->bHit = false;
		circles.push_back(circle);
		//cout << "idCount: " << idCount << endl;

		//Joint
		for (int i = 0; i < circles.size(); i++) {
			ofPtr<CustomJoint> joint = ofPtr<CustomJoint>(new CustomJoint());
			joint.get()->setup(box2d.getWorld(), circles[i].get()->body, circles[circles.size() - 1].get()->body);
			joint.get()->setFrequency(0.4);
			joint.get()->setDamping(0.1);
			joint.get()->setLength(100);
			joints.push_back(joint);
		}
		isContact = false;
	}


	for (int i = 0; i < joints.size(); i++) {
		float soundVolume = circles[0].get()->getVolume();
		joints[i].get()->draw(soundVolume);
	}
	for (int i = 0; i < circles.size(); i++) {
		circles[i].get()->draw();
	}
	//box2d.drawGround();

	for (int i = 0; i < joints2.size(); i++) {
		float soundVolume = circles2[0].get()->getVolume();
		joints2[i].get()->draw(soundVolume);
	}
	for (int i = 0; i < circles2.size(); i++) {
		circles2[i].get()->draw();
	}

	//Draw depth
	ofSetColor(255);
	//kinect.drawDepth(0, 0, ofGetWidth(), ofGetHeight());
	//Draw contour
	ofSetColor(255, 255, 100, 200);
	ofPushMatrix();
	ofScale(ofGetWidth() / float(kinect.width), ofGetHeight() / float(kinect.height));
	contourFinder.draw();
	//ó÷äsÇÃíÜêSÇ…â~Çï`Ç≠
	for (int i = 0; i < contourFinder.size(); i++) {
		cv::Point2f pos = contourFinder.getCentroid(i);
		ofCircle(pos.x, pos.y, 10);
		if (isWorldEnd == 0) {
			for (int i = 0; i < /*circles.size()*/2; i++) {
				ofVec2f vec;
				vec.x = pos.x - circles[i].get()->getPosition().x * float(kinect.width) / ofGetWidth();
				vec.y = pos.y - circles[i].get()->getPosition().y * float(kinect.height) / ofGetHeight();
				circles[i].get()->setVelocity(vec.x / 20.0, vec.y / 20.0);
			}
		}
	}
	ofPopMatrix();
	
	shader.end();

	//Gui
	//gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case 's': //(Debug) Change the scene
		ofBackground(0);
		shader[0]->setEnabled(true);
		shader[1]->setEnabled(false);
		shader[2]->setEnabled(false);
		for (int i = 0; i < circles.size(); i++) {
			circles[i].get()->setVelocity(0.0, 0.0);
		}
		for (int i = 0; i < joints.size(); i++) {
			joints[i].get()->setFrequency(5.0);
			joints[i].get()->setDamping(1.0);
		}
		box2d.setGravity(0.0, 10.0);
		box2d.disableEvents();
		break;
	case 'r': //(Debug) Change the scene
		ofBackground(77, 77, 92, 127);
		shader[0]->setEnabled(false);
		shader[1]->setEnabled(true);
		shader[2]->setEnabled(true);
		for (int i = 0; i < circles.size(); i++) {
			circles[i].get()->setVelocity(5.0*i, 2.0*i);
		}
		for (int i = 0; i < joints.size(); i++) {
			joints[i].get()->setFrequency(0.4);
			joints[i].get()->setDamping(0.1);
		}
		box2d.setGravity(0.0, 0.0);
		box2d.enableEvents();
		break;
	case 'f': //(Debug)Fullscreen
		ofToggleFullscreen();
		scale = MIN(ofGetWidth() / 1024, ofGetHeight() / 768);
		offsetX = (ofGetWidth() - 1024 * scale) / 2;
		offsetY = (ofGetHeight() - 768 * scale) / 2;
		break;
	case ' ': //(Debug)Force
		circles[circles.size() - 1].get()->setPosition(mouseX, mouseY);
		break;
	case 'c': //(Debug)destroy some circles
		for (int i = 2; i < joints.size(); i++) {
			joints[i].get()->destroy();
			if (joints.size() > 500) {
				joints.erase(joints.begin() + i);
			}
			//joints.erase(joints.begin() + i);
		}
		for (int i = 2; i < circles.size(); i++) {
			circles[i].get()->destroy();
			circles.erase(circles.begin() + i);
		}
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	//Create Circle in the 2nd world
	ofPtr<CustomCircle> circle = ofPtr<CustomCircle>(new CustomCircle(circles2.size()));
	circle.get()->setPhysics(1.0, 2.0, 0.5);//èdÇ≥ÅEîΩî≠óÕÅEñÄéCóÕ
	circle.get()->setup(box2d.getWorld(), ofGetWidth() / 2 + 200, ofGetHeight() / 2 + 100, 10);
	/*circle.get()->setData(new SoundData());
	SoundData * sd = (SoundData*)circle.get()->getData();
	if (idCount % 2 == 0) {
	sd->sex = 0;
	}
	else {
	sd->sex = 1;
	}
	sd->id = idCount;
	idCount++;
	sd->bHit = false;*/
	circles2.push_back(circle);
	//Joint
	for (int i = 0; i < circles2.size(); i++) {
		ofPtr<CustomJoint> joint = ofPtr<CustomJoint>(new CustomJoint());
		joint.get()->setup(box2d.getWorld(), circles2[i].get()->body, circles2[circles2.size() - 1].get()->body);
		joint.get()->setFrequency(0.4);
		joint.get()->setDamping(0.01);
		joint.get()->setLength(200);
		joints2.push_back(joint);
	}

	circles2[circles2.size() - 1].get()->setPosition(mouseX, mouseY);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	if (circles.size() > 0) {
		circles[0].get()->setPosition(ofGetWidth() / 2, ofGetHeight() / 2);
	}
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

void ofApp::sceneDead() {
	ofBackground(0);
	shader[0]->setEnabled(true);
	shader[1]->setEnabled(false);
	shader[2]->setEnabled(false);
	contactSound.setPaused(true);
	for (int i = 0; i < circles.size(); i++) {
		circles[i].get()->setVelocity(0.0, 0.0);
		circles[i].get()->sound.setPaused(true);
	}
	for (int i = 0; i < joints.size(); i++) {
		joints[i].get()->setFrequency(5.0);
		joints[i].get()->setDamping(1.0);
	}
	box2d.setGravity(0.0, 10.0);
	box2d.disableEvents();
	isWorldEnd = 1;
}

void ofApp::sceneAlive() {
	ofBackground(77, 77, 92, 127);
	shader[0]->setEnabled(false);
	shader[1]->setEnabled(true);
	shader[2]->setEnabled(true);
	contactSound.setPaused(false);
	for (int i = 0; i < circles.size(); i++) {
		circles[i].get()->setVelocity(5.0*i, 2.0*i);
		circles[i].get()->sound.setPaused(false);
	}
	for (int i = 0; i < joints.size(); i++) {
		joints[i].get()->setFrequency(0.4);
		joints[i].get()->setDamping(0.1);
	}
	box2d.setGravity(0.0, 0.0);
	box2d.enableEvents();
	isWorldEnd = 0;
}