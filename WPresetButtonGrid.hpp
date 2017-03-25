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


#include <mutex>
#include <Wt/WPushButton>
#include <Wt/WEvent>
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
	void actuate();
	
	bool isDown() {return _isDown;}

protected:
	
	DeviceActuatorList actuators;
	bool _isDown;
	bool ignoreMouseButtonUp;
	std::recursive_mutex mtx;

	
	void mouseButtonDown(const Wt::WMouseEvent&);
	void mouseButtonUp(const Wt::WMouseEvent&);
	void whileMouseButtonDown();
};


#endif