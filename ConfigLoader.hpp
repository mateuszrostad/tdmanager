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
	
	
	XMLElement* getDeviceFactoryDataRootElement() {return getRootElement("DeviceFactory");}
	XMLElement* getRFDispatcherDataRootElement()  {return getRootElement("RFDispatcher");}
	XMLElement* getWebAppDataRootElement()        {return getRootElement("WebApp");}

	bool parseInstance(        const XMLElement&, Factory::Key*, Device::DeviceId*, StateStrVec*);
	bool parseRegisteredDevice(const XMLElement&, Device::DeviceId*, int*);
	std::string& getStateParam(const XMLElement&);


protected:
	
	XMLElement* getRootElement(const std::string&);
	bool        validateElement(const XMLElement&, const std::string&, const std::vector<std::string>& attributes, bool verbose = true, bool hard = false);
	bool        validateElement(const XMLElement&, const std::string&, bool verbose = true, bool hard = false);
	
};


#endif