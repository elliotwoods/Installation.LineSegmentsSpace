#include "LineSet.h"

//----------
LineSet::LineSet() {
	this->clearSelection();
	this->clearHover();
	
	this->layers.insert("Layer 0");
}

//----------
void LineSet::draw(bool shift, bool shadows) {
	ofPushStyle();
	for(auto line : this->lines) {
		auto layer = this->layers[line.second->layer];
		if (layer->visible) {
			if (shadows) {
				line.second->drawShadow(0.0f);
			}
			line.second->draw(shift ? SHIFT_SIZE : NORMAL_SIZE, layer->color);
		}
	}
	ofPopStyle();
	
	for(auto index : this->selection) {
		this->lines[index]->draw(SELECTED_SIZE, ofColor::red);
	}
	
	if (this->lines.count(this->hover) != 0) { //should also discount == -1 case where no hover
		this->lines[hover]->draw(HOVER_SIZE, ofColor::blue);
	}
}

//----------
void setColorForIndex(Line::Index index) {
	ofSetColor(index % 256, (index / 256) % 256, (index / (256 * 256)) % 256);
}

//----------
void LineSet::updateIndexBuffer(bool shift) {
	int desiredWidth = ofGetWidth() / PICK_SAMPLE_DOWN;
	int desiredHeight = ofGetHeight() / PICK_SAMPLE_DOWN;
	if (this->indexBufferFbo.getWidth() != desiredWidth || this->indexBufferFbo.getHeight() != desiredHeight) {
		this->indexBufferFbo.allocate(desiredWidth, desiredHeight, GL_RGBA);
	}
	
	this->indexBufferFbo.bind();
	ofPushView();
	ofViewport(ofRectangle(0,0,desiredWidth, desiredHeight));
	ofClear(-1, 0, 0);
	
	ofPushStyle();
	ofSetLineWidth(float(shift ? SHIFT_SIZE : NORMAL_SIZE) / float(PICK_SAMPLE_DOWN));
	for (auto line : this->lines) {
		if (this->layers[line.second->layer]->visible) {
			auto index = line.first;
			setColorForIndex(index);
			ofLine(line.second->s, line.second->s + line.second->t);
		}
	}
	ofSetLineWidth(float(HOVER_SIZE) / float(PICK_SAMPLE_DOWN));
	if (this->lines.count(this->hover) > 0) {
		setColorForIndex(hover);
		auto line = this->lines[this->hover];
		ofLine(line->s, line->s + line->t);
	}
	ofSetLineWidth(float(SELECTED_SIZE) / float(PICK_SAMPLE_DOWN));
	for (auto index : this->selection) {
		setColorForIndex(index);
		auto line = this->lines[index];
		ofLine(line->s, line->s + line->t);
	}
	ofPopStyle();
	ofPopView();
	this->indexBufferFbo.unbind();
	
	this->indexBufferFbo.readToPixels(this->indexBufferPixels);
	this->hover = this->getIndexAtScreen(ofGetMouseX(), ofGetMouseY());
}

//----------
void LineSet::add(Line line) {
	for(auto otherLine : this->lines) {
		auto closestPointOnOldLineToNewStart = otherLine.second->closestPointOnRayTo(line.s);
		auto closestPointOnOldLineToNewEnd = otherLine.second->closestPointOnRayTo(line.s + line.t);
		float distanceOldLineToNewStart = (closestPointOnOldLineToNewStart - line.s).length();
		float distanceOldLineToNewEnd = (closestPointOnOldLineToNewEnd - (line.s + line.t)).length();
		
		if (distanceOldLineToNewStart < 1e-6) {
			this->splitLineAt(otherLine.first, closestPointOnOldLineToNewStart);
			break;
		}
		if (distanceOldLineToNewEnd < 1e-6) {
			this->splitLineAt(otherLine.first, closestPointOnOldLineToNewEnd);
			break;
		}
	}
	line.index = this->getNextFreeIndex();
	
	if (this->layers.size() == 0) {
		line.layer = "Layer 0";
		this->layers.insert(line.layer);
		ofNotifyEvent(this->onLayersChange, this->layers, this);
	} else {
		line.layer = this->layers.getSelection()->name;
	}
	
	auto addLine = ofPtr<Line>(new Line());
	*addLine = line;
	
	this->addUndoSnapshot();
	this->lines.insert(pair<Line::Index, ofPtr<Line> >(line.index, addLine));
	this->selection.clear();
	this->hover = line.index;
}

//----------
void LineSet::toggleHoverToSelection() {
	if (this->lines.count(this->hover) > 0) {
		if (this->selection.count(this->hover) == 0) {
			this->selection.insert(this->hover);
		} else {
			this->selection.erase(this->hover);
		}
	}
	for(auto index : selection) {
		this->layers.setSelection(this->lines[index]->layer);
	}
}

//----------
void LineSet::load(string filename) {
	this->addUndoSnapshot();
	ofFile file;
	file.open(filename);
	bool isXml = file.peek() == '<';
	file.close();
	
	this->clear();
	
	if (isXml) {
		this->loadXml(filename);
	} else {
		this->loadBinary(filename);
	}
	
	this->clearSelection();
	this->clearHover();
	ofNotifyEvent(this->onLayersChange, this->layers, this);
}

//----------
void LineSet::loadBinary(string filename) {
	ifstream file;
	filename = ofToDataPath(filename, true);
	file.open(filename.c_str(), ios::in | ios::binary);
	int index = 0;
	while(!file.eof()) {
		auto addLine = ofPtr<Line>(new Line());
		file.read((char*)addLine.get(), sizeof(Line) - sizeof(string) - sizeof(Line::Index)); //offset length by what we had before when using binary
		addLine->layer = "Layer 0";
		if (addLine->getLengthSquared() > 1e-7) { //sanity check
			addLine->index = index++;
			this->lines.insert(pair<Line::Index, ofPtr<Line> >(addLine->index, addLine));
		}
	}
	file.close();
}

//----------
void LineSet::loadXml(string filename) {
	ofXml xml;
	xml.load(filename);
	this->load(xml);
}

//----------
void LineSet::load(ofXml& xml) {
	if (xml.setTo("Room")) {
		this->undoStack.clear();
		if (xml.setTo("Layers")) {
			if (xml.setTo("Layer[0]")) {
				do {
					auto newLayer = ofPtr<Layer>(new Layer());
					newLayer->loadFrom(xml);
					this->layers.insert(newLayer);
				} while (xml.setToSibling());
				xml.setToParent();
			}
			xml.setToParent();
		}
		if (xml.setTo("Lines")) {
			if (xml.setTo("Line[0]")) {
				do {
					auto newLine = ofPtr<Line>(new Line());
					newLine->loadFrom(xml);
					if (this->lines.count(newLine->index) > 0) {
						newLine->index = this->getNextFreeIndex();
					}
					this->lines.insert(pair<Line::Index, ofPtr<Line> >(newLine->index, newLine));
					if (this->layers.count(newLine->layer) == 0) {
						this->layers.insert(newLine->layer);
					}
				} while (xml.setToSibling());
				xml.setToParent();
			}
			xml.setToParent();
		}
	}
}

//----------
void LineSet::save(string filename) const {
	this->getStateAsXml().save(filename);
	
	/*
	 old code doesn't work anyway since we've added a string to the struct (layer)
	 
	 ofstream file;
	 filename = ofToDataPath(filename, true);
	 file.open(filename.c_str(), ios::out | ios::binary);
	 for(auto & line : this->lines) {
	 file.write((char*)&line, sizeof(line));
	 }
	 file.close();
	 */
}

//----------
ofXml LineSet::getStateAsXml() const {
	ofXml xml;
	
	xml.addChild("Room");
	xml.setTo("Room");
	
	//--
	//
	xml.addChild("Lines");
	xml.setTo("Lines");
	
	for(auto line : this->lines) {
		line.second->saveTo(xml);
	}
	
	xml.setToParent();
	//
	//--
	
	//--
	//
	xml.addChild("Layers");
	xml.setTo("Layers");
	
	for(auto layer : this->layers) {
		layer.second->saveTo(xml);
	}
	xml.setToParent();
	//
	//--
	
	xml.setToParent();
	
	return xml;
}

//----------
void LineSet::clear() {
	this->addUndoSnapshot();
	this->lines.clear();
	this->layers.clear();
	this->clearSelection();
	this->clearHover();
	ofNotifyEvent(this->onLayersChange, this->layers, this);
}

//----------
void LineSet::clearSelection() {
	this->selection.clear();
}

//----------
void LineSet::clearHover() {
	this->hover = -1;
}

//----------
void LineSet::deleteSelected() {
	this->addUndoSnapshot();
	for(auto index : this->selection) {
		this->lines.erase(index);
	}
}

//----------
int LineSet::getCountAll() const {
	return this->lines.size();
}

//----------
int LineSet::getCountSelected() const {
	return this->selection.size();
}

//----------
float LineSet::getLengthAll() const {
	float total = 0.0f;
	for(auto line : this->lines) {
		total += line.second->getLength();
	}
	return total;
}

//----------
float LineSet::getLengthSelected() const {
	float total = 0.0f;
	for(auto index : this->selection) {
		total += this->lines.at(index)->getLength(); //we have to use this syntax for const
	}
	return total;
}

//----------
Line::Index LineSet::getHoverIndex() const {
	return this->hover;
}

//----------
Line& LineSet::getHover() {
	return *this->lines[this->hover];
}

//----------
Line::Index LineSet::getNextFreeIndex() const {
	if (this->lines.size() == 0) {
		return 0;
	} else {
		auto lastLine = this->lines.end();
		lastLine--;
		return lastLine->first + 1;
	}
}

//----------
void LineSet::splitLineAt(Line::Index index, const ofVec3f& point) {
	this->addUndoSnapshot();
	auto previousLine = this->lines[index];
	this->lines[index]->t = point - previousLine->s;
	
	auto newLine = ofPtr<Line>(new Line());
	newLine->layer = previousLine->layer;
	newLine->s = point;
	newLine->t = previousLine->t + previousLine->s - newLine->s;
	newLine->index = this->getNextFreeIndex();
	this->lines.insert(pair<Line::Index, ofPtr<Line> >(newLine->index, newLine));
}

//----------
void LineSet::addUndoSnapshot() {
	ofXml currentState = this->getStateAsXml();
	this->undoStack.push_back(currentState);
	if(this->undoStack.size() > UNDO_STACK_LENGTH) {
		this->undoStack.pop_front();
	}
}

//----------
void LineSet::undo() {
	if (this->undoStack.size() > 0) {
		this->clear();
		this->load(this->undoStack.back());
		this->undoStack.pop_back();
	}
}

//----------
int LineSet::getUndoStackLength() {
	return this->undoStack.size();
}

//----------
Line::Index LineSet::getIndexAtScreen(int x, int y) {
	x /= PICK_SAMPLE_DOWN;
	y /= PICK_SAMPLE_DOWN;
	
	auto pixelIndex = this->indexBufferPixels.getWidth() * (this->indexBufferPixels.getHeight() - y - 1) + x;
	if (pixelIndex >= 0 && pixelIndex < this->indexBufferPixels.getWidth() * this->indexBufferPixels.getHeight()) {
		if (this->indexBufferPixels[pixelIndex * 4 + 3] == 0) {
			return -1; //no line here
		}
		Line::Index index = 0;
		index += this->indexBufferPixels[pixelIndex * 4];
		index += this->indexBufferPixels[pixelIndex * 4 + 1] * 256;
		index += this->indexBufferPixels[pixelIndex * 4 + 2] * 256 * 256;
		return index;
	} else {
		return -1; //outside screen
	}
}

//----------
void LineSet::addLayer(const string& name) {
	this->layers.insert(name);
	ofNotifyEvent(this->onLayersChange, this->layers, this);
}

//----------
void LineSet::deleteLayer(ofPtr<Layer> layer) {
	this->selectLinesForLayer(layer);
	this->deleteSelected();
	ofNotifyEvent(this->onLayersChange, this->layers, this);
}

//----------
void LineSet::renameLayer(ofPtr<Layer> layer, const string& name) {
	auto tempLayer = ofPtr<Layer>(new Layer());
	*tempLayer = *this->layers[layer->name];
	
	auto oldName = layer->name;
	this->layers.erase(layer->name);
	
	auto newName = this->layers.checkUniqueName(name);
	
	tempLayer->name = newName;
	this->layers.insert(tempLayer);
	
	for(auto line : this->lines) {
		if (line.second->layer == oldName) {
			line.second->layer = newName;
		}
	}
	
	ofNotifyEvent(this->onLayersChange, this->layers, this);
}

//----------
void LineSet::selectLinesForLayer(ofPtr<Layer> layer) {
	this->clearSelection();
	for(auto line : this->lines) {
		if (line.second->layer == layer->name) {
			this->selection.insert(line.first);
		}
	}
}

//----------
void LineSet::changeLayerVisibility() {
	//clear any invisible elements from selection
	vector<int> toRemoveFromSelection;
	for(auto index : this->selection) {
		auto & line = this->lines[index];
		if (!this->layers[line->layer]->visible) {
			toRemoveFromSelection.push_back(index);
		}
	}
	for(auto index : toRemoveFromSelection) {
		this->selection.erase(index);
	}
	
	//clear hover if invisible
	if (this->hover != -1) {
		if (!this->layers[this->getHover().layer]->visible) {
			this->hover = -1;
		}
	}
}