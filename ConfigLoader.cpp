#include "ConfigLoader.hpp"
#include <cstring>


ConfigLoader* ConfigLoader::instance = nullptr;


ConfigLoader* ConfigLoader::getInstance()
{
	if (instance == nullptr)
		return (instance = new ConfigLoader());
	else
		return instance;
}


ConfigLoader::ConfigLoader(const char* xmlfilename)
{
	XMLCheckError(doc.LoadFile(xmlfilename));
}


ConfigLoader::~ConfigLoader() {}


bool ConfigLoader::parseInstance(const XMLElement& xmlElement, Factory::Key* factoryKey, Device::DeviceId* deviceId, StateStrVec* paramStrVec)
{
	if (!validateElement(xmlElement, "Instance", {"factorykey", "deviceid"}))
		return false;
	
	*factoryKey = (Factory::Key)     xmlElement.Attribute("factorykey");
	*deviceId   = (Device::DeviceId) std::stoi(xmlElement.Attribute("deviceid")); 

	paramStrVec->clear();
	for (const XMLElement* xmlStateParam = xmlElement.FirstChildElement("StateParam"); xmlStateParam != nullptr; xmlStateParam = xmlStateParam->NextSiblingElement("StateParam"))
		paramStrVec->push_back(getStateParam(*xmlStateParam));
	
	return true;
}


bool ConfigLoader::parseRegisteredDevice(const XMLElement& xmlElement, Device::DeviceId* deviceId, int* codeId)
{
	if (!validateElement(xmlElement, "RegisteredDevice", {"deviceid", "codeid"}))
		return false;

	*deviceId = (Device::DeviceId) std::stoi(xmlElement.Attribute("deviceid")); 
	*codeId   =                    std::stoi(xmlElement.Attribute("codeid")); 
	
	return true;
}


std::string ConfigLoader::getStateParam(const XMLElement& xmlElement)
{
	validateElement(xmlElement, "StateParam", {"value"}, true, true);
	return xmlElement->Attribute("value");
}


XMLElement* ConfigLoader::getRootElement(const std::string& elementName)
{
	XMLElement* xmlElement = doc.FirstChildElement(elementName.c_str());
	validateElement(*xmlElement, elementName, true, true);
	return xmlElement;
}


bool ConfigLoader::validateElement(const XMLElement& xmlElement, const std::string& name, const std::vector<std::string>& attributes, bool verbose, bool hard)
{
	if (!strcmp(xmlElement.Name(), name.c_str()))
	{
		if (verbose)
			std::cout << "ConfigLoader::validateElement(...): Error: Element name \"" << xmlElement.Name() << "\" does not match validation name \"" << name << "\"." << std::endl;
		if (hard)
			exit(EXIT_FAILURE);
		return false;
	}
	
	for (auto attribute : attributes)
	{
		if (xmlElement.Attribute(attribute.c_str()) == nullptr)
		{
			if (verbose)
				std::cout << "ConfigLoader::validateElement(...): Error: Element \"" << name << "\" is missing attribute \"" << attribute << "\"." << std::endl;
			if (hard)
				exit(EXIT_FAILURE);
			return false;
		}
	}
	
	return true;
}


bool ConfigLoader::validateElement(const XMLElement& xmlElement, const std::string& name, bool verbose, bool hard)
{
	return validateElement(xmlElement, name, std::vector<std::string>(), verbose, hard);
}