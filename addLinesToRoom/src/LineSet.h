#pragma once
#include "Line.h"

#include <deque>
#define NORMAL_SIZE 3.0f
#define SELECTED_SIZE 5.0f
#define HOVER_SIZE 10.0f
#define SHIFT_SIZE 10.0f
#define NEWLINE_SIZE 10.0f

#define PICK_SAMPLE_DOWN 2

#define UNDO_STACK_LENGTH 10

class LineSet {
public:
	LineSet();
	
	void draw(bool shift, bool shadows);
	void updateIndexBuffer(bool shift);
	
	void add(Line);
	void toggleHoverToSelection();
	
	void load(string filename);
	void loadBinary(string filename);
	void loadXml(string filename);
	void load(ofXml&);
	void save(string filename) const;
	ofXml getStateAsXml() const;
	
	void clear();
	void clearSelection();
	void clearHover();
	void deleteSelected();
	
	int getCountAll() const;
	int getCountSelected() const;	
	float getLengthAll() const;
	float getLengthSelected() const;
	
	Line::Index getHoverIndex() const;
	Line& getHover();
	
	Line::Index getNextFreeIndex() const;
	ofFbo & getIndexBufferFbo() { return this->indexBufferFbo; }
	void splitLineAt(Line::Index, const ofVec3f&);
	
	void undo();
	void addUndoSnapshot();
	int getUndoStackLength();
protected:
	Line::Index getIndexAtScreen(int x, int y);
	
	map<Line::Index, Line> lines;
	set<string> layers;
	
	set<Line::Index> selection;
	Line::Index hover;
	
	ofFbo indexBufferFbo;
	ofPixels indexBufferPixels;
	
	deque<ofXml> undoStack;
};