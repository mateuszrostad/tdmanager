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

//#include <thread>
//#include <chrono>
#include <iostream>
#include <functional>
#include <Wt/WDialog>
#include <Wt/WLabel>
#include <Wt/WTimer>


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
	//clicked().connect(this, &WPresetButton::actuate);
	mouseWentDown().connect(this, &WPresetButton::mouseButtonDown);
	mouseWentUp().connect(this, &WPresetButton::mouseButtonUp);
	
	wtimer = new Wt::WTimer();
	wtimer->setInterval(2000);
	wtimer->setSingleShot(true);
	wtimer->timeout().connect(this, &WPresetButton::longPressDialog);
}


WPresetButton::~WPresetButton()
{
	delete wtimer;
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
	if (event.button() == Wt::WMouseEvent::LeftButton)
	{
		wtimer->start();
	}
}


void WPresetButton::mouseButtonUp(const Wt::WMouseEvent& event)
{
	
	if (event.button() == Wt::WMouseEvent::LeftButton)
	{
		if (wtimer->isActive())
		{
			wtimer->stop();
			actuate();
		}
	}
}


void WPresetButton::actuate()
{
	for (auto actuator : actuators)
		actuator();
}


void WPresetButton::longPressDialog()
{
	Wt::WDialog* dialog = new Wt::WDialog("Edit button");
	Wt::WLabel* label = new Wt::WLabel("Her kommer innhold", dialog->contents());
	Wt::WPushButton* ok = new Wt::WPushButton("Ok", dialog->footer());
	Wt::WPushButton* remove = new Wt::WPushButton("Remove", dialog->footer());
	Wt::WPushButton* cancel = new Wt::WPushButton("Cancel", dialog->footer());
	dialog->rejectWhenEscapePressed();
	ok->clicked().connect(dialog, &Wt::WDialog::accept);
	remove->clicked().connect(dialog, &Wt::WDialog::reject);
	cancel->clicked().connect(dialog, &Wt::WDialog::reject);
	dialog->finished().connect(std::bind([=](){delete dialog;}));
	dialog->show();
}

