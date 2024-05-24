
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

#include "../include/buffer.h"




buffer* allocate_buffer(size_t init_size)
{
  buffer* buf = malloc (sizeof *buf);
  if (!buf)
    {
      return NULL;
    }
  init_size   = SIZE (init_size, MIN_BUF_SIZE);
  buf->buffer = calloc (init_size,sizeof(char));
  if (!buf->buffer)
    {
      free (buf);
      return NULL;
    }

  buf->size    = init_size;
  buf->cursor  = 0;
  buf->gap_end = init_size;
  return buf;

}



void free_buffer (buffer* buf)
{
  if (!buf)
    {
      return;
    }
  free (buf->buffer);
  free (buf);
}


#define gb_front(buf) ((buf)->cursor)                  
#define gb_back(buf)  ((buf)->size - (buf)->gap_end)   
#define gb_used(buf)  (gb_front (buf) + gb_back (buf))



void move_string (buffer* buf, char *new_buf, size_t new_size)
{
  memmove (new_buf + new_size - gb_back (buf),buf->buffer + buf->gap_end,gb_back (buf)); 
    
}

void shrink_buffer(buffer* buf)
{
    if(gb_used(buf) < buf->size)
    {
        
        char* new_buf = buf->buffer;

        if(gb_used(buf) > 0)
        {
            new_buf = realloc(buf->buffer,gb_used(buf));
            move_string(buf,new_buf,gb_used(buf));
        }
        
        buf->buffer = new_buf;
        buf->size = gb_used(buf);
        buf->gap_end = buf->cursor = buf->size;

    }



}




bool grow_buffer (buffer* buf,size_t new_size)
{
  
      if (new_size <= buf->size)
        {
          return false;
        }

      char *new_buf = realloc (buf->buffer, new_size);

      //memset(new_buf+buf->cursor,0,buf->gap_end-buf->cursor);

      if (!new_buf)
        {
          return false;
        }
      move_string (buf, new_buf, new_size);
      buf->buffer  = new_buf;
      buf->gap_end = new_size - gb_back (buf); 
      buf->size    = new_size;

      return true;

}


bool insert (buffer *buf, char c)
{
  if (buf->cursor == buf->gap_end)
    { 
        size_t new_size = (buf->size > 0) ? buf->size*2 : MIN_BUF_SIZE;


      if (!grow_buffer (buf, new_size*2))
        {
          return false;
        }
    }
  buf->buffer[buf->cursor++] = c;
  return true;
}




void cursor_left (buffer *buf)
{
  if (buf->cursor > 0)
    {
      buf->buffer[--buf->gap_end] = buf->buffer[--buf->cursor];
    }
}



void cursor_right (buffer *buf)
{
  if (buf->gap_end < buf->size)
    {
    
      buf->buffer[buf->cursor++] = buf->buffer[buf->gap_end++];
    }

}


void delete (buffer *buf)
{
  if (buf->cursor > 0) 
  {
        memmove(buf->buffer+(--buf->cursor),buf->buffer+buf->gap_end,gb_back(buf));
        memset(buf->buffer+gb_used(buf),0,1);
  }

    return;

}

