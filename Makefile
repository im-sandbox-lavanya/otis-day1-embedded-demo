CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -O2
SRC     := src/elevator_controller.c src/sensor_utils.c
BIN     := elevator_controller

.PHONY: build run clean

build: $(BIN)

$(BIN): $(SRC) src/elevator_controller.h src/sensor_utils.h
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

run: build
	./$(BIN)

clean:
	rm -f $(BIN)
