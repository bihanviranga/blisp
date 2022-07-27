CC = cc

CFLAGS = -std=c99 -Wall

SRC = main.c

TARGET = main
TARGET_DIR = build

all:
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET_DIR)/$(TARGET)

clean:
	rm -rd $(TARGET_DIR)/*
