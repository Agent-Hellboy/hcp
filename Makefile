CXX=g++
CXXFLAGS=-std=c++17 -O2 -Iinclude
TARGET=hcp
SRCS=$(wildcard src/*.cpp)

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) -lX11

clean:
	rm -f $(TARGET) 