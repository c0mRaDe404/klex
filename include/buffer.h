#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>


#include <stddef.h>

#define MIN_BUF_SIZE 1024 
#define SIZE(x,y) ((x)>(y) ? (x):(y))

#define gb_front(buf) ((buf)->cursor)  //no of characters before the cursor 
#define gb_back(buf)  ((buf)->size - (buf)->gap_end)   //no of characters after the cursor
#define gb_used(buf)  (gb_front (buf) + gb_back (buf)) //totoal no of character in buffer


typedef struct Buffer
{
    char* buffer; //buffer to hold string
    size_t gap_end; //end of the gap
    size_t cursor; // current position in buffer
    size_t size; // total size of the buffer
    size_t line; // line position of the buffer

}buffer;




buffer* allocate_buffer(size_t); //allocates buffer
void free_buffer (buffer*); // free buffer*
void move_string(buffer*,char*,size_t); // move the characters of the buffer
void shrink_buffer(buffer*); //shrinks the gap
bool grow_buffer(buffer*,size_t); // grow the gap
bool insert(buffer*,char); //insert a character
void cursor_left(buffer*); //moves the cursor one postion left in the buffer
void cursor_right(buffer*); //moves the cursor one postion left in the buffer
void delete(buffer*); //moves the curses one step back and mark the previous cursor positon as gap

#endif
