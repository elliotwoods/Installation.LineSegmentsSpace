#include "Layer.h"

//----------
Layer::Layer() {
	this->visible = true;
	this->selected = true;
	this->color = ofFloatColor(1,1,1);
}


//---------
void Layer::loadFrom(ofXml& xml) {
	this->visible = xml.getValue<int>("visible");
	this->selected = xml.getValue<int>("selected");
	this->color = xml.getValue<ofColor>("color");
	this->name = xml.getValue("name");
	cout << xml.getName();
}

//---------
void Layer::saveTo(ofXml& xml) {
	auto addXml = this->getXml();
	xml.addXml(addXml);
}

//---------
ofXml Layer::getXml() {
	ofXml xml;
	xml.addChild("Layer");
	xml.setTo("Layer");
	xml.addValue("visible", this->visible);
	xml.addValue("selected", this->selected);
	xml.addValue("color", this->color);
	xml.addValue("name", this->name);
	return xml;
}