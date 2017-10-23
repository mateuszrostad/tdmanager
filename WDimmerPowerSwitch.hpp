#ifndef WDimmerPowerSwitch_hpp
#define WDimmerPowerSwitch_hpp

#include <Wt/WContainerWidget>
#include "BasicDevices.hpp"
#include "ConfigLoader.hpp"

namespace Wt
{
	class WText;
	class WString;
	class WSlider;
	class WPushButton;
}


/*******************************************************************************
 * class WDimmerPowerSwitch
 */

class WDimmerPowerSwitch : public Wt::WContainerWidget
{
	
public:
	
	enum Style {Header, Body};
	
	WDimmerPowerSwitch(                                                                              Wt::WString, int, int, int, Style style = Body, Wt::WContainerWidget* parent = 0);
	WDimmerPowerSwitch(DimmerPowerSwitchInterface::PowerState, DimmerPowerSwitchInterface::DimLevel, Wt::WString, int, int, int, Style style = Body, Wt::WContainerWidget* parent = 0);

	Wt::WPushButton* getButtonOn()  {return buttonOn;}
	Wt::WPushButton* getButtonOff() {return buttonOff;}
	Wt::WSlider*     getSlider()    {return slider;}
	
	void updatePowerStateIndicator(DimmerPowerSwitchInterface::PowerState);
	void updateDimLevelIndicator(  DimmerPowerSwitchInterface::DimLevel);
	
protected:

	void setSliderParams(int, int, int);

	virtual void buttonOn_clicked() = 0;
	virtual void buttonOff_clicked() = 0;
	virtual void slider_valueChanged() = 0;
	
	bool buttonOnActive;
	bool buttonOffActive;

private:
	
	Wt::WText*       text;
	Wt::WSlider*     slider;
	Wt::WPushButton* buttonOn;
	Wt::WPushButton* buttonOff;
	
};


/*******************************************************************************
 * class WDimmerPowerSwitchSingleDevice
 */

class WDimmerPowerSwitchSingleDevice : public WDimmerPowerSwitch
{

public:

	static WDimmerPowerSwitchSingleDevice* parseXML(XMLElement*);

public:

	WDimmerPowerSwitchSingleDevice(                  Wt::WString, Style style = Body, Wt::WContainerWidget* parent = 0);
	WDimmerPowerSwitchSingleDevice(Device::DeviceId, Wt::WString, Style style = Body, Wt::WContainerWidget* parent = 0);

	~WDimmerPowerSwitchSingleDevice();

	void setDevice(Device::DeviceId);
	void removeDevice();
	bool             getDeviceIsSet() {return deviceIsSet;}
	Device::DeviceId getDeviceId()    {return deviceId;}

	virtual void buttonOn_clicked();
	virtual void buttonOff_clicked();
	virtual void slider_valueChanged();
	
protected:

	Device::DeviceId deviceId;
	bool             deviceIsSet;
	
	int              slotIdPowerState, slotIdDimLevel, slotIdBeforeDelete;
	
};


#endif