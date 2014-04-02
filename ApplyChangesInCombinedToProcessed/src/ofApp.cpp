#include "ofApp.h"

class Thread {
public:
	ofVec2f projectorStart;
	ofVec2f projectorEnd;
};

//--------------------------------------------------------------
void ofApp::setup(){
	ofXml newCombined;
	ofXml processed;
	map<int, Thread> combinedContents;
	vector<ofVec2f> movement;
	
	auto result = ofSystemLoadDialog("Select new lines_combined.xml");
	if (!result.bSuccess) {
		ofExit();
	}
	newCombined.load(result.filePath);
	
	auto result2 = ofSystemLoadDialog("Select existing lines_processed.xml");
	if (!result2.bSuccess) {
		ofExit();
	}
	processed.load(result2.filePath);
	
	if(newCombined.setTo("DocumentElement")) {
		if(newCombined.setTo("SpreadTable[0]")) {
			do {
				Thread newLine;
				int ID = newCombined.getIntValue("_x0020_ID");
				ofVec2f start, end;
				start.x = newCombined.getFloatValue("x1");
				start.y = newCombined.getFloatValue("_x0020_y1");
				end.x = newCombined.getFloatValue("_x0020_x2");
				end.y = newCombined.getFloatValue("_x0020_y2");
				newLine.projectorStart = start;
				newLine.projectorEnd = end;
				combinedContents.insert(pair<int,Thread>(ID,newLine));
			} while (newCombined.setToSibling());
			newCombined.setToParent();
		}
		newCombined.setToParent();
	}
	
	if(processed.setTo("Lines")) {
		if(processed.setTo("Line[0]")) {
			do {
				int ID = processed.getIntValue("ID");
				if(combinedContents.count(ID) == 0) {
					ofLogError() << "ID: " << ID << " not found in new combined lines";
				} else {
					auto & line = combinedContents[ID];
					ofVec2f oldStart = processed.getValue<ofVec2f>("projectorStart");
					ofVec2f oldEnd = processed.getValue<ofVec2f>("projectorEnd");
					
					if(oldStart != line.projectorStart || oldEnd != line.projectorEnd) {
						processed.setValue("projectorStart", ofToString(line.projectorStart));
						processed.setValue("projectorEnd", ofToString(line.projectorEnd));
						movement.push_back(line.projectorStart - oldStart);
						if (movement.back().length() > 0.001) {
							ofLogNotice() << "****start***";
						}
						movement.push_back(line.projectorEnd - oldEnd);
						if (movement.back().length() > 0.001) {
							ofLogNotice() << "****end***";
						}
						ofLogNotice() << "Updated ID: " << ID << " to (" << oldEnd << "\t -> \t" << line.projectorEnd << ")";
						

					}
				}
			} while (processed.setToSibling());
		}
		processed.setToParent();
	}
	processed.save(result2.filePath);
	
	ofVec2f totalMovement;
	for(auto move : movement) {
		totalMovement += move;
	}
	ofLogNotice() << "Total movement: " << totalMovement;
	ofLogNotice() << "Mean movement: " << (totalMovement / (float) movement.size());
	ofLogNotice() << "Mean movement: " << (ofVec2f(1920,1080) * totalMovement / (2 * (float) movement.size())) << "px";
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
