 #include "Thread.h"

//---------
ofVec3f random3uf() {
	return ofVec3f(ofRandomuf(), ofRandomuf(), ofRandomuf());
}

//---------
Thread::Thread() {
	this->projector = 0;
}

//---------
void Thread::drawWorldSpace(float edgeThickness, ofColor center, ofColor border) const {
	const ofVec3f start = this->s;
	const ofVec3f end = this->s + this->t;
	
	ofPushStyle();
	
	ofSetLineWidth(edgeThickness);
	ofSetColor(border);
	ofLine(start, end);
	
	ofSetLineWidth(1.0f);
	ofSetColor(255, 0, 0);
	ofLine(start, end);
	
	ofPopStyle();
}

//---------
void Thread::drawProjectionSpace() const {
	ofPushStyle();
	ofSetLineWidth(3.0f);
	ofSetColor(0);
	ofLine(projectorStart,projectorEnd);
	ofSetColor(255);
	ofSetLineWidth(1.0f);
	ofLine(projectorStart,projectorEnd);
	ofPopStyle();
}

//---------
void Thread::drawShadow(float floorHeight) const {
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
void Thread::saveTo(ofXml& xml) {
	auto addXml = this->getXml();
	xml.addXml(addXml);
}

//---------
ofXml Thread::getXml() {
	ofXml xml;
	xml.addChild("Line");
	xml.setTo("Line");
	xml.addValue("s", this->s);
	xml.addValue("t", this->t);
	xml.addValue("iProjector", this->projector);
	xml.addValue("ID", this->ID);
	xml.addValue("projectorStart", this->projectorStart);
	xml.addValue("projectorEnd", this->projectorEnd);
	return xml;
}