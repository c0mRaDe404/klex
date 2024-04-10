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

void InsertMode(int ch,buffer *row,int x,int y)
{
	//printw("%d-",x);
	(row+y)->line = y;
	(row+y)->pos = x;


	if(ch == KEY_BACKSPACE)
	{
		refresh();
		mvwdelch(stdscr,y,x-1);
		if(x==0)
		{
			move(y-1,(row+y-1)->pos);
		}
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
		(row+y+1)->text = (char*) calloc(BUFFER,sizeof(char));
		//realloc((row+y)->text,x);
		wmove(stdscr, y+1, 0);
		wrefresh(stdscr);
		(row+y)->line++;
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
		FILE* file = fopen("new.txt","wb");
		for(int i = 0;i<y;i++) 
			fwrite((row+i)->text,sizeof(char),(row+i)->pos,file);
		fclose(file);
	}
	else
	{

		
		(row+y)->text[x]=(char)ch;
		printw("%c",(row+y)->text[x]);
		//addch(ch);

	}

}



int main()
{
	int ch, max_x,max_y,cur_x=0,cur_y=0;

	buffer *row = (buffer*)calloc(BUFFER,sizeof(buffer)); // 1024 lines
	row->text = (char*) calloc(BUFFER,sizeof(char)); //1024 characters

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
				// printw("%d-",cur_x);
				InsertMode(ch,row,cur_x,cur_y);
				break;
		}

		
	}

	free(row->text);
	free(row);

	refresh();
	endwin();
	return 0;
}
