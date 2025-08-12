CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude -fsanitize=address 
TARGET = server
SOURCES = main.cpp server.cpp epoll.cpp socket.cpp connection.cpp buffer.cpp request.cpp response.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: clean
