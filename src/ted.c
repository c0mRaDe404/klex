#include "../include/buffer.h"
#include "../include/ted.h"
#include <ncurses.h>
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


void NormalMode(int ch,int x,int y)
{
	noecho();

    int maxy = getmaxy(stdscr);
	mvwprintw(stdscr,maxy-1,0,"%s", Mode());
	move(y,x);
	refresh();

	if(ch == 'i')
	{
		//echo();
		mode = INSERT;
		mvwprintw(stdscr,maxy-1,0,"%s", Mode());
		move(0,0);
		refresh();
	}

}




void InsertMode(buffer* buf,int ch,Line* line_ptr,int x,int y)
{

         
//    buf = ( buf->next == NULL) ? buf: buf->next;
 
    int maxy = getmaxy(stdscr);    


    if(ch == KEY_BACKSPACE) {

        refresh();
        if(!(buf->line == 0 && buf->cursor == 0))
        {
            size_t BufferLine;
            BufferLine = (buf->cursor == 0) ? --buf->line :buf->line;
    
            mvwdelch(stdscr,buf->line,delete(line_ptr->newline[BufferLine]));
;
         }
    }

    else if(ch == KEY_ESC) {

        noecho();
        mode = NORMAL;
        mvwprintw(stdscr,maxy-1,0,"%s", Mode());
        move(0,0);

    } else if (ch == KEY_LEFT) {
    
      cursor_left(line_ptr->newline[buf->line]);
      wmove(stdscr, getcury(stdscr), getcurx(stdscr) - 1);
      wrefresh(stdscr);

    } else if (ch == KEY_RIGHT) {
    
      cursor_right(line_ptr->newline[buf->line]);
      wmove(stdscr, getcury(stdscr), getcurx(stdscr) + 1);
      wrefresh(stdscr);

    } else if (ch == KEY_DOWN) {
     
      buf->line = (buf->line < line_ptr->linenumber) ? ++buf->line : buf->line;

      wmove(stdscr,buf->line,line_ptr->newline[buf->line]->cursor);
      wrefresh(stdscr);

    } else if (ch == KEY_UP) {
        
       
      buf->line = (buf->line > 0) ? --buf->line : buf->line;
        
      wmove(stdscr,buf->line,line_ptr->newline[buf->line]->cursor);
      wrefresh(stdscr);

    } else {

           size_t cursor = line_ptr->newline[buf->line]->cursor;
           insert(line_ptr->newline[buf->line], ch);
           mvprintw(buf->line,cursor,"%c",line_ptr->newline[buf->line]->buffer[cursor]);
         

    }

    
}


Line* allocate_ptr(size_t size)
{
    const size_t min_len = 100;
    size = (size > min_len) ? size : min_len;

    Line* line = malloc(sizeof(Line));
    line->newline = calloc(size,sizeof(buffer*));
    line->linenumber = 0;
    line->total_size = size;
    return line;

}


void insert_line(Line* line,buffer* buf,size_t line_no)
{

    
    if(line_no > line->total_size)
    {
        line->total_size = line->total_size*2;
        line->newline = realloc(line->newline,line->total_size*sizeof(buffer));
    }

    line->newline[line_no] = buf;
    line->linenumber = line_no;
}


int main()
{
	int     ch,cur_x = 0,cur_y = 0;
	buffer* buf = allocate_buffer(MIN_BUF_SIZE);
    buffer* head = buf;
    buf->line = cur_y;

    Line* line_ptr = allocate_ptr(100);

    insert_line(line_ptr,buf,buf->line);

    EditorStart();


	while (ch != 'q')
	{

		ch = getch();
		getyx(stdscr,cur_y,cur_x);

		switch (mode)
		{
			case NORMAL:
				NormalMode(ch,cur_x,cur_y);
				break;

			case INSERT:
                if(ch == ENTER)
                {

                        insert(buf,ch);
                       
                        buf->next = (buf->next == NULL) ? allocate_buffer(MIN_BUF_SIZE) : NULL;
                        buf->next->prev = buf;
                        buf = buf->next;
                        buf->line = buf->prev->line+1;
                        insert_line(line_ptr,buf,buf->line);
 
                        wmove(stdscr,buf->line,buf->cursor);
                        wrefresh(stdscr);

                } else if(ch == CTRL('s')) {
        
                            FILE* file = fopen("sample","w");

                    while(head!=NULL){

                            shrink_buffer(head);
                            fwrite(head->buffer,head->size,1,file);
                            head = head->next;
                    }

                    fclose(file);
                        
                 }  else InsertMode(line_ptr->newline[buf->line],ch,line_ptr,cur_x,cur_y);
				    break;

            case REPLACE:
                break;
		}

		
	}

    
   
	refresh();
	endwin();
	return 0;
}
