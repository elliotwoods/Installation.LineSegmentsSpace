#include "ofApp.h"

const ofVec3f roomMin(-2.5f, 0.0f, 0.0f);
const ofVec3f roomMax(2.5f, 2.7f, 7.0f);
const ofVec3f roomScale = roomMax - roomMin;
const ofVec3f roomCenter = (roomMin + roomMax) / 2.0f;

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(0);
	camera.setPosition((roomMax - roomCenter) * 2 + roomCenter);
	camera.lookAt(roomCenter);
	
	glDepthFunc(GL_LEQUAL);
	this->ofBaseApp::windowResized(ofGetWidth(), ofGetHeight());
	
	this->zoomed.allocate(256, 256, GL_RGBA, 4);
	
	this->setupRoom();
	
	this->state = Waiting;
	this->shift = false;
	this->shadow = true;
	this->grid = false;
}

//---------
void ofApp::setupRoom() {
	room.set(roomScale.x, roomScale.y, roomScale.z, 10, 10, 10);
	room.setPosition(roomCenter);
	light.setPosition(roomMax * 1.2f);
	
	//spin sides
	auto & roomMesh = this->room.getMesh();
	for(auto & normal : roomMesh.getNormals()) {
		normal *= -1.0f;
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	this->cursor = this->camera.getCursorWorld();
	if (this->lineSet.getHoverIndex() != -1) {
		this->cursor = this->lineSet.getHover().closestPointOnRayTo(this->cursor);
	}
	if (this->state == Adding) {
		this->newLine.t = this->cursor - this->newLine.s;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofBackgroundGradient(100, 50);
	
	camera.begin();
	this->lineSet.updateIndexBuffer(this->shift);
	this->drawScene();
	camera.end();
	
	//--
	//draw zoomed
	//
	if (this->shift) {
		zoomed.begin();
		ofClear(0,0,0,255);
		ofPushView();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		
		glScalef(10.0f, 10.0f, 1.0f);
		
		ofVec3f offset = this->camera.worldToCamera(this->camera.getCursorWorld());
		offset.z = 0.0f;
		ofTranslate(-offset);
		
		glMultMatrixf(camera.getProjectionMatrix().getPtr());
		
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(camera.getModelViewMatrix().getPtr());
		this->drawScene();
		ofPopView();
		zoomed.end();
		zoomed.draw(0, ofGetHeight(), zoomed.getWidth(), - zoomed.getHeight());
	}
	//
	//--
	
 	this->drawInstructions();
}

//----------
void ofApp::drawScene() {
	//--
	//draw room
	//
	ofPushStyle();
	ofFill();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	this->light.enable();
	room.draw();
	this->light.disable();
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	ofDisableLighting();
	ofPopStyle();
	//
	//--
	
	this->lineSet.draw(this->shift, this->shadow);
	
	if (this->state == Adding) {
		newLine.draw(NEWLINE_SIZE, ofColor::green);
	}
	
	if (this->grid) {
		ofPushMatrix();
		ofRotate(90, 0.0f, 0.0f, 1.0f);
		ofDrawGridPlane(7.0f, 7);
		ofPopMatrix();
	}
	
	this->drawCursor();
}

//----------
void thickerLine(const ofVec3f& start, const ofVec3f& end, const ofColor& color, float thickness = 1.0f) {
	ofPushStyle();
	
	ofSetLineWidth(thickness + 1.0f);
	ofSetColor(0);
	ofLine(start, end);

	ofSetLineWidth(thickness);
	ofSetColor(color);
	ofLine(start, end);
	
	ofPopStyle();
}
//----------
void ofApp::drawCursor() {
	const ofColor color = this->state == Adding ? ofColor(255,255,255) : ofColor(0,0,0);
	//draw the cursor as lines between the walls
	// (i.e. each line is 2 points where the cursor is projected onto the walls)
	
	ofVec3f A, B;
	
	A = this->cursor;
	B = this->cursor;
	A.x = roomMin.x;
	B.x = roomMax.x;
	thickerLine(A, B, color);
	
	A = this->cursor;
	B = this->cursor;
	A.y = roomMin.y;
	B.y = roomMax.y;
	thickerLine(A, B, color);
	
	A = this->cursor;
	B = this->cursor;
	A.z = roomMin.z;
	B.z = roomMax.z;
	thickerLine(A, B, color);
}

//----------
void ofApp::drawInstructions() {
	float totalLength = this->lineSet.getLengthAll();
	if (this->state == Adding) {
		totalLength += this->newLine.getLength();
	}
	
	stringstream instructions;
	if (this->state == Adding) {
		instructions << "[SPACE] = select end point for new line" << endl;
	} else {
		instructions << "[SPACE] = select start point for new line" << endl;
	}
	instructions << "[BACKSPACE] = delete last line" << endl;
	instructions << "[SHIFT] = show thicker lines" << endl;
	instructions << "[s] = save line set" << endl;
	instructions << "[l] = load line set" << endl;
	instructions << "[c] = clear all lines" << endl;
	instructions << "[h] = toggle shadow" << endl;
	instructions << "[g] = toggle grid" << endl;
	if (this->lineSet.getUndoStackLength() > 0) {
		instructions << "[z] = undo" << endl;		
	}
	instructions << endl;
	instructions << "Line count = " << (this->lineSet.getCountAll() + (this->state == Adding ? 1 : 0)) << endl;
	instructions << "Total length = " << totalLength << "m" << endl;
	if (this->lineSet.getCountSelected() > 0) {
		if (this->lineSet.getCountSelected() > 1) {
			instructions << "Selection count = " << this->lineSet.getCountSelected() << endl;
		}
		instructions << "Total selection length = " << this->lineSet.getLengthSelected() << "m" << endl;
	}
	
	instructions << "Hover line length = ";
	if (this->lineSet.getHoverIndex() != -1) {
		instructions << this->lineSet.getHover().getLength() << "m";
	}
	instructions << endl;
	
	ofDrawBitmapStringHighlight(instructions.str(), 20, 20, ofColor(200, 100, 100));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == ' ') {
		const ofVec3f cursor = this->camera.getCursorWorld();
		const ofVec3f cursorNormalised = (cursor - roomMin) / roomScale;
		const float epsilon = 0.01f;
		
		bool inside = true;
		inside &= cursorNormalised.x >= 0.0f - epsilon;
		inside &= cursorNormalised.x <= 1.0f + epsilon;
		inside &= cursorNormalised.y >= 0.0f - epsilon;
		inside &= cursorNormalised.y <= 1.0f + epsilon;
		inside &= cursorNormalised.z >= 0.0f - epsilon;
		inside &= cursorNormalised.z <= 1.0f + epsilon;
		
		if (inside) {
			switch (this->state) {
				case Waiting:
					this->newLine.s = this->cursor;
					this->state = Adding;
					break;
				case Adding:
				{
					this->lineSet.add(this->newLine);
					this->state = Waiting;
					break;
				}
				default:
					break;
			}
		}
	}
	
	if (key == 's') {
		auto result = ofSystemSaveDialog("lines.xml", "Save lines to binary file");
		if (result.bSuccess) {
			this->lineSet.save(result.getPath());
		}
	}
	
	if (key == 'l') {
		auto result = ofSystemLoadDialog();
		if (result.bSuccess) {
			this->lineSet.load(result.getPath());
		}
	}
	
	if (key == 'c') {
		this->lineSet.clear();
	}
	
	if (key == OF_KEY_BACKSPACE) {
		this->lineSet.deleteSelected();
	}
	
	if (key == OF_KEY_SHIFT) {
		this->shift = true;
	}
	
	if (key == 'h') {
		this->shadow ^= true;
	}
	
	if (key == 'g') {
		this->grid ^= true;
	}
	
	if (key == 'z') {
		this->lineSet.undo();
	}
}

//----------
ofVec3f ofApp::getPositionSnapped() {
	//return hover->closestPointOnRayTo(this->camera.getCursorWorld());
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == OF_KEY_SHIFT) {
		this->shift = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	this->mousePositionAtStartOfDown = ofVec2f(x,y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if (this->mousePositionAtStartOfDown == ofVec2f(x,y)) {
		if (this->state == Waiting) {
			if (!this->shift) {
				this->lineSet.clearSelection();
			}
			this->lineSet.toggleHoverToSelection();
		}
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
	for(auto filename : dragInfo.files) {
		this->lineSet.load(filename);
	}
}
