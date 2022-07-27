CC = cc

CFLAGS = -std=c99 -Wall
LFLAGS = -ledit -lm

SRC = mpc.c main.c

TARGET = main
TARGET_DIR = build

all:
	$(CC) $(SRC) $(CFLAGS) $(LFLAGS) -o $(TARGET_DIR)/$(TARGET)

clean:
	rm -rd $(TARGET_DIR)/*
