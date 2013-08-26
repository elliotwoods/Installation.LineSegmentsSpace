#include "LayerSet.h"

//----------
void LayerSet::insert(const string& name) {
	Layer layer;
	layer.name = name;
	layer.name = this->checkUniqueName(layer.name);
	auto layerPtr = ofPtr<Layer>(new Layer(layer));
	map<string, ofPtr<Layer> >::insert(pair<string, ofPtr<Layer> >(name,layerPtr));
	this->setSelection(name);
}

//----------
void LayerSet::insert(ofPtr<Layer> layer) {
	layer->name = this->checkUniqueName(layer->name);
	map<string, ofPtr<Layer> >::insert(pair<string, ofPtr<Layer> >(layer->name,layer));
	this->setSelection(layer->name);
}

//----------
void LayerSet::setSelection(const string& name) {
	if (this->count(name) > 0) {
		for(auto layer : *this) {
			layer.second->selected = layer.first == name;
		}
	}
}

//----------
bool LayerSet::getHasSelection() const {
	for(auto layer : *this) {
		if (layer.second->selected) {
			return true;
		}
	}
	return false;
}

//----------
ofPtr<Layer> LayerSet::getSelection() {
	if (this->size() == 0) {
		ofLogError() << "Fatal, returned blank selection";
		return ofPtr<Layer>();
	}
	
	string selection;
	bool found = false;
	for(auto layer : *this) {
		if (layer.second->selected) {
			if (!found) {
				selection = layer.first;
				found = true;
			} else {
				layer.second->selected = false;
			}
		}
	}
	if(found == false) {
		this->begin()->second->selected = true;
		return (*this)[selection];
	}
	
	return (*this)[selection];
}

//----------
string LayerSet::checkUniqueName(const string& name) {
	if (this->count(name) == 0) {
		return name;
	} else {
		string oldName = name;
		string newName;
		int i=0;
		do {
			newName = oldName + ofToString(i++);
		} while (this->count(newName) > 0);
		return newName;
	}
}