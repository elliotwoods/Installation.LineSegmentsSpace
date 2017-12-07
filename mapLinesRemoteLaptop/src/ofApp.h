#pragma once

#include "ofMain.h"
#include "ofxCvGui.h"

using json = nlohmann::json;

struct Line {
	int LineIndex;
	int ProjectorIndex;
	ofVec2f Start;
	ofVec2f End;
	string LastEditBy;
	float Age;
};

shared_ptr<Line> toLine(const json &);

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit() override;

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		void refresh();
		void addLine();
		void selectLine();
		void deleteLine();
		void pingLine();
		void selectByIndex(int index);
		void pushUpdate(shared_ptr<Line> line);

		void pickHover(const ofVec2f & panelCoordinate);

		shared_ptr<Line> pickLine(const ofVec2f & canvasCoordinate);

		void drawCanvas();

		ofxCvGui::Builder gui;
		ofImage projectorPreview;

		ofParameter<string> serverAddress{ "Server address", "http://192.168.0.116:8000" };
		ofParameter<string> myName{ "My name", "" };
		ofParameter<int> projectorSelection;
		ofParameter<int> lineEndSelection;

		map<int, shared_ptr<Line>> lines;
		weak_ptr<Line> hover;
		weak_ptr<Line> selection;

		ofURLFileLoader urlFileLoader;

		ofMatrix4x4 panelToCanvas;
		shared_ptr<ofxCvGui::Panels::Image> mainPanel;
		ofVec2f pickCoordinate;

		ofThreadChannel<ofHttpRequest> requests;
		bool running = true;
		std::thread thread;

		pair<string, int> highScore;
		int yourScore = 0;
		bool zKeyPressed = false; // bug means we can't use ofGetKeyPressed on OSX
		bool xKeyPressed = false; // bug means we can't use ofGetKeyPressed on OSX
};
