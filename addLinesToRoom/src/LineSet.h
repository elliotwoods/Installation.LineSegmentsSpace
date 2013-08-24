#pragma once
#include "Line.h"

#define NORMAL_SIZE 3.0f
#define SELECTED_SIZE 5.0f
#define HOVER_SIZE 10.0f
#define SHIFT_SIZE 10.0f
#define NEWLINE_SIZE 10.0f

class LineSet {
public:
	LineSet();
	
	void draw(bool shift, bool shadows);
	
	void load(string filename);
	void loadBinary(string filename);
	void loadXml(string filename);
	
	void save(string filename) const;
	
	void clear();
	void clearSelection();
	void clearHover();
	
	int getCountAll() const;
	int getCountSelected() const;	
	float getLengthAll() const;
	float getLengthSelected() const;
	
	Line::Index findIndexUnderCursor();
	
protected:
	map<Line::Index, Line> lines;
	set<string> layers;
	
	set<Line::Index> selection;
	Line::Index lineHover;
	
	ofFbo indexBuffer;
};