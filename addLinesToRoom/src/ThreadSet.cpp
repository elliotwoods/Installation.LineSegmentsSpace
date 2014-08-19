#include "ThreadSet.h"

//----------
ThreadSet::ThreadSet() {
	this->clearSelection();
	this->clearHover();
	
	this->layers.insert("Layer 0");
}

//----------
void ThreadSet::draw(bool shift, bool shadows) {
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
		auto line = this->lines[index];
		if (this->layers.count(line->layer) > 0) {
			line->draw(SELECTED_SIZE, this->layers[line->layer]->color, ofColor::red);
		}
	}
	
	if (this->lines.count(this->hover) != 0) { //should be ok to discount == -1 case where no hover
		auto line = this->lines[this->hover];
		if (this->layers.count(line->layer) > 0) {
			line->draw(HOVER_SIZE, this->layers[line->layer]->color, ofColor::blue);
		}
	}

}

//----------
void setColorForIndex(Thread::Index index) {
	ofSetColor(index % 256, (index / 256) % 256, (index / (256 * 256)) % 256);
}

//----------
void ThreadSet::updateIndexBuffer(bool shift) {
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
void ThreadSet::add(Thread line) {
	for(auto otherThread : this->lines) {
		auto closestPointOnOldThreadToNewStart = otherThread.second->closestPointOnRayTo(line.s);
		auto closestPointOnOldThreadToNewEnd = otherThread.second->closestPointOnRayTo(line.s + line.t);
		float distanceOldThreadToNewStart = (closestPointOnOldThreadToNewStart - line.s).length();
		float distanceOldThreadToNewEnd = (closestPointOnOldThreadToNewEnd - (line.s + line.t)).length();
		
		if (distanceOldThreadToNewStart < 1e-6) {
			this->splitThreadAt(otherThread.first, closestPointOnOldThreadToNewStart);
			break;
		}
		if (distanceOldThreadToNewEnd < 1e-6) {
			this->splitThreadAt(otherThread.first, closestPointOnOldThreadToNewEnd);
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
	
	auto addThread = ofPtr<Thread>(new Thread());
	*addThread = line;
	
	this->addUndoSnapshot();
	this->lines.insert(pair<Thread::Index, ofPtr<Thread> >(line.index, addThread));
	this->selection.clear();
	this->hover = line.index;
}

//----------
void ThreadSet::toggleHoverToSelection() {
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
void ThreadSet::load(string filename) {
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
void ThreadSet::loadBinary(string filename) {
	ifstream file;
	filename = ofToDataPath(filename, true);
	file.open(filename.c_str(), ios::in | ios::binary);
	int index = 0;
	while(!file.eof()) {
		auto addThread = ofPtr<Thread>(new Thread());
		file.read((char*)addThread.get(), sizeof(Thread) - sizeof(string) - sizeof(Thread::Index)); //offset length by what we had before when using binary
		addThread->layer = "Layer 0";
		if (addThread->getLengthSquared() > 1e-7) { //sanity check
			addThread->index = index++;
			this->lines.insert(pair<Thread::Index, ofPtr<Thread> >(addThread->index, addThread));
		}
	}
	file.close();
}

//----------
void ThreadSet::loadXml(string filename) {
	ofXml xml;
	xml.load(filename);
	this->load(xml);
}

//----------
void ThreadSet::load(ofXml& xml) {
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
					auto newThread = ofPtr<Thread>(new Thread());
					newThread->loadFrom(xml);
					if (this->lines.count(newThread->index) > 0) {
						newThread->index = this->getNextFreeIndex();
					}
					this->lines.insert(pair<Thread::Index, ofPtr<Thread> >(newThread->index, newThread));
					if (this->layers.count(newThread->layer) == 0) {
						this->layers.insert(newThread->layer);
					}
				} while (xml.setToSibling());
				xml.setToParent();
			}
			xml.setToParent();
		}
	}
}

//----------
void ThreadSet::save(string filename) const {
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
ofXml ThreadSet::getStateAsXml() const {
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
void ThreadSet::clear() {
	this->addUndoSnapshot();
	this->lines.clear();
	this->layers.clear();
	this->clearSelection();
	this->clearHover();
	ofNotifyEvent(this->onLayersChange, this->layers, this);
}

//----------
void ThreadSet::clearSelection() {
	this->selection.clear();
}

//----------
void ThreadSet::clearHover() {
	this->hover = -1;
}

//----------
void ThreadSet::deleteSelected() {
	this->addUndoSnapshot();
	for(auto index : this->selection) {
		this->lines.erase(index);
	}
	this->selection.clear();
	this->hover = -1;
}

//----------
int ThreadSet::getCountAll() const {
	return this->lines.size();
}

//----------
int ThreadSet::getCountSelected() const {
	return this->selection.size();
}

//----------
float ThreadSet::getLengthAll() const {
	float total = 0.0f;
	for(auto line : this->lines) {
		total += line.second->getLength();
	}
	return total;
}

//----------
float ThreadSet::getLengthSelected() const {
	float total = 0.0f;
	for(auto index : this->selection) {
		total += this->lines.at(index)->getLength(); //we have to use this syntax for const
	}
	return total;
}

//----------
Thread::Index ThreadSet::getHoverIndex() const {
	return this->hover;
}

//----------
Thread& ThreadSet::getHover() {
	return *this->lines[this->hover];
}

//----------
Thread::Index ThreadSet::getNextFreeIndex() const {
	if (this->lines.size() == 0) {
		return 0;
	} else {
		auto lastThread = this->lines.end();
		lastThread--;
		return lastThread->first + 1;
	}
}

//----------
void ThreadSet::splitThreadAt(Thread::Index index, const ofVec3f& point) {
	this->addUndoSnapshot();
	auto previousThread = *this->lines[index];
	this->lines[index]->t = point - previousThread.s;
	
	auto newThread = ofPtr<Thread>(new Thread());
	newThread->layer = previousThread.layer;
	newThread->s = point;
	newThread->t = previousThread.t + previousThread.s - newThread->s;
	newThread->index = this->getNextFreeIndex();
	this->lines.insert(pair<Thread::Index, ofPtr<Thread> >(newThread->index, newThread));
}

//----------
void ThreadSet::addUndoSnapshot() {
	//!HACK - ofXml seems to have allocation issues
	return;

	ofXml currentState = this->getStateAsXml();
	this->undoStack.push_back(currentState);
	if(this->undoStack.size() > UNDO_STACK_LENGTH) {
		this->undoStack.pop_front();
	}
}

//----------
void ThreadSet::undo() {
	if (this->undoStack.size() > 0) {
		this->clear();
		this->load(this->undoStack.back());
		this->undoStack.pop_back();
	}
}

//----------
int ThreadSet::getUndoStackLength() {
	return this->undoStack.size();
}

//----------
Thread::Index ThreadSet::getIndexAtScreen(int x, int y) {
	x /= PICK_SAMPLE_DOWN;
	y /= PICK_SAMPLE_DOWN;
	
	auto pixelIndex = this->indexBufferPixels.getWidth() * (this->indexBufferPixels.getHeight() - y - 1) + x;
	if (pixelIndex >= 0 && pixelIndex < this->indexBufferPixels.getWidth() * this->indexBufferPixels.getHeight()) {
		if (this->indexBufferPixels[pixelIndex * 4 + 3] == 0) {
			return -1; //no line here
		}
		Thread::Index index = 0;
		index += this->indexBufferPixels[pixelIndex * 4];
		index += this->indexBufferPixels[pixelIndex * 4 + 1] * 256;
		index += this->indexBufferPixels[pixelIndex * 4 + 2] * 256 * 256;
		return index;
	} else {
		return -1; //outside screen
	}
}

//----------
void ThreadSet::addLayer(const string& name) {
	this->layers.insert(name);
	ofNotifyEvent(this->onLayersChange, this->layers, this);
}

//----------
void ThreadSet::deleteLayer(ofPtr<Layer> layer) {
	this->selectThreadsForLayer(layer);
	this->deleteSelected();
	ofNotifyEvent(this->onLayersChange, this->layers, this);
}

//----------
void ThreadSet::renameLayer(ofPtr<Layer> layer, const string& name) {
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
void ThreadSet::selectThreadsForLayer(ofPtr<Layer> layer) {
	this->clearSelection();
	for(auto line : this->lines) {
		if (line.second->layer == layer->name) {
			this->selection.insert(line.first);
		}
	}
}

//----------
void ThreadSet::changeLayerVisibility() {
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