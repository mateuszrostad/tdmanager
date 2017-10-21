#include <iostream> // cout
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

	Device::freeAllDevices();

	return 0;
}


void makeDevices()
{
	XMLElement* xmlDeviceDefinitions = ConfigLoader::getInstance()->getRootElement("DeviceDefinitions");
	
	for(XMLElement* xmlDevice = xmlDeviceDefinitions->FirstChildElement("Device");
		xmlDevice != nullptr;
		xmlDevice             = xmlDevice->NextSiblingElement("Device"))
	{
		ConfigLoader::validateElement(*xmlDevice, "Device", {"id", "class", "name", "location"}, true, true);
		
		Factory::Key     factoryKey = xmlDevice->Attribute("factorykey");
		Device::DeviceId deviceId   = std::stoi(xmlDevice->Attribute("id"));

		Factory::make(factoryKey, deviceId);

		Device::getDevice(deviceId)->setClass(   xmlDevice->Attribute("class"));
		Device::getDevice(deviceId)->setName(    xmlDevice->Attribute("name"));
		Device::getDevice(deviceId)->setLocation(xmlDevice->Attribute("location"));
	}


	XMLElement* xmlDeviceStateLog = ConfigLoader::getInstance()->getRootElement("DeviceStateLog");

	for(XMLElement* xmlDevice = xmlDeviceStateLog->FirstChildElement("Device");
		xmlDevice != nullptr;
		xmlDevice             = xmlDevice->NextSiblingElement("Device"))
	{
		Device::setDeviceStateFromXML(xmlDevice);
	}
	
	
	// LEGACY BELOW //
	/*
	XMLElement* xmlDeviceFactory = ConfigLoader::getInstance()->getRootElement("DeviceFactory");

	for (XMLElement* xmlDevice = xmlDeviceFactory->FirstChildElement("Device"); xmlDevice != nullptr; xmlDevice = xmlDevice->NextSiblingElement("Device"))
	{
		ConfigLoader::validateElement(*xmlDevice, "Device", {"factorykey", "id", "name", "location"}, true, true);

		Factory::Key     factoryKey = xmlDevice->Attribute("factorykey");
		Device::DeviceId deviceId   = std::stoi(xmlDevice->Attribute("id"));
		StateStrVec      paramStrVec;
		for (const XMLElement* xmlStateParam = xmlDevice->FirstChildElement("StateParam"); xmlStateParam != nullptr; xmlStateParam = xmlStateParam->NextSiblingElement("StateParam"))
		{
			ConfigLoader::validateElement(*xmlStateParam, "StateParam", {"value"}, true, true);
			paramStrVec.push_back(xmlStateParam->Attribute("value"));
		}

		Factory::make(factoryKey, deviceId, paramStrVec);

		Device::getDevice(deviceId)->setName(    xmlDevice->Attribute("name"));
		Device::getDevice(deviceId)->setLocation(xmlDevice->Attribute("location"));
	}
	*/
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
