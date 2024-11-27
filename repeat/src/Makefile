CXX = g++
CXXFLAGS = -I /ucrt64/include/SDL2
LDFLAGS = -L /ucrt64/lib
LIBS = -lSDL2main -lSDL2
TARGET = main

SOURCES = main.cpp


all: $(TARGET)

$(TARGET) : $(SOURCES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)