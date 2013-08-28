#pragma once
#include "ofxRay.h"

class Thread : public ofxRay::Ray {
public:
	typedef int Index;
	void draw(float edgeThickness, ofColor center = ofColor::white, ofColor border = ofColor::black) const;
	void drawShadow(float floorHeight = 0.0f) const;

	void loadFrom(ofXml&);
	void saveTo(ofXml&);
	ofXml getXml();
	
	string layer;
	Index index;
};