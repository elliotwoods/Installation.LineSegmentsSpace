#include "Scene.h"

const ofVec3f roomMin(0.0f, 0.0f, -6.78f);
const ofVec3f roomMax(5.007f, 3.002f, 0.0f);
const ofVec3f roomScale = roomMax - roomMin;
const ofVec3f roomCenter = (roomMin + roomMax) / 2.0f;

//----------
Scene::Scene() {
	this->grid = true;
}

//----------
void Scene::init() {
	room.set(roomScale.x, roomScale.y, roomScale.z, 10, 10, 10);
	room.setPosition(roomCenter);
	light.setPosition(roomMax * 1.2f);
	
	//spin sides
	auto & roomMesh = this->room.getMesh();
	for(auto & normal : roomMesh.getNormals()) {
		normal *= -1.0f;
	}
	
	camera.setProjection(ofMatrix4x4());
	projector0.setProjection(ofMatrix4x4());
	projector1.setProjection(ofMatrix4x4());
	
	projector0.setWidth(1920);
	projector0.setHeight(1080);
	projector1.setWidth(1920);
	projector1.setHeight(1080);
	
	camera.setWidth(5184);
	camera.setHeight(3456);
	
	viewCamera.allocate(1024.0f * 1.5f, 1024.0f);
	viewProjector0.allocate(1920, 1080);
	viewProjector1.allocate(1920, 1080);
}

//----------
void Scene::updateViews() {
	drawSceneTo(camera, viewCamera);
	drawSceneTo(projector0, viewProjector0);
	drawSceneTo(projector1, viewProjector1);
}

//----------
void Scene::drawSceneTo(ofProjector& device, ofFbo& fbo) {
	fbo.begin();
	ofSetColor(0);
	device.beginAsCamera();
	ofClear(0);
	this->customDraw();
	device.endAsCamera();
	fbo.end();
}

//----------
void Scene::drawPoints() {
	imageCamera0.getTextureReference().bind();
	points0.drawVertices();
	imageCamera0.getTextureReference().unbind();
	
	imageCamera1.getTextureReference().bind();
	points1.drawVertices();
	imageCamera1.getTextureReference().unbind();
}

//----------
void drawDevice(ofxRay::Projector& device, string label) {
	ofPushMatrix();
	ofTranslate(device.getPosition());
	ofDrawBitmapString(label, ofVec3f());
	ofPopMatrix();
}

void Scene::customDraw() {
	ofDrawGrid(1.0f, 1.0f, true);
	
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
	
	
	//--
	//draw grid
	//
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
	//--
	
	
	this->camera.draw();
	this->projector0.draw();
	this->projector1.draw();
	
	ofPushStyle();
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	drawDevice(this->camera, "camera");
	drawDevice(this->projector0, "projector0");
	drawDevice(this->projector1, "projector1");
	ofPopStyle();
	
	this->drawPoints();
	
	if (imageCamera0.isAllocated()) {
		camera.drawOnNearPlane(imageCamera0);
	} else if (imageCamera1.isAllocated()) {
		camera.drawOnNearPlane(imageCamera1);
	}
	
	projector0.drawOnNearPlane(imageProjector0);
	projector1.drawOnNearPlane(imageProjector1);
}