#ifndef WPowerSwitch_hpp
#define WPowerSwitch_hpp

#include "BasicDevices.hpp"
//#include <set>
#include <map>
#include <Wt/WContainerWidget>

namespace Wt
{
	class WText;
	class WPushButton;
	class WString;
}


/*******************************************************************************
 * class WPowerSwitch
 */

// Abstract class
class WPowerSwitch : public Wt::WContainerWidget
{
	
public:

	enum Style {Header, Body};
	
	WPowerSwitch(                                  Wt::WString, Style style = Body, Wt::WContainerWidget* parent = 0);
	WPowerSwitch(PowerSwitchInterface::PowerState, Wt::WString, Style style = Body, Wt::WContainerWidget* parent = 0);

	Wt::WPushButton* getButtonOn()  {return buttonOn;}
	Wt::WPushButton* getButtonOff() {return buttonOff;}
	
	void updatePowerStateIndicator(PowerSwitchInterface::PowerState);

protected:

	virtual void buttonOn_clicked() = 0;
	virtual void buttonOff_clicked() = 0;
	
	bool buttonOnActive;
	bool buttonOffActive;
	
private:

	Wt::WText*       text;
	Wt::WPushButton* buttonOn;
	Wt::WPushButton* buttonOff;
	
};


/*******************************************************************************
 * class WPowerSwitchSingleDevice
 */

//class DevicePowerSwitch;

class WPowerSwitchSingleDevice : public WPowerSwitch
{

public:

	WPowerSwitchSingleDevice(                  Wt::WString, Style style = Body, Wt::WContainerWidget* parent = 0);
	WPowerSwitchSingleDevice(Device::DeviceId, Wt::WString, Style style = Body, Wt::WContainerWidget* parent = 0);
	//WPowerSwitchSingleDevice(DevicePowerSwitch*, Wt::WString, Style style = Body, Wt::WContainerWidget* parent = 0); // Unsafe

	~WPowerSwitchSingleDevice();
	
	void setDevice(Device::DeviceId);
	void removeDevice();
	//void setDevice(DevicePowerSwitch*); // Unsafe
	//DevicePowerSwitch* getDevice();
	bool             getDeviceIsSet() {return deviceIsSet;}
	Device::DeviceId getDeviceId()    {return deviceId;}

	virtual void buttonOn_clicked();
	virtual void buttonOff_clicked();
	
protected:

	Device::DeviceId deviceId;
	bool             deviceIsSet;

	int              slotIdPowerState, slotIdBeforeDelete;
	//bool             connectedToDeviceSignal;
	
};


/*******************************************************************************
 * class WPowerSwitchDeviceGroup
 */

class WPowerSwitchDeviceGroup : public WPowerSwitch
{

public:

	//typedef Device::GenericDeviceMap<DevicePowerSwitch*> DeviceMap;
	//typedef std::vector<DevicePowerSwitch*> DevicePtrListType;
	//typedef std::set<Device::DeviceId> DeviceIds;

	WPowerSwitchDeviceGroup(           Wt::WString, Style style = Body, Wt::WContainerWidget* parent = 0);
	//WPowerSwitchDeviceGroup(DeviceIds, Wt::WString, Style style = Body, Wt::WContainerWidget* parent = 0);
	//WPowerSwitchDeviceGroup(DeviceMap,   Wt::WString, Style style = Body, Wt::WContainerWidget* parent = 0);
	
	~WPowerSwitchDeviceGroup();

	//void addDevice(DevicePowerSwitch*);
	void addDevice(Device::DeviceId);
	//void removeDevice(DevicePowerSwitch*);
	void removeDevice(Device::DeviceId);
	void removeAllDevices();

	virtual void buttonOn_clicked();
	virtual void buttonOff_clicked();
	
protected:

	//DeviceIds deviceIds;
	typedef int SlotId;
	std::map<Device::DeviceId, SlotId> deviceMap;
	
	
};


#endif