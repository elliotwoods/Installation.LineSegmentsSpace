#pragma once

#include "ofMain.h"
#include "ofxGrabCam.h"
#include "ofxRay.h"

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
	
	vector<Line>::iterator getPickLine();
	void addPickLineToSelection();
	void findLineHover();
	ofVec3f getPositionSnapped();
	
	//this function doesn't exist
	void addIndices(int index, ...);

	ofxGrabCam camera;
	
	LineSet lineSet;
	Line newLine;
	
	set<string> layers; //cached list of layers
	
	ofBoxPrimitive room;
	
	ofLight light;
	bool shadow;
	bool grid;
	
	State state;
	bool shift;
	
	ofFbo zoomed;
};
