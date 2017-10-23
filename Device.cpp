#include "Device.hpp"
#include <iostream>
#include <cstdlib> // exit, EXIT_FAILURE
#include <cstring> // stoi
#include "RFDispatcher.hpp"
#include "ConfigLoader.hpp"


/*******************************************************************************
 * class Device implementation
 */


// Static vars

Device::DeviceMap Device::deviceMap;


// Static interface

bool Device::isValid(DeviceId id)
{
	return (deviceMap.count(id) == 1);
}


Device* Device::getDevice(DeviceId id)
{
	if (isValid(id))
		return deviceMap[id];
	else
	{
		std::cout << "Device::getDevice(DeviceId): Passed id " << id << " invalid. No device returned." << std::endl;
		return nullptr;
	}
}


void Device::freeDevice(DeviceId id)
{
	if (isValid(id))
	{
		delete deviceMap[id];
		deviceMap.erase(id);
	}
	else
		std::cout << "Device::freeDevice(DeviceId): Passed id " << id << " invalid. No device to free." << std::endl;

}


void Device::freeAllDevices()
{
	for (DeviceMap::iterator it = deviceMap.begin(); it != deviceMap.end(); ++it)
		delete it->second;
	deviceMap.clear();
}


DeviceActuator Device::getDeviceActuatorFromXML(XMLElement* xmlElement)
{
	ConfigLoader::validateElement(*xmlElement, "Actuator", {"deviceid"}, true, true);
	
	StateStrVec paramStrVec;
	for (const XMLElement* xmlStateParam = xmlElement->FirstChildElement("StateParam"); xmlStateParam != nullptr; xmlStateParam = xmlStateParam->NextSiblingElement("StateParam"))
	{
		ConfigLoader::validateElement(*xmlStateParam, "StateParam", {"value"}, true, true);
		paramStrVec.push_back(xmlStateParam->Attribute("value"));
	}

	Device::DeviceId id = std::stoi(xmlElement->Attribute("deviceid"));
	return Device::getDevice(id)->getActuator(paramStrVec);
}


// Constructor
Device::Device(DeviceId id) : deviceId(id), name(""), location("") //, rfDispatcher(NULL)
{
	if (!isValid(id))
		deviceMap[id] = this;
	else
	{
		std::cout << "Error in Device::Device(int): Device id " << id << " already in use." << std::endl;
		exit(EXIT_FAILURE);
	}
}


//Destructor
Device::~Device()
{
	beforeDeleteSignal.emit();
}
