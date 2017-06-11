#ifndef LocalDevices_hpp
#define LocalDevices_hpp

#include "BasicDevices.hpp"
#include "State.hpp"

#define DeviceEverflourishEMW200RA_cast(L) ((DeviceEverflourishEMW200RA*) (L))
#define DeviceNexaEYCR201_cast(L) ((DeviceNexaEYCR201*) (L))
#define DeviceNexaLGDR3500_cast(L) ((DeviceNexaLGDR3500*) (L))


/*******************************************************************************
 * class DeviceEverflourishEMW200RA
 */

class DeviceEverflourishEMW200RA : public DevicePowerSwitch
{
	
public:

	// Constructors
    DeviceEverflourishEMW200RA(int _id)                            : DevicePowerSwitch(_id) {}
    DeviceEverflourishEMW200RA(int _id, const StateType& _state)   : DevicePowerSwitch(_id, _state) {}
    DeviceEverflourishEMW200RA(int _id, const StateStrVec& _state) : DevicePowerSwitch(_id, _state) {}
    DeviceEverflourishEMW200RA(int _id, PowerState _powerState)    : DevicePowerSwitch(_id, _powerState) {}
	// Destructor
    ~DeviceEverflourishEMW200RA() {}
	
};


/*******************************************************************************
 * class DeviceNexaEYCR201
 */

#define L(n) 16*((n)+1)-1
#define D() {L(0), L(1), L(2), L(3), L(4), L(5), L(6), L(7), L(8), L(9), L(10), L(11), L(12), L(13), L(14), L(15)}

class DeviceNexaEYCR201 : public DeviceDimmerPowerSwitch
{
	
public:

	// Constructors
	DeviceNexaEYCR201(int _id)                                             : DeviceDimmerPowerSwitch(_id, D()) {}
	DeviceNexaEYCR201(int _id, const StateType& _state)                    : DeviceDimmerPowerSwitch(_id, D(), _state) {}
	DeviceNexaEYCR201(int _id, const StateStrVec& _state)                  : DeviceDimmerPowerSwitch(_id, D(), _state) {}
	DeviceNexaEYCR201(int _id, PowerState _powerState, DimLevel _dimLevel) : DeviceDimmerPowerSwitch(_id, D(), _powerState, _dimLevel) {}
	// Destructor
	~DeviceNexaEYCR201() {}

};

#undef D
#undef L


/*******************************************************************************
 * class DeviceNexaLGDR3500
 */

class DeviceNexaLGDR3500 : public DevicePowerSwitch
{
	
public:

	// Constructors
    DeviceNexaLGDR3500(int _id)                            : DevicePowerSwitch(_id) {}
    DeviceNexaLGDR3500(int _id, const StateType& _state)   : DevicePowerSwitch(_id, _state) {}
    DeviceNexaLGDR3500(int _id, const StateStrVec& _state) : DevicePowerSwitch(_id, _state) {}
    DeviceNexaLGDR3500(int _id, PowerState _powerState)    : DevicePowerSwitch(_id, _powerState) {}
	// Destructor
    ~DeviceNexaLGDR3500() {}
	
};


#endif