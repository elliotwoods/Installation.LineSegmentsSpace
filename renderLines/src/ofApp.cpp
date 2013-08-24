#include "ofApp.h"

const ofVec3f roomMin(-2.0f, 0.0f, 0.0f);
const ofVec3f roomMax(2.0f, 3.0f, 7.0f);
const ofVec3f roomScale = roomMax - roomMin;
const ofVec3f roomCenter = (roomMin + roomMax) / 2.0f;

//---------
ofVec3f random3uf() {
	return ofVec3f(ofRandomuf(), ofRandomuf(), ofRandomuf());
}

//---------
ofApp::Line::Line() {
	//create a random line
	this->start = random3uf() * roomScale + roomMin;
	this->end = random3uf() * roomScale + roomMin;
}

//---------
void ofApp::Line::draw() {
	ofPushStyle();
	
	ofSetLineWidth(10.0f);
	ofSetColor(0);
	ofLine(this->start, this->end);
	
	ofSetLineWidth(1.0f);
	ofSetColor(255);
	ofLine(this->start, this->end);
	
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(0);
	for(int i=0; i<100; i++) {
		this->lines.push_back(Line());
	}
	camera.setPosition((roomMax - roomCenter) * 2 + roomCenter);
	camera.lookAt(roomCenter);
	
	glDepthFunc(GL_LEQUAL);
	this->ofBaseApp::windowResized(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	camera.begin();
	ofDrawGrid(roomScale.length() * 2.0f, 4.0f, true, false, true, false);
	
	ofPushStyle();
	ofNoFill();
	ofDrawBox(roomCenter, roomScale.x, roomScale.y, roomScale.z);
	ofPopStyle();
	
	fbo.bind();
	ofClear(0,0,0,0);
	for(auto & line : lines) {
		line.draw();
	}
	fbo.unbind();
	
	camera.end();
	
	ofEnableAlphaBlending();
	fbo.draw(0, 0);
	ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	this->fbo.allocate(w, h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
