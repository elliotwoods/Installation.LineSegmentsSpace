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
	
	ofVboMesh points0;
	ofVboMesh points1;
	
	ofFbo viewCamera;
	ofFbo viewProjector0;
	ofFbo viewProjector1;
	
	ofImage imageCamera0;
	ofImage imageCamera1;
	ofImage imageProjector0;
	ofImage imageProjector1;
	
	map<uint32_t, uint32_t> activePixelsMapProjectorToCamera0;
	map<uint32_t, uint32_t> activePixelsMapProjectorToCamera1;
	
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