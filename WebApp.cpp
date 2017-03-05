//#include <vector>
#include <iostream>
//#include "Device.hpp"
//#include "SessionsContainer.hpp"
#include "WebApp.hpp"
#include "BasicDevices.hpp"
#include "WPowerSwitch.hpp"
#include "WDimmerPowerSwitch.hpp"
#include "WPresetButtonGrid.hpp"
#include <Wt/WApplication>
//#include <Wt/WPanel>
#include <Wt/WNavigationBar>
#include <Wt/WMenu>
#include <Wt/WStackedWidget>
#include <Wt/WContainerWidget>
#include <Wt/WVBoxLayout>
//#include <Wt/WLength>
//#include <Wt/WBreak>
//#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
//#include <Wt/WString>
#include <Wt/WSlider>
#include <Wt/WBootstrapTheme>


//WebApp::WebApp(const Wt::WEnvironment& env, Device::DeviceMap& _devices) : Wt::WApplication(env), devices(_devices) //, sessions(_sessions)
WebApp::WebApp(const Wt::WEnvironment& env) : Wt::WApplication(env)
{
	// Devices
	//const Device::DeviceMap& devices = Device::getDeviceMap();
	
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

	// Pointers for navBar menu-items
	Wt::WContainerWidget* container;
	Wt::WVBoxLayout*      layout;
	
	WPowerSwitchDeviceGroup* powerSwitchGroup;
	WPresetButtonGrid*       presetGrid;
	WPresetButton*           presetButton;

	// Set up navBar menu-item "Stue"
	layout = new Wt::WVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(4);

	powerSwitchGroup = new WPowerSwitchDeviceGroup("Alle lamper", WPowerSwitchDeviceGroup::Header);
	powerSwitchGroup->addDevice(0);
	powerSwitchGroup->addDevice(1);
	powerSwitchGroup->addDevice(2);
	powerSwitchGroup->addDevice(3);
	layout->addWidget(powerSwitchGroup);
	layout->addWidget(new WDimmerPowerSwitchSingleDevice(0, "Hjørnelampe"));
	layout->addWidget(new WPowerSwitchSingleDevice(      1, "Leselampe"));
	layout->addWidget(new WDimmerPowerSwitchSingleDevice(2, "Grasshopper-lampe"));
	layout->addWidget(new WDimmerPowerSwitchSingleDevice(3, "Bordlampe"));
	layout->addSpacing(0);
	layout->addSpacing(0);
	presetGrid = new WPresetButtonGrid();
	presetButton = new WPresetButton("Normalt lys");
	presetButton->addActuator(Device::getDevice(0)->getActuator({"1", "10"}));
	presetButton->addActuator(Device::getDevice(1)->getActuator({"1"      }));
	presetButton->addActuator(Device::getDevice(2)->getActuator({"1", "10"}));
	presetButton->addActuator(Device::getDevice(3)->getActuator({"1",  "6"}));
	presetGrid->addButton(presetButton);
	presetButton = new WPresetButton("Flomlys");
	presetButton->addActuator(Device::getDevice(0)->getActuator({"1", "15"}));
	presetButton->addActuator(Device::getDevice(1)->getActuator({"1"      }));
	presetButton->addActuator(Device::getDevice(2)->getActuator({"1", "15"}));
	presetButton->addActuator(Device::getDevice(3)->getActuator({"1", "15"}));
	presetGrid->addButton(presetButton);
	presetButton = new WPresetButton("Laveste dimming");
	presetButton->addActuator(Device::getDevice(0)->getActuator({"1", "0"}));
	presetButton->addActuator(Device::getDevice(1)->getActuator({"0"     }));
	presetButton->addActuator(Device::getDevice(2)->getActuator({"1", "0"}));
	presetButton->addActuator(Device::getDevice(3)->getActuator({"1", "0"}));
	presetGrid->addButton(presetButton);
	presetButton = new WPresetButton("Ingen tilstede");
	presetButton->addActuator(Device::getDevice(0)->getActuator({"1", "2"}));
	presetButton->addActuator(Device::getDevice(1)->getActuator({"0"     }));
	presetButton->addActuator(Device::getDevice(2)->getActuator({"1", "2"}));
	presetButton->addActuator(Device::getDevice(3)->getActuator({"1", "2"}));
	presetGrid->addButton(presetButton);
	layout->addWidget(presetGrid);
	
	container = new Wt::WContainerWidget();
	container->setLayout(layout);
	menu->addItem("Stue", container);

	// Set up navBar menu-item "Kjellerstue"
	layout = new Wt::WVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(4);
	
	powerSwitchGroup = new WPowerSwitchDeviceGroup("Alle lamper", WPowerSwitchDeviceGroup::Header);
	powerSwitchGroup->addDevice(4);
	powerSwitchGroup->addDevice(5);
	powerSwitchGroup->addDevice(6);
	layout->addWidget(powerSwitchGroup);
	layout->addWidget(new WDimmerPowerSwitchSingleDevice(4, "Lamper forran"));
	layout->addWidget(new WDimmerPowerSwitchSingleDevice(5, "Lamper bak"));
	layout->addWidget(new WDimmerPowerSwitchSingleDevice(6, "Leselampe"));
	layout->addSpacing(0);
	layout->addSpacing(0);
	presetGrid = new WPresetButtonGrid();
	presetButton = new WPresetButton("Jevnt lys");
	presetButton->addActuator(Device::getDevice(4)->getActuator({"1", "10"}));
	presetButton->addActuator(Device::getDevice(5)->getActuator({"1", "10"}));
	presetButton->addActuator(Device::getDevice(6)->getActuator({"1",  "8"}));
	presetGrid->addButton(presetButton);
	presetButton = new WPresetButton("Kinolys");
	presetButton->addActuator(Device::getDevice(4)->getActuator({"1",  "5"}));
	presetButton->addActuator(Device::getDevice(5)->getActuator({"1",  "3"}));
	presetButton->addActuator(Device::getDevice(6)->getActuator({"0",  "1"}));
	presetGrid->addButton(presetButton);
	presetButton = new WPresetButton("Laveste dimming");
	presetButton->addActuator(Device::getDevice(4)->getActuator({"1",  "0"}));
	presetButton->addActuator(Device::getDevice(5)->getActuator({"1",  "0"}));
	presetButton->addActuator(Device::getDevice(6)->getActuator({"1",  "0"}));
	presetGrid->addButton(presetButton);
	presetButton = new WPresetButton("Flomlys");
	presetButton->addActuator(Device::getDevice(4)->getActuator({"1", "15"}));
	presetButton->addActuator(Device::getDevice(5)->getActuator({"1", "15"}));
	presetButton->addActuator(Device::getDevice(6)->getActuator({"1", "15"}));
	presetGrid->addButton(presetButton);
	layout->addWidget(presetGrid);
	
	container = new Wt::WContainerWidget();
	container->setLayout(layout);
	menu->addItem("Kjellerstue", container);

	// Set up navBar menu-item "Soverom"
	layout = new Wt::WVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(4);

	powerSwitchGroup = new WPowerSwitchDeviceGroup("Alle lamper", WPowerSwitchDeviceGroup::Header);
	powerSwitchGroup->addDevice(7);
	powerSwitchGroup->addDevice(8);
	powerSwitchGroup->addDevice(9);
	layout->addWidget(powerSwitchGroup);
	layout->addWidget(new WDimmerPowerSwitchSingleDevice(7, "Taklampe"));
	layout->addWidget(new WDimmerPowerSwitchSingleDevice(8, "Nattbord innerst"));
	layout->addWidget(new WDimmerPowerSwitchSingleDevice(9, "Nattbord ytterst"));
	layout->addSpacing(0);
	layout->addSpacing(0);
	presetGrid = new WPresetButtonGrid();
	presetButton = new WPresetButton("Jevnt lys");
	presetButton->addActuator(Device::getDevice(7)->getActuator({"1", "10"}));
	presetButton->addActuator(Device::getDevice(8)->getActuator({"1", "10"}));
	presetButton->addActuator(Device::getDevice(9)->getActuator({"1", "10"}));
	presetGrid->addButton(presetButton);
	presetButton = new WPresetButton("Lavt lys");
	presetButton->addActuator(Device::getDevice(7)->getActuator({"1",  "5"}));
	presetButton->addActuator(Device::getDevice(8)->getActuator({"1", "10"}));
	presetButton->addActuator(Device::getDevice(9)->getActuator({"1", "10"}));
	presetGrid->addButton(presetButton);
	presetButton = new WPresetButton("Høyt lys");
	presetButton->addActuator(Device::getDevice(7)->getActuator({"1", "10"}));
	presetButton->addActuator(Device::getDevice(8)->getActuator({"1",  "6"}));
	presetButton->addActuator(Device::getDevice(9)->getActuator({"1",  "6"}));
	presetGrid->addButton(presetButton);
	presetButton = new WPresetButton("Diskret");
	presetButton->addActuator(Device::getDevice(7)->getActuator({"1",  "0"}));
	presetButton->addActuator(Device::getDevice(8)->getActuator({"1",  "1"}));
	presetButton->addActuator(Device::getDevice(9)->getActuator({"1",  "1"}));
	presetGrid->addButton(presetButton);
	presetButton = new WPresetButton("Laveste dimming");
	presetButton->addActuator(Device::getDevice(7)->getActuator({"1",  "0"}));
	presetButton->addActuator(Device::getDevice(8)->getActuator({"1",  "0"}));
	presetButton->addActuator(Device::getDevice(9)->getActuator({"1",  "0"}));
	presetGrid->addButton(presetButton);
	presetButton = new WPresetButton("Flomlys");
	presetButton->addActuator(Device::getDevice(7)->getActuator({"1", "15"}));
	presetButton->addActuator(Device::getDevice(8)->getActuator({"1", "15"}));
	presetButton->addActuator(Device::getDevice(9)->getActuator({"1", "15"}));
	presetGrid->addButton(presetButton);
	layout->addWidget(presetGrid);

	container = new Wt::WContainerWidget();
	container->setLayout(layout);
	menu->addItem("Hovedsoverom", container);

	//std::cout << std::endl << "WebApp session " << sessionId() << " constructor" << std::endl;
	/*
	if (SessionsContainer::hasInstance())
		SessionsContainer::getInstance()->add(this);
	*/
}


WebApp::~WebApp()
{
	//std::cout << std::endl << "WebApp session " << sessionId() << " destructor" << std::endl;
	/*
	if (SessionsContainer::hasInstance())
		SessionsContainer::getInstance()->remove(this);
	*/
	
}


void WebApp::callback()
{
	mtx.lock();
	std::cout << "Session " << sessionId() << " callback()" << std::endl;
	mtx.unlock();
}
