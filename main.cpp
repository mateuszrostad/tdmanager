#include <iostream> // cout
//#include <fstream>
#include <unistd.h> // usleep
#include <thread>
#include <csignal>
#include <string>
#include <vector>
#include <list>
#include <telldus-core.h>
#include "Factory.hpp"
#include "ConfigLoader.hpp"
#include "LocalDevices.hpp"
#include "RFDispatcher.hpp"
#include "WebApp.hpp"
#include "RawEventHandler.hpp"
//#include "SessionsContainer.hpp"
#include "main.hpp"
#include <Wt/WServer>
//#include <Wt/WApplication>
#include <Wt/WString>


#define sleepWhileSend usleep(1e6)


//DeviceListType devices;

//SessionsContainer* sessions;



//XMLElement* getRFDispatcherDataRootElement();


int main(int argc, char** argv)
{
	signal(SIGINT, &interruptFunction);
	
	makeDevices();
	registerDevicesWithRFDispatcher();

	/*
	if (std::ifstream(deviceStateFileName))
	{
		std::ifstream deviceStateFileIn;
		deviceStateFileIn.open(deviceStateFileName);
		istreamDevices(deviceStateFileIn);
		deviceStateFileIn.close();
	}
	*/
	
	tdInit();

	int callbackId = tdRegisterRawDeviceEvent(rawEventCallback, NULL);
	
	std::thread thr(&RFDispatcher::run, RFDispatcher::getInstance());
	std::cout << "Instance of RFDispatcher now runs in separate thread." << std::endl;

	// Webtoolkit, do some initialization
	Wt::WString::setDefaultEncoding(Wt::UTF8);
	// Construct SessionsContainer
	//SessionsContainer::instantiate();

	// Webtoolkint, run server
	Wt::WRun(argc, argv, &createWebApp);
	
	thr.join();
	RFDispatcher::getInstance()->free();
	std::cout << " RFDispatcher::run terminated" << std::endl;
	
	tdUnregisterCallback(callbackId);
	tdClose();

	/*
	std::ofstream deviceStateFileOut;
	deviceStateFileOut.open(deviceStateFileName);
	ostreamDevices(deviceStateFileOut);
	deviceStateFileOut.close();
	*/
	
	Device::freeAllDevices();

	return 0;
}


void makeDevices()
{
	Factory::Key     factoryKey;
	Device::DeviceId deviceId;
	StateStrVec      paramStrVec;

	XMLElement* xmlDeviceFactory = ConfigLoader::getInstance()->getDeviceFactoryDataRootElement();

	for (XMLElement* xmlInstance = xmlDeviceFactory->FirstChildElement("Instance"); xmlInstance != nullptr; xmlInstance = xmlDeviceFactory->NextSiblingElement("Instance"))
	{
		// Terminate if parser fails, indicating a faulty xml config file
		if (ConfigLoader::getInstance()->parseInstance(*xmlInstance, &factoryKey, &deviceId, &paramStrVec))
			Factory::make(factoryKey, deviceId, paramStrVec);
		else
			exit(EXIT_FAILURE);
	}
}


void registerDevicesWithRFDispatcher()
{
	Device::DeviceId deviceId;
	int codeId;
	
	XMLElement* xmlRFDispatcher = ConfigLoader::getInstance()->getRFDispatcherDataRootElement();

	for (XMLElement* xmlRegisteredDevice = xmlRFDispatcher->FirstChildElement("RegisteredDevice"); xmlRegisteredDevice != nullptr; xmlRegisteredDevice = xmlRFDispatcher->NextSiblingElement("RegisteredDevice"))
	{
		// Terminate if parser fails, indicating a faulty xml config file
		if (ConfigLoader::getInstance()->parseRegisteredDevice(*xmlRegisteredDevice, &deviceId, &codeId))
			RFDispatcher::getInstance()->registerDevice(Device::getDevice(deviceId), codeId);
		else
			exit(EXIT_FAILURE);
	}
}


/*
Device* addDevice(Device* _device)
{
	devices.push_back(_device);
	return _device;
}
*/


/*
#include <string>
#include <sstream>

void istreamDevices(std::istream& is)
{
	std::string line;
	std::getline(is, line);
	std::stringstream linestream(line);

	Device::DeviceList& devices = Device::getDeviceList();
	
	unsigned int _numDevices;
	linestream >> _numDevices;
	if (_numDevices != devices.size())
	{
		std::cout << "Number of devices changed. Ignoring current device state log file." << std::endl;
		return;
	}
	
	for (Device::DeviceList::iterator it = devices.begin(); it != devices.end(); ++it)
		is >> it->second;
}


void ostreamDevices(std::ostream& os)
{
	Device::DeviceList& devices = Device::getDeviceList();
	os << Device::getDeviceList().size() << std::endl;
	for (Device::DeviceList::iterator it = devices.begin(); it != devices.end(); ++it)
		os << it->second;
}
*/


Wt::WApplication* createWebApp(const Wt::WEnvironment& env)
{
	//Wt::WApplication* _wapp = new WebApp(env, Device::getDeviceMap());
	Wt::WApplication* _wapp = new WebApp(env);
	return _wapp;
}


void interruptFunction(int sig)
{
	RFDispatcher::setTerminateFlag();
}


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
				DeviceNexaEYCR201_cast(         devices.at(0))->turnOn();
				DeviceEverflourishEMW200RA_cast(devices.at(1))->turnOn();
				DeviceNexaEYCR201_cast(         devices.at(2))->turnOn();
				DeviceNexaEYCR201_cast(         devices.at(3))->turnOn();
			}
			else if (eventData->group == 1 && eventData->method == turnoff)
			{
				DeviceNexaEYCR201_cast(         devices.at(0))->turnOff();
				DeviceEverflourishEMW200RA_cast(devices.at(1))->turnOff();
				DeviceNexaEYCR201_cast(         devices.at(2))->turnOff();
				DeviceNexaEYCR201_cast(         devices.at(3))->turnOff();
			}

			else if (eventData->group == 0 && eventData->unit == 1 && eventData->method == turnon)
			{
				DeviceNexaEYCR201_cast(         devices.at(0))->turnOn();
			}
			else if (eventData->group == 0 && eventData->unit == 1 && eventData->method == turnoff)
			{
				DeviceNexaEYCR201_cast(         devices.at(0))->turnOff();
			}

			else if (eventData->group == 0 && eventData->unit == 2 && eventData->method == turnon)
			{
				DeviceEverflourishEMW200RA_cast(devices.at(1))->turnOn();
			}
			else if (eventData->group == 0 && eventData->unit == 2 && eventData->method == turnoff)
			{
				DeviceEverflourishEMW200RA_cast(devices.at(1))->turnOff();
			}

			else if (eventData->group == 0 && eventData->unit == 3 && eventData->method == turnon)
			{
				DeviceNexaEYCR201_cast(         devices.at(2))->turnOn();
			}
			else if (eventData->group == 0 && eventData->unit == 3 && eventData->method == turnoff)
			{
				DeviceNexaEYCR201_cast(         devices.at(2))->turnOff();
			}

			else if (eventData->group == 0 && eventData->unit == 4 && eventData->method == turnon)
			{
				DeviceNexaEYCR201_cast(         devices.at(3))->turnOn();
			}
			else if (eventData->group == 0 && eventData->unit == 4 && eventData->method == turnoff)
			{
				DeviceNexaEYCR201_cast(         devices.at(3))->turnOff();
			}
			

			else if (eventData->group == 0 && eventData->unit == 5 && eventData->method == turnon)
			{
				DeviceNexaEYCR201_cast(         devices.at(0))->dim(1);
			}
			else if (eventData->group == 0 && eventData->unit == 5 && eventData->method == turnoff)
			{
				DeviceNexaEYCR201_cast(         devices.at(0))->dim(-1);
			}

			else if (eventData->group == 0 && eventData->unit == 6 && eventData->method == turnon)
			{
				DeviceEverflourishEMW200RA_cast(devices.at(1))->turnOn();
			}
			else if (eventData->group == 0 && eventData->unit == 6 && eventData->method == turnoff)
			{
				DeviceEverflourishEMW200RA_cast(devices.at(1))->turnOff();
			}

			else if (eventData->group == 0 && eventData->unit == 7 && eventData->method == turnon)
			{
				DeviceNexaEYCR201_cast(         devices.at(2))->dim(1);
			}
			else if (eventData->group == 0 && eventData->unit == 7 && eventData->method == turnoff)
			{
				DeviceNexaEYCR201_cast(         devices.at(2))->dim(-1);
			}

			else if (eventData->group == 0 && eventData->unit == 8 && eventData->method == turnon)
			{
				DeviceNexaEYCR201_cast(         devices.at(3))->dim(1);
			}
			else if (eventData->group == 0 && eventData->unit == 8 && eventData->method == turnoff)
			{
				DeviceNexaEYCR201_cast(         devices.at(3))->dim(-1);
			}

			
			else if (eventData->group == 0 && eventData->unit == 9 && eventData->method == turnon)
			{
				//devices[0]->actuate({DevicePowerSwitch::On, 15});
				DeviceNexaEYCR201_cast(         devices.at(0))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 15});
				DeviceEverflourishEMW200RA_cast(devices.at(1))->setState(DevicePowerSwitch      ::StateType{DevicePowerSwitch::On});
				DeviceNexaEYCR201_cast(         devices.at(2))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 15});
				DeviceNexaEYCR201_cast(         devices.at(3))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 15});
			}

			else if (eventData->group == 0 && eventData->unit == 9 && eventData->method == turnoff)
			{
				DeviceNexaEYCR201_cast(         devices.at(0))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 3});
				DeviceEverflourishEMW200RA_cast(devices.at(1))->setState(DevicePowerSwitch      ::StateType{DevicePowerSwitch::Off});
				DeviceNexaEYCR201_cast(         devices.at(2))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 3});
				DeviceNexaEYCR201_cast(         devices.at(3))->setState(DeviceDimmerPowerSwitch::StateType{DevicePowerSwitch::On, 3});
			}
		}
		else // if (eventData->house == )
		{
			
		}
			
	}

	delete eventData;
}
