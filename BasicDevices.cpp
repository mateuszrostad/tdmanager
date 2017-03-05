#include "BasicDevices.hpp"
#include "Factory.hpp"
#include "RFDispatcher.hpp"
#include <iostream>
#include <cstdlib> // exit, EXIT_FAILURE
//#include <cctype> // isspace()
//#include <cstdio> // EOF

//#include "SessionsContainer.hpp"
//#include "WebApp.hpp"
//#include <Wt/WApplication>
//#include <Wt/WServer>


/*******************************************************************************
 * class DevicePowerSwitch
 */

// Register in Factory
//FactoryRegistrar<DevicePowerSwitch, int>                           registrar_DevicePowerSwitch(          "DevicePowerSwitch");
//FactoryRegistrar<DevicePowerSwitch, int, DevicePowerSwitch::State> registrar_DevicePowerSwitch_InitState("DevicePowerSwitch_InitState");

register_in_factory(DevicePowerSwitch)

// Funcs
void DevicePowerSwitch::setState(const StateType& _state, bool force)
{
	PowerState newPowerState = _state.get<POWERSTATE>();
	if (newPowerState != getPowerState() || force)
	{
		state.get<POWERSTATE>() = newPowerState;
		powerStateUpdated().emit(newPowerState);
		RFDispatcher::getInstance()->submitCommand(newPowerState == On ? RFDispatcher::TurnOn : RFDispatcher::TurnOff, this);
	}
	else
		std::cout << "tdmanager: DevicePowerSwitch (device id " << getId() << ") skipped submitting command " << (newPowerState == On ? "TurnOn" : "TurnOff") << " to tellstick daemon" << std::endl;
}


template <std::size_t N>
void DevicePowerSwitch::setState(StateType::Type<N> _stateElement, bool force)
{}


// Specialization template setState()
template <>
void DevicePowerSwitch::setState<DevicePowerSwitch::POWERSTATE>(StateType::Type<DevicePowerSwitch::POWERSTATE> newPowerState, bool force)
{
	if (newPowerState != getPowerState() || force)
	{
		state.get<POWERSTATE>() = newPowerState;
		powerStateUpdated().emit(newPowerState);
		RFDispatcher::getInstance()->submitCommand(newPowerState == On ? RFDispatcher::TurnOn : RFDispatcher::TurnOff, this);
	}
	else
		std::cout << "tdmanager: DevicePowerSwitch (device id " << getId() << ") skipped submitting command " << (newPowerState == On ? "TurnOn" : "TurnOff") << " to tellstick daemon" << std::endl;

	//StateType _state(state);
	//_state.get<POWERSTATE>() = newPowerState;
	//setState(_state);
	
	//setState(StateType(newPowerState), force);
}


void DevicePowerSwitch::setPowerState(PowerState newPowerState, bool force)
{
	setState<POWERSTATE>(newPowerState, force);
}


/*
void DevicePowerSwitch::actuate(StateType _state)
{
	setState(_state);
	actuate(force);
}
*/


/*
DeviceActuator DevicePowerSwitch::getActuator(DeviceStateBase _state, bool force)
{
	State* state = dynamic_cast<State*>(&_state);
	if (state == nullptr)
	{
		std::cout << "DevicePowerSwitch::getActuator(): Invalid state type." << std::endl;
		exit(EXIT_FAILURE);
	}
	
	//DeviceActuator f = std::bind<void(DevicePowerSwitch*, DeviceStateBase, bool)>(&DevicePowerSwitch::actuate, this, *state, force);

	//if      (state.powerState == On)
	//	f = std::bind(&DevicePowerSwitch::turnOn, this, force);
	//else if (state.powerState == Off)
	//	f = std::bind(&DevicePowerSwitch::turnOff, this, force);

	return [=](){this->actuate(*state, force);}; //std::bind(&DevicePowerSwitch::actuate, this, *state, force);
	//TODO: shorten body to one line
}
*/


/*
void DevicePowerSwitch::actuateTurnOn()
{
	setPowerState(On);
	rfDispatcher->submitCommand(RFDispatcher::TurnOn, this);
}


void DevicePowerSwitch::actuateTurnOff()
{
	setPowerState(Off);
	rfDispatcher->submitCommand(RFDispatcher::TurnOff, this);
}
*/

/*
void DevicePowerSwitch::actuateState()
{
	if      (powerState == On)
		actuateTurnOn();
	else if (powerState == Off)
		actuateTurnOff();
}
 */


 /*
void DevicePowerSwitch::actuate(DeviceStateBase _state, bool force)
{
	State* state = dynamic_cast<State*>(&_state);
	if (state == NULL)
	{
		std::cout << "DevicePowerSwitch::actuate(): Invalid state type." << std::endl;
		exit(EXIT_FAILURE);
	}
	
	if      (state->get<POWERSTATE>() == On)
		turnOn(force);
	else if (state->get<POWERSTATE>() == Off)
		turnOff(force);
}
*/



/*
int DevicePowerSwitch::getPowerState()
{
	return powerState;
}
*/


/*
void DevicePowerSwitch::setPowerState(PowerState _powerState)
{
	powerState = _powerState;
	powerStateUpdated().emit((int) _powerState);
	/ *
	if      (_powerState == On && powerState == Off)
	{
		powerState = On;
		powerStateChanged().emit(1);
	}
	else if (_powerState == Off && powerState == On)
	{
		powerState = Off;
		powerStateChanged().emit(0);
	}
	* /
}
*/


/*
void DevicePowerSwitch::streamout(std::ostream& os)
{
	Device::streamout(os);
	os << ' ' << powerState;
}
*/


/*
bool DevicePowerSwitch::streamin(std::istream& is)
{
	if (!Device::streamin(is))
		return false;

	if (is.eof())
	{
		std::cout << "DevicePowerSwitch: Device ID: " << getDeviceId() << " Log file parameters missing." << std::endl;
		return false;
	}
	int _powerState;
	is >> _powerState;
	powerState = (PowerState) _powerState;
	
	return true;
}
*/


/*******************************************************************************
 * class DeviceDimmerPowerSwitch
 */

// Register in Factory
//FactoryRegistrar<DeviceDimmerPowerSwitch, int, DeviceDimmerPowerSwitch::DimLevelValues>
//registrar_DeviceDimmerPowerSwitch(          "DeviceDimmerPowerSwitch");
//FactoryRegistrar<DeviceDimmerPowerSwitch, int, DeviceDimmerPowerSwitch::DimLevelValues, DeviceDimmerPowerSwitch::State>
//registrar_DeviceDimmerPowerSwitch_InitState("DeviceDimmerPowerSwitch_InitState");


//register_in_factory(DeviceDimmerPowerSwitch)
FactoryRegistrar<DeviceDimmerPowerSwitch, int, const DeviceDimmerPowerSwitch::HWDimValues&, const StateStrVec&> registrar_DeviceDimmerPowerSwitch("DeviceDimmerPowerSwitch");


// Funcs
void DeviceDimmerPowerSwitch::setState(const StateType& _state, bool force)
{
	PowerState oldPowerState = getPowerState();
	DimLevel   oldDimLevel   = getDimLevel();
	PowerState newPowerState = _state.get<POWERSTATE>();
	DimLevel   newDimLevel   = _state.get<DIMLEVEL>();
	bool powerChanged        = newPowerState != oldPowerState;
	bool powerTurnedOn       = powerChanged && newPowerState == On;
	bool powerTurnedOff      = powerChanged && newPowerState == Off;
	bool powerUnchangedOn    = !powerChanged && oldPowerState == On;
	bool dimLevelChanged     = newDimLevel != oldDimLevel;
	
	if (powerChanged || force)
	{
		state.get<POWERSTATE>() = newPowerState;
		powerStateUpdated().emit(newPowerState);
	}

	if (dimLevelChanged || force)
	{
		state.get<DIMLEVEL>() = newDimLevel;
		dimLevelUpdated().emit(newDimLevel);
	}

	if (force)
	{
		if (newPowerState == On)
			RFDispatcher::getInstance()->submitCommand(RFDispatcher::Dim, this, hwDimValues[newDimLevel]);
		else // implied: newPowerState == Off
			RFDispatcher::getInstance()->submitCommand(RFDispatcher::TurnOff, this);
	}
	else if (powerTurnedOn || (powerUnchangedOn && dimLevelChanged))
		RFDispatcher::getInstance()->submitCommand(RFDispatcher::Dim, this, hwDimValues[newDimLevel]);
	else if (powerTurnedOff)
		RFDispatcher::getInstance()->submitCommand(RFDispatcher::TurnOff, this);
	else
		std::cout << "tdmanager: DeviceDimmerPowerSwitch (device id " << getId() << ") skipped submitting command to tellstick daemon" << std::endl;
}


template <std::size_t N>
void DeviceDimmerPowerSwitch::setState(StateType::Type<N> _stateElement, bool force)
{}

// Specializations of template setState()
template <>
void DeviceDimmerPowerSwitch::setState<DeviceDimmerPowerSwitch::POWERSTATE>(StateType::Type<DeviceDimmerPowerSwitch::POWERSTATE> newPowerState, bool force)
{
	if (newPowerState != getPowerState() || force)
	{
		state.get<POWERSTATE>() = newPowerState;
		powerStateUpdated().emit(newPowerState);
		if (newPowerState == On) // changed or forced
			RFDispatcher::getInstance()->submitCommand(RFDispatcher::Dim, this, hwDimValues[getDimLevel()]);
		else // implied: newPowerState == Off, changed or forced
			RFDispatcher::getInstance()->submitCommand(RFDispatcher::TurnOff, this);
	}
	else // power state unchanged and unforced
		std::cout << "tdmanager: DevicePowerSwitch (device id " << getId() << ") skipped submitting command " << (newPowerState == On ? "Dim" : "TurnOff") << " to tellstick daemon" << std::endl;

	//StateType _state(state);
	//_state.get<POWERSTATE>() = newPowerState;
	//setState(_state);
}

// Specializations of template setState()
template <>
void DeviceDimmerPowerSwitch::setState<DeviceDimmerPowerSwitch::DIMLEVEL>(StateType::Type<DeviceDimmerPowerSwitch::DIMLEVEL> _dimLevel, bool force)
{
	bool commandSubmitted = false;
	DimLevel newDimLevel = validateDimLevel(_dimLevel);
	if (newDimLevel != getDimLevel() || force)
	{
		state.get<DIMLEVEL>() = newDimLevel;
		dimLevelUpdated().emit(newDimLevel);
		if (getPowerState() == On)
		{
			RFDispatcher::getInstance()->submitCommand(RFDispatcher::Dim, this, hwDimValues[newDimLevel]);
			commandSubmitted = true;
		}
	}
	if (!commandSubmitted)
		std::cout << "tdmanager: DeviceDimmerPowerSwitch (device id " << getId() << ") skipped submitting command Dim to tellstick daemon" << std::endl;

	//StateType _state(state);
	//_state.get<DIMLEVEL>() = _dimLevel;
	//setState(_state);
}


void DeviceDimmerPowerSwitch::setPowerState(PowerState newPowerState, bool force)
{
	setState<POWERSTATE>(newPowerState, force);
}


void DeviceDimmerPowerSwitch::setDimLevel(DimLevel _dimLevel, bool force)
{
	setState<DIMLEVEL>(_dimLevel, force);
}


DeviceDimmerPowerSwitch::DimLevel DeviceDimmerPowerSwitch::validateDimLevel(DimLevel dimLevel)
{
	if      (dimLevel > dimLevelMax)
		return dimLevelMax;
	else if (dimLevel < 0)
		return 0;
	else
		return dimLevel;
}


// Static var init
//const char* DeviceDimmerPowerSwitch::classname = "DeviceDimmerPowerSwitch";

// Constructor def
//DeviceDimmerPowerSwitch::DeviceDimmerPowerSwitch(int id, DimLevelValues _dimLevelValues) :
//Device(id), dimLevelValues(_dimLevelValues), dimLevelMax(_dimLevelValues.size()-1), state(On, dimLevelMax) {}

//DeviceDimmerPowerSwitch::DeviceDimmerPowerSwitch(int id, DimLevelValues _dimLevelValues, State _state) : 
//Device(id), dimLevelValues(_dimLevelValues), dimLevelMax(_dimLevelValues.size()-1), state(_state) {}


// Func def
/*
void DeviceDimmerPowerSwitch::setState(DeviceStateBase& _state)
{
	State* state = dynamic_cast<State*> (&_state);
	if (state == nullptr)
	{
		std::cout << "DeviceDimmerPowerSwitch::actuate(): Invalid state type." << std::endl;
		exit(EXIT_FAILURE);
	}
	this->state = *state;
}
*/


/*
void DeviceDimmerPowerSwitch::setState(State& _state)
{
	
	//State* state = dynamic_cast<State*> (&_state);
	//if (state == nullptr)
	//{
	//	std::cout << "DeviceDimmerPowerSwitch::actuate(): Invalid state type." << std::endl;
	//	exit(EXIT_FAILURE);
	//}
	
	this->state = state;
}


void DeviceDimmerPowerSwitch::turnOn(bool force)
{
	if (state.get<POWERSTATE>() == Off || force)
	{
		state.get<POWERSTATE>() = On;
		powerStateUpdated().emit((int) On);
		RFDispatcher::getInstance()->submitCommand(RFDispatcher::Dim, this, dimLevelValues[state.get<DIMLEVEL>()]);
	}
	else
		std::cout << "tdmanager: DeviceDimmerPowerSwitch (device id " << getId() << ") skipped submitting command Dim to tellstick daemon" << std::endl;
}


void DeviceDimmerPowerSwitch::turnOff(bool force)
{
	if (state.get<POWERSTATE>() == On || force)
	{
		state.get<POWERSTATE>() = Off;
		powerStateUpdated().emit((int) Off);
		RFDispatcher::getInstance()->submitCommand(RFDispatcher::Dim, this, dimLevelValues[state.get<DIMLEVEL>()]);
	}
	else
		std::cout << "tdmanager: DeviceDimmerPowerSwitch (device id " << getId() << ") skipped submitting command Dim to tellstick daemon" << std::endl;
}


void DeviceDimmerPowerSwitch::actuateTurnOn()
{
	setPowerState(On);
	rfDispatcher->submitCommand(RFDispatcher::Dim, this, dimLevelValues[dimLevel]);
}


void DeviceDimmerPowerSwitch::dim(DimLevel dimLevel, bool force)
{	
	std::cout << "Device " << getId() << " DeviceDimmerPowerSwitch::dim(" << dimLevel << ", " << int(force) << ")" << std::endl;

	dimLevel = validateDimLevel(dimLevel);
	if ((dimLevel != state.get<DIMLEVEL>()) || force)
	{
		state.get<DIMLEVEL>() = dimLevel;
		dimLevelUpdated().emit((int) dimLevel);
		if (state.get<POWERSTATE>() == On)
			RFDispatcher::getInstance()->submitCommand(RFDispatcher::Dim, this, dimLevelValues[dimLevel]);
		else
			std::cout << "tdmanager: DeviceDimmerPowerSwitch (device id " << getId() << ") skipped submitting command Dim to tellstick daemon" << std::endl;
	}
	else
		std::cout << "tdmanager: DeviceDimmerPowerSwitch (device id " << getId() << ") skipped submitting command Dim to tellstick daemon" << std::endl;

	//if ((powerState == On && dimLevelNew != dimLevel) || force)
	//{
	//	dimLevel = dimLevelNew;
	//	dimLevelUpdated().emit(dimLevel);
	//	rfDispatcher->submitCommand(RFDispatcher::Dim, this, dimLevelValues[dimLevel]);
	//}
	//else
	//{
	//	std::cout << "tdmanager: DeviceDimmerPowerSwitch (device id " << deviceId << ") skipped submitting command Dim to tellstick daemon" << std::endl;
	//}
}


void DeviceDimmerPowerSwitch::dimOffset(DimLevel levelOffset, bool force)
{	
	dim(state.get<DIMLEVEL>() + levelOffset, force);
}


void DeviceDimmerPowerSwitch::actuateState()
{
	if      (powerState == On) //&& dimLevelNew != dimLevel)
		rfDispatcher->submitCommand(RFDispatcher::Dim, this, dimLevelValues[getDimLevel()]);
	else if (powerState == Off)
		rfDispatcher->submitCommand(RFDispatcher::TurnOff, this);
}


void DeviceDimmerPowerSwitch::actuate(DeviceStateBase _state, bool force)
{
	State* state = dynamic_cast<State*>(&_state);
	if (state == NULL)
	{
		std::cout << "DeviceDimmerPowerSwitch::actuate(): Invalid state type." << std::endl;
		exit(EXIT_FAILURE);
	}
	
	PowerState powerState = state->get<POWERSTATE>();
	DimLevel   dimLevel   = state->get<DIMLEVEL>();
	
	if      (this->state.get<POWERSTATE>() == On  && powerState == On)
	{
		dim(dimLevel, force);
	}
	else if (this->state.get<POWERSTATE>() == On  && powerState == Off)
	{
		turnOff(force);
		dim(dimLevel, force);
	}
	else if (this->state.get<POWERSTATE>() == Off && powerState == On)
	{
		dim(dimLevel, force);
		turnOn(force);
	}
	else if (this->state.get<POWERSTATE>() == Off && powerState == Off)
	{
		dim(dimLevel, force);
	}
}


DeviceActuator DeviceDimmerPowerSwitch::getActuator(DeviceStateBase _state, bool force)
{
	State* state = dynamic_cast<State*>(&_state);
	if (state == NULL)
	{
		std::cout << "DeviceDimmerPowerSwitch::getActuator(): Invalid state type." << std::endl;
		exit(EXIT_FAILURE);
	}
	
	//DeviceActuator f;
	//f = std::bind(&DeviceDimmerPowerSwitch::actuate, this, *state, force);
	//if      (state.powerState == On)
	//	f = std::bind(&DevicePowerSwitch::turnOn, this);
	//else if (state.powerState == Off)
	//	f = std::bind(&DevicePowerSwitch::turnOff, this);

return [=](){this->actuate(_state, force);}; //std::bind(&DeviceDimmerPowerSwitch::actuate, this, *state, force);
}


int DeviceDimmerPowerSwitch::getDimLevel()
{
	return dimLevel;
}


int DeviceDimmerPowerSwitch::getDimLevelMax()
{
	return dimLevelMax;
}


DeviceDimmerPowerSwitch::DimLevel DeviceDimmerPowerSwitch::validateDimLevel(DimLevel dimLevel)
{
	if      (dimLevel > dimLevelMax)
		return dimLevelMax;
	else if (dimLevel < 0)
		return 0;
	else
		return dimLevel;
}



void DeviceDimmerPowerSwitch::streamout(std::ostream& os)
{
	DevicePowerSwitch::streamout(os);
	os << ' ' << dimLevelMax << ' ' << dimLevel;
}


bool DeviceDimmerPowerSwitch::streamin(std::istream& is)
{
	if (!DevicePowerSwitch::streamin(is))
		return false;

	int _dimLevelMax, _dimLevel, _dimLevelValidated;

	if (is.eof())
	{
		std::cout << "DeviceDimmerPowerSwitch: Device ID: " << getDeviceId() << ": Log file parameters missing." << std::endl;
		return false;
	}

	is >> _dimLevelMax;
	if (_dimLevelMax != dimLevelMax)
	{
		std::cout << "DeviceDimmerPowerSwitch: Device ID: " << getDeviceId() << ": Log file parameter dimLevelMax is not equal to that of actual device." << std::endl;
		return false;
	}
	
	if (is.eof())
	{
		std::cout << "DeviceDimmerPowerSwitch: Device ID: " << getDeviceId() << ": Log file parameter dimLevl missing." << std::endl;
		return false;
	}

	is >> _dimLevel;
	_dimLevelValidated = validateDimLevel(_dimLevel);
	if (_dimLevelValidated != _dimLevel)
	{
		std::cout << "DeviceDimmerPowerSwitch: Device ID: " << getDeviceId() << ": Log file parameter dimLevel value " << _dimLevel << " did not pass validation. Using cleaned value " << _dimLevelValidated << std::endl;
	}
	dimLevel = _dimLevelValidated;
	
	return true;
}
*/
