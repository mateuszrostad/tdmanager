SOURCES    = $(wildcard *.cpp) 
#main.cpp RawEventHandler.cpp RFDispatcher.cpp Device.cpp BasicDevices.cpp LocalDevices.cpp WebApp.cpp SessionsContainer.cpp WDimmerPowerSwitch.cpp WPowerSwitch.cpp
HEADERS    = $(SOURCES:.cpp=.hpp)
OBJECTS    = $(SOURCES:.cpp=.o)
BUILDDIR   = build
EXECUTABLE = tdmanager
CC         = g++
CFLAGS     = -c -Wall -std=c++14
LDFLAGS    = -Larm-linux-gnueabihf -ltelldus-core -lpthread -lwthttp -lwt -ltinyxml2

all: $(SOURCES) $(HEADERS) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

#.cpp.o:
%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
	
cleanSimpleSignal:
	rm WPowerSwitch.o WDimmerPowerSwitch.o DeviceAbstract.o