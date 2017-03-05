#ifndef _MAIN_HPP
#define _MAIN_HPP


#include <vector>

namespace Wt
{
    class WEnvironment;
	class WApplication;
}

class Device;
//typedef std::vector<Device*> DeviceListType; // //
const char* deviceStateFileName = "device-state-log";

//Device* addDevice(Device*);
//void    istreamDevices(std::istream&);
//void    ostreamDevices(std::ostream&);
//void    freeDevices();


void              makeDevices();
void              registerDevicesWithRFDispatcher();
Wt::WApplication* createWebApp(const Wt::WEnvironment&);
void              interruptFunction(int);
void              rawEventCallback(const char*, int, int, void*);


#endif