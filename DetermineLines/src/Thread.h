#pragma once
#include "ofxRay.h"

class Thread : public ofxRay::Ray {
public:
	typedef int Index;

	Thread();
	void drawWorldSpace(float edgeThickness, ofColor center = ofColor::white, ofColor border = ofColor::black) const;
	void drawProjectionSpace() const;
	void drawShadow(float floorHeight = 0.0f) const;

	void saveTo(ofXml&);
	ofXml getXml();
	
	ofVec2f projectorStart;
	ofVec2f projectorEnd;
	
	int projector;
	int ID;
};