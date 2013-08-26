#pragma once
#include "ofxRay.h"

class Line : public ofxRay::Ray {
public:
	typedef int Index;
	void draw(float edgeThickness, ofColor center = ofColor::white) const;
	void drawShadow(float floorHeight = 0.0f) const;

	void loadFrom(ofXml&);
	void saveTo(ofXml&);
	ofXml getXml();
	
	string layer;
	Index index;
};