CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = firmware_tpe_simulator
SRC = firmware_tpe.c

all:
	$(CC) $(CFLAGS) -o $(TARGET).elf $(SRC)

clean:
	rm -f *.elf
