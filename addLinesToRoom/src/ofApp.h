#pragma once

#include "ofMain.h"

#include "ofxGrabCam.h"
#include "ofxRay.h"
#include "ofxUi.h"

#include "ThreadSet.h"
#include "Thread.h"

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

	void layersChanged(LayerSet&);
	void rebuildLayersGui();
	void layerGuiEvent(ofxUIEventArgs&);
	
	ofxGrabCam camera;
	
	ofVec3f cameraCursorCached;
	ofVec3f cursor;
	ofVec2f mousePositionAtStartOfDown;
	ThreadSet lineSet;
	Thread newLine;
	
	ofBoxPrimitive room;
	ofLight light;
	
	bool shadow;
	bool grid;
	
	State state;
	bool shift;
	
	ofFbo zoomed;
	
	bool layerGuiRebuildFlag;
	ofxUICanvas layerGui;
	bool layerGuiAdd, layerGuiDelete, layerGuiRename;
	bool cursorInLayerGui;
};
