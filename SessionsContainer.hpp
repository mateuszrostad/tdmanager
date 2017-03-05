#ifndef SessionsContainer_HPP
#define SessionsContainer_HPP

#include <iostream>
#include <map>
#include <mutex>
//#include <boost/thread/mutex.hpp>
#include <string>

//#include <functional>
//#include <Wt/WSignal>


/*
namespace Wt
{
	class WApplication;
}
*/

// TODO: try only using base class WApplication instead of WebApp. Should be enough.
class WebApp;


class SessionsContainer
{
	
private:
	
	//typedef std::map<std::string, Wt::WApplication*> ContainerType;
	typedef std::map<std::string, WebApp*> ContainerType;
	
	// Private constructor. Instantiation only through static function instatiate()
	SessionsContainer();

public:

	~SessionsContainer();
	//void add(Wt::WApplication*);
	//void remove(Wt::WApplication*);
	void add(WebApp*);
	void remove(WebApp*);
	
	//void negotiateSignal(std::bind(DevicePowerSwitch::powerStateChanged, DevicePowerSwitch_cast(device)), std::bind(WPowerSwitch::updatePowerStateIndicator, _1), deviceId())
	//template <class _SignalClass, class _FunctionClass> void negotiateSignal(_SignalClass*, _FunctionClass, std::string); //Wt::Signal<...>, std::function<...>, 
	//void negotiateSignal(Wt::Signal<int>&, std::function<void(int)>, std::string); //Wt::Signal<...>, std::function<...>, 
	//void negotiateSignal(Wt::Signal<int>&, std::function<void(WPowerSwitch*, int)>, std::string);
	//template <class _Signal, class _Function> void negotiateSignal(_Signal&, _Function, std::string);
	void postAll();
	
	static SessionsContainer* instantiate();
	static bool               hasInstance();
	static SessionsContainer* getInstance();
	
private:

	std::mutex mtx;
	//boost::mutex mtx;
	ContainerType sessions;
	
	static SessionsContainer* instance;
	
};

#endif