#ifndef CONFIGLOADER_HPP
#define CONFIGLOADER_HPP

#include <tinyxml2.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

using namespace tinyxml2;


#ifndef XMLCheckError
	#define XMLCheckError(result) if (result != XML_NO_ERROR) { std::cout << "Error: " << result << std::endl; exit(EXIT_FAILURE);}
#endif

#ifndef XMLAssignCheckNull
	#define XMLAssignCheckNull(result, lvalue)  if (result == nullptr) { std::cout << "Error: XMLCheckNull received null pointer." << std::endl; exit(EXIT_FAILURE);} else {lvalue = result;}
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
	
	XMLElement* getRootElement(const std::string&);
	static bool validateElement(const XMLElement&, const std::string&, const std::vector<std::string>& attributes, bool verbose = true, bool hard = false);
	static bool validateElement(const XMLElement&, const std::string&, bool verbose = true, bool hard = false);
	static bool elementIs(const XMLElement& xmlElement, const std::string& name) {return validateElement(xmlElement, name, false, false);}
	
};


#endif