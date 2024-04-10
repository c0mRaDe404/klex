#include <ncurses.h>
#include <stdlib.h>
#include "ted.h"


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

	case REPLACE:
		return "-- REPLACE --";
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
	mvwprintw(stdscr,getmaxy(stdscr)-1,0,Mode());
	move(0,0);
	refresh();


}


void NormalMode(int ch,int x,int y)
{
	noecho();
	mvwprintw(stdscr,getmaxy(stdscr)-1,0,Mode());
	move(y,x);
	refresh();

	if(ch == 'i')
	{
		//echo();
		mode = INSERT;
		mvwprintw(stdscr,getmaxy(stdscr)-1,0,Mode());
		move(0,0);
		refresh();
	}

}

void InsertMode(int ch,int x,int y)
{
	

	if(ch == KEY_BACKSPACE)
	{
		refresh();
		//printw("%d",x);
		mvwdelch(stdscr,y,x-1);
	}
	else if(ch == KEY_ESC)
	{
		noecho();
		mode = NORMAL;
		mvwprintw(stdscr,getmaxy(stdscr)-1,0,Mode());
		move(0,0);

	}
	else if (ch == KEY_ENTER)
	{
		wmove(stdscr, y+1, x);
		wrefresh(stdscr);
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
	else addch(ch);

}



int main()
{
	int ch, max_x,max_y,cur_x=0,cur_y=0;


	EditorStart(); 

	getmaxyx(stdscr,max_y,max_x);

	while (ch != 'q')
	{

		ch = getch();
		getyx(stdscr,cur_y,cur_x);
		//refresh();

		switch (mode)
		{
			case NORMAL:
				NormalMode(ch,cur_x,cur_y);
				break;

			case INSERT:
				InsertMode(ch,cur_x,cur_y);
				break;
		}

		
	}

	refresh();
	endwin();
	return 0;
}
