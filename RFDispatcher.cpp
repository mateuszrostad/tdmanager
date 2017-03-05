#include <cstdlib>
#include <telldus-core.h>
#include <iostream> // cout
#include <unistd.h> // usleep
#include <vector>
#include <list>
#include <thread>
#include "Device.hpp"
#include "RFDispatcher.hpp"


bool RFDispatcher::terminateFlag = false;


RFDispatcher::RFDispatcher()
{
	//running = false;
	//tdInit();
}


RFDispatcher::~RFDispatcher()
{
	//freeDevices();
	//tdClose();
}

	
void RFDispatcher::registerDevice(Device* device, int codeId)
{
	//mtx.lock();
	//_device->setRFDispatcher(this);
	//_device->setDeviceId(deviceVector.size()); // This should be moved out
	deviceMap[device->getId()] = DeviceData(device, codeId);
	//deviceVector.emplace_back(_device, codeId);
	commandQueue[device->getId()] = DeviceCommandQueue();
	//mtx.unlock();
}
	

/*
void RFDispatcher::freeDevices()
{
	for (DeviceVector::iterator it = deviceVector.begin(); it != deviceVector.end(); ++it)
		delete it->device;
}
*/


void RFDispatcher::runThread()
{
	//std::thread newThread(run);
	std::thread thr(&RFDispatcher::run, this);
}


void RFDispatcher::run()
{
	while (!terminateFlag)
	{
		for (CommandQueue::iterator it = commandQueue.begin(); it != commandQueue.end(); ++it)
		{
			while (!it->second.empty())
			{
				CommandData command = it->second.front();
				switch (command.command)
				{
				case TurnOn:
					doTurnOn(command.codeId);
					break;
				case TurnOff:
					doTurnOff(command.codeId);
					break;
				case Dim:
					doDim(command.codeId, command.param);
					break;
				default:
					continue;
				}
				//std::cout << "Code ID: " << command.codeId << "  Command: " << command.command << std::endl;
				//mtx.lock();
				it->second.clear();
				//mtx.unlock();
			}
		}
		//std::cout << "RFDispatcher::run() usleep(1e6)" << std::endl;
		usleep(100e3);
	}
}
	

void RFDispatcher::submitCommand(Command command, Device* device)
{
	std::cout << "RFDispatcher::submitCommand(" << command << ", " << "deviceId " << device->getId() << ")" << std::endl;
	
	int deviceId = device->getId();
	mtx.lock();
	commandQueue[deviceId].emplace_front(command, getDeviceData(deviceId).codeId);
	mtx.unlock();
}


void RFDispatcher::submitCommand(Command command, Device* device, unsigned char value)
{
	std::cout << "RFDispatcher::submitCommand(" << command << ", " << "device " << device->getId() << ", " << int(value) << ")" << std::endl;

	int deviceId = device->getId();
	mtx.lock();
	commandQueue[deviceId].emplace_front(command, getDeviceData(deviceId).codeId, value);
	mtx.unlock();
}


const DeviceData& RFDispatcher::getDeviceData(const Device::DeviceId deviceId)
{
	if (deviceMap.count(deviceId) != 1)
	{
		std::cout << "RFDispatcher::getDeviceData(): Device " << deviceId << " not regestired with RFDispatcher." << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		return deviceMap[deviceId];
}


void RFDispatcher::doTurnOn(int codeId)
{
	tdTurnOn(codeId);
	std::cout << "tdmanager: RF-dispatcher sent command tdTrunOn(" << codeId << ") to tellstick daemon" << std::endl;
	usleep(1000e3);
}


void RFDispatcher::doTurnOff(int codeId)
{
	tdTurnOff(codeId);
	std::cout << "tdmanager: RF-dispatcher sent command tdTrunOff(" << codeId << ") to tellstick daemon" << std::endl;
	usleep(1000e3);
}


void RFDispatcher::doDim(int codeId, unsigned char value)
{
	tdDim(codeId, value);
	std::cout << "tdmanager: RF-dispatcher sent command tdDim(" << codeId << ", " << int(value) << ") to tellstick daemon" << std::endl;
	usleep(1000e3);
}

	
void RFDispatcher::setTerminateFlag()
{
	terminateFlag = true;
}


RFDispatcher* RFDispatcher::getInstance()
{
	if (instance == nullptr)
		return (instance = new RFDispatcher());
	else
		return instance;
}


void RFDispatcher::free()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}


RFDispatcher* RFDispatcher::instance = nullptr;
