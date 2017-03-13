#ifndef CONFIGLOADER_HPP
#define CONFIGLOADER_HPP

#include <tinyxml2.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include "Factory.hpp"
#include "Device.hpp"
#include "State.hpp"

using namespace tinyxml2;


#ifndef XMLCheckError
	#define XMLCheckError(result) if (result != XML_NO_ERROR) { std::cout << "Error: " << result << std::endl; exit(EXIT_FAILURE);}
#endif


class ConfigLoader
{
	
protected:

	static ConfigLoader* instance;
	XMLDocument doc;

public:
	
	static ConfigLoader* getInstance();
	
	ConfigLoader(const char* xmlfilename = "config.xml");
	~ConfigLoader();
	
	
	XMLElement* getRootElementDeviceFactory() {return getRootElement("DeviceFactory");}
	XMLElement* getRootElementRFDispatcher()  {return getRootElement("RFDispatcher");}
	XMLElement* getRootElementWebApp()        {return getRootElement("WebApp");}

	static bool parseInstance(        const XMLElement&, Factory::Key*, Device::DeviceId*, StateStrVec*);
	static bool parseRegisteredDevice(const XMLElement&, Device::DeviceId*, int*);
	static std::string& getStateParam(const XMLElement&);


protected:
	
	XMLElement* getRootElement(const std::string&);
	static bool validateElement(const XMLElement&, const std::string&, const std::vector<std::string>& attributes, bool verbose = true, bool hard = false);
	static bool validateElement(const XMLElement&, const std::string&, bool verbose = true, bool hard = false);
	static bool elementIs(const XMLElement& xmlElement, const std::string& name) {return validateElement(xmlElement, name, false, false);}
	
};


#endif