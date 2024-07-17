# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++11 -pthread

# Include directories
INCLUDES = -I.

# Source files
SOURCES = main.cpp CoEditor.cpp Dispatcher.cpp Producer.cpp ScreenManager.cpp Bounded_Buffer.cpp Unbounded_Buffer.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable name
EXECUTABLE = ex3.out

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
