#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define MIN_BUF_SIZE 1024
#define SIZE(x,y) ((x)>(y) ? (x):(y))

#define gb_front(buf) ((buf)->cursor)
#define gb_back(buf)  ((buf)->size - (buf)->gap_end)   
#define gb_used(buf)  (gb_front (buf) + gb_back (buf))


typedef struct Buffer
{
    char* buffer;
    size_t gap_start,gap_end;
    size_t cursor;
    size_t size;
    size_t line;
    struct Buffer* prev;
    struct Buffer* next;

}buffer;




buffer* allocate_buffer(size_t);
void free_buffer (buffer*);
void move_string(buffer*,char*,size_t);
void shrink_buffer(buffer*);
bool grow_buffer(buffer*,size_t);
bool insert(buffer*,char);
void cursor_left(buffer*);
void cursor_right(buffer*);
int delete(buffer*);

#endif
