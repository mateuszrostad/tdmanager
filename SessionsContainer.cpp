#include <iostream>
#include "WebApp.hpp"
#include "SessionsContainer.hpp"
#include <Wt/WApplication>


SessionsContainer* SessionsContainer::instance = NULL;


SessionsContainer::SessionsContainer() {}


SessionsContainer::~SessionsContainer()
{
	instance = NULL;
}


//void SessionsContainer::add(Wt::WApplication* wapp)
void SessionsContainer::add(WebApp* wapp)
{
	mtx.lock();
	//std::cout << std::endl << "SessionContainer::add(...) session " << wapp->sessionId() << std::endl;
	sessions[wapp->sessionId()] = wapp;
	mtx.unlock();
}


//void SessionsContainer::remove(Wt::WApplication* wapp)
void SessionsContainer::remove(WebApp* wapp)
{
	mtx.lock();
	//std::cout << std::endl << "SessionContainer::remove(...) session " << wapp->sessionId() << std::endl;
	sessions.erase(wapp->sessionId());
	mtx.unlock();
}


/*
template <class _Signal, class _Function>
void SessionsContainer::negotiateSignal(_Signal& _signal, _Function _function, std::string _sessionId)
{
	//_signal.connect();
}
*/


void SessionsContainer::postAll()
{
	mtx.lock();
	for (ContainerType::iterator it = sessions.begin(); it != sessions.end(); ++it)
	{
		Wt::WApplication::UpdateLock lock(it->second);
		if (lock)
			it->second->callback();
	}
	mtx.unlock();
}


SessionsContainer* SessionsContainer::instantiate()
{
	instance = new SessionsContainer();
	return instance;
}


bool SessionsContainer::hasInstance()
{
	return (getInstance() != NULL);
}


SessionsContainer* SessionsContainer::getInstance()
{
	return instance;
}
