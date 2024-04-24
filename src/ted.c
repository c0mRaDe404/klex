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




void InsertMode(buffer* buf,int ch,int x,int y)
{

         
//    buf = ( buf->next == NULL) ? buf: buf->next;
 
    int maxy = getmaxy(stdscr);    


    if(ch == KEY_BACKSPACE) {

        refresh();
        mvwdelch(stdscr,y,x-1);
        delete(buf);
    }

    else if(ch == KEY_ESC) {

        noecho();
        mode = NORMAL;
        mvwprintw(stdscr,maxy-1,0,"%s", Mode());
        move(0,0);

    } else if (ch == KEY_LEFT) {
    
      cursor_left(buf);
      wmove(stdscr, getcury(stdscr), getcurx(stdscr) - 1);
      wrefresh(stdscr);

    } else if (ch == KEY_RIGHT) {
    
      cursor_right(buf);
      wmove(stdscr, getcury(stdscr), getcurx(stdscr) + 1);
      wrefresh(stdscr);

    } else if (ch == KEY_DOWN) {
    
      wmove(stdscr, buf->line++, buf->cursor);

      wrefresh(stdscr);

    } else if (ch == KEY_UP) {
        
        

     wmove(stdscr, buf->line--, buf->cursor);
      wrefresh(stdscr);

    } else {

          size_t cursor = buf->cursor;
          insert(buf, ch);
          mvprintw(buf->line,buf->cursor,"%c",buf->buffer[cursor]);
          //addch(buf->buffer[cursor]);

    }

    
}



int main()
{
	int     ch,cur_x = 0,cur_y = 0;
	buffer* buf = allocate_buffer(MIN_BUF_SIZE);
    buffer* head = buf;
    buf->line = cur_y;
    
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


                        buf->next = (buf->next == NULL) ? allocate_buffer(MIN_BUF_SIZE) : NULL;
                        buf->next->prev = buf;
                        buf = buf->next;
                        buf->line = cur_y+1;
                        
                        wmove(stdscr,cur_y,0);
                        wrefresh(stdscr);

                } else if(ch == CTRL('s')) {
        
                            FILE* file = fopen("sample","w");

                    while(head!=NULL){

                            shrink_buffer(head);
                            fwrite(head->buffer,head->size,1,file);
                            head = head->next;
                    }

                    fclose(file);
                        
                 } 

                InsertMode(buf,ch,cur_x,cur_y);
				break;
            case REPLACE:
                break;
		}

		
	}

    
   
	refresh();
	endwin();
	return 0;
}
