PHONY := clean

CC := gcc
SRC := ./
SRCS := $(wildcard $(SRC)/*.c)
MISC := $(wildcard $(SRC)/*.o *.s *.i)

BUILD_DIR := ../build
CFLAGS := -I../include/ -g 


TARGET := klex

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -lcurses -o $@
	cp $(TARGET) $(BUILD_DIR)
#	mv $(MISC) -t ../
clean:
	rm $(TARGET) $(MISC)
