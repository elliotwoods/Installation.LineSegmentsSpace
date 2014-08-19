#pragma once

#include "ofMain.h"
#include "ofxGrabCam.h"


class ofApp : public ofBaseApp{
	class Line {
	public:
		Line();
		void draw();
		ofVec3f start;
		ofVec3f end;
	};
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofxGrabCam camera;
	vector<Line> lines;
	ofFbo fbo;
};
