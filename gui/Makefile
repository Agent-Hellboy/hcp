CXX = g++
CXXFLAGS = -std=c++11 `pkg-config --cflags gtk+-3.0 appindicator3-0.1` -I../include
LDFLAGS = `pkg-config --libs gtk+-3.0 appindicator3-0.1` -lX11

SRC = trey_app.cpp ../src/clipboard_mgmt.cpp ../src/db.cpp ../src/logging.cpp 
TARGET = hcp-tray

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) 