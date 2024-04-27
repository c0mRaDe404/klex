#ifndef TED_H
#define TED_H

#include <stddef.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

#include "./buffer.h"


#define TAB_SPACE 8
#define ENTER 10
#define KEY_INSERT 'i'
#define KEY_Q 'q'
#define KEY_ESC 27
#define BUFFER 1024
#define CTRL(x) ((x) & 0x1f)

typedef enum
{
	NORMAL,
	INSERT,
	REPLACE
} MODE;





typedef struct 
{
    buffer** line_ptr; //holds pointer to different (buffer*) 
    size_t cur_pos; //current editing line
    size_t buf_end;  //end of the buffer
    size_t total_lines; //how many lines it currently holds
    size_t total_size;  //capacity of the buffer

}Line;



char* Mode(); //mode specifier
void EditorStart(); // initalizes ncurses screen
void NormalMode(Line*,buffer*,int,int,int); // Normal mode
void InsertMode(Line*,buffer*,int,size_t*); //Insert mode



Line* allocate_ptr(size_t); // allocates the line buffer for the given size
bool insert_line(Line*,buffer*,size_t); //insert a buffer* to the line buffer at a given line position
void free_line(Line*);
void move_line(Line*,buffer**,size_t);
bool grow_line(Line*,size_t);
void shrink_line(Line*);
void prev_line(Line*);
void next_line(Line*);
void delete_line(Line*);

#endif
