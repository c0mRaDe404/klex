#ifndef KLEX_H
#define KLEX_H

#include <stddef.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

#include "./buffer.h"


#define TAB_SPACE 4
#define ENTER 10
#define KEY_INSERT 'i'
#define KEY_Q 'q'
#define KEY_ESC 27
#define BUFFER 1024
#define BRACE_CAP 128
#define CTRL(x) ((x) & 0x1f)
#define JMP_LEN 8

typedef enum
{
	NORMAL,
	INSERT,
	REPLACE,
    VISUAL,
    COMMAND
} MODE;


typedef enum
{
    BRACE_NONE,
    BRACE_OPEN_CURLY,
    BRACE_CLOSE_CURLY,
    BRACE_OPEN_ROUND,
    BRACE_CLOSE_ROUND,
    BRACE_OPEN_SQUARE,
    BRACE_CLOSE_SQUARE
}BraceType;


typedef struct 
{
    buffer** line_ptr; //holds pointer to different (buffer*) 
    size_t cur_pos; //current editing line
    size_t total_lines; //how many lines it currently holds
    size_t total_size;  //capacity of the buffer
    char* file_name;
}Line;


typedef struct
{
    BraceType Brace;
    bool Closing;
}Brace;


char* Mode(); //mode specifier
void EditorStart(); // initalizes ncurses screen
void NormalMode(WINDOW*,Line*,buffer*,int,int,int); // Normal mode
void CommandMode(WINDOW*,buffer*,Line*,int);
void InsertMode(Line*,buffer*,int,size_t*); //Insert mode
void ruler(WINDOW*,WINDOW*,size_t,size_t,size_t,size_t);
void load_file(char*,Line*);
void save_file(Line*);
void render_line(WINDOW*,Line*,size_t*,size_t);
void handle_mode(WINDOW*,Line*,buffer*,int,size_t,size_t);
int find_brace(Line*,Brace,size_t);
Brace find_pair_brace(Brace);
void exec_command(buffer*,Line*);



Line* allocate_ptr(size_t); // allocates the line buffer for the given size
bool insert_line(Line*,buffer*,size_t); //insert a buffer* to the line buffer at a given line position
void free_line(Line*);
void move_line(Line*,buffer**,size_t);
bool grow_line(Line*,size_t);
void shrink_line(Line*);
void prev_line(Line*);
void next_line(Line*);
void delete_line(Line*);
void shift_down(Line*,buffer*);
void shift_up(Line*);
#endif
