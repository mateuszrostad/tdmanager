#ifndef _DEVICE_HPP
#define _DEVICE_HPP


#include <vector>
#include <map>
#include <functional>
#include <string>
#include "State.hpp"
#include "Factory.hpp"
#include "SimpleSignal.hpp"
#include "ConfigLoader.hpp"

#define FORCEDEFAULT false

// Macro for registrating in class Factory a derived class of Device.
// The derived class must implement a constructor with input signature
// <int, const stateStrVec&> (device id, state initialization string vector)
#define register_device_in_factory(DeviceType)\
class DeviceType;\
FactoryRegistrar<DeviceType, Device::DeviceId>              registrar_##DeviceType(#DeviceType);\
FactoryRegistrar<DeviceType, Device::DeviceId, StateStrVec> registrar_##DeviceType##_StateStrVec(#DeviceType);


typedef std::function<void(void)>   DeviceActuator;
typedef std::vector<DeviceActuator> DeviceActuatorList;


/*******************************************************************************
 * class Device definition
 */

class Device : public Manufacturable {

public:    // typedefs
	using DeviceId         = int; // TODO: Rename typedef DeviceId to just Id
	template <class DeviceT>
	using GenericDeviceMap = std::map<DeviceId, DeviceT>;
    using DeviceMap        = GenericDeviceMap<Device*>;

private:   // Static vars
    static DeviceMap deviceMap;

public:    // Static interface
    static const DeviceMap& getDeviceMap() {return deviceMap;}
	static bool             isValid(DeviceId);
	static Device*          getDevice(DeviceId);
    static void             freeDevice(DeviceId);
    static void             freeAllDevices();
	static void             setDeviceStateFromXML(XMLElement*);
    static bool             checkXMLDeviceDefs();
	static DeviceActuator   getDeviceActuatorFromXML(XMLElement*);

private:   // Member vars
    DeviceId         deviceId;
	SignalSessions<> beforeDeleteSignal;
	std::string      _class, name, location; // TODO: implement these vars and their setter and getter funcs in a location manager rather than here

	
public:    // Public interface

    // Constructor
    Device(DeviceId);

	SignalSessions<>&                beforeDelete() {return beforeDeleteSignal;}
	
	// Non-virtual interface
	void                             setClass(   const std::string& __class)    {_class    = __class;}
	void                             setName(    const std::string&  _name)     { name     =  _name;}
	void                             setLocation(const std::string&  _location) { location =  _location;}
	int                              getId()                                    {return  deviceId;}
	std::string                      getClass()                                 {return _class;}
	std::string                      getName()                                  {return  name;}
	std::string                      getLocation()                              {return  location;}
	
	// Virtual interface
	virtual void                     setState(const StateStrVec&, bool force = FORCEDEFAULT)=0;
	virtual StateStrVec              getStateStringVector()=0;
	virtual void                     forceActuate()=0;
	virtual DeviceActuator           getActuator(bool force = FORCEDEFAULT)=0;
	virtual DeviceActuator           getActuator(const StateStrVec&, bool force = FORCEDEFAULT)=0;

	
protected: // Local/protected interface

	// Destructor is protected. Use freeDevice(int) to deallocate
    virtual ~Device();

};

#endif