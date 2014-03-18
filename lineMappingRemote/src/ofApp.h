#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

struct Line {
	ofVec2f start;
	ofVec2f end;
	int iProjector;
	bool changed;
};

enum Status {
	Waiting,
	Dragging
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
	void drawLineSet(const ofRectangle&, bool zoom);
	
	ofVec2f normToScreen(const ofVec2f&);
	ofVec2f screenToNorm(const ofVec2f&);
	
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
	void sendSelection();
	void sendSelectedIndex();
	void updateLines();
	
	void next();
	void back();
	
	ofRectangle mainArea;
	ofRectangle zoomedArea;
	ofRectangle projectorButton[2];
	ofRectangle newLineButton;
	ofRectangle backButton;
	ofRectangle nextButton;
	
	int iProjector;
	float buttonHeight;
	float buttonWidth;
	string serverIP;
	
	map<int, Line> lines;
	
	int selection;
	Status status;
	ofVec2f lastMouse;
	int endSelected;
	bool shift;
	bool ctrl;
	
	ofxOscSender osc;
	ofThread sleeper;
};
