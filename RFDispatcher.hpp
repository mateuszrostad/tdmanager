#ifndef RFDispatcher_hpp
#define RFDispatcher_hpp


#include <vector>
#include <list>
#include <mutex>
#include "Device.hpp"


struct DeviceData;
struct CommandData;


class RFDispatcher
{

protected:

	static RFDispatcher* instance;
	
	static bool          terminateFlag;

public:

	enum Command {TurnOn, TurnOff, Dim};

	static RFDispatcher* getInstance();
	static void free();
	
	RFDispatcher();
	~RFDispatcher();
	
	void registerDevice(Device*, int);
	void runThread();
	void run();
	
	void submitCommand(Command, Device*);
	void submitCommand(Command, Device*, unsigned char);

	const DeviceData& getDeviceData(const Device::DeviceId deviceId);
	
	static void setTerminateFlag();
	
protected:

	typedef std::map<Device::DeviceId, DeviceData> DeviceMap;
	DeviceMap deviceMap;

	typedef std::list<CommandData>                         DeviceCommandQueue;
	typedef std::map<Device::DeviceId, DeviceCommandQueue> CommandQueue;
	CommandQueue                                           commandQueue;
	
	std::recursive_mutex mtx;

	void doTurnOn(int);
	void doTurnOff(int);
	void doDim(int, unsigned char);
	
};


struct DeviceData
{
	Device* device;
	int codeId;
};


struct CommandData
{
	CommandData(RFDispatcher::Command _command, int _codeId, unsigned char _param) : command(_command), codeId(_codeId), param(_param) {}
	CommandData(RFDispatcher::Command _command, int _codeId)                       : command(_command), codeId(_codeId), param(0) {}
	
	RFDispatcher::Command command;
	int                   codeId;
	unsigned char         param;
};



#endif