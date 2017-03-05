#ifndef _BASIC_DEVICES_HPP
#define _BASIC_DEVICES_HPP

#include "Device.hpp"
#include "State.hpp"
#include "SimpleSignal.hpp"
#include <vector>
//#include <string> // stoi, to_string


/*******************************************************************************
 * class DevicePowerSwitch
 */
 
#define DevicePowerSwitch_cast(L)       ((DevicePowerSwitch*)       (L))

//struct DevicePowerSwitchStateParamTypes
//{
struct PowerSwitchInterface
{
	enum {POWERSTATE = 0};
    enum PowerState {Off, On};

	virtual void       turnOn( bool force = FORCEDEFAULT)=0;
	virtual void       turnOff(bool force = FORCEDEFAULT)=0;
	virtual void       setPowerState(PowerState, bool force = FORCEDEFAULT)=0;
	virtual PowerState getPowerState()=0;

    SignalSessions<PowerState>& powerStateUpdated() {return powerStateUpdatedSignal;}
protected:
    SignalSessions<PowerState> powerStateUpdatedSignal;
};

// define_state_class(DevicePowerSwitch, DevicePowerSwitchStateParamTypes::PowerState)
define_state_class(DevicePowerSwitch, PowerSwitchInterface::PowerState)


class DevicePowerSwitch : public Device, public PowerSwitchInterface {

private: // Static vars
    //static const char* classname;

public: // Typedefs
	typedef stateType(DevicePowerSwitch) StateType;

private: // Member vars
	StateType                  state;
    //SignalSessions<PowerState> powerStateUpdatedSignal;


public: // Public interface

    // Constructors
	DevicePowerSwitch(int _id)                                  : Device(_id), state(On)                {}
	DevicePowerSwitch(int _id, const StateType&   _state)       : Device(_id), state(_state)            {}
	DevicePowerSwitch(int _id, const StateStrVec& _stateStrVec) : Device(_id), state(_stateStrVec)      {}
	DevicePowerSwitch(int _id, PowerState         _powerState)  : Device(_id), state(_powerState)       {}
	
    //SignalSessions<PowerState>&      powerStateUpdated()                                                {return powerStateUpdatedSignal;}

	void                             setState(const StateStrVec& _state,    bool force = FORCEDEFAULT)  {setState(StateType(_state), force);}           // Inherited from Device
	void                             setState(const StateType&,             bool force = FORCEDEFAULT);
	template <std::size_t N>
	void                             setState(StateType::Type<N>,           bool force = FORCEDEFAULT);
	void                             setPowerState(PowerState _powerState,  bool force = FORCEDEFAULT); //  {setState<POWERSTATE>(_powerState, force);} // Inherited from PowerSwitchInterface
    //virtual void                     actuate(StateType,                     bool force = FORCEDEFAULT);
    void                             turnOn(                                bool force = FORCEDEFAULT)  {setPowerState(On, force);}                     // Inherited from PowerSwitchInterface
    void                             turnOff(                               bool force = FORCEDEFAULT)  {setPowerState(Off, force);}                    // Inherited from PowerSwitchInterface

	StateStrVec                      getSateStringVector()                                              {return state.getStringVector();}               // Inherited from Device
	StateType                        getState()                                                         {return state;}
	template <std::size_t N>
	StateType::Type<N>               getState()                                                         {return state.get<N>();}
	PowerState                       getPowerState()                                                    {return getState<POWERSTATE>();}                // Inherited from PowerSwitchInterface
	
	virtual void                     forceActuate()                                                     {setState(state, true);}                        // Inherited from Device
	virtual DeviceActuator           getActuator(                           bool force = FORCEDEFAULT)  {return getActuator(state, force);}             // Inherited from Device
	virtual DeviceActuator           getActuator(const StateStrVec& _state, bool force = FORCEDEFAULT)  {return getActuator(StateType(_state), force);} // Inherited from Device
    virtual DeviceActuator           getActuator(StateType _state,          bool force = FORCEDEFAULT)  {return [=](){this->setState(_state, force);};}


	// Non-virtual interface
	// Virtual interface
	// Inherited virtual interface
    //virtual const char*     getClassname()                                           {return classname;}
    //PowerState getPowerState() {return powerState;}
    //virtual void actuateTurnOn();
    //virtual void actuateTurnOff();


	// Local/protected interface
protected:
    // Destructor is protected. Use freeDevice(int) to deallocate
    virtual ~DevicePowerSwitch() {}

    // Local non-virtual func decl
    //void setPowerState(PowerState);
    // Local virtual func decl/def
    //virtual void streamout(std::ostream&);
    //virtual bool streamin(std::istream&);
    // Local var def
    //PowerState powerState;
};


//define_state_converter_toString(  DevicePowerSwitch, DevicePowerSwitch::POWERSTATE, std::to_string)
//define_state_converter_fromString(DevicePowerSwitch, DevicePowerSwitch::POWERSTATE, std::stoi)

// Static var init
//const char* DevicePowerSwitch::classname = "DevicePowerSwitch";


/*******************************************************************************
 * class DeviceDimmerPowerSwitch
 */

#define DeviceDimmerPowerSwitch_cast(L) ((DeviceDimmerPowerSwitch*) (L))

//struct DeviceDimmerPowerSwitchStateParamTypes : public DevicePowerSwitchStateParamTypes
struct DimmerPowerSwitchInterface : public PowerSwitchInterface
{
	enum {DIMLEVEL = 1};
	typedef unsigned int DimLevel;

	virtual void     setDimLevel(DimLevel, bool force = FORCEDEFAULT)=0;
    virtual void     dim(        DimLevel, bool force = FORCEDEFAULT)=0;
	virtual DimLevel getDimLevel()=0;
    virtual DimLevel getDimLevelMax()=0;
	
    SignalSessions<DimLevel>& dimLevelUpdated() {return dimLevelUpdatedSignal;}
protected:
    SignalSessions<DimLevel> dimLevelUpdatedSignal;
};

define_state_class(DeviceDimmerPowerSwitch, DimmerPowerSwitchInterface::PowerState, DimmerPowerSwitchInterface::DimLevel)
//define_state_class(PowerState, DimLevel)
//define_state_converter_toString(  POWERSTATE, std::to_string) // PowerState  -> std::string
//define_state_converter_toString(  DIMLEVEL,   std::to_string) // DimLevel    -> std::string
//define_state_converter_fromString(POWERSTATE, std::stoi)      // std::string -> PowerState
//define_state_converter_fromString(DIMLEVEL,   std::stoi)      // std::string -> DimLevel


class DeviceDimmerPowerSwitch : public Device, public DimmerPowerSwitchInterface {

private:  // Static vars
    //static const char* classname;

public:   // Typedefs
	typedef stateType(DeviceDimmerPowerSwitch) StateType;
    typedef unsigned char                      HWDimValue; // Dim level values can only be integers from 0 to 255
	typedef std::vector<HWDimValue>            HWDimValues;

private:  // Member vars
	StateType                  state;
    const HWDimValues          hwDimValues;
    const DimLevel             dimLevelMax;
    //SignalSessions<PowerState> powerStateUpdatedSignal;
    //SignalSessions<DimLevel>   dimLevelUpdatedSignal;


public:	 // Public interface

    // Constructors
	DeviceDimmerPowerSwitch(int _id, const HWDimValues& _hwDimValues)                                             : Device(_id), state(On, _hwDimValues.size()-1), hwDimValues(_hwDimValues), dimLevelMax(_hwDimValues.size()-1) {}
	DeviceDimmerPowerSwitch(int _id, const HWDimValues& _hwDimValues, const StateType&   _state)                  : Device(_id), state(_state),                    hwDimValues(_hwDimValues), dimLevelMax(_hwDimValues.size()-1) {}
	DeviceDimmerPowerSwitch(int _id, const HWDimValues& _hwDimValues, const StateStrVec& _stateStrVec)            : Device(_id), state(_stateStrVec),              hwDimValues(_hwDimValues), dimLevelMax(_hwDimValues.size()-1) {}
	DeviceDimmerPowerSwitch(int _id, const HWDimValues& _hwDimValues, PowerState _powerState, DimLevel _dimLevel) : Device(_id), state(_powerState, _dimLevel),    hwDimValues(_hwDimValues), dimLevelMax(_hwDimValues.size()-1) {}

	// Funcs
    //SignalSessions<PowerState>&      powerStateUpdated()                                                {return powerStateUpdatedSignal;}
    //SignalSessions<DimLevel>&        dimLevelUpdated()                                                  {return dimLevelUpdatedSignal;}

	void                             setState(const StateStrVec& _state,    bool force = FORCEDEFAULT)  {setState(StateType(_state), force);}           // Inherited from Device
	void                             setState(const StateType&,             bool force = FORCEDEFAULT);
	template <std::size_t N>
	void                             setState(StateType::Type<N>,           bool force = FORCEDEFAULT);
	void                             setPowerState(PowerState _powerState,  bool force = FORCEDEFAULT); //  {setState<POWERSTATE>(_powerState, force);}// Inherited from PowerSwitchInterface
	void                             setDimLevel(DimLevel _dimLevel,        bool force = FORCEDEFAULT); //  {setState<DIMLEVEL>(_dimLevel, force);}    // Inherited from DimmerPowerSwitchInterface
    //virtual void                     actuate(StateType,                     bool force = FORCEDEFAULT);
    void                             turnOn(                                bool force = FORCEDEFAULT)  {setPowerState(On, force);}                    // Inherited from PowerSwitchInterface
    void                             turnOff(                               bool force = FORCEDEFAULT)  {setPowerState(Off, force);}                   // Inherited from PowerSwitchInterface
    void                             dim(DimLevel _dimLevel,                bool force = FORCEDEFAULT)  {setDimLevel(getDimLevel() + _dimLevel);}      // Inherited from DimmerPowerSwitchInterface

	StateStrVec                      getSateStringVector()                                              {return state.getStringVector();}               // Inherited from Device
	StateType                        getState()                                                         {return state;}
	template <std::size_t N>
	StateType::Type<N>               getState()                                                         {return state.get<N>();}
	PowerState                       getPowerState()                                                    {return getState<POWERSTATE>();}                // Inherited from PowerSwitchInterface
    DimLevel                         getDimLevel()                                                      {return getState<DIMLEVEL>();}                  // Inherited from DimmerPowerSwitchInterface
    DimLevel                         getDimLevelMax()                                                   {return dimLevelMax;}                           // Inherited from DimmerPowerSwitchInterface

	virtual void                     forceActuate()                                                     {setState(state, true);}                        // Inherited from Device
	virtual DeviceActuator           getActuator(                           bool force = FORCEDEFAULT)  {return getActuator(state, force);}             // Inherited from Device
	virtual DeviceActuator           getActuator(const StateStrVec& _state, bool force = FORCEDEFAULT)  {return getActuator(StateType(_state), force);} // Inherited from Device
    virtual DeviceActuator           getActuator(StateType _state,          bool force = FORCEDEFAULT)  {return [=](){this->setState(_state, force);};}


	// Non-virtual interface
	// Virtual interface
	// Inherited virtual interface
    //virtual const char*     getClassname()                                           {return classname;}
    //PowerState getPowerState() {return powerState;}
    //virtual void actuateTurnOn();
    //virtual void actuateTurnOff();


	// Local/protected interface
protected:
    // Destructor is protected. Use freeDevice(int) to deallocate
    virtual ~DeviceDimmerPowerSwitch() {}

private:
    DimLevel validateDimLevel(DimLevel);

    // Local non-virtual func decl
    //void setPowerState(PowerState);
    // Local virtual func decl/def
    //virtual void streamout(std::ostream&);
    //virtual bool streamin(std::istream&);
    // Local var def
    //PowerState powerState;
};

//define_state_converter_toString(  DeviceDimmerPowerSwitch, DeviceDimmerPowerSwitch::POWERSTATE, std::to_string) // PowerState  -> std::string
//define_state_converter_toString(  DeviceDimmerPowerSwitch, DeviceDimmerPowerSwitch::DIMLEVEL,   std::to_string) // DimLevel    -> std::string
//define_state_converter_fromString(DeviceDimmerPowerSwitch, DeviceDimmerPowerSwitch::POWERSTATE, std::stoi)      // std::string -> PowerState
//define_state_converter_fromString(DeviceDimmerPowerSwitch, DeviceDimmerPowerSwitch::DIMLEVEL,   std::stoi)      // std::string -> DimLevel

// Static var init
//const char* DeviceDimmerPowerSwitch::classname = "DeviceDimmerPowerSwitch";




/*
class _DeviceDimmerPowerSwitch : public DevicePowerSwitch {

public:

    // Member type def
    typedef unsigned char DimLevelValueType; // Dim level values can only be integers from 0 to 255
	typedef std::vector<DimLevelValueType> DimLevelList;
    //struct State {PowerState powerState; int dimLevel;};

    struct State : public DevicePowerSwitch::State
    {
        int dimLevel;
        State(PowerState _powerState, int _dimLevel) : DevicePowerSwitch::State(_powerState), dimLevel(_dimLevel) {}
        virtual ~State() {}
    };
	
    // Constructor decl
    DeviceDimmerPowerSwitch(int, DimLevelList);
    DeviceDimmerPowerSwitch(int, DimLevelList, PowerState, int);

    // Virtual interface func decl
    virtual void turnOn(bool force = FORCEDEFAULT);
    //virtual void actuateTurnOn();
    virtual void dimOffset(int, bool force = FORCEDEFAULT);
    virtual void dim(      int, bool force = FORCEDEFAULT);
    //virtual void actuateState();

    //virtual void           actuate(    Device::State, bool force = FORCEDEFAULT);
    //virtual DeviceActuator getActuator(Device::State, bool force = FORCEDEFAULT);
    virtual void           actuate(    DeviceStateBase, bool force = FORCEDEFAULT);
    virtual DeviceActuator getActuator(DeviceStateBase, bool force = FORCEDEFAULT);

    // Non-virtual interface func decl/def

    int getDimLevel()    {return dimLevel;}
    int getDimLevelMax() {return dimLevelMax;}

    SignalSessions<int>& dimLevelUpdated() {return dimLevelUpdatedSignal;}

protected:

    // Destructor is protected. Use freeDevice(int) to deallocate
    virtual ~DeviceDimmerPowerSwitch() {}

    // Virtual local func decl/def
    virtual void streamout(std::ostream&);
    virtual bool streamin(std::istream&);

    virtual const char* getClassname()
    {
        return classname;
    }

    // Local var def
    DimLevelList dimLevelValues;
    const int dimLevelMax;
    int dimLevel;

    SignalSessions<int> dimLevelUpdatedSignal;

private:

    // Non-virtual local func decl
    int validateDimLevel(int);

    // Static var def
    static const char* classname;

};
*/

#endif