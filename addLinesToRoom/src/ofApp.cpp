#include "ofApp.h"

const ofVec3f roomMin(-2.5f, 0.0f, 0.0f);
const ofVec3f roomMax(2.5f, 3.0f, 7.0f);
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
	
	this->layerGui.init(20, 20);

	ofAddListener(this->layerGui.newGUIEvent,this, &ofApp::layerGuiEvent);
	this->windowResized(ofGetWidth(), ofGetHeight());
	ofAddListener(this->lineSet.onLayersChange, this, &ofApp::layersChanged);
	this->layerGuiAdd = false;
	this->layerGuiDelete = false;
	this->layerGuiRename = false;
	this->layerGuiRebuildFlag = true;
	this->cursorInLayerGui = false;
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
	if (!this->cursorInLayerGui) {
		this->cursor = this->cameraCursorCached;
	}
	if (this->lineSet.getHoverIndex() != -1) {
		this->cursor = this->lineSet.getHover().closestPointOnRayTo(this->cursor);
	}
	if (this->state == Adding) {
		this->newLine.t = this->cursor - this->newLine.s;
	}
	if (this->layerGuiRebuildFlag) {
		this->layerGuiRebuildFlag = false;
		this->rebuildLayersGui();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofBackgroundGradient(100, 50);
	
	camera.begin();
	this->lineSet.updateIndexBuffer(this->shift);
	this->drawScene();
	this->cameraCursorCached = this->camera.getCursorWorld();
	this->drawCursor();
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
		this->drawCursor();
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
		
		ofPushStyle();
		ofSetColor(0);
		ofNoFill();
		for(float x = roomMin.x; x<= roomMax.x; x++) {
			ofPushMatrix();
			ofTranslate(x, 0, 0);
			ofRotate(-90, 0, 1.0f, 0);
			ofRect(roomMin.z, roomMin.y, roomMax.z - roomMin.z, roomMax.y - roomMin.y);
			ofPopMatrix();
		}
		
		for(float z = roomMin.z; z<= roomMax.z; z++) {
			ofPushMatrix();
			ofTranslate(0, 0, z);
			ofRect(roomMin.x, roomMin.y, roomMax.x - roomMin.x, roomMax.y - roomMin.y);
			ofPopMatrix();
		}
		
		for(float y = roomMin.y; y<= roomMax.y; y++) {
			ofPushMatrix();
			ofTranslate(0, y, 0);
			ofRotate(90, 1.0f, 0, 0);
			ofRect(roomMin.x, roomMin.z, roomMax.x - roomMin.x, roomMax.z - roomMin.z);
			ofPopMatrix();
		}
		ofPopStyle();
	}
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
	instructions << "[BACKSPACE] = delete selected lines" << endl;
	instructions << "[SHIFT] = show thicker lines" << endl;
	instructions << "[s] = save" << endl;
	instructions << "[l] = load" << endl;
	instructions << "[c] = clear all" << endl;
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

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == OF_KEY_SHIFT) {
		this->shift = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	this->cursorInLayerGui = this->layerGui.getRect()->inside(ofVec2f(x, y));
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (!this->layerGui.getRect()->inside(ofVec2f(x, y))) {
		this->mousePositionAtStartOfDown = ofVec2f(x,y);
	}
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
	this->layerGui.setPosition(ofGetWidth() - 200 - 20, 20);
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

//--------------------------------------------------------------
void ofApp::layersChanged(LayerSet& layers) {
	this->layerGuiRebuildFlag = true;
}

//--------------------------------------------------------------
void ofApp::rebuildLayersGui() {
	auto & layers = this->lineSet.getLayers();
	layerGui.removeWidgets();

	this->layerGui.resetPlacer();
	ofxUIRectangle firstLayerBounds;
	bool firstLayer = true;
	for(auto layer : layers) {
		auto selected = this->layerGui.addToggle(layer.first, &layer.second->selected);
		if (firstLayer) {
			firstLayerBounds = *selected->getRect();
			firstLayer = false;
		}
	}
	
	if (layers.getHasSelection()) {
		auto selected = layers.getSelection();
		
		this->layerGui.addSpacer();
		this->layerGui.addLabel(selected->name);
		this->layerGui.addSlider("Red", 0, 1.0f, &selected->color.r);
		this->layerGui.addSlider("Green", 0, 1.0f, &selected->color.g);
		this->layerGui.addSlider("Blue", 0, 1.0f, &selected->color.b);
	}
	
	this->layerGui.addSpacer();
	this->layerGui.addLabelButton("ADD LAYER", &this->layerGuiAdd);
	this->layerGui.addLabelButton("DELETE LAYER", &this->layerGuiDelete);
	this->layerGui.addLabelButton("RENAME LAYER", &this->layerGuiRename);
	
	this->layerGui.autoSizeToFitWidgets();
	
	//--
	//go back to start and do visible toggles
	//
	firstLayer = true;
	ofxUIWidget* lastVisible;
	float pitch;
	if (layers.size() > 1) {
		auto firstLayer = layers.begin();
		auto secondLayer = firstLayer;
		secondLayer++;
		auto firstWidget = this->layerGui.getWidget(firstLayer->first);
		auto secondWidget = this->layerGui.getWidget(secondLayer->first);
		pitch = secondWidget->getRect()->y - firstWidget->getRect()->y;
	}
	for(auto layer : layers) {
		auto visible = new ofxUIToggle("", &layer.second->visible, 20,OFX_UI_GLOBAL_SLIDER_HEIGHT);
			auto bounds = visible->getRect();
		if (firstLayer) {
			*bounds = firstLayerBounds;
			bounds->x += 180;
			bounds->width = bounds->height;
			this->layerGui.addWidget(visible);
			firstLayer = false;
		} else {			
			*bounds = *lastVisible->getRect();
			bounds->y += pitch;
			
			this->layerGui.addWidget(visible);

		}
		lastVisible = visible;
	}
	//
	//--
	
	this->layerGui.autoSizeToFitWidgets();
}

//--------------------------------------------------------------
void ofApp::layerGuiEvent(ofxUIEventArgs& args) {
	auto widgetName = args.widget->getName();
	if (widgetName == "ADD LAYER" && this->layerGuiAdd) {
		auto name = ofSystemTextBoxDialog("New layer name");
		if(name != "") {
			this->lineSet.addLayer(name);
		}
	} else if (widgetName == "DELETE LAYER" && this->layerGuiDelete) {
		this->lineSet.deleteLayer(this->lineSet.getLayers().getSelection());
	} else if (widgetName == "RENAME LAYER" && this->layerGuiRename) {
		auto name = ofSystemTextBoxDialog("New layer name");
		if (name != "") {
			this->lineSet.renameLayer(this->lineSet.getLayers().getSelection(), name);
		}
	} else if (widgetName == "Red" || widgetName == "Green" || widgetName == "Blue") {
		
	} else if (args.widget->getName().length() > 0) {
		//we've changed layer selection
		auto & layers = this->lineSet.getLayers();
		if (layers.count(widgetName) > 0) {
			layers.setSelection(widgetName);
			this->lineSet.selectThreadsForLayer(layers[widgetName]);
		}
		this->layerGuiRebuildFlag = true;
	} else {
		this->lineSet.changeLayerVisibility();
	}
}