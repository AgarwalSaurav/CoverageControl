CC := g++
CFLAGS := -Wall -Wextra -std=c++11
SRCS := main.cpp  udp_tx.cpp
OBJS := $(SRCS:%.cpp=build/%.o)
EXECUTABLE := build/program

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

build/%.o: %.cpp
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build