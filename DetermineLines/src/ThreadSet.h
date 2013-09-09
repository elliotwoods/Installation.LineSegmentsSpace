#pragma once
#include "Thread.h"

class ThreadSet : public vector<Thread> {
public:
	void load(string filename);
	void save(string filename="");
	void drawProjectionSpace();
	void drawWorldSpace();
};