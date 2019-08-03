#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <ncursesw/ncurses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Manager.h"

#define MAX_NAME_LEN 10
#define BUF_SIZE 1000

void sig_winch(int signo)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) & size);
	resizeterm(size.ws_row, size.ws_col);
}

int editor()
{
	WINDOW * menu;
	WINDOW * text;
	WINDOW * submenu;
	WINDOW * subtext;

	int fl_open = 0;
	int fl_but = 0;
	int fd;

	initscr();
	signal(SIGWINCH, sig_winch);
	cbreak();
	noecho();
	curs_set(0);
	refresh();
	start_color();
	draw_menu(&menu, &submenu);
	out_text(&text, &subtext);
	keypad(menu, TRUE);
	while (fl_but != KEY_F(3))
	{
		fl_but = wgetch(menu);
		switch (fl_but)
		{
			case KEY_F(1):
					if(fl_open == 0){
								fd = file_reader(fd, text, subtext);
								if (fd == -1) printf("ERROR");
								fl_open = 1;
							}
					break;

			case KEY_F(2):
					break;
			case KEY_F(3):
					break;
		}

	}


//end
	close(fd);
	delwin(menu);
	delwin(text);
	delwin(submenu);
	delwin(subtext);
	endwin();
}

int file_reader(int fd, WINDOW* text, WINDOW* subtext)
{
	char buffer[BUF_SIZE] = {" "};
	int fl_read = 0;

	fd = open ("raven.txt", O_RDWR);
	if (fd == -1) return -1;

	fl_read = read(fd, buffer, BUF_SIZE);
	if (fl_read == -1) return -1;

	wmove(subtext, 0, 0);
	wprintw(subtext, "%s", buffer);
	wmove(subtext, 0, 0);
	wrefresh(subtext);

	return fd;
}

int draw_menu(WINDOW ** menu, WINDOW **submenu)
{
	*menu = newwin(5,33, 1, 1);
	if(*menu == NULL) return -1;

	init_pair(1, COLOR_YELLOW, COLOR_BLUE);
	box(*menu, '|', '-');
	*submenu = derwin(*menu, 3, 31, 1, 1);
	wbkgd(*menu, COLOR_PAIR(1));
	wbkgd(*submenu, COLOR_PAIR(1));
	wmove(*submenu, 0, 0);
	wprintw(*submenu, "\nF1 - open, F2 - save, F3 - exit");
	wrefresh(*submenu);
	wrefresh(*menu);
	return 0;
}

int out_text(WINDOW ** text, WINDOW **subtext)
{
	*text = newwin(40, 100, 6, 1);
	if (*text == NULL) return -1;

	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	box(*text, '|', '~');
	*subtext = derwin(*text, 39, 99, 1, 1);
	wbkgd(*text, COLOR_PAIR(2));
	wbkgd(*subtext, COLOR_PAIR(2));
	wrefresh(*text);
	wrefresh(*subtext);
}

int main()
{
	editor();
	return 0;
}
