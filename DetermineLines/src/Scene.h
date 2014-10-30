#pragma once

#include "ofMain.h"
#include "ofxRay.h"
#include "ThreadSet.h"

class Scene : public ofNode {
public:
public:
	Scene();
	void init();
	void updateViews();
	
	ofxRay::Camera camera;
	ofxRay::Projector projector0;
	ofxRay::Projector projector1;
	ofxRay::Projector projector2;
	
	ofVboMesh points0;
	ofVboMesh points1;
	ofVboMesh points2;
	
	ofFbo viewCamera;
	ofFbo viewProjector0;
	ofFbo viewProjector1;
	ofFbo viewProjector2;
	
	ofImage imageCamera0;
	ofImage imageCamera1;
	ofImage imageCamera2;
	ofImage imageProjector0;
	ofImage imageProjector1;
	ofImage imageProjector2;
	
	map<uint32_t, uint32_t> activePixelsMapProjectorToCamera0;
	map<uint32_t, uint32_t> activePixelsMapProjectorToCamera1;
	map<uint32_t, uint32_t> activePixelsMapProjectorToCamera2;
	
	void setThreadSet(ThreadSet&);
protected:
	void drawSceneTo(ofProjector&, ofFbo&);
	void drawPoints();
	void customDraw();
	ofBoxPrimitive room;
	ofLight light;
	bool grid;
	ThreadSet* threadSet;
};