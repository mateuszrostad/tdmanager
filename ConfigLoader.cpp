#include "ConfigLoader.hpp"
#include <cstring> // strcmp


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


void ConfigLoader::saveFile(const char* xmlfilename)
{
	XMLCheckError(doc.SaveFile(xmlfilename));
}

	
XMLElement* ConfigLoader::getRootElement(const std::string& elementName)
{
	XMLElement* xmlElement = doc.FirstChildElement(elementName.c_str());
	validateElement(*xmlElement, elementName, true, true);
	return xmlElement;
}


bool ConfigLoader::validateElement(const XMLElement& xmlElement, const std::string& name, const std::vector<std::string>& attributes, bool verbose, bool hard)
{
	if (std::strcmp(xmlElement.Name(), name.c_str()) != 0)
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