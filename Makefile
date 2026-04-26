# Variables

CXX = g++
CXXFLAGS = -std=c++11 -Wall
TARGET = Aether
SOURCES = main.cpp mySocket.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# detecting OS

ifeq ($(OS),Windows_NT)
	LIBS = -lws2_32
	RM = del /Q
	TARGET_EXT = $(TARGET).exe
else
	LIBS = 
	RM = rm -f
	TARGET_EXT = 
endif

# All rule
all: $(TARGET)$(TARGET_EXT)

$(TARGET)$(TARGET_EXT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET)$(TARGET_EXT) $(OBJECTS) $(LIBS)

# cpp to object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	$(RM) $(OBJECTS) $(TARGET)$(TARGET_EXT)