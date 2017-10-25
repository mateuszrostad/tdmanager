#ifndef WSIGNAL_HPP
#define WSIGNAL_HPP

#include <functional>
#include <map>
#include <mutex>
#include <iostream>
#include <Wt/WApplication>
#include <string>


template <typename... Args>
class WSignal
{

protected:

	struct DataElement
	{
		Wt::WApplication*            session;
		std::string                  sessionId;
		std::function<void(Args...)> callback;
	};

	std::map<int, DataElement> sessions;
	int newSessionKey;
	std::string activeSessionId;
	std::recursive_mutex mtx;
	
	static std::map<int, int> instanceCount;

public:

	WSignal() : newSessionKey(0) {}
	

	// Copy constructor, creates new SimpleSignal
	WSignal(WSignal const& _signal) : newSessionKey(0) {}
	

	// Assignment, creates new SimpleSignal
	WSignal& operator=(WSignal const& signal)
	{
		mtx.lock();
		disconnectAll();
		mtx.unlock();
	}
	
	
	// Connect a std::function
	int connect(Wt::WApplication* session, std::function<void(Args...)> const& _callback)
	{
		mtx.lock();
		sessions[newSessionKey] = DataElement{session, session->sessionId(), _callback};

		/*
		if (instanceCount.count(newSessionKey) == 0)
			instanceCount[newSessionKey] = 1;
		else
			instanceCount[newSessionKey]++;
		std::cout << "WSignal::connect(...) Session " << newSessionKey << " " << sessions[newSessionKey].sessionId << "  # of instances active " << instanceCount[newSessionKey] << std::endl;
		*/

		mtx.unlock();
		return newSessionKey++;
	}

	
	void disconnect(int sessionKey)
	{
		mtx.lock();

		/*
		instanceCount[sessionKey]--;
		std::cout << "WSignal::disconnect(...) Session " << sessionKey << " " << sessions[sessionKey].sessionId << "  # of instances left " << instanceCount[sessionKey] << std::endl;
		*/

		if (sessions[sessionKey].sessionId == activeSessionId)
			activeSessionId.clear();
		sessions.erase(sessionKey);
		mtx.unlock();
	}

	
	void disconnectAll()
	{
		mtx.lock();
		sessions.clear();
		activeSessionId.clear();
		mtx.unlock();
	}

	
	void lockFromSession(std::string _activeSessionId)
	{
		mtx.lock();
		activeSessionId = _activeSessionId;
	}

	
	bool unlockFromSession(std::string _activeSessionId)
	{
		bool retval = false;
		mtx.lock();
		if (_activeSessionId == activeSessionId)
		{
			mtx.unlock();
			activeSessionId.clear();
			retval = true;
		}
		mtx.unlock();
		return retval;
	}
	

	// Emit locks each session before emitting
	void emit(Args... p)
	{
		mtx.lock();

		/*
		std::cout << "WSignal::emit(...) ";
		for (auto session : sessions)
			std::cout << session.first << ":" << session.second.sessionId << " ";
		std::cout << std::endl;
		*/
		
		for (auto session : sessions)
		{
			if (session.second.sessionId == activeSessionId)
			{
				session.second.callback(p...);
			}
			else
			{
				Wt::WApplication::UpdateLock lock(session.second.session);
				if (lock)
				{
					session.second.callback(p...);
				}
			}

		}

		mtx.unlock();
	}

};


template <typename... Args>
std::map<int, int> WSignal<Args...>::instanceCount;

#endif