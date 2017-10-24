#ifndef SIMPLESIGNAL_HPP
#define SIMPLESIGNAL_HPP

#include <functional>
#include <map>
#include <mutex>

#include <iostream>


template <typename... Args>
class SimpleSignal
{

public:

	SimpleSignal() : currentId(0) {}
	
	// Copy constructor, creates new SimpleSignal
	SimpleSignal(SimpleSignal const& _signal) : currentId(0) {}
	
	// Assignment, creates new SimpleSignal
	SimpleSignal& operator=(SimpleSignal const& signal)
	{
		mtx.lock();
		disconnectAll();
		mtx.unlock();
	}
	
	// Connect a member function of a given object
	/*
	template<typename Member, typename... A>
	int connectMember(Member&& member, A&& ... args)
	{
		mtx.lock();
		//callbacks.insert(std::make_pair(++currentId, std::bind(member, args...)));
		callbacks.insert({++currentId, std::bind(member, args...)});
		mtx.unlock();
		return currentId;
	}
	*/
	
	// Connect a std::function
	int connect(std::function<void(Args...)> const& _callback)
	{
		mtx.lock();
		callbacks.insert(std::make_pair(++currentId, _callback));
		mtx.unlock();
		return currentId;
	}
	
	void disconnect(int id)
	{
		mtx.lock();
		callbacks.erase(id);
		mtx.unlock();
	}
	
	void disconnectAll()
	{
		mtx.lock();
		callbacks.clear();
		mtx.unlock();
	}
	
	void emit(Args... p)
	{
		mtx.lock();
		for (auto it : callbacks)
			it.second(p...);
		mtx.unlock();
	}

protected:

	std::map<int, std::function<void(Args...)> > callbacks;
	int currentId;

	std::recursive_mutex mtx;
	
};

#endif
