#include "../include/buffer.h"
#include "../include/ted.h"
#include <ncurses.h>

#define TAB_SPACE 8
#define ENTER 10




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
	mvwprintw(stdscr,getmaxy(stdscr)-1,0,"%s", Mode());
	move(0,0);
	refresh();


}


void NormalMode(Line* line_buf,buffer* buf,int ch,int x,int y)
{
	noecho();

    int maxy = getmaxy(stdscr);
	mvwprintw(stdscr,maxy-1,0,"%s", Mode());
	move(y,x);
	refresh();

	if(ch == 'i')
	{
		mode = INSERT;
		mvwprintw(stdscr,maxy-1,0,"%s", Mode());
		move(0,0);
		refresh();
	}

    if(ch == CTRL('s')) {
        
            FILE* file = fopen("sample","w");

                            
            for (int i = 0;i <= line_buf->total_lines-1;i++)
            {                    
                    shrink_buffer(line_buf->line_ptr[i]);
                    fwrite(line_buf->line_ptr[i]->buffer,line_buf->line_ptr[i]->size,1,file);
                   // printw("%s",line_buf->line_ptr[i]->buffer);
                
            }

            fclose(file);
                        
    } 



}




void InsertMode(Line* line_buffer,buffer* buf,int ch)
{

    int maxy = getmaxy(stdscr);

    if(ch == KEY_BACKSPACE || ch == 8) {

        delete(buf);
        wmove(stdscr,buf->line,buf->cursor);
        delch();
    } else if( ch == '\t')
    {
        memset(buf->buffer+buf->cursor,' ',TAB_SPACE);
        buf->cursor += TAB_SPACE;
        move(buf->line,buf->cursor);
    }

    else if(ch == KEY_ESC) {

      noecho();
      mode = NORMAL;
      mvwprintw(stdscr,maxy-1,0,"%s", Mode());
      move(0,0);

    } else if (ch == KEY_LEFT) {
    
      cursor_left(line_buffer->line_ptr[buf->line]);
      wmove(stdscr, getcury(stdscr), getcurx(stdscr) - 1);
      wrefresh(stdscr);

    } else if (ch == KEY_RIGHT) {
    
      cursor_right(line_buffer->line_ptr[buf->line]);
      wmove(stdscr, getcury(stdscr), getcurx(stdscr) + 1);
      wrefresh(stdscr);

    } else if (ch == KEY_DOWN) {


      buf->line = (buf->line < line_buffer->total_lines-1) ? ++buf->line : buf->line;

      move(buf->line,line_buffer->line_ptr[buf->line]->cursor);
      wrefresh(stdscr);

    } else if (ch == KEY_UP) {
        
      buf->line = (buf->line > 0) ? --buf->line : buf->line;
        
      move(buf->line,line_buffer->line_ptr[buf->line]->cursor);
      wrefresh(stdscr);

    } else {

      size_t cursor = line_buffer->line_ptr[buf->line]->cursor;
      insert(line_buffer->line_ptr[buf->line], ch);
      mvprintw(buf->line,cursor,"%c",line_buffer->line_ptr[buf->line]->buffer[cursor]);
         

    }

    
}


Line* allocate_ptr(size_t size)
{
    const size_t min_len = 100;
    size = (size > min_len) ? size : min_len;

    Line* line = malloc(sizeof(Line));
    line->line_ptr = calloc(size,sizeof(buffer*));
    line->total_lines = 0;
    line->total_size = size;
    return line;

}


void insert_line(Line* line,buffer* buf,size_t line_no)
{

    
    if(line_no > line->total_size)
    {
        line->total_size = line->total_size*2;
        line->line_ptr = realloc(line->line_ptr,line->total_size*sizeof(buffer));
    }

    line->line_ptr[line_no] = buf;
    line->line_ptr[line_no]->line = line_no;
    line->total_lines = ++line_no;
}


int main()
{
	int ch,cur_x = 0,cur_y = 0;

	buffer* buf = allocate_buffer(MIN_BUF_SIZE);
    Line* line_buffer = allocate_ptr(100);


    size_t CurrentLine = 0;
    
    insert_line(line_buffer,buf,CurrentLine);
    
        

    EditorStart();


	while (ch != CTRL('q'))
	{

		ch = getch();
		getyx(stdscr,cur_y,cur_x);

		switch (mode)
		{
			case NORMAL:
                NormalMode(line_buffer,buf,ch,cur_x,cur_y);
				break;

			case INSERT:
                if(ch == ENTER)
                {
                    
                        insert(line_buffer->line_ptr[CurrentLine],ch);
                        buffer* current_buffer = allocate_buffer(MIN_BUF_SIZE);
                        insert_line(line_buffer,current_buffer,++CurrentLine);               
                        wmove(stdscr,line_buffer->line_ptr[CurrentLine]->line,line_buffer->line_ptr[CurrentLine]->cursor);
                        


                } else InsertMode(line_buffer,line_buffer->line_ptr[CurrentLine],ch);


                break;

            case REPLACE:
                break;
		}

		
	}

    
   
	refresh();
	endwin();
	return 0;
}
