# Makefile para compilar con linux

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

TARGET = BEP_HC_Greedy.exe
SRC = BEP_HC_Greedy.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

