//#include "Device.hpp"
#include "WPowerSwitch.hpp"
#include <iostream>
#include <cstdlib> // exit, EXIT_FAILURE
#include <cstring> // stoi
#include <functional>
#include <Wt/WApplication>
#include <Wt/WHBoxLayout>
#include <Wt/WLength>
#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WString>
//#include <Wt/WLength>
#include <Wt/WBreak>


/*******************************************************************************
 * class WPowerSwitch
 */

WPowerSwitch::WPowerSwitch(Wt::WString title, Style style, Wt::WContainerWidget* parent) :
Wt::WContainerWidget(parent), buttonOnActive(false), buttonOffActive(false), buttonOn(nullptr), buttonOff(nullptr)
{

	Wt::WString additionalStyling;
	if (style == Header)
		additionalStyling = " primetext";


	//setHeight(50);

	setStyleClass("wpowerswitch_container");
	
	Wt::WHBoxLayout* layout = new Wt::WHBoxLayout();
	layout->setContentsMargins(4, 1, 4, 1);
	layout->setSpacing(4);
	setLayout(layout);
	
	text = new Wt::WText(title);
	text->setStyleClass("wpowerswitch_text" + additionalStyling);
	layout->addWidget(text, 1, Wt::AlignMiddle);

	//new Wt::WBreak(this);
	layout->addSpacing(Wt::WLength(4, Wt::WLength::Pixel));

	buttonOff = new Wt::WPushButton("Av");
	//buttonOff->resize(Wt::WLength(10, Wt::WLength::FontEm), buttonOff->height());
	//buttonOff->setVerticalAlignment(Wt::AlignMiddle);
	buttonOff->setStyleClass("wpowerswitch_buttonOff" + additionalStyling);
	buttonOff->clicked().connect(this, &WPowerSwitch::buttonOff_clicked);
	layout->addWidget(buttonOff);

	buttonOn  = new Wt::WPushButton("På");
	//buttonOn->resize(Wt::WLength(10, Wt::WLength::FontEm), buttonOn->height());
	//buttonOn->setVerticalAlignment(Wt::AlignMiddle);
	buttonOn->setStyleClass("wpowerswitch_buttonOn" + additionalStyling);
	buttonOn->clicked().connect(this, &WPowerSwitch::buttonOn_clicked);
	layout->addWidget(buttonOn);

	//if (powerState == DevicePowerSwitch::Off || powerState == DevicePowerSwitch::On)
	//	updatePowerStateIndicator(powerState);
}


WPowerSwitch::WPowerSwitch(PowerSwitchInterface::PowerState powerState, Wt::WString title, Style style, Wt::WContainerWidget* parent) :
WPowerSwitch(title, style, parent)
{
	updatePowerStateIndicator(powerState);
}


void WPowerSwitch::updatePowerStateIndicator(PowerSwitchInterface::PowerState newPowerState)
{
	bool updated = false;
	if      (newPowerState == PowerSwitchInterface::On)
	{
		if (getButtonOff()->hasStyleClass("btn-primary"))
		{
			getButtonOff()->removeStyleClass("btn-primary");
			buttonOffActive = false;
			updated = true;
		}
		if (!(getButtonOn()->hasStyleClass("btn-primary")))
		{
			getButtonOn()->addStyleClass("btn-primary");
			buttonOnActive = true;
			updated = true;
		}
	}
	else if (!buttonOffActive && newPowerState == PowerSwitchInterface::Off) // Why the extra test for !buttonOffActive here?
	{
		if (getButtonOn()->hasStyleClass("btn-primary"))
		{
			getButtonOn()->removeStyleClass("btn-primary");
			buttonOnActive = false;
			updated = true;
		}
		if (!(getButtonOff()->hasStyleClass("btn-primary")))
		{
			getButtonOff()->addStyleClass("btn-primary");
			buttonOffActive = true;
			updated = true;
		}
	}
	else
	{
		std::cout << "WPowerSwitch::updatePowerStateIndicator(int): recieved invalid new state " << newPowerState << "." << std::endl;
	}
	
	if (updated)
		Wt::WApplication::instance()->triggerUpdate();
}


/*******************************************************************************
 * class WPowerSwitchSingleDevice
 */

WPowerSwitchSingleDevice* WPowerSwitchSingleDevice::parseXML(XMLElement* xmlElement)
{
	ConfigLoader::validateElement(*xmlElement, "PowerSwitchSingleDevice", {"deviceid", "style"}, true, true)

	WPowerSwitchSingleDevice::Style style;
	if      (std::strcmp(xmlElement->Attribute("style"), "Header"))
		style = WPowerSwitchSingleDevice::Header;
	else if (std::strcmp(xmlElement->Attribute("style"), "Body"))
		style = WPowerSwitchSingleDevice::Body;

	Device::DeviceId deviceId = std::stoi(xmlDevice->Attribute("deviceid"));

	return new WPowerSwitchSingleDevice(deviceId, Device::getDevice(deviceId)->getName(), style));
}


WPowerSwitchSingleDevice::WPowerSwitchSingleDevice(                            Wt::WString title, Style style, Wt::WContainerWidget* parent) :
WPowerSwitch(title, style, parent), deviceIsSet(false) //, connectedToDeviceSignal(false)
{}


WPowerSwitchSingleDevice::WPowerSwitchSingleDevice(Device::DeviceId newDeviceId, Wt::WString title, Style style, Wt::WContainerWidget* parent) :
WPowerSwitchSingleDevice(title, style, parent)
{
	setDevice(newDeviceId);
}


// WPowerSwitchSingleDevice::WPowerSwitchSingleDevice(DevicePowerSwitch* device, Wt::WString title, Style style, Wt::WContainerWidget* parent) :
// WPowerSwitchSingleDevice(title, style, parent)
// {
	// setDevice(device);
// }


WPowerSwitchSingleDevice::~WPowerSwitchSingleDevice()
{
	removeDevice();
	//deviceSignalDisconnect();
}


void WPowerSwitchSingleDevice::setDevice(Device::DeviceId newDeviceId)
{
	// Validate new device id
	if (!Device::isValid(newDeviceId))
	{
		std::cout << "WPowerSwitchSingleDevice::setDevice(Device::DeviceId): Cannot set device. Received invalid device id: " << newDeviceId << "." << std::endl;
		//exit(EXIT_FAILURE); // TODO: use assert and throw signal, or use exceptions
		return;
	}

	Device*               newDevice    = Device::getDevice(newDeviceId);
	PowerSwitchInterface* newDevicePSI = dynamic_cast<PowerSwitchInterface*>(newDevice);

	// Validate new device class inheritance
	if (newDevicePSI == nullptr)
	{
		std::cout << "WPowerSwitchSingleDevice::setDevice(Device::DeviceId): Cannot set device. Device pointer cannot be dynamically cast to PowerSwitchInterface*. Device id " << newDeviceId << "." << std::endl;
		//exit(EXIT_FAILURE); // TODO: use assert and throw signal, or use exceptions
		return;
	}

	// If other device currently set, remove this first
	removeDevice();

	// Set local vars
	deviceId    = newDeviceId;
	deviceIsSet = true;

	// Connect to device signals
	slotIdBeforeDelete = newDevice   ->beforeDelete()     .connect(Wt::WApplication::instance(), std::bind(&WPowerSwitchSingleDevice::removeDevice,              this));
	slotIdPowerState   = newDevicePSI->powerStateUpdated().connect(Wt::WApplication::instance(), std::bind(&WPowerSwitchSingleDevice::updatePowerStateIndicator, this, std::placeholders::_1));
	//connectedToDeviceSignal = true;
	//deviceSignalConnect();

	// Update appearence
	updatePowerStateIndicator(newDevicePSI->getPowerState());
}


void WPowerSwitchSingleDevice::removeDevice()
{
	if (deviceIsSet)
	{
		Device*               device    = Device::getDevice(deviceId);
		PowerSwitchInterface* devicePSI = dynamic_cast<PowerSwitchInterface*>(device);

		device   ->beforeDelete()     .disconnect(slotIdBeforeDelete);
		devicePSI->powerStateUpdated().disconnect(slotIdPowerState);
		deviceIsSet = false;
	}
}


// void WPowerSwitchSingleDevice::setDevice(DevicePowerSwitch* device)
// {
	// deviceSignalDisconnect();
	// registeredDeviceId = device->getId();
	// deviceIsSet = true;
	// deviceSignalConnect();
	// updatePowerStateIndicator(device->getPowerState());
// }


// DevicePowerSwitch* WPowerSwitchSingleDevice::getDevice()
// {
	// DevicePowerSwitch* device = nullptr;
	// if      (!deviceIsSet)
		// std::cout << "WPowerSwitchSingleDevice::getDevice(): Cannot get device pointer. Device not set." << std::endl;
	// else if (!deviceIdIsValid())
		// std::cout << "WPowerSwitchSingleDevice::getDevice(): Cannot get device pointer. Current device id " << deviceId << " is invalid." << std::endl;
	// else if ((device = dynamic_cast<DevicePowerSwitch*>(Device::getDevice(deviceId))) == nullptr)
		// std::cout << "WPowerSwitchSingleDevice::getDevice(): Cannot get device pointer. Device pointer cannot be dynamically cast to DevicePowerSwitch*." << std::endl;
	// return device;
// }


void WPowerSwitchSingleDevice::buttonOn_clicked()
{
	if (deviceIsSet)
		dynamic_cast<PowerSwitchInterface*>(Device::getDevice(deviceId))->turnOn();
}


void WPowerSwitchSingleDevice::buttonOff_clicked()
{
	if (deviceIsSet)
		dynamic_cast<PowerSwitchInterface*>(Device::getDevice(deviceId))->turnOff();
}


/*******************************************************************************
 * class WPowerSwitchDeviceGroup
 */

WPowerSwitchDeviceGroup* WPowerSwitchDeviceGroup::parseXML(XMLElement* xmlElement)
{
	ConfigLoader::validateElement(*xmlElement, "PowerSwitchDeviceGroup", {"string", "style"}, true, true);
	
	WPowerSwitchDeviceGroup::Style style;
	if      (std::strcmp(xmlElement->Attribute("style"), "Header"))
		style = WPowerSwitchDeviceGroup::Header;
	else if (std::strcmp(xmlElement->Attribute("style"), "Body"))
		style = WPowerSwitchDeviceGroup::Body;

	WPowerSwitchDeviceGroup* powerSwitchDeviceGroup = new WPowerSwitchDeviceGroup(xmlElement->Attribute("string"), style);

	for (XMLElement* xmlDevice = xmlElement->FirstChildElement("Device"); xmlDevice != nullptr; xmlDevice = xmlDevice->NextSiblingElement("Device"))
	{
		ConfigLoader::validateElement(*xmlDevice, "Device", {"id"}, true, true);
		powerSwitchDeviceGroup->addDevice(std::stoi(xmlDevice->Attribute("id")));
	}

	return powerSwitchDeviceGroup;
}


WPowerSwitchDeviceGroup::WPowerSwitchDeviceGroup(                       Wt::WString title, Style style, Wt::WContainerWidget* parent) :
WPowerSwitch(title, style, parent)
{}


// WPowerSwitchDeviceGroup::WPowerSwitchDeviceGroup(DeviceIds _devicesIds, Wt::WString title, Style style, Wt::WContainerWidget* parent) :
// WPowerSwitchDeviceGroup(title, style, parent)
// {
	// for (auto deviceId : _devicesIds)
		// addDevice(*deviceId);
// }


WPowerSwitchDeviceGroup::~WPowerSwitchDeviceGroup()
{
	removeAllDevices();
}

// WPowerSwitchDeviceGroup::WPowerSwitchDeviceGroup(DeviceIdListType ids, Wt::WString title, Style style, Wt::WContainerWidget* parent) :
// WPowerSwitch(title, style, parent)
// {
//	Add devices here
// }


// void WPowerSwitchDeviceGroup::addDevice(DevicePowerSwitch* _device)
// {
	// devicesStue.push_back(DevicePowerSwitch_cast(_devices[i]));
// }


void WPowerSwitchDeviceGroup::addDevice(Device::DeviceId newDeviceId)
{
	// Validate new device id
	if (!Device::isValid(newDeviceId))
	{
		std::cout << "WPowerSwitchDeviceGroup::setDevice(Device::DeviceId): Cannot add device. Received invalid device id: " << newDeviceId << "." << std::endl;
		//exit(EXIT_FAILURE); // TODO: use assert and throw signal, or use exceptions
		return;
	}

	Device*               newDevice    = Device::getDevice(newDeviceId);
	PowerSwitchInterface* newDevicePSI = dynamic_cast<PowerSwitchInterface*>(newDevice);
	
	if (newDevicePSI == nullptr)
	{
		std::cout << "WPowerSwitchDeviceGroup::setDevice(Device::DeviceId): Cannot add device. Device pointer cannot be dynamically cast to PowerSwitchInterface*. Device id " << newDeviceId << "." << std::endl;
		//exit(EXIT_FAILURE); // TODO: use assert and throw signal, or use exceptions
		return;
	}
	if (deviceMap.count(newDeviceId) != 0)
	{
		std::cout << "WPowerSwitchDeviceGroup::setDevice(Device::DeviceId): Cannot add device. Device id " << newDeviceId << " is already registered." << std::endl;
		//exit(EXIT_FAILURE); // TODO: use assert and throw signal, or use exceptions
		return;
	}

	deviceMap[newDeviceId] = newDevice->beforeDelete().connect(Wt::WApplication::instance(), std::bind(&WPowerSwitchDeviceGroup::removeDevice, this, newDeviceId));
}


// void WPowerSwitchDeviceGroup::removeDevice(DevicePowerSwitch* _device)
// {
//	
// }


void WPowerSwitchDeviceGroup::removeDevice(Device::DeviceId deviceId)
{
	if (deviceMap.count(deviceId) != 0)
	{
		Device::getDevice(deviceId)->beforeDelete().disconnect(deviceMap[deviceId]);
		deviceMap.erase(deviceId);
	}
}


void WPowerSwitchDeviceGroup::removeAllDevices()
{
	for (auto device : deviceMap)
		Device::getDevice(device.first)->beforeDelete().disconnect(device.second);
	deviceMap.clear();
}


void WPowerSwitchDeviceGroup::buttonOn_clicked()
{
	for (auto device : deviceMap)
		dynamic_cast<PowerSwitchInterface*>(Device::getDevice(device.first))->turnOn(); //DevicePowerSwitch_cast
}


void WPowerSwitchDeviceGroup::buttonOff_clicked()
{
	for (auto device : deviceMap)
		dynamic_cast<PowerSwitchInterface*>(Device::getDevice(device.first))->turnOff(); //DevicePowerSwitch_cast
}
