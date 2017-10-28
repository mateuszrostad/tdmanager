#ifndef _MAIN_HPP
#define _MAIN_HPP


#include <vector>

namespace Wt
{
    class WEnvironment;
	class WApplication;
}

class Device;

void              makeDevices();
void              registerDevicesWithRFDispatcher();
Wt::WApplication* createWebApp(const Wt::WEnvironment&);
void              interruptFunction(int);
void              rawEventCallback(const char*, int, int, void*);


#endif