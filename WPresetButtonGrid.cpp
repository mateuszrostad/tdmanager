#include "WPresetButtonGrid.hpp"
#include <Wt/WGridLayout>
#include <Wt/WPushButton>
#include "Device.hpp"


WPresetButtonGrid* WPresetButtonGrid::parseXML(XMLElement* xmlElement)
{
	// No need for validation of xmlElement, contains no attributes
	
	WPresetButtonGrid* presetGrid = new WPresetButtonGrid();

	for (XMLElement* xmlButton = xmlElement->FirstChildElement("PresetButton"); xmlButton != nullptr; xmlButton = xmlButton->NextSiblingElement("PresetButton"))
		presetGrid->addButton(WPresetButton::parseXML(xmlButton));

	return presetGrid;
}


WPresetButtonGrid::WPresetButtonGrid(Wt::WContainerWidget* parent) : Wt::WContainerWidget(parent), rowCurrent(0), colCurrent(0)
{

	setStyleClass("wpresetbuttongrid_container");
	setMargin(0);
	
	layout = new Wt::WGridLayout();
	//layout->setContentsMargins(4, 4, 4, 4);
	layout->setContentsMargins(4, 1, 4, 1);
	layout->setHorizontalSpacing(4);
	layout->setVerticalSpacing(4);
	setLayout(layout);
	
}


void WPresetButtonGrid::incrRowCol(int& row, int& col)
{
	if (!(++col %= 3)) row++;
}


void WPresetButtonGrid::addButton(WPresetButton* button)
{
	layout->addWidget(button, rowCurrent, colCurrent);
	incrRowCol(rowCurrent, colCurrent);
	buttons.push_back(button);
}


// WPresetButton implementation

#include <thread>
#include <chrono>
#include <iostream>

WPresetButton* WPresetButton::parseXML(XMLElement* xmlElement)
{
	ConfigLoader::validateElement(*xmlElement, "PresetButton", {"string"}, true, true);

	WPresetButton* presetButton = new WPresetButton(xmlElement->Attribute("string"));

	for (XMLElement* xmlActuator = xmlElement->FirstChildElement("Actuator"); xmlActuator != nullptr; xmlActuator = xmlActuator->NextSiblingElement("Actuator"))
		presetButton->addActuator(Device::getDeviceActuatorFromXML(xmlActuator));

	return presetButton;
}


WPresetButton::WPresetButton(const Wt::WString& title, Wt::WContainerWidget* parent) : WPushButton(title, parent), _isDown(false), ignoreMouseButtonUp(false)
{
	setStyleClass("wpresetbuttongrid_button");
	//clicked().connect(this, &WPresetButton::actuate);
	mouseWentDown().connect(this, &WPresetButton::mouseButtonDown);
	mouseWentUp().connect(this, &WPresetButton::mouseButtonUp);
}


void WPresetButton::addActuator(DeviceActuator actuator)
{
	actuators.push_back(actuator);
}


void WPresetButton::addActuator(DeviceActuatorList _actuators)
{
	actuators.insert(actuators.end(), _actuators.begin(), _actuators.end());
}


void WPresetButton::mouseButtonDown(const Wt::WMouseEvent& event)
{
	mtx.lock();
	_isDown = true;
	mtx.unlock();
	
	std::thread thr(&WPresetButton::whileMouseButtonDown, this);
	thr.detach();
}

void WPresetButton::whileMouseButtonDown()
{
	std::this_thread::sleep_for(std::chrono::seconds(2));

	mtx.lock();
	if (isDown())
	{
		ignoreMouseButtonUp = true;
		mtx.unlock();
		actuate();
	}
	else
		mtx.unlock();
}


void WPresetButton::mouseButtonUp(const Wt::WMouseEvent& event)
{
	mtx.lock();
	_isDown = true;
	mtx.unlock();
	
	if (event.button() == Wt::WMouseEvent::LeftButton)
	{
	mtx.lock();
		if (ignoreMouseButtonUp)
		{
			ignoreMouseButtonUp = false;
			mtx.unlock();
		}
		else
		{
			mtx.unlock();
			actuate();
		}
	}
}


void WPresetButton::actuate()
{
	for (auto actuator : actuators)
		actuator();
}
