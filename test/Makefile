CXX = g++
CXXFLAGS = -std=c++11 -I../include
LDFLAGS = -lX11

all: test_trey_app test_db_clipboard

test_trey_app: test_trey_app.cpp ../src/db.cpp ../src/clipboard_mgmt.cpp ../src/logging.cpp
	$(CXX) $(CXXFLAGS) $^ `pkg-config --cflags --libs gtk+-3.0 appindicator3-0.1` -lX11 -o $@

test_db_clipboard: test_db_clipboard.cpp ../src/db.cpp ../src/clipboard_mgmt.cpp ../src/logging.cpp
	$(CXX) $(CXXFLAGS) $^ -lX11 -o $@

clean:
	rm -f test_trey_app test_db_clipboard