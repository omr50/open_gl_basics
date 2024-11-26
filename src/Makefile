CXX = g++
CXXFLAGS = -I /ucrt64/include/SDL2 -I ../include/
LDFLAGS = -L /ucrt64/lib
LIBS = -lSDL2main -lSDL2 -lopengl32 -lglew32
TARGET = main
SOURCES = main.cpp display.cpp shader.cpp mesh.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)
