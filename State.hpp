/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   State.hpp
 * Author: mmr
 *
 * Created on 5. oktober 2016, 22:40
 */

#ifndef DEVICESTATE_HPP
#define DEVICESTATE_HPP

#include "Sequence.hpp"
#include <tuple>
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib> // exit, EXIT_FAILURE

//struct StateBase {virtual ~StateBase() {}};

typedef std::vector<std::string> StateStrVec;

template <typename Derived, typename... Args>
struct State //: public StateBase
{

private:
	
	/* Proxy class for Curiously Recurring Template Pattern.
	 * Used in the mechanism to simulate static polymorphism.
	 * Through template parameter Derived, a derived class of State gives
	 * State permission to access Derived's private members, by proxy of
	 * the CRTP_proxy-struct deriving Derived. Static casts of this-pointer are
	 * then used to assume and call identically named members in Derived, ensuring
	 * compilation error if these members are not implemented in Derived. Thus
	 * State acts as an abstract class requiring the user to overload/shadow
	 * certain methods in Derived. */
	struct CRTP_proxy : Derived	{ friend class State;	};

	
	/* Function template that distributes strings to converters, one for each
	 * state-tuple element, thus setting all elements of the state-tuple.
	 * Second parameter must always be the static value returned by staticIndexSequence().
	 * This extracts, by inference, a static integral sequence S... of indices
	 * to elements of state-tuple. S... is then unpacked with a pattern that
	 * calls the appropriate converter for each input string. Since parameter
	 * unpacking only works in argument lists, a variadic dummy function
	 * must be called.
	 * Note: Order of execution of unpacked arguments is undefined. */
	template <std::size_t... S>
	void _parse(const StateStrVec& strvec, const Sequence<S...>)
	{
		if (strvec.size() != sizeof...(S))
		{
			std::cout << "Error in State<...>::_parse(...): Received string vector is not the same size as internal state-tuple." << std::endl;
			exit(EXIT_FAILURE); // TODO: use assert and throw signal, or use exceptions
		}		
		else
			/* Argument unpacking pattern combines the call to
			 * void setFromString<typename int>(string) with a literal 1 in a
			 * comma-separated list. Thus dummyArgumentCatcher always receives an
			 * unknown number of integers of value 1. */
			dummyArgumentCatcher((parse<S>(strvec[S]), 1)...);
	}
	
	/* Dummy function that does nothing but provide a variadic parameter pack as
	 * a stage for execution of a pattern of arbitrary number of elements. */
	void dummyArgumentCatcher(int...) {}

	/* Function template that distributes elements of the state tuple to converters,
	 * one for each state-tuple element, thus setting all elements of the state-tuple.
	 * First parameter must always be the static value returned by staticIndexSequence().
	 * This extracts, by inference, a static integral sequence S... of indices
	 * to elements of state-tuple. S... is then unpacked with a pattern that
	 * calls the appropriate converter for each state-tuple element. Since parameter
	 * unpacking only works in argument lists, a curly brace initialization is called
	 * for the return type.
	 * Note: Order of execution of unpacked arguments is undefined.
	 */
	template <std::size_t... S>
	StateStrVec _getStringVector(const Sequence<S...>)
	{
		// No need for size check of S... since the current function is private and will
		// only be called by members of State<...> that presumably know the deal.
		return { (getString<S>())... };
	}

	
protected:
	
	// Main data structure
	// Type-alias for the main data structure state-tuple
	typedef std::tuple<Args...> StateTuple;
	// Instance of state-tuple
	StateTuple stateTuple;


public:
	
	// Type-alias template for N-th element of state-tuple.
	// N must be known at compile-time
	template <std::size_t N>
	using Type = typename std::tuple_element<N, StateTuple>::type;

	
public:

	// Constructors: default-, direct state-tuple initialization, parse from string vector initialization
	State() = default;
	State(Args... args) : stateTuple(args...) {}
	State(const StateStrVec& strvec)          {parse(strvec);}
	// Copy- and move-constructors are implicitly-declared and -defined
	//State(State const & state)       : stateTuple(state.stateTuple)     {} // Copy state-tuple
	//State(State const && state)      : stateTuple(state())     {} // Copy state-tuple

	// Special copy-tuple- and move-tupoe-constructors are obsolete
	//State(StateTuple  const & _stateTuple) : stateTuple(_stateTuple) {} // Copy state-tuple
	//State(StateTuple  const && _stateTuple) : stateTuple(_stateTuple) {} // Copy state-tuple
	
	// Copy- and move-assignment operatos are implicitly-declared and -defined
	//State& operator=(State const & state) {stateTuple = state.stateTuple; return *this;}
	
	// Special tuple-assignment operators are obsolete
	//State& operator=(StateTuple _stateTuple)    {stateTuple = _stateTuple;      return *this;}
	
	// Static number of elements in state-tuple
	static constexpr std::size_t             size = sizeof...(Args);
	// Static Sequence template set to number of elements in state-tuple.
	// It should be used with the argument unpacking mechanism to simulate
	// iterated access to elements of the state-tuple, since non-constexpr integer
	// values (like local index variables in for loops) cannot be used.
	static constexpr SequenceGenerator<size> staticIndexSequence() {return SequenceGenerator<size>();}

	// Return reference to N-th element of state-tuple.
	// N must be known at compile-time.
	template <std::size_t N>
	Type<N>&       get()        {return std::get<N>(stateTuple);}
	template <std::size_t N>
	const Type<N>& get() const  {return std::get<N>(stateTuple);}
	// Return reference to state-tuple, both through function call and ()-operator.
	StateTuple& get()        {return stateTuple;}
	StateTuple& operator()() {return stateTuple;}

	// Parse single string and set N-th element of state-tuple. Call converter
	// function template defined in sub-class, accessed through CRTP_proxy class.
	// N must be known at compile time.
	template <std::size_t N>
	void parse(std::string str)                   {get<N>() = static_cast<CRTP_proxy*>(this)->fromString<N>(str);}
	// Parse vector of strings and set all elements of state-tuple.
	// Vector must be of same length as number of elements in state-tuple, and
	// vector elements must be in the same order as tuple elements
	void parse(const StateStrVec&  strvec) {_parse(strvec, staticIndexSequence());}
	void parse(const StateStrVec&& strvec) {_parse(strvec, staticIndexSequence());}

	// Parse a single element of state-tuple and return as a string. Call converter
	// function template defined in sub-class, accessed through CRTP_proxy class.
	// N must be known at compile time.
	template <std::size_t N>
	std::string getString()          {return static_cast<CRTP_proxy*>(this)->toString<N>(get<N>());}
	// Parse all elements of state-tuple and return as a vector of strings
	// in the same order as they appear in the state-tuple.
	StateStrVec getStringVector()    {return _getStringVector(staticIndexSequence());}

	
	/* 2 "abstract" function templates for conversion of state-tuple elements to
	 * and from strings (for external/dynamic storage in config file).
	 * They simulate static polymorphism by requiring the user to implement
	 * identical shadowing methods in a sub-class. If user fails to implement,
	 * then these "abstract" versions will be called, resulting in a guaranteed
	 * compiler error. */

	// Returns string-version of N-th state-tuple element.
	// N must be known at compile-time.
	template<std::size_t N>
	std::string toString(const Type<N>&)
	{
		static_assert(N!=N, "Missing implmementation of function template toString() in derived class of State.");
		return "";
	}
	
	// Parses string and returns a corresponding value of same type as N-th element of state-tuple.
	// N must be known at compile-time.
	template<std::size_t N>
	Type<N>     fromString(const std::string&)
	{
		static_assert(N!=N, "Missing implmementation of function template fromString() in derived class of State.");
		return Type<N>();
	}

};


/* Macro for easy definition of derived class of State.
 * Defines struct State : public State<State, args...>, where args... is
 * substituted by the types of the elements of the state-tuple in State.
 * Also defined automatically are member function templates toString<int N>(...)
 * and fromString<int N>(...) where N is the constexpr index of an element of
 * the state-tuple in State. These need to be specialized for each N 
 * corresponding to a state-tuple element.
 */

#define stateType(Type) Type##State

//#define declare_state_class(Type)
//struct stateType(Type);

#define define_state_class(T, ...)\
struct stateType(T) : public State<stateType(T), ##__VA_ARGS__> {\
stateType(T)()                                         : State() {}\
stateType(T)(const stateType(T)& st)                   : State(st) {}\
template <typename... Args>	stateType(T)(Args... args) : State(args...) {}\
stateType(T)(const StateStrVec& strvec)                : State(strvec) {}\
template <std::size_t N> std::string toString(  const Type<N>&     value) {return std::to_string(value);}\
template <std::size_t N> Type<N>     fromString(const std::string& str)   {return (Type<N>) std::stoi(str);       }};
//template <std::size_t N> std::string toString(  Type<N>)            {static_assert(N!=N, "No specialization for function template toString() in derived class of State."); return "";}
//template <std::size_t N> Type<N>     fromString(const std::string&) {static_assert(N!=N, "No specialization for function template fromString() in derived class of State."); return Type<N>();}};

//stateType(T)(const stateType(T)&& st) = default;

/* Macros for easy specialization of member function templates toString<int>(...)
 * and fromString<N>(...). Parameter fname must be a callable name of a function
 * that can be substituted directly into the body of the function template and
 * has the same signature as the function template. */
#define define_state_converter_toString(  T, N, fname) template<> std::string           stateType(T)::toString<N>(  const Type<N>&     value) {return fname(value);}
#define define_state_converter_fromString(T, N, fname) template<> stateType(T)::Type<N> stateType(T)::fromString<N>(const std::string& str)   {return fname(str);}
//#define define_state_converter_toString(cname, N, fname) template<> std::string cname::State::toString<N>(typename cname::State::Type<N> value) {return fname(value);}
//#define define_state_converter_fromString(cname, N, fname) template<> typename cname::State::Type<N> cname::State::fromString<N>(const std::string& str) {return (typename cname::State::Type<N>)fname(str);}

#endif /* DEVICESTATE_HPP */

