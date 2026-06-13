CXX = clang++
CXXFLAGS = -std=c++17 -Wall
LDFLAGS = -lgsl -lgslcblas

BREW_PREFIX := $(shell brew --prefix 2>/dev/null)
ifneq ($(BREW_PREFIX),)
    CXXFLAGS += -I$(BREW_PREFIX)/include
    LDFLAGS += -L$(BREW_PREFIX)/lib
endif

SRC = main.cpp functions.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = fitting

TEST_SRC = test.cpp functions.cpp
TEST_OBJ = $(TEST_SRC:.cpp=.o)
TEST_TARGET = test_fitting

.PHONY: all clean run test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(TEST_TARGET): $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(OBJ) $(TARGET) $(TEST_OBJ) $(TEST_TARGET) data_points.dat fit_line.dat plot.gp fit_output.png test_data.csv bad.csv

run: $(TARGET)
	./$(TARGET) icecream.csv 1 0 1 50