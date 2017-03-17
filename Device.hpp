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
#define register_in_factory(DeviceType)\
class DeviceType;\
FactoryRegistrar<DeviceType, Device::DeviceId, StateStrVec> registrar_##DeviceType(#DeviceType);


//class XMLElement;	
//class Device;
//class RFDispatcher;

typedef std::function<void(void)>   DeviceActuator;
typedef std::vector<DeviceActuator> DeviceActuatorList;


//struct State {virtual ~State() {}};


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
    //static const char* classname;

public:    // Static interface
    static const DeviceMap& getDeviceMap() {return deviceMap;}
	static bool             isValid(DeviceId);
	static Device*          getDevice(DeviceId);
    static void             freeDevice(DeviceId);
    static void             freeAllDevices();
	static DeviceActuator   getDeviceActuatorFromXML(XMLElement*);


private:   // Member vars
    DeviceId         deviceId;
	SignalSessions<> beforeDeleteSignal;
    //RFDispatcher* rfDispatcher;
	std::string      name, location; // TODO: implement these vars and their setter and getter funcs in a location manager rather than here
	
public:    // Public interface

    // Constructor
    Device(DeviceId);

	SignalSessions<>&                beforeDelete() {return beforeDeleteSignal;}
	
	// Non-virtual interface
	int                              getId()        {return deviceId;}
	//void                             setRFDispatcher(RFDispatcher* _rfDispatcher) {rfDispatcher = _rfDispatcher;}
	//void                             setRFDispatcher(RFDispatcher& _rfDispatcher) {rfDispatcher = &_rfDispatcher;}
	//const RFDispatcher*              getRFDispatcher() {return rfDispatcher;}
	void                             setName(const std::string& _name)         {name = _name;}
	void                             setLocation(const std::string& _location) {location = _location;}
	std::string                      getName()                                 {return name;}
	std::string                      getLocation()                             {return location;}
	
	// Virtual interface
	//virtual void                     setState(DerivedDeviceState& _state)=0;
	//virtual DerivedDeviceState       getState()=0;
	virtual void                     setState(const StateStrVec&, bool force = FORCEDEFAULT)=0;
	virtual StateStrVec              getSateStringVector()=0;
    //virtual void                     actuate(DeviceStateBase, bool force = FORCEDEFAULT)=0;
    //virtual DeviceActuator           getActuator(DeviceStateBase, bool force = FORCEDEFAULT)=0;
	virtual void                     forceActuate()=0;
	virtual DeviceActuator           getActuator(bool force = FORCEDEFAULT)=0;
	virtual DeviceActuator           getActuator(const StateStrVec&, bool force = FORCEDEFAULT)=0;
    //virtual const char*              getClassname()=0; // {return classname;}
	
	
	
    // Non-member friend func decl
    //friend std::ostream& operator<<(std::ostream&, Device*);
    //friend std::istream& operator>>(std::istream&, Device*);
	

protected: // Local/protected interface

	// Destructor is protected. Use freeDevice(int) to deallocate
    virtual ~Device();

    // Local virtual interface
    //virtual void streamout(std::ostream&);
    //virtual bool streamin(std::istream&);

};


// Static var init
//const char* Device::classname = "Device";


#endif