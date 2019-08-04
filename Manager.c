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

#define NAME_LEN 30
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
	int row = 0, col = 0;
	int fd = 0, testfd = 0;
	char textbuf[BUF_SIZE*BUF_SIZE] = {" "};
	chtype charbuf;

	initscr();
	signal(SIGWINCH, sig_winch);
	cbreak();
	noecho();
	curs_set(FALSE);
	refresh();
	start_color();
	draw_menu(&menu, &submenu);
	out_text(&text, &subtext);
	keypad(text, TRUE);
	while (fl_but != KEY_F(3))
	{
		fl_but = wgetch(text);
		getyx(subtext, row, col);
		switch (fl_but)
		{
			case KEY_F(1):
					if(fd != 0)
					{
						close(fd);
					}
						wclear(subtext);
						wrefresh(subtext);
						testfd = file_reader(fd, &text, &subtext);
						if (testfd == -1)
						{
							wprintw(subtext, "ERROR");
						}else{
							fd = testfd;
							fl_open = 1;
						}
					break;

			case KEY_F(2):
					if ((fd != 0) && (fd != -1))
					{
						lseek(fd, 0, SEEK_SET);
						for (row = 0; row < 38; row++)
						for (col = 0; col < 98; col++)
						{
							charbuf = mvwinch(subtext, row, col);
							write(fd, &charbuf, 1);
						}
						wmove(subtext, 0, 0);
					}
					break;
			case KEY_F(3):
					break;
			case KEY_LEFT:
					wmove(subtext, row, col-1);
					break;
			case KEY_RIGHT:
					wmove(subtext, row, col+1);
					break;
			case KEY_UP:
					wmove(subtext, row-1, col);
					break;
			case KEY_DOWN:
					wmove(subtext, row+1, col);
					break;
			case KEY_BACKSPACE:
					wmove(subtext, row, col-1);
					wprintw(subtext, " ");
					wmove(subtext, row, col-1);
					wrefresh(subtext);
					break;
			default:
					waddch(subtext, fl_but);
					break;
		}
		wrefresh(subtext);
	}


//end
	close(fd);
	delwin(menu);
	delwin(text);
	delwin(submenu);
	delwin(subtext);
	endwin();
}

int file_reader(int fd, WINDOW** text, WINDOW** subtext)
{
	char buffer[BUF_SIZE] = {" "};
	int fl_read = 0;
	char fname[NAME_LEN] = {" "};
	wmove(*subtext,0,0);
	wprintw(*subtext, "Enter the name of the text file \n");
	curs_set(TRUE);
	wgetnstr(*subtext, fname, NAME_LEN);
	fd = open (fname, O_RDWR);
	if (fd == -1) return -1;

	fl_read = read(fd, buffer, BUF_SIZE);
	if (fl_read == -1) return -1;
	wclear(*subtext);
	wmove(*subtext, 0, 0);
	wprintw(*subtext, "%s", buffer);
	wmove(*subtext, 0, 0);
	wrefresh(*subtext);
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
	init_pair(1, COLOR_YELLOW, COLOR_BLUE);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	box(*text, '|', '~');
	*subtext = derwin(*text, 38, 98, 1, 1);
	wbkgd(*text, COLOR_PAIR(1));
	wbkgd(*subtext, COLOR_PAIR(2));
	wrefresh(*text);
	wrefresh(*subtext);
}

int main()
{
	editor();
	return 0;
}
