#include <iostream> // cout
#include <cstdlib> // exit, EXIT_FAILURE
#include <unistd.h> // usleep
#include <thread>
#include <csignal>
#include <string>
#include <vector>
#include <list>
#include <telldus-core.h>
#include "Factory.hpp"
#include "ConfigLoader.hpp"
#include "Device.hpp"
#include "RFDispatcher.hpp"
#include "WebApp.hpp"
#include "RawEventHandler.hpp"
#include "main.hpp"
#include <Wt/WServer>
#include <Wt/WString>


#define sleepWhileSend usleep(1e6)



int main(int argc, char** argv)
{
	signal(SIGINT, &interruptFunction);
	
	makeDevices();
	registerDevicesWithRFDispatcher();
	

	tdInit();

	int callbackId = tdRegisterRawDeviceEvent(rawEventCallback, NULL);
	
	std::thread thr(&RFDispatcher::run, RFDispatcher::getInstance());
	std::cout << "Instance of RFDispatcher now runs in separate thread." << std::endl;

	// Webtoolkit, do some initialization
	Wt::WString::setDefaultEncoding(Wt::UTF8);

	// Webtoolkint, run server
	Wt::WRun(argc, argv, &createWebApp);
	
	thr.join();
	RFDispatcher::getInstance()->free();
	std::cout << " RFDispatcher::run terminated" << std::endl;
	
	tdUnregisterCallback(callbackId);
	tdClose();

	saveDeviceLog();
	Device::freeAllDevices();

	return 0;
}


void makeDevices()
{
	// Collect device definitions from xml document
	XMLElement* xmlDeviceDefinitions = ConfigLoader::getInstance()->getRootElement("DeviceDefinitions");
	
	struct DeviceData
	{
		std::string class_, name, location;
	};
	std::map<Device::DeviceId, DeviceData> deviceMap;
	
	for(XMLElement* xmlDevice = xmlDeviceDefinitions->FirstChildElement("Device");
		xmlDevice != nullptr;
		xmlDevice             = xmlDevice->NextSiblingElement("Device"))
	{
		ConfigLoader::validateElement(*xmlDevice, "Device", {"id", "class", "name", "location"}, true, true);

		deviceMap[std::stoi(xmlDevice->Attribute("id"))] = DeviceData{xmlDevice->Attribute("class"), xmlDevice->Attribute("name"), xmlDevice->Attribute("location")};
	}


	// Collect device states from xml document
	XMLElement* xmlDeviceStateLog = ConfigLoader::getInstance()->getRootElement("DeviceStateLog");
	
	std::map<Device::DeviceId, StateStrVec> deviceStateMap;
	
	for(XMLElement* xmlDevice = xmlDeviceStateLog->FirstChildElement("Device");
		xmlDevice != nullptr;
		xmlDevice             = xmlDevice->NextSiblingElement("Device"))
	{
		ConfigLoader::validateElement(*xmlDevice, "Device", {"id"}, true, true);

		Device::DeviceId id = std::stoi(xmlDevice->Attribute("id"));
		
		if (deviceMap.count(id) != 1)
		{
			std::cout << "main::makeDevices(): In device state log of config xml document, device id " << id << " not defined." << std::endl;
			exit(EXIT_FAILURE);
		}
		
		for(XMLElement* xmlStateParam = xmlDevice->FirstChildElement("StateParam");
			xmlStateParam != nullptr;
			xmlStateParam             = xmlStateParam->NextSiblingElement("StateParam"))
		{
			ConfigLoader::validateElement(*xmlStateParam, "StateParam", {"index", "value"}, true, true);
		
			deviceStateMap[id].push_back(xmlStateParam->Attribute("value"));
		}
	}
	
	
	// Init devices
	for (auto deviceData : deviceMap)
	{
		Device::DeviceId id = deviceData.first;
		
		if (deviceStateMap.count(id) == 0)
			Factory::make(deviceData.second.class_, id);
		else
			Factory::make(deviceData.second.class_, id, deviceStateMap[id]);
		
		Device::getDevice(id)->setClass(   deviceData.second.class_);
		Device::getDevice(id)->setName(    deviceData.second.name);
		Device::getDevice(id)->setLocation(deviceData.second.location);
	}

}


void saveDeviceLog()
{
	XMLElement* xmlDeviceStateLog = ConfigLoader::getInstance()->getRootElement("DeviceStateLog");
	
	xmlDeviceStateLog->DeleteChildren();
	
	for (auto device : Device::getDeviceMap())
	{
		XMLElement* deviceXML = ConfigLoader::getInstance()->getDocument().NewElement("Device");
		deviceXML->SetAttribute("id", device.second->getId());
		
		StateStrVec stateStrVec = device.second->getStateStringVector();
		for (unsigned int i = 0; i < stateStrVec.size(); i++)
		{
			XMLElement* stateXML = ConfigLoader::getInstance()->getDocument().NewElement("StateParam");
			stateXML->SetAttribute("index", i);
			stateXML->SetAttribute("value", stateStrVec[i].c_str());
			deviceXML->InsertEndChild(stateXML);
		}
		
		xmlDeviceStateLog->InsertEndChild(deviceXML);
	}
	
	ConfigLoader::getInstance()->saveFile();
}


void registerDevicesWithRFDispatcher()
{
	
	XMLElement* xmlRFDispatcher = ConfigLoader::getInstance()->getRootElement("RFDispatcher");

	for(XMLElement* xmlDevice = xmlRFDispatcher->FirstChildElement("RegisteredDevice");
		xmlDevice != nullptr;
		xmlDevice             = xmlDevice->NextSiblingElement("RegisteredDevice"))
	{
		ConfigLoader::validateElement(*xmlDevice, "RegisteredDevice", {"deviceid", "codeid"}, true, true);
		
		Device::DeviceId deviceId = std::stoi(xmlDevice->Attribute("deviceid"));
		int              codeId   = std::stoi(xmlDevice->Attribute("codeid"));
		
		RFDispatcher::getInstance()->registerDevice(Device::getDevice(deviceId), codeId);
	}
}


Wt::WApplication* createWebApp(const Wt::WEnvironment& env)
{
	Wt::WApplication* _wapp = new WebApp(env);
	return _wapp;
}


void interruptFunction(int sig)
{
	RFDispatcher::setTerminateFlag();
}


#include "LocalDevices.hpp" // For rawEventCallback, TODO: restrict local usage to Device.hpp only when rawEventCallback is implemented separately

void rawEventCallback(const char* data, int controllerId, int callbackId, void* context)
{
	const Device::DeviceMap& devices = Device::getDeviceMap();

	RemoteEventData* eventData = getRemoteEventData(data);

	if (eventData->class_ == command && eventData->protocol == arctech && eventData->model == selflearning && eventData->house == 20007718)
	{
		std::cout << "tdmanager: Raw event callback " << callbackId << " triggered by controller " << controllerId << ": tellstick daemon sent data \"" << data << "\"" << std::endl;
		
		if (eventData->house == 20007718) 
		{

			if      (eventData->group == 1 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(0))->turnOn();
				DeviceDimmerPowerSwitch_cast(devices.at(1))->turnOn();
				DeviceDimmerPowerSwitch_cast(devices.at(2))->turnOn();
				DeviceDimmerPowerSwitch_cast(devices.at(3))->turnOn();
				DeviceDimmerPowerSwitch_cast(devices.at(4))->turnOn();
				DeviceDimmerPowerSwitch_cast(devices.at(5))->turnOn();
			}
			else if (eventData->group == 1 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(0))->turnOff();
				DeviceDimmerPowerSwitch_cast(devices.at(1))->turnOff();
				DeviceDimmerPowerSwitch_cast(devices.at(2))->turnOff();
				DeviceDimmerPowerSwitch_cast(devices.at(3))->turnOff();
				DeviceDimmerPowerSwitch_cast(devices.at(4))->turnOff();
				DeviceDimmerPowerSwitch_cast(devices.at(5))->turnOff();
			}

			else if (eventData->group == 0 && eventData->unit == 1 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(0))->turnOn();
			}
			else if (eventData->group == 0 && eventData->unit == 1 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(0))->turnOff();
			}

			else if (eventData->group == 0 && eventData->unit == 2 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(1))->turnOn();
			}
			else if (eventData->group == 0 && eventData->unit == 2 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(1))->turnOff();
			}

			else if (eventData->group == 0 && eventData->unit == 3 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(2))->turnOn();
			}
			else if (eventData->group == 0 && eventData->unit == 3 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(2))->turnOff();
			}

			else if (eventData->group == 0 && eventData->unit == 4 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(3))->turnOn();
			}
			else if (eventData->group == 0 && eventData->unit == 4 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(3))->turnOff();
			}
			

			else if (eventData->group == 0 && eventData->unit == 5 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(0))->dim(1);
			}
			else if (eventData->group == 0 && eventData->unit == 5 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(0))->dim(-1);
			}

			else if (eventData->group == 0 && eventData->unit == 6 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(1))->dim(1);
			}
			else if (eventData->group == 0 && eventData->unit == 6 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(1))->dim(-1);
			}

			else if (eventData->group == 0 && eventData->unit == 7 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(2))->dim(1);
			}
			else if (eventData->group == 0 && eventData->unit == 7 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(2))->dim(-1);
			}

			else if (eventData->group == 0 && eventData->unit == 8 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(3))->dim(1);
			}
			else if (eventData->group == 0 && eventData->unit == 8 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(3))->dim(-1);
			}

			
			else if (eventData->group == 0 && eventData->unit == 9 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(0))->dim(2);
			}
			else if (eventData->group == 0 && eventData->unit == 9 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(0))->dim(-2);
			}

			else if (eventData->group == 0 && eventData->unit == 10 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(1))->dim(2);
			}
			else if (eventData->group == 0 && eventData->unit == 10 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(1))->dim(2);
			}

			else if (eventData->group == 0 && eventData->unit == 11 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(2))->dim(2);
			}
			else if (eventData->group == 0 && eventData->unit == 11 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(2))->dim(-2);
			}

			else if (eventData->group == 0 && eventData->unit == 12 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(3))->dim(2);
			}
			else if (eventData->group == 0 && eventData->unit == 12 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(3))->dim(-2);
			}

			
			else if (eventData->group == 0 && eventData->unit == 13 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(0))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 10});
				DeviceDimmerPowerSwitch_cast(devices.at(1))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 10});
				DeviceDimmerPowerSwitch_cast(devices.at(2))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 10});
				DeviceDimmerPowerSwitch_cast(devices.at(3))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 6});
				DeviceDimmerPowerSwitch_cast(devices.at(4))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 10});
				DeviceDimmerPowerSwitch_cast(devices.at(5))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 10});
			}

			else if (eventData->group == 0 && eventData->unit == 13 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(0))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 3});
				DeviceDimmerPowerSwitch_cast(devices.at(1))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::Off, 3});
				DeviceDimmerPowerSwitch_cast(devices.at(2))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 3});
				DeviceDimmerPowerSwitch_cast(devices.at(3))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 3});
				DeviceDimmerPowerSwitch_cast(devices.at(4))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 3});
				DeviceDimmerPowerSwitch_cast(devices.at(5))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 3});
			}

			else if (eventData->group == 0 && eventData->unit == 14 && eventData->method == turnon)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(0))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 15});
				DeviceDimmerPowerSwitch_cast(devices.at(1))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 15});
				DeviceDimmerPowerSwitch_cast(devices.at(2))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 15});
				DeviceDimmerPowerSwitch_cast(devices.at(3))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 15});
				DeviceDimmerPowerSwitch_cast(devices.at(4))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 15});
				DeviceDimmerPowerSwitch_cast(devices.at(5))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 15});
			}

			else if (eventData->group == 0 && eventData->unit == 14 && eventData->method == turnoff)
			{
				DeviceDimmerPowerSwitch_cast(devices.at(0))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 0});
				DeviceDimmerPowerSwitch_cast(devices.at(1))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 0});
				DeviceDimmerPowerSwitch_cast(devices.at(2))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 0});
				DeviceDimmerPowerSwitch_cast(devices.at(3))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 0});
				DeviceDimmerPowerSwitch_cast(devices.at(4))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 0});
				DeviceDimmerPowerSwitch_cast(devices.at(5))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 0});
			}
		}
		
		else // if (eventData->house == )
		{
			
		}
			
	}

	delete eventData;
}
