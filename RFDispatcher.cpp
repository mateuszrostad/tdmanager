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
{}


RFDispatcher::~RFDispatcher()
{}

	
void RFDispatcher::registerDevice(Device::DeviceId deviceId, int codeId)
{
	if (~Device::isValid(deviceId))
	{
		std::cout << "RFDispatcher::registerDevice(...): Received invalid device id " << deviceId << "." << std::endl;
		exit(EXIT_FAILURE);
	}

	//mtx.lock();
	//deviceMap[device->getId()] = DeviceData{device, codeId};
	//commandQueue[device->getId()] = DeviceCommandQueue();
	deviceMap[deviceId] = DeviceData{codeId};
	commandQueue[deviceId] = DeviceCommandQueue();
	//mtx.unlock();
}
	

void RFDispatcher::runThread()
{
	std::thread thr(&RFDispatcher::run, this);
}


void RFDispatcher::run()
{
	while (!terminateFlag)
	{
		for (auto it : commandQueue)
		{
			while (!it.second.empty())
			{
				CommandData command = it.second.front();
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
				it.second.clear();
				//mtx.unlock();
			}
		}
		usleep(100e3);
	}
}
	

void RFDispatcher::submitCommand(Command command, Device::DeviceId deviceId)
{
	if (~Device::isValid(deviceId))
	{
		std::cout << "RFDispatcher::submitCommand(Command, Device::DeviceId): Received invalid device id " << deviceId << "." << std::endl;
		exit(EXIT_FAILURE);
	}
	
	std::cout << "RFDispatcher::submitCommand(" << command << ", " << "deviceId " << deviceId << ")" << std::endl;
	
	mtx.lock();
	commandQueue[deviceId].emplace_front(command, getDeviceData(deviceId).codeId);
	mtx.unlock();
}


void RFDispatcher::submitCommand(Command command, Device::DeviceId deviceId, unsigned char value)
{
	if (~Device::isValid(deviceId))
	{
		std::cout << "RFDispatcher::submitCommand(Command, Device::DeviceId, unsigned char): Received invalid device id " << deviceId << "." << std::endl;
		exit(EXIT_FAILURE);
	}
	
	std::cout << "RFDispatcher::submitCommand(" << command << ", " << "device " << deviceId << ", " << int(value) << ")" << std::endl;

	mtx.lock();
	commandQueue[deviceId].emplace_front(command, getDeviceData(deviceId).codeId, value);
	mtx.unlock();
}


const DeviceData& RFDispatcher::getDeviceData(const Device::DeviceId deviceId)
{
	if (deviceMap.count(deviceId) != 1)
	{
		std::cout << "RFDispatcher::getDeviceData(Device::DeviceId): Device " << deviceId << " not regestired with RFDispatcher." << std::endl;
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
