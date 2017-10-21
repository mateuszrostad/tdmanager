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


void Device::setDeviceStateFromXML(XMLElement* xmlDevice)
{
	ConfigLoader::validateElement(*xmlDevice, "Device", {"id"}, true, true);
	
	StateStrVec stateStrVec;
	for(XMLElement* xmlStateParam = xmlDevice->FirstChildElement("StateParam");
		xmlStateParam != nullptr;
		xmlStateParam             = xmlStateParam->NextSiblingElement("StateParam"))
	{
		ConfigLoader::validateElement(*xmlStateParam, "StateParam", {"index", "value"}, true, true);
		
		stateStrVec.push_back(xmlStateParam->Attribute("value"));
	}		

	Device::DeviceId id = std::stoi(xmlDevice->Attribute("id"));
	Device::getDevice(id)->setState(stateStrVec);
}


bool Device::checkXMLDeviceDefs()
{
	struct
	{
		std::string _class;
		std::string  name;
		std::string  location;
	} XMLDevice;

	std::map<DeviceId, XMLDevice> xmlDeviceMap;

	XMLElement* xmlDeviceDefinitions = ConfigLoader::getInstance()->getRootElement("DeviceDefinitions");

	// Iterate through all xml device defs, check against Device instance map
	for(XMLElement* xmlDeviceDef = xmlDeviceDefinitions->FirstChildElement("Device");
		xmlDeviceDef != nullptr;
		xmlDeviceDef             = xmlDeviceDef->NextSiblingElement("Device"))
	{
		ConfigLoader::validateElement(*xmlDeviceDef, "Device", {"id", "class", "name", "location"}, true, true);

		DeviceId  id        = std::stoi(xmlDeviceDef->Attribute("id"));
		XMLDevice xmlDevice = XMLDevice{xmlDeviceDev->Attribute("class"), xmlDeviceDev->Attribute("name"), xmlDeviceDev->Attribute("location")};
		if (deviceMap.count(id) == 0)
		{
			std::cout << "Device::checkXMLDeviceDefs(): In xml config document, device definition id " << id << " does not correspond to any Device instance." << std::endl;
			return false;
		}
		else if (xmlDevice._class != deviceMap[id]->getClass())
		{
			std::cout << "Device::checkXMLDeviceDefs(): In xml config document, device definition id " << id << ", device class " << xmlDevice._class << " does not correspond to Device instance class (" << deviceMap[id]->getClass() << ")." << std::endl;
			return false;
		}
		else if (xmlDevice.name != deviceMap[id]->getName())
		{
			std::cout << "Device::checkXMLDeviceDefs(): In xml config document, device definition id " << id << ", device name " << xmlDevice.name << " does not correspond to Device instance name (" << deviceMap[id]->getName() << ")." << std::endl;
			return false;
		}
		else if (xmlDevice.location != deviceMap[id]->getLocation())
		{
			std::cout << "Device::checkXMLDeviceDefs(): In xml config document, device definition id " << id << ", device location " << xmlDevice.location << " does not correspond to Device instance class (" << deviceMap[id]->getLocation() << ")." << std::endl;
			return false;
		}

		// Add xml device def to map for easy comparison with Device instance map in for loop below.
		xmlDeviceMap[id] = xmlDevice;
	}

	// Iterate through Device instance map, check against xml device def map
	for (auto device : deviceMap)
	{
		DeviceId     id       = device->first();
		std::string _class    = device->second()->getClass();
		std::string  name     = device->second()->getName();
		std::string  location = device->second()->getLocation();

		if (xmlDeviceMap.count(id) == 0)
		{
			std::cout << "Device::checkXMLDeviceDefs(): Device instance id " << id << " does not correspond to any device def in xml config document." << std::endl;
			return false;
		}
		else if (_class != xmlDeviceMap[id]._class)
		{
			std::cout << "Device::checkXMLDeviceDefs(): Device instance id " << id << ", device class " << _class << " does not correspond to device def in xml config document (" << xmlDeviceMap[id]._class << ")." << std::endl;
			return false;
		}
		else if (name != xmlDeviceMap[id].name)
		{
			std::cout << "Device::checkXMLDeviceDefs(): Device instance id " << id << ", device name " << name << " does not correspond to device def in xml config document (" << xmlDeviceMap[id].name << ")." << std::endl;
			return false;
		}
		else if (location != xmlDeviceMap[id].location)
		{
			std::cout << "Device::checkXMLDeviceDefs(): Device instance id " << id << ", device location " << location << " does not correspond to device def in xml config document (" << xmlDeviceMap[id].location << ")." << std::endl;
			return false;
		}
	}
	
	return true;
}


/*
void Device::parseDeviceStatesToXML()
{
	// Load existing device state log from xml doc
	XMLElement* xmlDeviceStateLog = ConfigLoader::getInstance()->getRootElement("DeviceStateLog");

	std::map<DeviceId, XMLElement*> xmlDevices;

	for(XMLElement* xmlDevice = xmlDeviceStateLog->FirstChildElement("Device");
		xmlDevice != nullptr;
		xmlDevice             = xmlDevice->NextSiblingElement("Device"))
	{
		ConfigLoader::validateElement(*xmlDevice, "Device", {"id"}, true, true);
	
		xmlDevices[std::stoi(xmlDevice->Attribute("id"))] = xmlDevice;
	}

	// TODO: Load existing device defs from xml doc.
	// Overlap device defs with device state log
	// Error if device found in state log but not in defs
	// Warning if device found in defs but not i state log, create new state log entry from current device state
	
	for(auto device : deviceMap)
	{
		// if device is in xmlDevices
		//   update xmlDevices entry
		// else
		//   create new XMLElement and add to xmlDeviceStateLog

		if (xmlDevices.count(device->first) == 1)
		{
			DeviceId id = device->first;
			XMLElement* xmlDevice = xmlDevices[id];
			
			//StateStrVec stateStrVec;
			for(XMLElement* xmlStateParam = xmlDevice->FirstChildElement("StateParam");
				xmlStateParam != nullptr;
				xmlStateParam             = xmlStateParam->NextSiblingElement("StateParam"))
			{
				ConfigLoader::validateElement(*xmlStateParam, "StateParam", {"index", "value"}, true, true);
				
				stateStrVec.push_back(xmlStateParam->Attribute("value"));
			}		
		}
		else
		{
			std::cout << "Device::parseDeviceStatesToXML(): Device with id " << id << " not found in xml device state log."
		}
	
		//device.first
	}
	
	
}
*/


DeviceActuator Device::getDeviceActuatorFromXML(XMLElement* xmlDevice)
{
	ConfigLoader::validateElement(*xmlElement, "Device", {"id"}, true, true);
	
	StateStrVec stateStrVec;
	for(XMLElement* xmlStateParam = xmlDevice->FirstChildElement("StateParam");
		xmlStateParam != nullptr;
		xmlStateParam             = xmlStateParam->NextSiblingElement("StateParam"))
	{
		ConfigLoader::validateElement(*xmlStateParam, "StateParam", {"index", "value"}, true, true);

		stateStrVec.push_back(xmlStateParam->Attribute("value"));
	}

	Device::DeviceId id = std::stoi(xmlDevice->Attribute("id"));
	return Device::getDevice(id)->getActuator(stateStrVec);
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
