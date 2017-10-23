#include "WDimmerPowerSwitch.hpp"
#include <iostream>
#include <cstdlib> // exit, EXIT_FAILURE
#include <cstring> // stoi, strcmp
#include <Wt/WHBoxLayout>
#include <Wt/WLength>
#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WSlider>
#include <Wt/WString>
#include <Wt/WBreak>


/*******************************************************************************
 * class WDimmerPowerSwitch
 */

WDimmerPowerSwitch::WDimmerPowerSwitch(Wt::WString title, int dimValueMin, int dimValueMax, int dimValueInterval, Style style, Wt::WContainerWidget* parent) :
Wt::WContainerWidget(parent), buttonOnActive(false), buttonOffActive(false), slider(nullptr), buttonOn(nullptr), buttonOff(nullptr)
{
	
	Wt::WString additionalStyling;
	if (style == Header)
		additionalStyling = " primetext";
	
	//setHeight(50);
	
	setStyleClass("wdimmerpowerswitch_container");
	
	Wt::WHBoxLayout* layout = new Wt::WHBoxLayout();
	layout->setContentsMargins(4, 1, 4, 1);
	layout->setSpacing(4);
	setLayout(layout);
	
	text = new Wt::WText(title);
	text->setStyleClass("wdimmerpowerswitch_text" + additionalStyling);
	layout->addWidget(text, 1, Wt::AlignMiddle);

	//new Wt::WBreak(this);
	layout->addSpacing(Wt::WLength(4, Wt::WLength::Pixel));
	
	slider = new Wt::WSlider(Wt::Horizontal);
	//slider->setNativeControl(true);
	slider->setTickPosition(Wt::WSlider::TicksBelow | Wt::WSlider::TicksAbove);

	setSliderParams(dimValueMin, dimValueMax, dimValueInterval);
	
	slider->resize(500, 50);
	//slider->setVerticalAlignment(Wt::AlignMiddle);
	slider->setStyleClass("wdimmerpowerswitch_slider");
	slider->valueChanged().connect(this, &WDimmerPowerSwitch::slider_valueChanged);
	//slider->sliderMoved().connect(this, &WDimmerPowerSwitch::slider_valueChanged);
	layout->addWidget(slider);
	
	layout->addSpacing(Wt::WLength(4, Wt::WLength::Pixel));

	buttonOff = new Wt::WPushButton("Av", this);
	//buttonOff->setVerticalAlignment(Wt::AlignMiddle);
	buttonOff->setStyleClass("wdimmerpowerswitch_buttonOff" + additionalStyling);
	buttonOff->clicked().connect(this, &WDimmerPowerSwitch::buttonOff_clicked);
	layout->addWidget(buttonOff);

	buttonOn  = new Wt::WPushButton("På");
	//buttonOn->setVerticalAlignment(Wt::AlignMiddle);
	buttonOn->setStyleClass("wdimmerpowerswitch_buttonOn" + additionalStyling);
	buttonOn->clicked().connect(this, &WDimmerPowerSwitch::buttonOn_clicked);
	layout->addWidget(buttonOn);
	
	//new Wt::WBreak(container);

}


WDimmerPowerSwitch::WDimmerPowerSwitch(DimmerPowerSwitchInterface::PowerState powerState, DimmerPowerSwitchInterface::DimLevel dimLevel, Wt::WString title, int dimValueMin, int dimValueMax, int dimValueInterval, Style style, Wt::WContainerWidget* parent) :
WDimmerPowerSwitch(title, dimValueMin, dimValueMax, dimValueInterval, style, parent)
{
	updatePowerStateIndicator(powerState);
	updateDimLevelIndicator(dimLevel);
}


void WDimmerPowerSwitch::updatePowerStateIndicator(DimmerPowerSwitchInterface::PowerState newPowerState)
{
	bool updated = false;
	if      (newPowerState == DimmerPowerSwitchInterface::On) // On
	{
		if (buttonOff->hasStyleClass("btn-primary"))
		{
			buttonOff->removeStyleClass("btn-primary");
			buttonOffActive = false;
			updated = true;
		}
		if (!(buttonOn->hasStyleClass("btn-primary")))
		{
			buttonOn->addStyleClass("btn-primary");
			buttonOnActive = true;
			updated = true;
		}
	}
	else if (!buttonOffActive && newPowerState == DimmerPowerSwitchInterface::Off) // Off // Why the extra test for !buttonOffActive here?
	{
		if (buttonOn->hasStyleClass("btn-primary"))
		{
			buttonOn->removeStyleClass("btn-primary");
			buttonOnActive = false;
			updated = true;
		}
		if (!(buttonOff->hasStyleClass("btn-primary")))
		{
			buttonOff->addStyleClass("btn-primary");
			buttonOffActive = true;
			updated = true;
		}
	}
	else
		std::cout << "WDimmerPowerSwitch::updatePowerStateIndicator(...): recieved invalid new state " << newPowerState << "." << std::endl;
	
	if (updated)
		Wt::WApplication::instance()->triggerUpdate();
}


void WDimmerPowerSwitch::updateDimLevelIndicator(DimmerPowerSwitchInterface::DimLevel newDimLevel)
{
	if (((int) newDimLevel) >= slider->minimum() && ((int) newDimLevel) <= slider->maximum())
	{
		// The (int) cast here is redundant and serves only as a trivial example of a possible more generic future implementation of conversion between a Device paramter and the Wt parameter
		if (slider->value() != ((int) newDimLevel))
		{
			slider->setValue((int) newDimLevel);
			Wt::WApplication::instance()->triggerUpdate();
		}
	}
	else
		std::cout << "WDimmerPowerSwitch::updateDimLevelIndicator(...): recieved invalid new dim level " << newDimLevel << "." << std::endl;

}


void WDimmerPowerSwitch::setSliderParams(int dimValueMin, int dimValueMax, int dimValueInterval)
{
	slider->setMinimum(dimValueMin);
	slider->setMaximum(dimValueMax);
	slider->setTickInterval(dimValueInterval);
}


/*******************************************************************************
 * class WDimmerPowerSwitchSingleDevice
 */


WDimmerPowerSwitchSingleDevice* WDimmerPowerSwitchSingleDevice::parseXML(XMLElement* xmlElement)
{
	ConfigLoader::validateElement(*xmlElement, "DimmerPowerSwitchSingleDevice", {"deviceid", "style"}, true, true);

	WDimmerPowerSwitchSingleDevice::Style style;
	const char* styleStr = xmlElement->Attribute("style");
	if      (std::strcmp(styleStr, "Header") == 0)
		style = WDimmerPowerSwitchSingleDevice::Header;
	else if (std::strcmp(styleStr, "Body") == 0)
		style = WDimmerPowerSwitchSingleDevice::Body;
	else
	{
		std::cout << "WDimmerPowerSwitchSingleDevice::parseXML(...): Error: Element \"" << xmlElement->Name() << "\", attribute \"style\" has invalid value \"" << styleStr << "\"." << std::endl;
		exit(EXIT_FAILURE);
	}

	Device::DeviceId deviceId = std::stoi(xmlElement->Attribute("deviceid"));

	return new WDimmerPowerSwitchSingleDevice(deviceId, Device::getDevice(deviceId)->getName(), style);
}


WDimmerPowerSwitchSingleDevice::WDimmerPowerSwitchSingleDevice(                              Wt::WString title, Style style, Wt::WContainerWidget* parent) :
WDimmerPowerSwitch(title, 0, 1, 1, style, parent), deviceIsSet(false)
{
}


WDimmerPowerSwitchSingleDevice::WDimmerPowerSwitchSingleDevice(Device::DeviceId newDeviceId, Wt::WString title, Style style, Wt::WContainerWidget* parent) :
WDimmerPowerSwitchSingleDevice(title, style, parent)
{
	setDevice(newDeviceId);
}


WDimmerPowerSwitchSingleDevice::~WDimmerPowerSwitchSingleDevice()
{
	removeDevice();
}


void WDimmerPowerSwitchSingleDevice::setDevice(Device::DeviceId newDeviceId)
{
	// Validate new device id
	if (!Device::isValid(newDeviceId))
	{
		std::cout << "WDimmerPowerSwitchSingleDevice::setDevice(Device::DeviceId): Cannot set device. Received invalid device id: " << newDeviceId << "." << std::endl;
		//exit(EXIT_FAILURE); // TODO: use assert and throw signal, or use exceptions
		return;
	}

	Device*                     newDevice     = Device::getDevice(newDeviceId);
	DimmerPowerSwitchInterface* newDeviceDPSI = dynamic_cast<DimmerPowerSwitchInterface*>(newDevice);

	// Validate new device class inheritance
	if (newDeviceDPSI == nullptr)
	{
		std::cout << "WDimmerPowerSwitchSingleDevice::setDevice(Device::DeviceId): Cannot set device. Device pointer cannot be dynamically cast to DimmerPowerSwitchInterface*. Device id " << newDeviceId << "." << std::endl;
		//exit(EXIT_FAILURE); // TODO: use assert and throw signal, or use exceptions
		return;
	}
	
	// If other device currently set, remove this first
	removeDevice();

	// Set local vars
	deviceId    = newDeviceId;
	deviceIsSet = true;

	// Connect to device signals
	slotIdBeforeDelete = newDevice    ->beforeDelete()     .connect(Wt::WApplication::instance(), std::bind(&WDimmerPowerSwitchSingleDevice::removeDevice,              this));
	slotIdPowerState   = newDeviceDPSI->powerStateUpdated().connect(Wt::WApplication::instance(), std::bind(&WDimmerPowerSwitchSingleDevice::updatePowerStateIndicator, this, std::placeholders::_1));
	slotIdDimLevel     = newDeviceDPSI->dimLevelUpdated()  .connect(Wt::WApplication::instance(), std::bind(&WDimmerPowerSwitchSingleDevice::updateDimLevelIndicator,   this, std::placeholders::_1));

	// Update appearence
	setSliderParams(0, newDeviceDPSI->getDimLevelMax(), 1);
	updatePowerStateIndicator(newDeviceDPSI->getPowerState());
	updateDimLevelIndicator(  newDeviceDPSI->getDimLevel());
}


void WDimmerPowerSwitchSingleDevice::removeDevice()
{
	if (deviceIsSet)
	{
		Device*                     device     = Device::getDevice(deviceId);
		DimmerPowerSwitchInterface* deviceDPSI = dynamic_cast<DimmerPowerSwitchInterface*>(device);

		device    ->beforeDelete()     .disconnect(slotIdBeforeDelete);
		deviceDPSI->powerStateUpdated().disconnect(slotIdPowerState);
		deviceDPSI->dimLevelUpdated()  .disconnect(slotIdDimLevel);
		deviceIsSet = false;
	}
}


void WDimmerPowerSwitchSingleDevice::buttonOn_clicked()
{
	if (deviceIsSet)
		dynamic_cast<DimmerPowerSwitchInterface*>(Device::getDevice(deviceId))->turnOn();
}


void WDimmerPowerSwitchSingleDevice::buttonOff_clicked()
{
	if (deviceIsSet)
		dynamic_cast<DimmerPowerSwitchInterface*>(Device::getDevice(deviceId))->turnOff();
}


void WDimmerPowerSwitchSingleDevice::slider_valueChanged()
{
	//std::cout << "Session " << Wt::WApplication::instance()->sessionId() <<" WDimmerPowerSwitchSingleDevice::slider_valueChanged()" << std::endl;
	if (deviceIsSet)
		dynamic_cast<DimmerPowerSwitchInterface*>(Device::getDevice(deviceId))->setDimLevel(getSlider()->value());
}
