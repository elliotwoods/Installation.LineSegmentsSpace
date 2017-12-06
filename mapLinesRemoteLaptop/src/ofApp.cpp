#include "pch.h"
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60);

	this->gui.init();

	auto strip = this->gui.addStrip();
	{
		strip->setCellSizes({ -1, 300 });
	}

	{
		this->projectorPreview.allocate(1280, 800, OF_IMAGE_GRAYSCALE);
		this->projectorPreview.getPixels().setColor(ofColor(0));
		this->projectorPreview.update();
	}

	auto mainPanel = ofxCvGui::Panels::makeImage(this->projectorPreview);
	{
		mainPanel->onDrawImage += [this](ofxCvGui::DrawImageArguments & args) {
			this->drawCanvas();
		};
		auto mainPanelRaw = mainPanel.get();
		mainPanel->onMouse.addListener([this, mainPanelRaw](ofxCvGui::MouseArguments & args) {
			//args.takeMousePress(mainPanelRaw);
			if (args.action == ofxCvGui::MouseArguments::Action::Moved) {
				this->pickHover(args.local);
			}
			if (args.action == ofxCvGui::MouseArguments::Action::Dragged) {
				auto selection = this->selection.lock();
				if (selection) {
					auto previousPosition = ofVec3f(args.local - args.movement) * this->panelToCanvas;
					auto newPosition = ofVec3f(args.local) * this->panelToCanvas;
					auto movement = newPosition - previousPosition;
					movement *= ofGetKeyPressed(OF_KEY_SHIFT) ? 1.0 : 0.1;
					if (args.button == 0) {
						//movement along the line

						auto & vertex = this->lineEndSelection.get()
							? selection->End
							: selection->Start;

						//clamp movement to line
						if (ofGetKeyPressed(OF_KEY_LEFT_ALT)) {
							auto lineVector = (selection->End - selection->Start).getNormalized();
							movement = movement.dot(lineVector) * lineVector;
						}
						vertex += movement;
					}
					else {
						selection->Start += movement;
						selection->End += movement;
					}
					this->pushUpdate(selection);
				}
			}
		}, this, -1);

		this->mainPanel = mainPanel;
		strip->add(mainPanel);
	}

	auto widgets = ofxCvGui::Panels::makeWidgets();
	{
		widgets->addTitle("Line Segments Space");
		widgets->addButton("Refresh", [this]() {
			this->refresh();
		});
		widgets->addEditableValue<string>(this->myName)->onValueChange += [](const string & name) {
			ofFile file;
			file.open("myName.txt", ofFile::WriteOnly, false);
			file << name;
		};

		widgets->addLiveValue<size_t>("Line count", [this]() {
			return this->lines.size();
		});

		widgets->addLiveValue<string>("Hover", [this]() {
			auto hover = this->hover.lock();
			if (hover) {
				return ofToString(hover->LineIndex);
			}
			else {
				return string();
			}
		});

		widgets->addLiveValue<string>("Selection", [this]() {
			auto selection = this->selection.lock();
			if (selection) {
				return ofToString(selection->LineIndex);
			}
			else {
				return string();
			}
		});
		{
			auto selectProjector = widgets->addMultipleChoice("Projector selection");
			for (int i = 0; i < 8; i++) {
				selectProjector->addOption(ofToString(i));
			}
			selectProjector->entangle(this->projectorSelection);
		}
		{
			auto selectLineEnd = widgets->addMultipleChoice("Line end [TAB]", { "A", "B" });
			selectLineEnd->entangle(this->lineEndSelection);
		}
		widgets->addButton("Select by index", [this]() {
			auto response = ofSystemTextBoxDialog("Line index");
			if (response.empty()) {
				return;
			}

			auto index = ofToInt(response);
			this->selectByIndex(index);
		});
		widgets->addButton("Delete", [this]() {
			this->deleteLine();
		}, OF_KEY_BACKSPACE);

		strip->add(widgets);
	}

	this->thread = std::thread([this]() {
		ofURLFileLoader urlFileLoader;
		ofHttpRequest request;
		while(this->running) {
			bool found = false;
			while (this->requests.tryReceive(request)) {
				found = true;
			}
			if (found) {
				urlFileLoader.handleRequest(request);
			}
			ofSleepMillis(1);
		}
	});

	{
		ofFile file;
		file.open("myName.txt", ofFile::ReadOnly, false);
		auto contents = file.readToBuffer();
		auto name = contents.getText();
		if (name.empty()) {
			auto result = ofSystemTextBoxDialog("Please type in your name:");
			this->myName = result;
		}
		else {
			this->myName = name;
		}
	}

	this->refresh();
}

//--------------------------------------------------------------
void ofApp::update(){
	{
		ofMatrix4x4 imageToCanvas;

		imageToCanvas.scale(2.0f / this->projectorPreview.getWidth(), -2.0f / this->projectorPreview.getHeight(), 1.0f);
		imageToCanvas.translate(-1, 1, 0);

		this->panelToCanvas = this->mainPanel->getPanelToImageTransform().getInverse() * imageToCanvas;
	}

	{
		auto hover = this->hover.lock();
		if (hover) {
			if (hover->ProjectorIndex != this->projectorSelection.get()) {
				this->hover.reset();
			}
		}
	}

	{
		auto selection = this->selection.lock();
		if (selection) {
			if (selection->ProjectorIndex != this->projectorSelection.get()) {
				this->selection.reset();
			}
		}
	}

	{
		//ping line
		if (ofGetFrameNum() % 100 == 0) {
			this->refresh();
			this->pingLine();
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

}

void ofApp::exit()
{
	this->running = false;
	this->thread.join();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case 'r':
	case 'R':
		this->refresh();
		break;
	case 'n':
	case 'N':
		this->addLine();
		break;
	case ' ':
		this->selectLine();
		break;
	case OF_KEY_TAB:
		this->lineEndSelection = 1 - this->lineEndSelection.get();
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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::refresh() {
	auto selection = this->selection.lock();
	auto response = ofLoadURL(this->serverAddress.get() + "/api/lines");
	try {
		auto responseData = json::parse(response.data.getText());
		if (!responseData["success"]) {
			ofLogError() << responseData;
			throw(exception());
		}
		const auto & jsonLines = responseData["content"];
		this->lines.clear();
		for (const auto & jsonLine : jsonLines) {
			auto newLine = toLine(jsonLine);
			this->lines.emplace(newLine->LineIndex, newLine);
		}
		if (selection) {
			this->selectByIndex(selection->LineIndex);
		}
	}
	catch (std::exception & e) {
		ofLogError() << e.what();
	}
}

//--------------------------------------------------------------
void ofApp::addLine() {
	ofVec2f start = this->pickCoordinate;
	ofVec2f end = start + ofVec2f(0, 0.1f / this->mainPanel->getZoomFactor());
	
	if (!ofRectangle(-1, -1, 2, 2).inside(start)) {
		return;
	}

	auto requestData = json{
		{"ProjectorIndex", this->projectorSelection.get()}
		, {"Start", {
			{"x", start.x}
			, {"y", start.y}
		}}
		, { "End",{
			{ "x", end.x }
			,{ "y", end.y }
		}
		, {"LastEditBy", this->myName.get() }
		}
	};

	ofHttpRequest request;
	{
		request.body = requestData.dump();
		request.url = this->serverAddress.get() + "/api/newline";
		request.method = ofHttpRequest::Method::POST;
	}

	auto response = this->urlFileLoader.handleRequest(request);
	try {
		auto responseData = json::parse(response.data.getText());
		if (!responseData["success"]) {
			ofLogError("addLine") << responseData["error"];
			throw(exception());
		}

		const auto & content = responseData["content"];
		auto newLine = toLine(content);
		this->lines.emplace(newLine->LineIndex, newLine);
	}
	catch (const exception & e) {
		ofLogError("addLine") << e.what();
	}
}

//--------------------------------------------------------------
void ofApp::pushUpdate(shared_ptr<Line> line)
{
	auto requestData = json{
		{"LineIndex", line->LineIndex}
		, { "ProjectorIndex", line->ProjectorIndex }
		,{ "Start",{
			{ "x", line->Start.x }
			,{ "y", line->Start.y }
		} }
		,{ "End",{
			{ "x", line->End.x }
			,{ "y", line->End.y }
		} }
		, {"LastEditBy", this->myName.get() }
	};

	ofHttpRequest request;
	{
		request.body = requestData.dump();
		request.url = this->serverAddress.get() + "/api/updateline";
		request.method = ofHttpRequest::Method::POST;
	}

	this->requests.send(request);
}

//--------------------------------------------------------------
void ofApp::pickHover(const ofVec2f & panelCoordinate) {
	auto canvasCoordinate = (ofVec3f)panelCoordinate * this->panelToCanvas;
	this->hover = this->pickLine(canvasCoordinate);
}

//--------------------------------------------------------------
//https://stackoverflow.com/questions/4438244/how-to-calculate-shortest-2d-distance-between-a-point-and-a-line-segment-in-all
double LSDotProduct(ofVec2f pointA, ofVec2f pointB, ofVec2f pointC)
{
	ofVec2f AB, BC;
	AB[0] = pointB[0] - pointA[0];
	AB[1] = pointB[1] - pointA[1];
	BC[0] = pointC[0] - pointB[0];
	BC[1] = pointC[1] - pointB[1];
	double dot = AB[0] * BC[0] + AB[1] * BC[1];

	return dot;
}

//Compute the cross product AB x AC
double LSCrossProduct(ofVec2f pointA, ofVec2f pointB, ofVec2f pointC)
{
	ofVec2f AB, AC;

	AB[0] = pointB[0] - pointA[0];
	AB[1] = pointB[1] - pointA[1];
	AC[0] = pointC[0] - pointA[0];
	AC[1] = pointC[1] - pointA[1];
	double cross = AB[0] * AC[1] - AB[1] * AC[0];

	return cross;
}

//Compute the distance from A to B
double LSDistance(ofVec2f pointA, ofVec2f pointB)
{
	double d1 = pointA[0] - pointB[0];
	double d2 = pointA[1] - pointB[1];

	return sqrt(d1 * d1 + d2 * d2);
}

double LSLineToPointDistance2D(ofVec2f pointA, ofVec2f pointB, ofVec2f pointC,
	bool isSegment)
{
	double dist = LSCrossProduct(pointA, pointB, pointC) / LSDistance(pointA, pointB);
	if (isSegment)
	{
		double dot1 = LSDotProduct(pointA, pointB, pointC);
		if (dot1 > 0)
			return LSDistance(pointB, pointC);

		double dot2 = LSDotProduct(pointB, pointA, pointC);
		if (dot2 > 0)
			return LSDistance(pointA, pointC);
	}
	return abs(dist);
}

shared_ptr<Line> ofApp::pickLine(const ofVec2f & canvasCoordinate) {
	auto closestLine = shared_ptr<Line>();
	float closestDistance = numeric_limits<float>::max();
	for (const auto & lineIt : this->lines) {
		const auto & line = lineIt.second;
		if (line->ProjectorIndex != this->projectorSelection.get()) {
			continue;
		}

		auto distance = LSLineToPointDistance2D(line->Start, line->End, canvasCoordinate, true);
		if (distance < closestDistance) {
			closestDistance = distance;
			closestLine = lineIt.second;
		}
	}
	this->pickCoordinate = canvasCoordinate;
	return closestLine;
}

//--------------------------------------------------------------
void ofApp::selectLine() {
	this->selection = this->hover;
	auto selection = this->selection.lock();
	if (selection) {
		//select the closest line ending
		this->lineEndSelection = (selection->Start - this->pickCoordinate).lengthSquared()
			> (selection->End - this->pickCoordinate).lengthSquared();
	}
}

//--------------------------------------------------------------
void ofApp::deleteLine() {
	auto selection = this->selection.lock();
	if (selection) {
		this->lines.erase(selection->LineIndex);
		auto requestData = json{
			{ "LineIndex", selection->LineIndex }
		};

		ofHttpRequest request;
		{
			request.body = requestData.dump();
			request.url = this->serverAddress.get() + "/api/deleteline";
			request.method = ofHttpRequest::Method::POST;
		}

		auto response = this->urlFileLoader.handleRequest(request);
		try {
			auto responseData = json::parse(response.data.getText());
			if (!responseData["success"]) {
				ofLogError("deleteLine") << responseData["error"];
				throw(exception());
			}

			const auto & content = responseData["content"];
			//do we need to do anything here?
		}
		catch (const exception & e) {
			ofLogError("deleteLine") << e.what();
			this->refresh();
		}
	}
}

//--------------------------------------------------------------
void ofApp::pingLine() {
	auto selection = this->selection.lock();
	if (selection) {
		auto requestData = json{
			{ "LineIndex", selection->LineIndex }
		};

		ofHttpRequest request;
		{
			request.body = requestData.dump();
			request.url = this->serverAddress.get() + "/api/pingline";
			request.method = ofHttpRequest::Method::POST;
		}

		auto response = this->urlFileLoader.handleRequest(request);
		try {
			auto responseData = json::parse(response.data.getText());
			if (!responseData["success"]) {
				ofLogError("pingLine") << responseData["error"];
				throw(exception());
			}

			const auto & content = responseData["content"];
			//do we need to do anything here?
		}
		catch (const exception & e) {
			ofLogError("pingLine") << e.what();
			this->refresh();
		}
	}
}

//--------------------------------------------------------------
void ofApp::selectByIndex(int index) {
	auto findLine = this->lines.find(index);
	if (findLine != this->lines.end()) {
		this->selection = findLine->second;
		this->projectorSelection = findLine->second->ProjectorIndex;
	}
}

//--------------------------------------------------------------
void ofApp::drawCanvas() {
	ofPushMatrix();
	{
		ofScale(this->projectorPreview.getWidth(), this->projectorPreview.getHeight());
		ofTranslate(0.5, 0.5);
		ofScale(0.5f, -0.5f);

		ofPushStyle();
		{
			//draw lines
			{
				ofSetColor(50);
				for (const auto & lineIt : this->lines) {
					auto & line = lineIt.second;
					if (line->ProjectorIndex != this->projectorSelection.get()) {
						continue;
					}

					if (line->Age < 5.0f){// && line->LastEditBy != this->myName.get()) {
						ofDrawBitmapString(line->LastEditBy, (line->Start + line->End) / 2.0f);
					}

					ofDrawLine(line->Start, line->End);
				}
			}

			//draw hover
			{
				auto hover = this->hover.lock();
				ofSetColor(100);
				if (hover) {
					ofDrawLine(hover->Start, hover->End);
				}
			}

			//draw selection
			{
				auto selection = this->selection.lock();
				auto b = ofGetElapsedTimeMillis() / 10;
				b %= 155;
				b += 100;
				ofSetColor(255, b, b);
				if (selection) {
					ofDrawLine(selection->Start, selection->End);

					ofSetLineWidth(1.0f);
					ofNoFill();
					ofDrawCircle(this->lineEndSelection.get() == 0
						? selection->Start
						: selection->End
						, 0.01f);
				}
			}
		}
		ofPopStyle();
		ofDrawCircle(this->pickCoordinate, 0.01);
	}
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

shared_ptr<Line> toLine(const json & jsonLine)
{
	auto lineIndex = (int)jsonLine["LineIndex"];
	auto projectorIndex = (int)jsonLine["ProjectorIndex"];

	ofVec2f start, end;
	start.x = jsonLine["Start"]["x"];
	start.y = jsonLine["Start"]["y"];
	end.x = jsonLine["End"]["x"];
	end.y = jsonLine["End"]["y"];

	string lastEditBy;
	float age = 100.0f;
	try {
		if (jsonLine.find("LastEditBy") == jsonLine.end()) {
			throw(exception());
		}
		if (jsonLine.find("Age") == jsonLine.end()) {
			throw(exception());
		}
		lastEditBy = jsonLine["LastEditBy"].get<string>();
		age = jsonLine["Age"].get<float>();
	}
	catch (...) {

	}

	return make_shared<Line>(Line{
		lineIndex,
		projectorIndex,
		start,
		end,
		lastEditBy,
		age
	});
}
