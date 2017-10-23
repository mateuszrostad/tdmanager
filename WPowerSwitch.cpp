#include "WPowerSwitch.hpp"
#include <iostream>
#include <cstdlib> // exit, EXIT_FAILURE
#include <cstring> // stoi, strcmp
#include <functional>
#include <Wt/WApplication>
#include <Wt/WHBoxLayout>
#include <Wt/WLength>
#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WString>
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
	ConfigLoader::validateElement(*xmlElement, "PowerSwitchSingleDevice", {"deviceid", "style"}, true, true);

	WPowerSwitchSingleDevice::Style style;
	const char* styleStr = xmlElement->Attribute("style");
	if      (std::strcmp(styleStr, "Header") == 0)
		style = WPowerSwitchSingleDevice::Header;
	else if (std::strcmp(styleStr, "Body") == 0)
		style = WPowerSwitchSingleDevice::Body;
	else
	{
		std::cout << "WPowerSwitchSingleDevice::parseXML(...): Error: Element \"" << xmlElement->Name() << "\", attribute \"style\" has invalid value \"" << styleStr << "\"." << std::endl;
		exit(EXIT_FAILURE);
	}

	Device::DeviceId deviceId = std::stoi(xmlElement->Attribute("deviceid"));

	return new WPowerSwitchSingleDevice(deviceId, Device::getDevice(deviceId)->getName(), style);
}


WPowerSwitchSingleDevice::WPowerSwitchSingleDevice(                            Wt::WString title, Style style, Wt::WContainerWidget* parent) :
WPowerSwitch(title, style, parent), deviceIsSet(false) //, connectedToDeviceSignal(false)
{}


WPowerSwitchSingleDevice::WPowerSwitchSingleDevice(Device::DeviceId newDeviceId, Wt::WString title, Style style, Wt::WContainerWidget* parent) :
WPowerSwitchSingleDevice(title, style, parent)
{
	setDevice(newDeviceId);
}


WPowerSwitchSingleDevice::~WPowerSwitchSingleDevice()
{
	removeDevice();
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
	const char* styleStr = xmlElement->Attribute("style");
	if      (std::strcmp(styleStr, "Header") == 0)
		style = WPowerSwitchDeviceGroup::Header;
	else if (std::strcmp(styleStr, "Body") == 0)
		style = WPowerSwitchDeviceGroup::Body;
	else
	{
		std::cout << "WPowerSwitchDeviceGroup::parseXML(...): Error: Element \"" << xmlElement->Name() << "\", attribute \"style\" has invalid value \"" << styleStr << "\"." << std::endl;
		exit(EXIT_FAILURE);
	}

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


WPowerSwitchDeviceGroup::~WPowerSwitchDeviceGroup()
{
	removeAllDevices();
}

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
