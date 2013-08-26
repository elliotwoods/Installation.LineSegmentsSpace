#pragma once

#include "ofMain.h"

#include "ofxGrabCam.h"
#include "ofxRay.h"
#include "ofxUi.h"

#include "LineSet.h"
#include "Line.h"

class ofApp : public ofBaseApp{
	enum State {
		Waiting,
		Adding
	};
	
public:
	void setup();
	void setupRoom();
	void update();
	void draw();
	void drawScene();
	void drawCursor();
	void drawInstructions();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofVec3f getPositionSnapped();
	
	ofxGrabCam camera;
	
	ofVec3f cursor;
	ofVec2f mousePositionAtStartOfDown;
	LineSet lineSet;
	Line newLine;
	
	ofBoxPrimitive room;
	
	ofLight light;
	bool shadow;
	bool grid;
	
	State state;
	bool shift;
	
	ofFbo zoomed;
	
	ofxUICanvas layers;
};
