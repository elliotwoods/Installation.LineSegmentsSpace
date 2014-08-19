#pragma once

#include "ofMain.h"

class Layer {
public:
	Layer();
	void loadFrom(ofXml&);
	void saveTo(ofXml&);
	ofXml getXml();
	
	string name;
	bool visible;
	bool selected;
	ofFloatColor color;
};
