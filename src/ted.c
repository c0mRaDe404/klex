#include <string.h>
#include <ncurses.h>
#include <stdlib.h>
#include <ted.h>
#include <buffer.h>


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

void InsertMode(int ch,int x,int y)
{
    int maxy = getmaxy(stdscr);


	if(ch == KEY_BACKSPACE)
	{
		refresh();
		mvwdelch(stdscr,y,x-1);
		
	}
	else if(ch == KEY_ESC)
	{
		noecho();
		mode = NORMAL;
		mvwprintw(stdscr,maxy-1,0,"%s", Mode());
		move(0,0);

	}
	else if (ch == KEY_ENTER)
	{
        


		wmove(stdscr, y+1, 0);
		wrefresh(stdscr);
	
	}
    else if(ch == 9)
    {
        return;

    }
	else if (ch == KEY_LEFT)
	{
		wmove(stdscr, getcury(stdscr), getcurx(stdscr) - 1);
		wrefresh(stdscr);
	}
	else if (ch == KEY_RIGHT)
	{
		wmove(stdscr, getcury(stdscr), getcurx(stdscr) + 1);
		wrefresh(stdscr);
	}
	else if (ch == KEY_DOWN)
	{
		wmove(stdscr, y+1, x);
		wrefresh(stdscr);
	}
	else if (ch == KEY_UP)
	{
		wmove(stdscr, y - 1,x);
		wrefresh(stdscr);
	}
	else if(ch == CTRL('s'))
	{

		return;
	}
	else
	{

		return;
	}



}



int main()
{
	int ch,cur_x=0,cur_y=0;

	EditorStart(); 

	while (ch != 'q')
	{

		ch = getch();
		getyx(stdscr,cur_y,cur_x);
		refresh();

		switch (mode)
		{
			case NORMAL:
				NormalMode(ch,cur_x,cur_y);
				break;

			case INSERT:
				break;
            case REPLACE:
                break;
		}

		
	}


	refresh();
	endwin();
	return 0;
}
