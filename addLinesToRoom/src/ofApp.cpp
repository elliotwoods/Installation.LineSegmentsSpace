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

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofBackgroundGradient(100, 50);
	
	camera.begin();
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
		zoomed.draw(0, ofGetHeight() - zoomed.getHeight());
	}
	//
	//--
	
 	this->drawInstructions();
}

//----------
void ofApp::drawScene() {
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
	
	this->lineSet.draw(this->shift, this->shadow);
	
	if (this->state == Adding) {
		newLine.draw(false, NEWLINE_SIZE);
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
	const ofVec3f cursor = this->shift ? this->getPositionSnapped() : this->camera.getCursorWorld();
	const ofColor color = this->state == Adding ? ofColor(255,255,255) : ofColor(0,0,0);
	//draw the cursor as lines between the walls
	// (i.e. each line is 2 points where the cursor is projected onto the walls)
	
	ofVec3f A, B;
	
	A = cursor;
	B = cursor;
	A.x = roomMin.x;
	B.x = roomMax.x;
	thickerLine(A, B, color);
	
	A = cursor;
	B = cursor;
	A.y = roomMin.y;
	B.y = roomMax.y;
	thickerLine(A, B, color);
	
	A = cursor;
	B = cursor;
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
	instructions << endl;
	instructions << "Line count = " << (this->lineSet.getCountAll() + (this->state == Adding ? 1 : 0)) << endl;
	instructions << "Total length = " << totalLength << endl;
	
	if (this->lineSet.getCountSelected() > 0) {
		if (this->lineSet.getCountSelected() > 1) {
			instructions << "Selection count = " << this->lineSet.getCountSelected() << endl;
		}
		instructions << "Total selection length = " << this->lineSet.getLengthSelected() << endl;
	}
	
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
					this->findLineHover();
					if (hover != this->lines.end() && this->shift) {
						//we're on an existing line
						this->newLine.s = this->getPositionSnapped();
					} else {
						this->newLine.s = cursor;
					}
					this->state = Adding;
					break;
				case Adding:
				{
					this->lines.push_back(this->newLine);
					this->selection = this->lines.end() - 1;
					this->findLineHover();
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
//		for (auto line : this->selection) {
//			this->lines.erase(line);
//			this->selection.clear();
//			findLineHover();
//		}
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
}

//----------
vector<Line>::iterator ofApp::getPickLine() {
	ofxRay::Ray pickRay;
	pickRay.s = this->camera.getPosition();
	pickRay.t = this->camera.getCursorWorld() - pickRay.s;
	
	auto inverseViewProjection = this->camera.getModelViewProjectionMatrix().getInverse();
	auto pickRayInClip = pickRay * inverseViewProjection;
	
	const float pickDistance = this->shift ? SHIFT_SIZE : 3.0f; // threshold in pixels
	auto lengthSquareThreshold = pow(pickDistance / ofGetWidth(),2);
	
	vector<Line>::iterator closest = this->lines.end();
	float closestDistance = std::numeric_limits<float>::max();
	
	for (vector<Line>::iterator line = this->lines.begin(); line != this->lines.end(); line++) {
		auto lineInClip = *line * inverseViewProjection;
		float u1, u2;
		auto intersectInClip = lineInClip.intersect(pickRayInClip, u1, u2);
		
		if (u1 >= 0.0f && u1 <= 1.0f) {
			auto intersectInClipFlatZ = intersectInClip;
			intersectInClipFlatZ.s.z = 0.0f;
			intersectInClipFlatZ.t.z = 0.0f;
			float distance = intersectInClipFlatZ.getLengthSquared();
			if (distance < closestDistance && line->getLength() > 0.001f) { //sanity check for nuff lines
				closestDistance = distance;
				closest = line;
			}
		}
	}
	
	if (closest != this->lines.end()) {
		if (closestDistance < lengthSquareThreshold) {
			return closest;
		}
	}
	
	return lines.end();
}

//----------
void ofApp::addPickLineToSelection() {
	this->selection = this->getPickLine();
}

//----------
void ofApp::findLineHover() {
	this->hover = this->getPickLine();
}

//----------
ofVec3f ofApp::getPositionSnapped() {
	return hover->closestPointOnRayTo(this->camera.getCursorWorld());
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == OF_KEY_SHIFT) {
		this->shift = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	this->findLineHover();
	
	if (this->state == Adding) {
		this->camera.updateCursorWorld();
		auto cursor = this->camera.getCursorWorld();
		if (hover != this->lines.end() && this->shift) {
			//we're on an existing line
			this->newLine.t = this->getPositionSnapped() - this->newLine.s;
		} else {
			//we're just picking in space
			this->newLine.t = cursor - this->newLine.s;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (this->state == Waiting) {
		this->addPickLineToSelection();
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
		this->load(filename);
	}
}
