#include "ThreadSet.h"

//---------
void ThreadSet::load(string filename) {
	ofXml xml;
	xml.load(filename);
	if (xml.setTo("DocumentElement")) {
		if (xml.setTo("SpreadTable[0]")) {
			do {
				auto dataItems = xml.getAttributes();
				Thread newThread;
				ofVec2f & start = newThread.projectorStart;
				ofVec2f & end = newThread.projectorEnd;
				
				start.x = xml.getFloatValue("x1");
				start.y = xml.getFloatValue("_x0020_y1");
				end.x = xml.getFloatValue("_x0020_x2");
				end.y = xml.getFloatValue("_x0020_y2");
				
				newThread.projector = xml.getIntValue("_x0020_iProjector");
				newThread.ID = xml.getIntValue("_x0020_ID");
				this->push_back(newThread);
			} while (xml.setToSibling());
		}
		xml.setToParent();
	}
}

//---------
void ThreadSet::save(string filename) {
	if (filename == "") {
		auto result = ofSystemSaveDialog("lines_processed.xml", "Save xml of lines");
		if (!result.bSuccess) {
			ofLogError() << "Didn't select a filename";
			return;
		} else {
			filename = result.filePath;
		}
	}
	
	ofLogNotice() << "Saving all lines to " << filename;
	ofXml xml;
	xml.addChild("Lines");
	xml.setTo("Lines");
	for (auto thread : *this) {
		ofXml innerXml = thread.getXml();
		xml.addXml(innerXml);
	}
	xml.save(filename);
	ofLogNotice() << "Saving complete";
}

//---------
void ThreadSet::drawProjectionSpace() {
	ofPushView();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	for(auto thread : *this) {
		thread.drawProjectionSpace();
	}
	ofPopView();
}

//----------
void ThreadSet::drawWorldSpace() {
	for(auto thread : *this) {
		thread.drawWorldSpace(3.0f);
	}
}