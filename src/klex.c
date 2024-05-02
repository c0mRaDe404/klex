#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

#include "../include/buffer.h"
#include "../include/klex.h"

MODE mode = NORMAL;

char *Mode()
{
    switch (mode)
    {
    case NORMAL:
        return "-- NORMAL --";
        break;

    case INSERT:
        return "-- INSERT --";
        break;

    default:
        return "-- NORMAL --";
        break;
    }
}

void EditorStart()
{
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    set_escdelay(1);
    mvwprintw(stdscr, getmaxy(stdscr) - 1, 0, "%s", Mode());
    move(0, 0);
    refresh();
}

void NormalMode(Line *line_buf, buffer *buf, int ch, int x, int y)
{
    noecho();

    int maxy = getmaxy(stdscr);
    mvwprintw(stdscr, maxy - 1, 0, "%s", Mode());
    move(y, x);
    refresh();

    if (ch == 'i')
    {
        mode = INSERT;
        mvwprintw(stdscr, maxy - 1, 0, "%s", Mode());
        move(0, 0);
        refresh();
    }

    if (ch == CTRL('s'))
    {

        FILE *file = fopen("out.txt", "w");
        

        for (int i = 0; i <= line_buf->total_lines - 1; i++)
        {
            shrink_buffer(line_buf->line_ptr[i]);
            fwrite(line_buf->line_ptr[i]->buffer, line_buf->line_ptr[i]->size, 1, file);
        
        }

        fclose(file);
    }
}

#define ln_front(line_buf) ((line_buf)->cur_pos)                         
#define ln_back(line_buf) ((line_buf)->total_size - (line_buf)->buf_end) 
#define ln_used(line_buf) (ln_front(line_buf) + ln_back(line_buf))       

void InsertMode(Line *line_buffer, buffer *buf, int ch, size_t *current_line)
{

    int maxy = getmaxy(stdscr);

    if (ch == ENTER)
    {


        buffer *prev_buf = buf;
        buffer *current_buffer = allocate_buffer(MIN_BUF_SIZE);

    
        insert(buf, ch);

        if (gb_back(line_buffer->line_ptr[*current_line]))
        {


            memmove(current_buffer->buffer, (prev_buf->buffer + prev_buf->gap_end), gb_back(prev_buf));           
            insert_line(line_buffer, current_buffer, ++line_buffer->cur_pos);
            

            size_t line_pos = line_buffer->cur_pos;
            size_t line_end = line_buffer->buf_end;

            for(int i = 0;i< ln_back(line_buffer);i++)
            {
                insert_line(line_buffer,line_buffer->line_ptr[++line_end],++line_pos);
            }
            
            current_buffer->cursor += gb_back(prev_buf);
            prev_buf->gap_end += gb_back(prev_buf);
            
            buf = current_buffer;

        }
        else
        {
            size_t line = line_buffer->cur_pos;

            if(!line_buffer->line_ptr[++line]) {
            insert_line(line_buffer, current_buffer,++line_buffer->cur_pos);
            buf = current_buffer;
            }
            else {
                buf = line_buffer->line_ptr[++line_buffer->cur_pos];
            }
    
        }

        move(buf->line,0);


    }
    else if (ch == KEY_BACKSPACE || ch == 8)
    {

        if (buf->cursor > 0)
        {

            delete (buf);
            wmove(stdscr, *current_line, buf->cursor);
            delch();
        }
        else
        {
            buf = (buf->line > 0) ? line_buffer->line_ptr[--(*current_line)] : buf;
            delete (buf);
            wmove(stdscr, buf->line, buf->cursor-1);
        }
    }
    else if (ch == '\t')
    {

        memset(buf->buffer + buf->cursor, ' ', TAB_SPACE);
        buf->cursor += TAB_SPACE;
        move(buf->line, buf->cursor);
    }

    else if (ch == KEY_ESC)
    {

        noecho();
        mode = NORMAL;
        mvwprintw(stdscr, maxy - 1, 0, "%s", Mode());
        move(0, 0);
    }
    else if (ch == KEY_LEFT)
    {

        cursor_left(buf);
        wmove(stdscr, getcury(stdscr), getcurx(stdscr) - 1);
        wrefresh(stdscr);
    }
    else if (ch == KEY_RIGHT)
    {

        cursor_right(buf);
        wmove(stdscr, getcury(stdscr), getcurx(stdscr) + 1);
        wrefresh(stdscr);
    }
    else if (ch == KEY_DOWN)
    {
        next_line(line_buffer);

        buf = line_buffer->line_ptr[line_buffer->cur_pos];
       

        move(buf->line, buf->cursor);
        wrefresh(stdscr);
    }
    else if (ch == KEY_UP)
    {
        prev_line(line_buffer);
        buf = line_buffer->line_ptr[line_buffer->cur_pos];

        move(buf->line, buf->cursor);
        wrefresh(stdscr);
    }
    else
    {

        size_t cursor = buf->cursor;
        insert(buf, ch);
    }
}


void ruler(size_t row, size_t col)
{
    mvprintw(getmaxy(stdscr) - 1, getmaxx(stdscr) - 15, "%.3zu:%.3zu", col, row);
    move(col, row);
    wrefresh(stdscr);
}

int main(void)
{
    int ch, cur_x = 0, cur_y = 0;

    buffer *buf = allocate_buffer(MIN_BUF_SIZE);
    Line *line_buffer = allocate_ptr(100);

    insert_line(line_buffer, buf, line_buffer->cur_pos);

    EditorStart();


    while (ch != CTRL('q'))
    {

        for (int i = 0; i < line_buffer->total_lines; i++)
             for (int j = 0;j< line_buffer->line_ptr[i]->cursor;j++ )
                    mvprintw(i, j, "%c",line_buffer->line_ptr[i]->buffer[j]);


        cur_x = line_buffer->line_ptr[line_buffer->cur_pos]->cursor;
        cur_y = line_buffer->cur_pos;



        move(cur_y,cur_x);
        ruler(cur_x,cur_y);

        ch = getch();

        switch (mode)
        {
        case NORMAL:
            NormalMode(line_buffer, buf, ch, cur_x, cur_y);
            break;

        case INSERT:
        {
                InsertMode(line_buffer, line_buffer->line_ptr[line_buffer->cur_pos], ch, &line_buffer->cur_pos);
        }
            break;

        case REPLACE:
            break;
        }
    }

    refresh();
    endwin();
    return 0;
}
