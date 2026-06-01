CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -O3 -march=native -funroll-loops -fopt-info-loop -fopt-info-vec
CXXFLAGS_DBG := -std=c++20 -Wall -Wextra -pedantic -g -fno-omit-frame-pointer
TARGET := main
SOURCE := main.cpp

.PHONY: all run test dbg clean help

all: $(SOURCE)
	$(CXX) $(CXXFLAGS) $< -o $(TARGET)

dbg: $(SOURCE)
	$(CXX) $(CXXFLAGS_DBG) $< -o $(TARGET)

run: all
	./$(TARGET)

test: all
# 	@test "$$(./$(TARGET))" = "6040578838"
	@test "$$(./$(TARGET))" = "6745589558"
	@echo "baseline output matched"

clean:
	rm -f $(TARGET)

help:
	@echo "Targets:"
	@echo "  all    - build the baseline program"
	@echo "  run    - build and run the baseline program"
	@echo "  test   - check the baseline output"
	@echo "  clean  - remove the binary"
