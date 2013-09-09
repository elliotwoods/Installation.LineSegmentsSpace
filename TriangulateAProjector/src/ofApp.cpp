#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	this->payload.init(1920, 1080);
	this->decoder.init(this->payload);
	this->scene.init();
	
	gui.init();
	auto screenScene = gui.add(scene, "World");
	gui.add(scene.viewCamera, "Camera View");
	gui.add(scene.viewProjector0, "Projector0 View");
	gui.add(scene.viewProjector1, "Projector1 View");
	gui.add(scene.imageCamera0, "Camera0 Median");
	gui.add(scene.imageCamera1, "Camera1 Median");
	
	
	screenScene->setGridLabelsEnabled(false);
	screenScene->setGridEnabled(false);
	
	auto & camera = screenScene->getCamera();
	camera.setPosition(2.0f, 2.0f, -1.0f);
	camera.lookAt(ofVec3f(0,0,-6.0f));
	camera.setFov(90.0f);
	camera.setCursorDraw(true);
	camera.setNearClip(0.01f);
	camera.setFarClip(100.0f);
	
	ofBackground(50);
	ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'c') {
		scene.points0.clear();
		scene.points1.clear();
		scene.camera.setView(ofMatrix4x4());
		scene.camera.setProjection(ofMatrix4x4());
		scene.camera.distortion.clear();
		scene.projector0.setView(ofMatrix4x4());
		scene.projector0.setProjection(ofMatrix4x4());
		scene.projector1.setView(ofMatrix4x4());
		scene.projector1.setProjection(ofMatrix4x4());
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
vector<double> loadDoubles(string& filename) {
	ifstream file;
	file.open(ofToDataPath(filename).c_str(), ios::in | ios::binary);
	vector<double> result;
	if (file.is_open()) {
		while(!file.eof()) {
			double peek;
			file.read((char*)&peek, sizeof(double));
			result.push_back(peek);
		}
		result.pop_back();
	} else {
		ofLogError() << "Failed to load file " << filename;
	}
	return result;
}

ofMatrix4x4 loadMatrix(string& filename) {
	auto buffer = loadDoubles(filename);
	if (buffer.size() >= 16) {
		ofMatrix4x4 m;
		auto floatBuffer = m.getPtr();
		for(int i=0; i<16; i++) {
			floatBuffer[i] = buffer[i];
		}
		
		ofMatrix4x4 reverseZ;
		reverseZ.scale(1.0f, 1.0f, -1.0f);
		return reverseZ * m * reverseZ;
	} else {
		return ofMatrix4x4();
	}
}

vector<float> loadDistortion(string& filename) {
	auto doublesBuffer = loadDoubles(filename);
	vector<float> floatBuffer(doublesBuffer.size());
	for(int i=0; i<doublesBuffer.size(); i++) {
		floatBuffer[i] = (float) doublesBuffer[i];
	}
	return floatBuffer;
}

void ofApp::dragEvent(ofDragInfo dragInfo){
	for(auto entry : dragInfo.files) {
		bool isGraycode = entry.find("dataset") != string::npos;
		bool isProjector = !isGraycode && entry.find("projector") != string::npos;
		bool isCamera = !isProjector;
		bool isDistortion = entry.find("distortion") != string::npos;
		bool isView = entry.find("view") != string::npos;
		bool isProjection = !(isView || isDistortion);
		int iProjector;
		
		if (isProjector) {
			if (entry.find("0") != string::npos) {
				iProjector = 0;
			} else {
				iProjector = 1;
			}
		} else if(isGraycode) {
			if (entry.find("0") != string::npos) {
				iProjector = 0;
			} else {
				iProjector = 1;
			}
		}
		
		if (isGraycode) {
			this->decoder.loadDataSet(entry);
			auto & projector = iProjector == 0 ? scene.projector0 : scene.projector1;
			auto & mesh = iProjector == 0 ? scene.points0 : scene.points1;
			ofLogNotice() << "Triangulating points for projector " << iProjector;
			ofxTriangulate::Triangulate(this->decoder.getDataSet(), scene.camera, projector, mesh, 0.1f, false, true);
			
			auto & imageProjector = (iProjector == 0 ? scene.imageProjector0 : scene.imageProjector1);
			imageProjector = decoder.getDataSet().getMedianInverse();
			imageProjector.update();
			
			auto & imageCamera = (iProjector == 0 ? scene.imageCamera0 : scene.imageCamera1);
			imageCamera = decoder.getDataSet().getMedian();
			imageCamera.update();
			
			scene.updateViews();
		} else if (isDistortion) {
			scene.camera.distortion = loadDistortion(entry);
		} else {
			auto matrix = loadMatrix(entry);
			
			if (isView) {
				if (isCamera) {
					scene.camera.setView(matrix);
				} else {
					(iProjector == 0 ? scene.projector0 : scene.projector1).setView(matrix);
				}
			} else {
				ofMatrix4x4 reverseZ;
				if (isCamera) {
					scene.camera.setProjection(matrix);
				} else {	
					(iProjector == 0 ? scene.projector0 : scene.projector1).setProjection(matrix);
				}
			}
		}
	}
}
