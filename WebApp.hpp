#ifndef WebApp_HPP
#define WebApp_HPP

//#include "Device.hpp"

#include <vector>
#include <list>
#include <mutex>
#include <Wt/WApplication>

//class Device;
//class WDimmerPowerSwitch;
//class WPowerSwitch;
//class SessionsContainer;

namespace Wt
{
	class WEnvironment;
	class WContainerWidget;
	class WSlider;
}


class WebApp : public Wt::WApplication
{

public:

	//typedef std::vector<Device*> DeviceListType;

	WebApp(const Wt::WEnvironment&);
	//WebApp(const Wt::WEnvironment&, const Device::DeviceMap&);
	//WebApp(const Wt::WEnvironment&, Device::DeviceMap&);
	//WebApp(const Wt::WEnvironment&, DeviceListType&, SessionsContainer*);
	~WebApp();
	
	void callback();

	
private:
	
	std::mutex     mtx;
	//Device::DeviceMap& devices;
	//SessionsContainer* sessions;

};


#endif