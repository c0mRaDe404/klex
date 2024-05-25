#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <ncurses.h>

#include "../include/buffer.h"
#include "../include/klex.h"

#define CLEAR

#define PRINT(ARG) mvprintw(10, 0, "%zu", (ARG))
#define PRINT_STRING(ARG) mvprintw(11, 0, "%s", (ARG))


#define ln_front(line_buf) ((line_buf)->cur_pos)
#define ln_back(line_buf) (((line_buf)->total_lines - (line_buf)->cur_pos)-1)
#define ln_used(line_buf) (ln_front(line_buf) + ln_back(line_buf))



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
    move(0, 0);
    refresh();
}



BraceType get_brace_type(char ch)
{
    switch(ch)
    {
        case '{': return BRACE_OPEN_CURLY;
        case '}': return BRACE_CLOSE_CURLY;
        case '(': return BRACE_OPEN_ROUND;
        case ')': return BRACE_CLOSE_ROUND;
        case '[': return BRACE_OPEN_SQUARE;
        case ']': return BRACE_CLOSE_SQUARE;
        default : return BRACE_NONE;
    }
}



void NormalMode(char *File, Line *line_buf, buffer *buf, int ch, int x, int y)
{
    noecho();

    if (ch == 'i')
    {
        mode = INSERT;
        for (int i = 0; i < line_buf->total_lines; i++) if(buf->cursor != 0) cursor_right(line_buf->line_ptr[i]);
 
    }
    else if (ch == KEY_LEFT || ch == 'h')
    {
        cursor_left(buf);
    }
    else if (ch == KEY_RIGHT || ch == 'l')
    {
        cursor_right(buf);
    }
    else if (ch == KEY_DOWN || ch == 'j')
    {
        next_line(line_buf);
        buf = line_buf->line_ptr[line_buf->cur_pos];
    }
    else if (ch == KEY_UP || ch == 'k')
    {
        prev_line(line_buf);
        buf = line_buf->line_ptr[line_buf->cur_pos];
    }
    else if(ch == 'g') line_buf->cur_pos = 0;
    else if(ch == 'G') line_buf->cur_pos = line_buf->total_lines-1;
    else if(ch == '0') while(buf->cursor != 0) cursor_left(buf);
    else if(ch == '$') while(buf->cursor != gb_used(buf)) cursor_right(buf);
    else if(ch == 'w') //next word's beginning
    {
        char* buffer = buf->buffer;
        if(isspace(buffer[buf->cursor])) cursor_right(buf);

        while(!isspace(buffer[buf->cursor])) 
        { 
            if(buf->cursor == strlen(buffer)) break;
            else cursor_right(buf);
        }
        cursor_right(buf);
    }
    else if(ch == 'b') // previous word's beginning
    {
        char* buffer = buf->buffer;
        cursor_left(buf);
        if(isspace(buffer[buf->cursor])) cursor_left(buf);

        while(!isspace(buffer[buf->cursor])) 
        { 
            if(buf->cursor == 0) break;
            else cursor_left(buf);
        }
        if(buf->cursor != 0) cursor_right(buf);
    }
    else if(ch == 'e') // word's end
    {
        char* buffer = buf->buffer;

        cursor_right(buf);
        if(isspace(buffer[buf->cursor])) cursor_right(buf);

        while(!isspace(buffer[buf->cursor])) 
        { 
            if(buf->cursor == strlen(buffer)) break;
            else cursor_right(buf);
        }
        cursor_left(buf);
    }
    else if(ch == 'd')
    {
        size_t line_pos = line_buf->cur_pos;
        size_t total_lines = line_buf->total_lines;

        if(line_pos >= 0)
        {
            if(line_pos == 0 && ln_back(line_buf) == 0) 
            {
                memset(line_buf->line_ptr[line_pos]->buffer,0,line_buf->line_ptr[line_pos]->size);
                line_buf->line_ptr[line_pos]->cursor = 0;
            }
            
        memmove(line_buf->line_ptr+line_pos,line_buf->line_ptr+1+line_pos,sizeof(buffer*)*ln_back(line_buf));
        for(int i = 0;i< total_lines;i++) line_buf->line_ptr[i]->line = i;
        line_buf->total_lines = (line_pos == 0 && ln_back(line_buf) == 0) ? total_lines : total_lines-1;
        }

        if(line_pos == total_lines-1) line_buf->cur_pos = (line_pos == 0) ? line_pos : line_pos-1;

        

    }
    else if(ch == 'x')
    {
        if(!gb_used(buf)) NormalMode(File,line_buf,buf,'d',x,y);
    
        if(gb_back(buf) >= 0) 
        {
            
                memmove(buf->buffer+(buf->cursor),buf->buffer+1+buf->cursor,gb_back(buf));
                buf->gap_end += (buf->gap_end < buf->size) ? 1 : 0;
                if(!gb_back(buf)) cursor_left(buf);
        }
               
    }

    else if(ch == '%')
    {
            BraceType brace = get_brace_type(buf->buffer[buf->cursor]);
            size_t pos = find_brace(line_buf,brace,line_buf->cur_pos);
            while(pos != buf->cursor)
            {
                if(buf->cursor > pos) cursor_left(buf);
                else cursor_right(buf);
            }
               
    }
    else if (ch == CTRL('s'))
    {
            save_file(File,line_buf);
    }
}


BraceType find_pair_brace(BraceType brace)
{
    switch(brace)
    {
        case BRACE_OPEN_CURLY:
            return BRACE_CLOSE_CURLY;

        case BRACE_OPEN_ROUND:
            return BRACE_CLOSE_ROUND;

        case BRACE_OPEN_SQUARE:
            return BRACE_CLOSE_SQUARE;
        default:
            return BRACE_NONE;

    }
}

int find_brace(Line* line_buf,BraceType brace,size_t line)
{

    buffer* buf = line_buf->line_ptr[line]; 
    BraceType pair_brace = find_pair_brace(brace);

    for(size_t cursor=0;cursor<gb_used(buf);cursor++)
    {
        if(get_brace_type(buf->buffer[cursor]) == pair_brace) return cursor;  
    }
    return find_brace(line_buf,brace,++line_buf->cur_pos);
}


void save_file(char* file_name,Line* line_buf)
{
        FILE *file = fopen(file_name, "w");

        for (int i = 0; i <= line_buf->total_lines - 1; i++)
        {
            buffer* Buffer = line_buf->line_ptr[i];
            size_t cur_x = line_buf->line_ptr[i]->cursor;
            size_t len = strlen(Buffer->buffer);
            Buffer->buffer[len] = '\n';
            shrink_buffer(Buffer);
            fwrite(Buffer->buffer,len+1, 1, file);
        
        }

        fclose(file);
}


void InsertMode(Line *line_buffer, buffer *buf, int ch, size_t *current_line)
{

    int maxy = getmaxy(stdscr);

    if (ch == ENTER)
    {

        buffer *prev_buf = buf;
        buffer *current_buffer = allocate_buffer(MIN_BUF_SIZE);


        if (gb_back(line_buffer->line_ptr[*current_line]))
        {

            memmove(current_buffer->buffer, (prev_buf->buffer + prev_buf->gap_end), gb_back(prev_buf));
            
            memset(prev_buf->buffer+prev_buf->cursor,0,gb_back(prev_buf));

            shift_down(line_buffer,current_buffer);

            current_buffer->cursor += gb_back(prev_buf);
            prev_buf->gap_end += gb_back(prev_buf);
            buf = current_buffer;  
        }

        else
        {
        
            shift_down(line_buffer,current_buffer);
            buf = current_buffer;
                 
        }
        while(buf->cursor) cursor_left(buf);

    }

    else if (ch == KEY_BACKSPACE || ch == 8)
    {

        if (buf->cursor > 0)
        {
            delete (buf);
            delch();
        }

        else if ( buf->line > 0 && buf->cursor == 0)
        {

            size_t line_pos = line_buffer->cur_pos;

            buffer *current_buffer = line_buffer->line_ptr[line_pos];
            buffer* prev_buffer = line_buffer->line_ptr[--line_pos];

            memmove(prev_buffer->buffer + prev_buffer->cursor, current_buffer->buffer + current_buffer->gap_end, gb_used(current_buffer));
            prev_buffer->cursor += gb_used(current_buffer); 

            delete_line(line_buffer);
            shift_up(line_buffer);

 
        }
    }

    else if (ch == '\t')
    {
        memset(buf->buffer+buf->cursor,' ',TAB_SPACE);
        buf->cursor += TAB_SPACE;
    }

    else if (ch == KEY_ESC)
    {
        noecho();
        mode = NORMAL;
        for (int i = 0; i < line_buffer->total_lines; i++) cursor_left(line_buffer->line_ptr[i]);
    }

    else if (ch == KEY_LEFT)
    {

        cursor_left(buf);
        
    }

    else if (ch == KEY_RIGHT)
    {
        cursor_right(buf);
    }

    else if (ch == KEY_DOWN)
    {
        next_line(line_buffer);
        buf = line_buffer->line_ptr[line_buffer->cur_pos];
    
    }

    else if (ch == KEY_UP)
    {
        prev_line(line_buffer);
        buf = line_buffer->line_ptr[line_buffer->cur_pos];
    }

    else if(get_brace_type(ch) == BRACE_OPEN_CURLY)
    {
        insert(buf,ch);
        insert(buf,'}');
        cursor_left(buf);
    }
    else if(get_brace_type(ch) == BRACE_OPEN_ROUND)
    {
        insert(buf,ch);
        insert(buf,')');
        cursor_left(buf);
    }
    else if(get_brace_type(ch) == BRACE_OPEN_SQUARE)
    {
        insert(buf,ch);
        insert(buf,']');
        cursor_left(buf);
    }
    else
    {

        buf->line = line_buffer->cur_pos;

        if(!buf->buffer[buf->cursor]) insert(buf, ch);

        else if (buf->cursor < buf->gap_end)
        {
            size_t b_cur = buf->cursor;
            size_t b_end = buf->gap_end;
            memmove(buf->buffer+b_cur+1,buf->buffer+buf->gap_end,gb_back(buf));
            insert(buf,ch);

        }
    }
}



void load_file(char *file_name, Line *line_buf)
{

    FILE *file = fopen(file_name, "rw");

    if(!file) return;


    fseek(file,0,SEEK_END);
    size_t length = ftell(file);
    fseek(file,0,SEEK_SET);
    
    char* buf = malloc(sizeof(char)*length); 
    fread(buf,sizeof(char)*length,1,file);
    size_t line = line_buf->cur_pos;


    for(int i = 0;i < length;i++)
    {
        if(buf[i] == '\n') { 
            insert_line(line_buf,line_buf->line_ptr[line],line);
            line_buf->line_ptr[++line] = allocate_buffer(MIN_BUF_SIZE);
        }
        else insert(line_buf->line_ptr[line],buf[i]);

    }

    for (int i = 0; i < line; i++) cursor_left(line_buf->line_ptr[i]);
 
    line_buf->total_lines = line;
    line_buf->cur_pos = 0;

    return;
}




void ruler(WINDOW* status_bar,WINDOW* line_bar,size_t row, size_t col)
{
    start_color();
    init_pair(1,COLOR_YELLOW,COLOR_BLACK);
    wattron(line_bar,COLOR_PAIR(1));
    wattron(line_bar,WA_BOLD);
    mvwprintw(line_bar,col,1,"%zu",col+1);
    mvwprintw(status_bar,0, getmaxx(status_bar) - 15, "%.3zu:%.3zu", col, row);
    mvwprintw(status_bar,0, 0, "%s", Mode());
    wrefresh(status_bar);
    wrefresh(line_bar);
}


void display_line(WINDOW* main_win,Line* line_buf,size_t x,size_t y,size_t* offset)
{
    #ifdef CLEAR
    wclear(main_win);
    #endif

    
    for (int i = 0; i < line_buf->total_lines; i++)
    {
        if(line_buf->cur_pos == getmaxy(main_win))
        {
            *offset = getmaxy(main_win)-1;
            i = i%getmaxy(main_win);

            wclear(main_win);
        }

        mvwprintw(main_win,i, 0, "%s", line_buf->line_ptr[i+(*offset)]->buffer);
    }

     wmove(main_win,y%getmaxy(main_win), x);

}


void handle_mode(char* file_name,Line* line_buf,int ch,size_t x,size_t y)

{

    switch (mode)
    {
    
        case NORMAL:
            NormalMode(file_name, line_buf, line_buf->line_ptr[line_buf->cur_pos], ch,x, y);
            break;

        case INSERT:
            InsertMode(line_buf, line_buf->line_ptr[line_buf->cur_pos], ch, &line_buf->cur_pos);
            break;

        case REPLACE:
            break;


    }


}




int main(int argc, char **argv)
{

    EditorStart();


    int row,col;
    getmaxyx(stdscr,col,row);
    WINDOW* main_win = newwin(col*0.96,row,0,4);
    WINDOW* status_bar = newwin(col*0.05,row,col-2,0);
    WINDOW* line_bar = newwin(col-2,4,0,0);
    keypad(main_win,TRUE);
    wrefresh(line_bar);
    wrefresh(main_win);
    wrefresh(status_bar);


    int ch,x = 0,y = 0;
    size_t offset = 0;
    buffer *buf = allocate_buffer(MIN_BUF_SIZE);
    Line *line_buf = allocate_ptr(100);
    insert_line(line_buf, buf, line_buf->cur_pos);




    if (argc < 2) 
        argv[1] = "untitled";
    else 
        load_file(argv[1], line_buf);

    
    while (ch != CTRL('q'))
    {

        x = line_buf->line_ptr[line_buf->cur_pos]->cursor;
        y = line_buf->cur_pos;

       
        display_line(main_win,line_buf,x,y,&offset);
        ruler(status_bar,line_bar,x,y);
       
        ch = wgetch(main_win);
        handle_mode(argv[1],line_buf,ch,x,y);

     }

    refresh();
    endwin();
    return 0;
}
