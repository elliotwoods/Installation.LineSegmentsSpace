#include "LineSet.h"

//----------
LineSet::LineSet() {
	this->clearSelection();
	this->clearHover();
}

//----------
void LineSet::draw(bool shift, bool shadows) {
	for(auto line : this->lines) {
		if (shadows) {
			line.second.drawShadow(0.0f);
		}
		line.second.draw(shift ? SHIFT_SIZE : NORMAL_SIZE);
	}
	
	for(auto index : this->selection) {
		this->lines[index].draw(SELECTED_SIZE, ofColor::red);
	}
	
	if (this->lines.count(this->lineHover) != 0) { //should also discount == -1 case where no hover
		this->lines[lineHover].draw(HOVER_SIZE, ofColor::blue);
	}
}

//----------
void LineSet::load(string filename) {
	ofFile file;
	file.open(filename);
	bool isXml = file.peek() == '<';
	file.close();
	
	if (isXml) {
		this->loadXml(filename);
	} else {
		this->loadBinary(filename);
	}
	
	this->clearSelection();
	this->clearHover();
}

//----------
void LineSet::loadBinary(string filename) {
	this->lines.clear();
	
	ifstream file;
	filename = ofToDataPath(filename, true);
	file.open(filename.c_str(), ios::in | ios::binary);
	int index = 0;
	while(!file.eof()) {
		Line addLine;
		file.read((char*)&addLine, sizeof(Line) - sizeof(string) - sizeof(Line::Index)); //offset length by what we had before when using binary
		addLine.layer = "cheil don't pay on time?";
		if (addLine.getLengthSquared() > 1e-7) { //sanity check
			addLine.index = index++;
			this->lines.insert(pair<Line::Index, Line>(addLine.index, addLine));
		}
	}
	file.close();
}

//----------
void LineSet::loadXml(string filename) {
	
	this->lines.clear();
	this->layers.clear();
	
	ofXml xml;
	xml.load(filename);
	if (xml.setTo("Room")) {
		if (xml.setTo("Lines")) {
			if (xml.setTo("Line[0]")) {
				do {
					Line newLine;
					newLine.loadFrom(xml);
					
					this->lines.insert(pair<Line::Index, Line>(newLine.index, newLine));
					if (this->layers.count(newLine.layer) == 0) {
						this->layers.insert(newLine.layer);
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
	ofXml xml;
	
	xml.addChild("Room");
	xml.setTo("Room");
	xml.addChild("Lines");
	xml.setTo("Lines");
	
	for(auto line : this->lines) {
		line.second.saveTo(xml);
	}
	
	xml.save(filename);
	
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
void LineSet::clear() {
	this->lines.clear();
	this->clearSelection();
	this->clearHover();
}

//----------
void LineSet::clearSelection() {
	this->selection.clear();
}

//----------
void LineSet::clearHover() {
	this->lineHover = -1;
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
		total += line.second.getLength();
	}
	return total;
}

//----------
float LineSet::getLengthSelected() const {
	float total = 0.0f;
	for(auto index : this->selection) {
		total += this->lines.at(index).getLength(); //we have to use this syntax for const
	}
	return total;
}

//----------
Line::Index LineSet::findIndexUnderCursor() {
	int desiredWidth = ofGetWidth() / 2.0f;
	int desiredHeight = ofGetHeight() / 2.0f;
	if (this->indexBuffer.getWidth() != desiredWidth || this->indexBuffer.getHeight() != desiredHeight) {
		this->indexBuffer.allocate(desiredWidth, desiredHeight, GL_RGBA32F); //wasteful, but compatible
	}
}