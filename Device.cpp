#include "Device.hpp"
#include <iostream>
//#include <cctype> // isspace()
//#include <cstdio> // EOF
#include <cstdlib> // exit, EXIT_FAILURE
#include <cstring> // stoi
#include "RFDispatcher.hpp"
#include "ConfigLoader.hpp"


//#include "SessionsContainer.hpp"
//#include "WebApp.hpp"
//#include <Wt/WApplication>
//#include <Wt/WServer>


/*******************************************************************************
 * class Device implementation
 */


// Register in Factory
// or rather not, as long as Device contains pure virtual functions
//FactoryRegistrar<Device, int> registrar_Device("Device");


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

// Func def


/*
void Device::actuate(State state, bool force)
{}
*/	

/*
DeviceActuator Device::getActuator(State state, bool force)
{
	return std::bind([](){});
}
*/

/*
void Device::streamout(std::ostream& os)
{
	os << deviceId << " " << getClassname();
}
*/

/*
bool Device::streamin(std::istream& is)
{
	int _deviceId;
	std::string _classname;
	
	is >> _deviceId;
	if (_deviceId != getDeviceId())
	{
		std::cout << "Device: Log file device ID " << _deviceId << " does not match actual device id: " << getDeviceId() << std::endl;
		return false;
	}
	
	is >> _classname;
	if (_classname.compare(getClassname()) != 0)
	{
		std::cout  << "Device: Log file device ID: " << _deviceId << " Classname: " << _classname << " does not match device classname: " << getClassname() << std::endl;
		return false;
	}

	return true;
}
*/

/*
std::ostream& operator<<(std::ostream& os, Device* obj)
{
	obj->streamout(os);
	os << std::endl;
	return os;
}
*/

/*
#include <string>
#include <sstream>

std::istream& operator>>(std::istream& is, Device* obj)
{
	std::string line;
	std::getline(is, line);
	std::stringstream linestream(line);
	obj->streamin(linestream);
	return is;
}
*/

