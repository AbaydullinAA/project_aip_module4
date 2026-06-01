CXX = clang++
CXXFLAGS = -std=c++17 -Wall
LDFLAGS = -lgsl -lgslcblas

BREW_PREFIX := $(shell brew --prefix 2>/dev/null)
ifneq ($(BREW_PREFIX),)
    CXXFLAGS += -I$(BREW_PREFIX)/include
    LDFLAGS += -L$(BREW_PREFIX)/lib
endif

CXXFLAGS += -I. -Isciplot

SRC = main.cpp
TARGET = fitting

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f $(TARGET) data_points.dat fit_line.dat plot.gp fit_output.png

run: $(TARGET)
	./$(TARGET) icecream.csv 1 0 1 50