 #include "Line.h"

//---------
ofVec3f random3uf() {
	return ofVec3f(ofRandomuf(), ofRandomuf(), ofRandomuf());
}

//---------
void Line::draw(float edgeThickness, ofColor center) const {
	const ofVec3f start = this->s;
	const ofVec3f end = this->s + this->t;
	
	ofPushStyle();
	
	ofSetLineWidth(edgeThickness);
	ofSetColor(0);
	ofLine(start, end);
	
	ofSetLineWidth(1.0f);
	ofSetColor(center);
	ofLine(start, end);
	
	ofPopStyle();
}

//---------
void Line::drawShadow(float floorHeight) const {
	ofPushStyle();
	ofVec3f start = this->s;
	ofVec3f end = this->t + start;
	
	start.y = floorHeight;
	end.y = floorHeight;
	
	ofSetColor(20, 20, 20, 100);
	ofLine(start, end);
	ofPopStyle();
}

//---------
void Line::loadFrom(ofXml& xml) {
	this->s = xml.getValue<ofVec3f>("s");
	this->t = xml.getValue<ofVec3f>("t");
	this->layer = xml.getValue("layer");
	this->index = xml.getValue<int>("index");
}

//---------
void Line::saveTo(ofXml& xml) {
	auto addXml = this->getXml();
	xml.addXml(addXml);
}

//---------
ofXml Line::getXml() {
	ofXml xml;
	xml.addChild("Line");
	xml.setTo("Line");
	xml.addValue("s", this->s);
	xml.addValue("t", this->t);
	xml.addValue("layer", this->layer);
	xml.addValue("index", (int) this->index);
	return xml;
}