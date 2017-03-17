#include "BasicDevices.hpp"
#include "Factory.hpp"
#include "RFDispatcher.hpp"
#include <iostream>
#include <cstdlib> // exit, EXIT_FAILURE


/*******************************************************************************
 * class DevicePowerSwitch
 */

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

}


void DevicePowerSwitch::setPowerState(PowerState newPowerState, bool force)
{
	setState<POWERSTATE>(newPowerState, force);
}


/*******************************************************************************
 * class DeviceDimmerPowerSwitch
 */

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
