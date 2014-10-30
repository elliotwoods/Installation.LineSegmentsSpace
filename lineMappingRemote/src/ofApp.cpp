#include "ofApp.h"

//from http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
//v=start, w=end, p=point
float minimum_distance(ofVec2f start, ofVec2f end, ofVec2f point) {
	// Return minimum distance between line segment vw and point p
	const float l2 = (start - end).lengthSquared();  // i.e. |w-v|^2 -  avoid a sqrt
	if (l2 == 0.0) return (point - start).lengthSquared();   // v == w case
	
	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line.
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	const float t = (point - start).dot(end - start) / l2;
	if (t < 0.0) return (point - start).length();       // Beyond the 'v' end of the segment
	else if (t > 1.0) return (point - end).length();  // Beyond the 'w' end of the segment
	const ofVec2f projection = start + t * (end - start);  // Projection falls on the segment
	return (point - projection).length();
}

//--------------------------------------------------------------
void ofApp::setup(){
	this->iProjector = 0;
	this->selection = -1;
	this->status = Waiting;
	this->shift = false;
	this->ctrl = false;
	
	this->windowResized(ofGetWidth(), ofGetHeight());

	ofBackground(40);
	
	this->serverIP = ofSystemTextBoxDialog("Server IP address", "192.168.100.12");
	osc.setup(this->serverIP, 4000);
	
	ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void ofApp::update(){
	if (ofGetFrameNum() % 100 == 0) {
		this->updateLines();
	}
	if (ofGetFrameNum() % 2 == 0) {
		this->sendSelectedIndex();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	//--
	//draw button outlines
	//--
	//
	ofPushStyle();
	ofNoFill();
	ofSetColor(255);
	ofSetLineWidth(1.0f);
	ofRect(mainArea);
	ofRect(zoomedArea);
	for(int i=0; i<PROJECTOR_COUNT; i++) {
		ofRect(projectorButton[i]);
	}
	ofRect(nextButton);
	ofRect(backButton);
	ofPopStyle();
	//
	//--
	
	
	//--
	//draw zoom crosshair
	//--
	//
	ofPushStyle();
	ofSetColor(255);
	ofSetLineWidth(1.0f);
	ofLine(zoomedArea.getCenter() - ofVec2f(0,10), zoomedArea.getCenter() + ofVec2f(0,10));
	ofLine(zoomedArea.getCenter() - ofVec2f(10,0), zoomedArea.getCenter() + ofVec2f(10,0));
	ofPopStyle();
	//
	//--
	
	//--
	//draw button fills
	//--
	//
	ofPushStyle();
	ofFill();
	ofSetColor(255, 0, 0);
	ofRect(projectorButton[iProjector]);
	ofPopStyle();
	//
	//--
	
	//--
	//print controls to screen
	//--
	//
	stringstream controls;
	controls << "[ ] = select line near mouse cursor" << endl;
	controls << "[n] = new line" << endl;
	controls << "[BACKSPACE] = delete selected line" << endl;
	controls << "[f] = fullscreen" << endl;
	controls << endl;
	controls << "[SHIFT] = move slowly" << endl;
	controls << "[CTRL] = move awesomely" << endl;
	controls << "[ALT] = move both lines at same time" << endl;
	
	ofDrawBitmapString(controls.str(), this->projectorButton[0].x + 10, this->projectorButton[0].y + this->projectorButton[0].height + 20);
	
	for(int i=0; i < PROJECTOR_COUNT; i++) {
		ofDrawBitmapString("Projector " + ofToString(i), projectorButton[i].x + 40, buttonHeight / 2.0f + 5.0f);
	}
	ofDrawBitmapString("Back", 40 + backButton.x, buttonHeight / 2.0f + 5.0f);
	ofDrawBitmapString("Next", 40 + nextButton.x, buttonHeight / 2.0f + 5.0f);
	
	this->drawLineSet(mainArea, false);
	this->drawLineSet(zoomedArea, true);
	ofDrawBitmapString(ofToString(selection), 0, ofGetHeight());
}
	
//---------
void ofApp::drawLineSet(const ofRectangle & rect, bool zoom) {
	ofPushView();
	ofViewport(rect);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (zoom) {
		const float zoomFactor = 5.0f;
		ofScale(zoomFactor / (rect.width / rect.height), zoomFactor);
		ofTranslate(-screenToNorm(ofVec2f(ofGetMouseX(),ofGetMouseY())));
	}
	ofPushStyle();
	for(auto line : lines) {
		if(line.second.iProjector == iProjector) {
			if(line.first == selection) {
				ofSetColor(255);
			} else if (line.second.changed) {
				ofSetColor(0, 0, 255);
			} else {
				ofSetColor(255, 0, 0);
			}
			ofLine(line.second.start, line.second.end);
		}
	}
	ofPopStyle();
	ofPopView();
}
	
//--------------------------------------------------------------
ofVec2f ofApp::normToScreen(const ofVec2f& norm) {
	ofVec2f result = norm;
	result.x += 1.0f;
	result.y = 1.0f - norm.y;
	result.x *= this->mainArea.width / 2.0f + this->mainArea.x;
	result.y *= this->mainArea.height / 2.0f + this->mainArea.y;
	return result;
}

//--------------------------------------------------------------
ofVec2f ofApp::screenToNorm(const ofVec2f& screen) {
	ofVec2f result = screen;
	result.x -= this->mainArea.x;
	result.y -= this->mainArea.y;
	result.x /= this->mainArea.width;
	result.y /= this->mainArea.height;
	
	result *= 2.0f;
	result.x -= 1.0f;
	result.y = 1.0f - result.y;
	return result;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	ofVec2f vecStep(2.0f / 1920.0f, 2.0f / 1080.0f);
	
	if (key == OF_KEY_SHIFT) {
		this->shift = true;
	} else if (key == OF_KEY_CONTROL) {
		this->ctrl = true;
	} else if (key == OF_KEY_ALT) {
		this->alt = true;
	} else if (key == OF_KEY_UP  || key == OF_KEY_DOWN || key == OF_KEY_LEFT || key == OF_KEY_RIGHT) {
		if (this->selection != -1) {
			auto & line = this->lines[selection];
			ofVec2f vecStep;
			switch(key) {
				case OF_KEY_UP:
					vecStep *= ofVec2f(0, 1);
					break;
				case OF_KEY_DOWN:
					vecStep *= ofVec2f(0, -1);
					break;
				case OF_KEY_LEFT:
					vecStep *= ofVec2f(-1, 0);
					break;
				case OF_KEY_RIGHT:
					vecStep *= ofVec2f(1, 0);
					break;
			}
			if (this->shift) {
				line.start += vecStep;
				line.end += vecStep;
			} else {
				auto & end = endSelected == 0 ? line.start : line.end;
				end += vecStep;
			}
			this->sendSelection();
		}
		this->sendSelection();
	} else if (key == 'f') {
		ofToggleFullscreen();
	} else if (key == OF_KEY_BACKSPACE) {
		ofxOscMessage msg;
		msg.setAddress("/delete");
		msg.addIntArg(this->selection);
		osc.sendMessage(msg);
		this->sleeper.sleep(100);
		this->updateLines();
		this->next();
	} else if (key == ' ') {
		float minDistance = std::numeric_limits<float>::max();
		const ofVec2f mouseInArea = this->screenToNorm(ofVec2f(ofGetMouseX(), ofGetMouseY()));
		for(auto line : this->lines) {
			if (line.second.iProjector == iProjector) {
				float distance = minimum_distance(line.second.start, line.second.end, mouseInArea);
				if (distance < minDistance) {
					minDistance = distance;
					selection = line.first;
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == OF_KEY_SHIFT) {
		this->shift = false;
	} else if (key == OF_KEY_CONTROL) {
		this->ctrl = false;
	} else if (key == OF_KEY_ALT) {
		this->alt = false;
	}
	
	if (key == 'n' && this->status==Waiting) {
		ofxOscMessage msg;
		msg.setAddress("/newline");
		
		//msg.addIntArg(iProjector);
		msg.addFloatArg(iProjector); // we do this as float now so all args are same type (compatability mode)
		
		auto cursor = this->screenToNorm(ofVec2f(ofGetMouseX(), ofGetMouseY()));
		msg.addFloatArg(cursor.x);
		msg.addFloatArg(cursor.y);
		msg.addFloatArg(cursor.x);
		msg.addFloatArg(cursor.y - 0.2f);
		osc.sendMessage(msg);
		this->sleeper.sleep(100);
		this->updateLines();
		
		auto back = this->lines.end();
		back--;
		this->selection = back->first;
	}
	
	if (key == OF_KEY_TAB) {
		if (this->shift) {
			this->back();
		} else {
			this->next();
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	ofVec2f mouse(x,y);
	if (this->status == Dragging && this->selection != -1) {
		auto& line = this->lines[selection];
		auto displace = screenToNorm(mouse) - screenToNorm(lastMouse);
		if (this->shift) {
			displace *= 0.1f;
		}
		if (this->ctrl) {
			auto lineDir = (line.end - line.start).normalize();
			float distance = displace.dot(lineDir);
			displace = distance * lineDir;
		}
		if (this->alt) {
			line.start += displace;
			line.end += displace;
		} else {
			ofVec2f& point = endSelected == 0 ? line.start : line.end;
			point += displace;
		}
		
		this->sendSelection();
	}
	this->lastMouse = mouse;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	ofVec2f mouse(x,y);
	if (backButton.inside(x, y)) {
		this->back();
	} else if (nextButton.inside(x, y)) {
		this->next();
	} else {
		for(int i=0; i<PROJECTOR_COUNT; i++) {
			if(projectorButton[i].inside(x, y)) {
				iProjector = i;
			}
		}
	}
	
	this->sendSelectedIndex();
	
	if (mainArea.inside(x,y) && this->status == Waiting && this->selection != -1) {
		this->status = Dragging;
		this->lastMouse = mouse;
		
		float distanceToStart2 = (this->lines[selection].start - screenToNorm(mouse)).lengthSquared();
		float distanceToEnd2 = (this->lines[selection].end - screenToNorm(mouse)).lengthSquared();
		
		if (distanceToStart2 < distanceToEnd2) {
			this->endSelected = 0;
		} else {
			this->endSelected = 1;
		}
	}

	this->lastMouse = mouse;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	ofVec2f mouse(x,y);
	if (this->status == Dragging) {
		this->status = Waiting;
		this->sleeper.sleep(100);
		this->updateLines();
	}
	this->lastMouse = mouse;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	this->buttonHeight = 50;
	this->buttonWidth = ofGetWidth() / (float) (3 + PROJECTOR_COUNT);
	
	this->mainArea = ofRectangle(0, buttonHeight, ofGetWidth() - 200, ofGetHeight() - buttonHeight);
	this->zoomedArea = ofRectangle(ofGetWidth() - 200, buttonHeight, 200, ofGetHeight() - buttonHeight);
	
	int iButton=0;
	for(int i=0; i<PROJECTOR_COUNT; i++) {
		this->projectorButton[i] = ofRectangle(buttonWidth * iButton++,0,buttonWidth, buttonHeight);
		cout << this->projectorButton[i] << endl;
	}
	this->backButton = ofRectangle(buttonWidth * iButton++,0,buttonWidth, buttonHeight);
	this->nextButton = ofRectangle(buttonWidth * iButton++,0,buttonWidth, buttonHeight);

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::updateLines() {
	if (this->status == Dragging) {
		return;
	}
	ofURLFileLoader requester;
	auto response = requester.get("http://" + serverIP);
	cout << "Status: " << response.status << endl;
	if (response.status == 200) {
		ofXml xml;
		xml.loadFromBuffer(response.data.getText());
		if (xml.setTo("Lines")) {
			if (xml.setTo("Line[0]")) {
				this->lines.clear();
				do {
					Line newLine;
					int ID;
					newLine.start = xml.getValue<ofVec2f>("start");
					newLine.end = xml.getValue<ofVec2f>("end");
					newLine.iProjector = xml.getIntValue("iProjector");
					newLine.changed = xml.getIntValue("Changed") == 1;
					if(xml.setTo("ID")) {
						ID = xml.getIntValue();
						xml.setToParent();
					} else {
						if (this->lines.size() > 0) {
							auto last = lines.end();
							last--;
							ID = last->first + 1;
						} else {
							ID = 0;
						}
					}
					this->lines.insert(pair<int, Line>(ID, newLine));
				} while (xml.setToSibling());
				xml.setToParent();
			}
			xml.setToParent();
			requester.clear();
		}
		if (this->lines.count(this->selection) == 0) {
			this->selection = -1;
		}
		if (this->selection == -1 && this->lines.size() > 0) {
			this->selection = this->lines.begin()->first;
		}
	} else {
		ofSystemAlertDialog("Server connection failed");
		ofExit();
	}
}

//----------
void ofApp::sendSelection() {
	auto & line = this->lines[selection];
	ofxOscMessage msg;
	
	//clamp the current line to screen coordinates
	line.start.x = ofClamp(line.start.x, -1.0f, 1.0f);
	line.start.y = ofClamp(line.start.y, -1.0f, 1.0f);
	line.end.x = ofClamp(line.end.x, -1.0f, 1.0f);
	line.end.y = ofClamp(line.end.y, -1.0f, 1.0f);
	
	msg.setAddress("/point");
	msg.addFloatArg(selection);
	msg.addFloatArg(line.start.x);
	msg.addFloatArg(line.start.y);
	msg.addFloatArg(line.end.x);
	msg.addFloatArg(line.end.y);
	osc.sendMessage(msg);
}

//----------
void ofApp::sendSelectedIndex() {
	ofxOscMessage msg;
	msg.setAddress("/selection");
	msg.addIntArg(selection);
	osc.sendMessage(msg);
}

//----------
void ofApp::back() {
	if (this->lines.size() > 1) {
		auto findBack = this->lines.lower_bound(selection-1);
		int countSearches = 0;
		if (findBack != this->lines.end()) {
			do {
				if (findBack->second.iProjector == iProjector && findBack->first != selection) {
					this->selection = findBack->first;
					break;
				}
				if (countSearches++ > this->lines.size()) {
					break;
				}
				
				if (findBack == lines.begin()) {
					findBack = lines.end();
				}
				findBack--;
				
			} while (true);
		}
	}
}

//----------
void ofApp::next() {
	if (this->lines.size() > 1) {
		auto findNext = this->lines.lower_bound(selection);
		int countSearches = 0;
		if (findNext != this->lines.end()) {
			do {
				findNext++;
				if (findNext == lines.end()) {
					findNext = lines.begin();
				} else if (findNext->second.iProjector == iProjector && findNext->first != this->selection) {
					this->selection = findNext->first;
					break;
				}
				if (countSearches++ > this->lines.size()) {
					break;
				}
			} while (true);
		}
	}
}