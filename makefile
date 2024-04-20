.PHONY := clean

SOURCE := $(wildcard ./src/*.c)
TARGET := ted
FLAGS := -I ./include/ -lcurses -Wall -Wextra -o

$(TARGET):$(SOURCE)
	gcc $< $(FLAGS) $@  

clean:
	rm ted
