.PHONY := clean

ted:ted.c
	gcc $< -lcurses -o $@ -Wextra -Wall -I./include/ 

clean:
	rm main
