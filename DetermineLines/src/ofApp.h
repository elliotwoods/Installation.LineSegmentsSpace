#pragma once

#include "ofMain.h"
#include "ofxGraycode.h"
#include "ofxGrabCam.h"
#include "ofxCvGui.h"
#include "ofxTriangulate.h"
#include "ofxPolyFit.h"

#include "Scene.h"
#include "ThreadSet.h"

#define TRIANGULATE_MAX_DISTANCE 0.2f

struct Find {
	float x;
	ofVec2f projectorXY;
	ofVec3f worldXYZ;
	uint32_t cameraIndex;
	uint32_t projectorIndex;
};

class DebugNode : public ofNode {
public:
	void customDraw() {
		this->cameraRay.draw();
		this->projectorRay.draw();
		this->intersectRay.draw();
	}
	ofxRay::Ray cameraRay;
	ofxRay::Ray projectorRay;
	ofxRay::Ray intersectRay;
};

class ofApp : public ofBaseApp{

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
	
	void triangulateLine3D();
	
	ofxCvGui::Builder gui;
	ofxGraycode::Decoder decoder[2];
	ofxGraycode::PayloadGraycode payload;
	Scene scene;
	ThreadSet threads;
	
	ofFbo fboProjectorView;
	
	vector<string> filesDragged;
	
	ofVec2f debugProjectorCoord;
	ofVec2f debugCameraCoord;
	
	DebugNode debugNode;
	
	ofxCvGui::PanelPtr debugCameraPanel;
	ofxCvGui::PanelPtr debugProjectorPanel;
	
	ofImage debugCameraMedian;
	ofImage debugProjectorMedian;
};
