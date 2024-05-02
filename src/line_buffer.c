#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>


#include "../include/klex.h"


#define MIN_LINE_BUF_SIZE 100
#define MAX(x,y) ( (x)>(y) ? (x):(y))


//Currently adding line shifting when a newline character entered

Line *allocate_ptr(size_t init_size)
{

    Line *line_buf = malloc(sizeof(Line));
    init_size =  MAX(init_size,MIN_LINE_BUF_SIZE);
    line_buf->line_ptr = calloc(init_size, sizeof(buffer *));


    line_buf->total_size = init_size;
    line_buf->buf_end = init_size;
    line_buf->cur_pos = 0;
    line_buf->total_lines = 0;
    return line_buf;
}

void free_line(Line* line_buf)
{
    if(!line_buf) return;

    free(line_buf->line_ptr);
    free(line_buf);

}






#define ln_front(line_buf) ((line_buf)->cur_pos)  //no of lines before the cursor 
#define ln_back(line_buf)  ((line_buf)->total_size - (line_buf)->buf_end)   //no of lines after the cursor
#define ln_used(line_buf)  (ln_front (line_buf) + ln_back (line_buf)) //total no of lines in line buffer



void move_line(Line* line_buf,buffer** new_buf,size_t new_size)
{
    memmove(new_buf+new_size-ln_back(line_buf),line_buf->line_ptr+line_buf->buf_end,ln_back(line_buf));

}


bool grow_line(Line* line_buf,size_t new_size)
{
    if(new_size <= line_buf->total_size) return false;

    buffer** new_buf = realloc(line_buf->line_ptr,new_size);

    move_line(line_buf,new_buf,new_size);

    line_buf->line_ptr = new_buf;
    line_buf->buf_end = new_size-ln_back(line_buf);
    line_buf->total_size = new_size;
    
    return true;
}





bool insert_line(Line *line_buf, buffer *buf, size_t line_no)
{

    if(line_buf->cur_pos == line_buf->buf_end)
    {
        size_t new_size = (line_buf->total_size > 0) ? line_buf->total_size*2 : MIN_LINE_BUF_SIZE;

        if(!grow_line(line_buf,new_size)) return false;

    }

    
    //line_buf->cur_pos = line_no;
    line_buf->line_ptr[line_no] = buf;
    line_buf->line_ptr[line_no]->line = line_no;
    line_buf->total_lines = ++line_no;

    return true;
}


void shrink_line(Line* line_buf)
{
    if(ln_used(line_buf) < line_buf->total_size)
    {
        buffer** new_buf = line_buf->line_ptr;

        if(ln_used(line_buf) > 0)
        {
            new_buf = realloc(line_buf->line_ptr,ln_used(line_buf));
            move_line(line_buf,new_buf,ln_used(line_buf));
        }

        line_buf->line_ptr = new_buf;
        line_buf->total_size = ln_used(line_buf);
        line_buf->buf_end = line_buf->cur_pos = line_buf->total_lines = line_buf->total_size;

}

}


void prev_line(Line* line_buf)
{
    
    if(line_buf->cur_pos > 0) line_buf->line_ptr[line_buf->buf_end--] = line_buf->line_ptr[line_buf->cur_pos--];
}

void next_line(Line* line_buf)
{
    
    if(line_buf->buf_end < line_buf->total_size) line_buf->line_ptr[++line_buf->cur_pos] = line_buf->line_ptr[++line_buf->buf_end];
    
}



void delete_line(Line* line_buf)
{
    if(line_buf->cur_pos > 0) line_buf->line_ptr[line_buf->cur_pos--] = NULL;

}
