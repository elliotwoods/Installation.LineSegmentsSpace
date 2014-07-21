#pragma once
#include "Thread.h"
#include "LayerSet.h"

#include <deque>
#define NORMAL_SIZE 3.0f
#define SELECTED_SIZE 5.0f
#define HOVER_SIZE 10.0f
#define SHIFT_SIZE 10.0f
#define NEWLINE_SIZE 10.0f

#define PICK_SAMPLE_DOWN 2

#define UNDO_STACK_LENGTH 10

class ThreadSet {
public:
	ThreadSet();
	
	void draw(bool shift, bool shadows);
	void updateIndexBuffer(bool shift);
	
	void add(Thread);
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
	
	Thread::Index getHoverIndex() const;
	Thread& getHover();
	
	Thread::Index getNextFreeIndex() const;
	ofFbo & getIndexBufferFbo() { return this->indexBufferFbo; }
	void splitThreadAt(Thread::Index, const ofVec3f&);
	
	void undo();
	void addUndoSnapshot();
	int getUndoStackLength();
	
	LayerSet& getLayers() { return this->layers; }
	ofEvent<LayerSet> onLayersChange;
	void addLayer(const string&);
	void deleteLayer(ofPtr<Layer>);
	void renameLayer(ofPtr<Layer>, const string&);
	void selectThreadsForLayer(ofPtr<Layer>);
	void changeLayerVisibility();
protected:
	Thread::Index getIndexAtScreen(int x, int y);
	
	map<Thread::Index, ofPtr<Thread> > lines;
	LayerSet layers;
	
	set<Thread::Index> selection;
	Thread::Index hover;
	
	ofFbo indexBufferFbo;
	ofPixels indexBufferPixels;
	
	deque<ofXml> undoStack;
};