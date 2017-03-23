#ifndef WPRESETBUTTONGRID_HPP
#define WPRESETBUTTONGRID_HPP

#include <Wt/WContainerWidget>
#include <list>
#include <vector>
#include "ConfigLoader.hpp"

namespace Wt
{
	class WGridLayout;
}

class Device;
class WPresetButton;


class WPresetButtonGrid : public Wt::WContainerWidget
{
	
public:

	static WPresetButtonGrid* parseXML(XMLElement*);

public:

	WPresetButtonGrid(Wt::WContainerWidget* parent = 0);
	void addButton(WPresetButton*);

protected:

	Wt::WGridLayout* layout;
	std::vector<WPresetButton*> buttons;
	int rowCurrent, colCurrent;
	
	void incrRowCol(int& row, int& col);

};


#include <Wt/WPushButton>
#include <Wt/WMouseEvent>
#include "Device.hpp"

namespace Wt
{
	class WString;
}

class WPresetButton : public Wt::WPushButton
{
	
public:
	
	static WPresetButton* parseXML(XMLElement* xmlElement);

public:
	
	WPresetButton(const Wt::WString&, Wt::WContainerWidget* parent = 0);
	void addActuator(DeviceActuator);
	void addActuator(DeviceActuatorList);
	void mouseDown(const Wt::WMouseEvent&);
	void actuate();

protected:
	
	DeviceActuatorList actuators;

};


#endif