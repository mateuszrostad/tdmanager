#ifndef RawEventHandler_hpp
#define RawEventHandler_hpp


enum RemoteEventDataClass    {command = 1};
enum RemoteEventDataProtocol {arctech = 1};
enum RemoteEventDataModel    {selflearning = 1};
enum RemoteEventDataMethod   {turnon = 1, turnoff};


struct RemoteEventData
{
	int class_;
	int protocol;
	int model;
	int house;
	int unit;
	int group;
	int code;
	int method;
	
	RemoteEventData() : class_(0), protocol(0), model(0), house(0), unit(0), group(0), code(0), method(0) {}
};

RemoteEventData* getRemoteEventData(const char*);


#endif