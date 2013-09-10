#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	glDepthFunc(GL_LEQUAL);
	
	this->payload.init(1920, 1080);
	this->decoder[0].init(this->payload);
	this->decoder[1].init(this->payload);
	this->scene.init();
	this->scene.setThreadSet(this->threads);
	
	gui.init();
	gui.add(fboProjectorView, "Projection Space lines");
	this->debugCameraPanel = gui.add(this->debugCameraMedian, "Camera Median");
	this->debugProjectorPanel = gui.add(this->debugProjectorMedian, "Projector Median");
	auto screenScene = gui.add(scene, "World");
	
//	gui.add(scene.viewCamera, "Camera View");
//	gui.add(scene.viewProjector0, "Projector0 View");
//	gui.add(scene.viewProjector1, "Projector1 View");
//	gui.add(scene.imageCamera0, "Camera0 Median");
//	gui.add(scene.imageCamera1, "Camera1 Median");
	
<<<<<<< HEAD

	screenScene->push(this->debugNode);
=======
>>>>>>> e1790b4a9892e4d1cf92d6018625af74f944849d
	screenScene->setGridLabelsEnabled(false);
	screenScene->setGridEnabled(false);
	screenScene->setCursorEnabled();
	
	auto & camera = screenScene->getCamera();
	camera.setPosition(2.0f, 2.0f, -1.0f);
	camera.lookAt(ofVec3f(0,0,-6.0f));
	camera.setFov(90.0f);
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
	ofPushStyle();
	ofNoFill();
	
	ofPushView();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	
	ofViewport(this->debugCameraPanel->getBounds());
	ofCircle(debugCameraCoord, 0.1f);
	
	ofViewport(this->debugProjectorPanel->getBounds());
	ofCircle(debugProjectorCoord, 0.1f);
	
	ofPopView();
	ofPopStyle();
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
		threads.clear();
		this->decoder[0].reset();
		this->decoder[1].reset();
	}
	
	if (key == 's') {
		ofstream saveFilesDragged;
		saveFilesDragged.open(ofToDataPath("filesDragged.txt").c_str(), ios::out | ios::trunc);
		for(auto filename : filesDragged) {
			saveFilesDragged << filename << endl;
		}
		saveFilesDragged.close();
	}
	
	if (key == 'l') {
		ifstream loadFilesDragged;
		loadFilesDragged.open(ofToDataPath("filesDragged.txt").c_str(), ios::in);
		if (loadFilesDragged.is_open()) {
			string filename;
			for( std::string line; getline( loadFilesDragged, line ); ) {
				ofDragInfo dragInfo;
				dragInfo.files = vector<string>(1, line);
				this->dragEvent(dragInfo);
			}
			loadFilesDragged.close();
		}
		triangulateLine3D();
		threads.save();
	}
	
	if (key == 'o') {
		threads.save();
	}
	
	if (key == 't') {
		triangulateLine3D();
	}
	
	if (key == 'p') {
		bool found = false;
		int cameraIndex = 0;
		int projectorIndex;
		auto & decoder = this->decoder[0];
		auto & projector = this->scene.projector0;
		auto & dataSet = decoder.getDataSet();
		while(!found) {
			cameraIndex = ofRandom(0, decoder.getWidth() * decoder.getHeight() - 1.0f);
			if (dataSet.getActive().getPixels()[cameraIndex]) {
				found = true;
				projectorIndex = dataSet.getData()[cameraIndex];
			}
		}
		this->debugCameraCoord = scene.camera.getCoordinateFromIndex(cameraIndex);
		this->debugProjectorCoord = projector.getCoordinateFromIndex(projectorIndex);
		
		this->debugNode.cameraRay = scene.camera.castCoordinate(this->debugCameraCoord);
		this->debugNode.projectorRay = projector.castCoordinate(this->debugProjectorCoord);
		this->debugNode.intersectRay = this->debugNode.cameraRay.intersect(this->debugNode.projectorRay);
		
		this->debugNode.cameraRay.color = ofColor(255,0,0);
		this->debugNode.projectorRay.color = ofColor(0,255,0);
		this->debugNode.intersectRay.color = ofColor(0,0,255);
		
		ofLogNotice() << "Debug viewing camera pixel:" << cameraIndex << ", projector pixel:" << projectorIndex << ", finds point:" << this->debugNode.intersectRay.getMidpoint();
		ofLogNotice() << "Camera coordinate:" << this->debugCameraCoord;
		ofLogNotice() << "Projector coordinate:" << this->debugProjectorCoord;
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
		ofLogNotice() << "Adding file " << entry;
		filesDragged.push_back(entry);
		bool isGraycode = entry.find("dataset") != string::npos;
		bool isProjector = !isGraycode && entry.find("projector") != string::npos;
		bool isCamera = !isProjector;
		bool isDistortion = entry.find("distortion") != string::npos;
		bool isView = entry.find("view") != string::npos;
		bool isProjection = !(isView || isDistortion);
		bool isLineSet = entry.find("lines") != string::npos;
		
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
		} else {
			if (entry.find("0") != string::npos) {
				iProjector = 0;
			} else {
				iProjector = 1;
			}
		}
		
		if (isGraycode) {
			auto & decoder = this->decoder[iProjector];
			decoder.loadDataSet(entry);
			decoder.setThreshold(10);
			auto & projector = iProjector == 0 ? scene.projector0 : scene.projector1;
			auto & mesh = iProjector == 0 ? scene.points0 : scene.points1;
			ofLogNotice() << "Added dataset for projector " << iProjector;
			
			auto & imageProjector = (iProjector == 0 ? scene.imageProjector0 : scene.imageProjector1);
			imageProjector = decoder.getDataSet().getMedianInverse();
			imageProjector.update();
			
			auto & imageCamera = (iProjector == 0 ? scene.imageCamera0 : scene.imageCamera1);
			imageCamera = decoder.getDataSet().getMedian();
			imageCamera.update();
			
			ofxTriangulate::Triangulate(decoder.getDataSet(), scene.camera, projector, mesh, TRIANGULATE_MAX_DISTANCE);
			
			if (iProjector==0) {
				this->debugCameraMedian = this->decoder[0].getDataSet().getMedian();
				this->debugCameraMedian.update();
				this->debugProjectorMedian = this->decoder[0].getDataSet().getMedianInverse();
				this->debugProjectorMedian.update();
			}
		} else if (isDistortion) {
			scene.camera.distortion = loadDistortion(entry);
		} else if (isLineSet) {
			threads.load(entry);
			ofLogNotice() << "Drawing lines into projector space";
			fboProjectorView.allocate(1920, 1080);
			fboProjectorView.begin();
			ofClear(0);
			threads.drawProjectionSpace();
			fboProjectorView.end();
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

//---------
void ofApp::triangulateLine3D() {
	ofxPolyFitf fit;
	fit.init(1, 1, 3, BASIS_SHAPE_TRIANGLE);
	for(int iProjector=0; iProjector<2; iProjector++) {
		auto & mapping = (iProjector == 0 ? scene.activePixelsMapProjectorToCamera0 : scene.activePixelsMapProjectorToCamera1);
		auto & camera = scene.camera;
		auto & projector = (iProjector == 0 ? scene.projector0 : scene.projector1);
		
		mapping.clear();
		auto & decoder = this->decoder[iProjector];
		auto projectorInCamera = decoder.getDataSet().getData().getPixels();
		auto active = decoder.getDataSet().getActive().getPixels();
		for(uint32_t cameraPixel=0; cameraPixel<decoder.getWidth() * decoder.getHeight(); cameraPixel++) {
			if (active[cameraPixel]) {
				uint32_t projectorPixel = projectorInCamera[cameraPixel];
				if (projectorPixel != 0) {
					mapping.insert(std::pair<uint32_t, uint32_t>(cameraPixel, projectorPixel));
				}
			}
		}
		
//		for(auto map : mapping) {
//			cout << map.first << "->" << map.second << endl;
//		}
		
		const int searchLocations = 20;
		const int searchSize = 3; // kernel size in projector space
		
		int currentIndex = 0;
		for (auto & thread : threads) {
			if (thread.projector != iProjector) {
				currentIndex++;
				continue;
			}
			cout << "Triangulating thread " << currentIndex++ << "/" << threads.size() << endl;
			vector<Find> finds;
			for(int iSearchLocation=0; iSearchLocation<searchLocations; iSearchLocation++) {
				float x = (float) iSearchLocation / (float) (searchLocations - 1);
				const ofVec2f searchLocation = x * (thread.projectorEnd - thread.projectorStart) + thread.projectorStart;
				const ofVec2f searchPixelLocation = projector.getIndexFromCoordinate(searchLocation);
				for(auto correspondence : mapping) {
					const int projector_x = correspondence.second % this->payload.getWidth();
					const int projector_y = correspondence.second / this->payload.getWidth();
					if (projector_y < searchPixelLocation.y - searchSize) {
						continue;
					} else if (projector_y > searchPixelLocation.y + searchSize) {
						continue; //this can be break if we're organising mapping by projector pixels
					}
					const ofVec2f projectorPixelLocation = ofVec2f(projector_x, projector_y);

					ofVec3f worldXYZ;	
					if ((projectorPixelLocation - searchPixelLocation).lengthSquared() <= searchSize * searchSize) {
						if (ofxTriangulate::Triangulate(correspondence.first, correspondence.second, camera, projector, worldXYZ, TRIANGULATE_MAX_DISTANCE)) {
							Find find;
							find.x = x;
							find.projectorXY.x = projector_x;
							find.projectorXY.y = projector_y;
							find.worldXYZ = worldXYZ;
							find.cameraIndex = correspondence.first;
							find.projectorIndex = correspondence.second;
							finds.push_back(find);
						}
					}
				}
			}
			
			if (finds.size() == 0) {
				continue;
			}
			
	//		cout << "Finds for thread at " << thread.projectorStart << " = {" << endl;
	//		for(auto find : finds) {
	//			cout << "[" << find.x << "] : \t" << find.worldXYZ << " \t->\t" << find.projectorXY << "\t..." << find.cameraIndex << "<>" << find.projectorIndex << endl;
	//		}
	//		cout << "}" << endl;
			
			pfitDataSetf fitDataSet;
			fitDataSet.init(1, 3);
			fitDataSet.resize(finds.size());
			pfitDataPoint<float> point = fitDataSet.begin();
			for(auto find : finds) {
				*point.getInput() = find.x;
				*(ofVec3f*) point.getOutput() = find.worldXYZ;
				++point;
			}
			const float selectionProbability = 0.3f;
			const int maxIterations = 200;
			const float searchWidth = 0.3f;
			const float inclusionThreshold = 0.5f;
			fitDataSet.setActiveAll();
			
			cout << fitDataSet.toString();
			
			//--
			//check we've got a range of points
			//--
			//
			bool hasRange = false;
			bool firstFind = true;
			float seenX;
			for(auto find : finds) {
				if (firstFind) {
					seenX = find.x;
					firstFind = false;
				} else {
					if(find.x != seenX) {
						hasRange = true;
						break;
					}
				}
			}
			//
			//--
			
			if (hasRange) {
				fit.correlate(fitDataSet);
				fit.RANSAC(fitDataSet, maxIterations, selectionProbability, searchWidth, inclusionThreshold);
				
				pfitDataPointf evaluatePoint(1, 3);
				
				*evaluatePoint.getInput() = 0.0f;
				fit.evaluate(evaluatePoint);
				thread.s = * (const ofVec3f*) evaluatePoint.getOutput();
				
				*evaluatePoint.getInput() = 1.0f;
				fit.evaluate(evaluatePoint);
				thread.t = * (const ofVec3f*) evaluatePoint.getOutput() - thread.s;
				
				cout << "Thread set to: " << thread.s << "\t->\t" << (thread.s + thread.t) << endl << endl;
			} else {
				cout << "We don't have a range of points so can't triangulate thread" << endl;
			}
		}
	}
}
