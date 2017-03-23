//#include <vector>
#include <iostream>
#include <cstring> // stoi
#include "WebApp.hpp"
#include "BasicDevices.hpp"
#include "WPowerSwitch.hpp"
#include "WDimmerPowerSwitch.hpp"
#include "WPresetButtonGrid.hpp"
#include <Wt/WApplication>
#include <Wt/WNavigationBar>
#include <Wt/WMenu>
#include <Wt/WStackedWidget>
#include <Wt/WContainerWidget>
#include <Wt/WVBoxLayout>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WSlider>
#include <Wt/WBootstrapTheme>


Wt::WVBoxLayout* WebApp::getPanelLayoutFromXML(XMLElement* xmlPanel)
{
	ConfigLoader::validateElement(*xmlPanel, "Panel", {"menuitemstring"}, true, true);

	Wt::WVBoxLayout* layout = new Wt::WVBoxLayout();
	
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(4);

	for (XMLElement* xmlElement = xmlPanel->FirstChildElement(); xmlElement != nullptr; xmlElement = xmlElement->NextSiblingElement())
	{
		if      (ConfigLoader::elementIs(*xmlElement, "PowerSwitchDeviceGroup"))
			layout->addWidget(WPowerSwitchDeviceGroup::parseXML(xmlElement));

		else if (ConfigLoader::elementIs(*xmlElement, "PowerSwitchSingleDevice"))
			layout->addWidget(WPowerSwitchSingleDevice::parseXML(xmlElement));

		else if (ConfigLoader::elementIs(*xmlElement, "DimmerPowerSwitchSingleDevice"))
			layout->addWidget(WDimmerPowerSwitchSingleDevice::parseXML(xmlElement));

		else if (ConfigLoader::elementIs(*xmlElement, "PresetButtonGrid"))
			layout->addWidget(WPresetButtonGrid::parseXML(xmlElement));

		else if (ConfigLoader::elementIs(*xmlElement, "VerticalSpace"))
		{
			layout->addSpacing(0);
			layout->addSpacing(0);
		}
	}
	
	return layout;
}


WebApp::WebApp(const Wt::WEnvironment& env) : Wt::WApplication(env)
{
	//setCssTheme("polished");
	setTheme(new Wt::WBootstrapTheme());
	useStyleSheet("styling.css");
	setTitle("Stordamsnaret 28");
	enableUpdates();

	// Set up navBar with menu
	Wt::WNavigationBar* navBar        = new Wt::WNavigationBar(root());
	Wt::WStackedWidget* contentsStack = new Wt::WStackedWidget(root());
	Wt::WMenu*          menu          = new Wt::WMenu(contentsStack); //, root());
	//menu->setStyleClass("navbar_menu");
	navBar->setStyleClass("navbar_menu");
	navBar->addMenu(menu);
	/*
	Wt::WText* debugtext = new Wt::WText("Session ID: " + sessionId());
	debugtext->setStyleClass("debugtext");
	navBar->addWidget(debugtext, Wt::AlignRight);
	*/
	navBar->setMargin(4, Wt::Left | Wt::Right);
	navBar->setMargin(12, Wt::Top | Wt::Bottom);

	// Load panels from xml config-file
	XMLElement* xmlWebApp = ConfigLoader::getInstance()->getRootElement("WebApp");
	for (XMLElement* xmlElement = xmlWebApp->FirstChildElement(); xmlElement != nullptr; xmlElement = xmlElement->NextSiblingElement())
	{
		if (ConfigLoader::elementIs(*xmlElement, "Panel"))
		{
			Wt::WContainerWidget* container = new Wt::WContainerWidget();
			container->setLayout(getPanelLayoutFromXML(xmlElement));
			menu->addItem(xmlElement->Attribute("menuitemstring"), container);
		}
	}
	
	//std::cout << std::endl << "WebApp session " << sessionId() << " constructor" << std::endl;

}


WebApp::~WebApp()
{}


void WebApp::callback()
{
	mtx.lock();
	std::cout << "Session " << sessionId() << " callback()" << std::endl;
	mtx.unlock();
}
