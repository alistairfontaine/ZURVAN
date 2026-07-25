CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -Iinclude -pthread

SRC = src/main.cpp src/core/zurvan.cpp src/shell/shell.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = zurvan-vfs

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)
