# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -IC:/msys64/ucrt64/include
LDFLAGS = -LC:/msys64/ucrt64/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio


SRC = $(wildcard *.cpp) $(wildcard src/*.cpp)
TARGET = game.exe

# Default rule
all: $(TARGET)

# Compile and link
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Compile AND Run
game: $(TARGET)
	.\$(TARGET)

# Clean rule
clean:
	if exist $(TARGET) del /Q $(TARGET)