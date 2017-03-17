#ifndef WebApp_HPP
#define WebApp_HPP

#include <vector>
#include <list>
#include <mutex>
#include <Wt/WApplication>
#include "ConfigLoader.hpp"

//class SessionsContainer;

namespace Wt
{
	class WEnvironment;
	class WVBoxLayout;
	//class WContainerWidget;
	//class WSlider;
}


class WebApp : public Wt::WApplication
{

public: // Static interface

	// XML-parser
	static Wt::WVBoxLayout* getPanelLayoutFromXML(XMLElement*);
	
public:

	WebApp(const Wt::WEnvironment&);
	~WebApp();
	
	void callback();

private:
	
	std::mutex     mtx;
	//SessionsContainer* sessions;

};


#endif