CXX = g++
CXXFLAGS = -Wall -Wextra

RM = rm

TARGET = silc
TARGETTWO = sil

INPUT = silc.cpp
INPUTTWO = sil.cpp

all: $(TARGET)

$(TARGET): $(TARGETTWO)
	$(CXX) $(INPUT) -o $(TARGET)
$(TARGETTWO):
	$(CXX) $(INPUTTWO) -o $(TARGETTWO)
clean:
	$(RM) $(TARGET) $(TARGETTWO)
.PHONY: clean