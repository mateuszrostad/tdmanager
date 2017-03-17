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

WPresetButton* WPresetButton::parseXML(XMLElement* xmlElement)
{
	ConfigLoader::validateElement(*xmlElement, "PresetButton", {"string"}, true, true);

	WPresetButton* presetButton = new WPresetButton(xmlElement->Attribute("string"));

	for (XMLElement* xmlActuator = xmlElement->FirstChildElement("Actuator"); xmlActuator != nullptr; xmlActuator = xmlActuator->NextSiblingElement("Actuator"))
		presetButton->addActuator(Device::getDeviceActuatorFromXML(xmlActuator));

	return presetButton;
}


WPresetButton::WPresetButton(const Wt::WString& title, Wt::WContainerWidget* parent) : WPushButton(title, parent)
{
	setStyleClass("wpresetbuttongrid_button");
	clicked().connect(this, &WPresetButton::actuate);
}


void WPresetButton::addActuator(DeviceActuator actuator)
{
	actuators.push_back(actuator);
}


void WPresetButton::addActuator(DeviceActuatorList _actuators)
{
	actuators.insert(actuators.end(), _actuators.begin(), _actuators.end());
}


void WPresetButton::actuate()
{
	for (auto actuator : actuators)
		actuator();
}
