.PHONY := clean

CC := gcc
SRC := ./
SRCS := $(wildcard $(SRC)/*.c)
CFLAGS := -I../include/  -g


TARGET := ted

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -lcurses -o $@ 
clean:
	rm $(TARGET)
