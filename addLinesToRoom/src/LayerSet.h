#pragma once

#include "ofMain.h"

#include "Layer.h"

class LayerSet : public map<string, ofPtr<Layer> > {
public:
	void insert(const string&);
	void insert(ofPtr<Layer>);
	void setSelection(const string&);
	
	bool getHasSelection() const;
	ofPtr<Layer> getSelection();
	string checkUniqueName(const string&);
};