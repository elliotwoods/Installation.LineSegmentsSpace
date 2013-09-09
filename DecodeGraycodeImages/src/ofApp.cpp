#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	payload.init(1920, 1080);
	decoder.init(payload);
	this->showOther = false;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	if (this->decoder.hasData()) {
		auto & image = showOther ? decoder.getCameraInProjector() : decoder.getProjectorInCamera();
		image.draw(0, 0, ofGetWidth(), ofGetHeight());
		ofDrawBitmapStringHighlight(ofToString( (int) decoder.getThreshold()), 20, 20);
	} else {
		ofDrawBitmapStringHighlight("Drag a folder of images here (not a set of images, but the folder)", 10, 10);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
		case 's':
			if (this->decoder.hasData()) {
				decoder.saveDataSet();
				
				//save camera pixel index as r,g,b binary split
				ofImage customSave;
				customSave.allocate(this->payload.getWidth(), this->payload.getHeight(), OF_IMAGE_COLOR);
				int pixelCount = customSave.getWidth() * customSave.getHeight();
				auto & cameraInProjector = this->decoder.getDataSet().getDataInverse();
				auto output = customSave.getPixels();
				auto input = cameraInProjector.getPixels();
				
				for(int i=0; i<pixelCount; i++) {
					*output++ = *input & 255;
					*output++ = (*input >> 8) & 255;
					*output++ = (*input >> 16) & 255;
					*input++;
				}
				
				auto filename = decoder.getDataSet().getFilename();
				customSave.saveImage(filename + ".rgbraw.bmp");
			}
			break;
		case OF_KEY_UP:
		{
			uint8_t oldThreshold = decoder.getThreshold();
			if(oldThreshold < 255) {
				decoder.setThreshold(oldThreshold + 1);
			}
		}
			break;
		case OF_KEY_DOWN:
		{
			uint8_t oldThreshold = decoder.getThreshold();
			if(oldThreshold > 0) {
				decoder.setThreshold(oldThreshold - 1);
			}
		}
			break;
		case ' ':
			showOther = !showOther;
			break;
		default:
			break;
	}
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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
	for(auto item : dragInfo.files) {
		if (ofFile(item).isDirectory()) {
			auto dir = ofDirectory();
			dir.sort();
			dir.listDir(item);
			
			ofImage loader;
			int index = 0;
			for (auto & file : dir.getFiles()) {
				ofLogNotice("ofApp::dragEvent") << "loading " << file.getFileName() << " #" << index++;
				loader.loadImage(file);
				ofLogNotice("ofApp::dragEvent") << "adding " << file.getFileName();
				decoder << loader;
			}
			decoder.update();
		}
	}
}
