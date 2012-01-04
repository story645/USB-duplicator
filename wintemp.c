/******************************************************************************
*   Title: 	        wintemp.c
*   Author:	        Hannah Aizenman
*   Created on:     December 11 2011
*   Description:    Functions for managing window
*   Purpose:        Generates main window border
*   Build with:     make wintemp
**/

#include "wintemp.h"
//creates window
WINDOW *setup_window(){
   	WINDOW *win;
   	int height = 20;
   	int width = 60;
   	int starty = 4;
   	int startx = 4;
   
    initscr();          
   //	raw();
    cbreak();
   	noecho();
   	start_color();
   	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
   
  	attron(COLOR_PAIR(1));
   	win = newwin(height, width, starty, startx);
   	keypad(win, TRUE);
   	return win;
}

//deletes window and resets terminal
void teardown_window(WINDOW *win){
   curs_set(1);
   attroff(COLOR_PAIR(1));
   delwin(win);
   endwin();
}

//sets name, border, general formatting
void set_border(WINDOW *win){
   wclear(win);
   box(win, 0, 0);
   print_in_middle(win, 1, 0, 40, "USB Duplicator Menu", COLOR_PAIR(1));
   mvwaddch(win, 2, 0, ACS_LTEE);
   mvwhline(win, 2, 1, ACS_HLINE, 58); //width-2
   mvwaddch(win, 2, 59, ACS_RTEE);//width-1
   wrefresh(win);
   attron(COLOR_PAIR(2));
   mvprintw(LINES - 1, 0, "Use the arrow keys to navigate");
   attroff(COLOR_PAIR(2));
}	


//http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/menus.html
//floats words in middle of window 
void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color){
	int length, x, y;
	float temp;
	if(win == NULL)
		win = stdscr;
	getyx(win, y, x);
   
	if(startx != 0)
		x = startx;
	if(starty != 0)
		y = starty;
	if(width == 0)
		width = 80;
	
   	length = strlen(string);
   	temp = (width - length)/ 2;
	x = startx + (int)temp;
	wattron(win, color);
	mvwprintw(win, y, x, "%s", string);
	wattroff(win, color);
	refresh();
}

int progress_bar_init(WINDOW *win, int block_size, int total_size){
   int number_passes = (int) (total_size/block_size);
   if ((total_size%block_size)!=0)
     number_passes++;//accounts for extra
   waddch(win, '[');
   int i,k;
   
   for (i =0; i<number_passes; i++)
     waddch(win, 0x20);
   
   waddch(win, ']');
   
   for(k=0; k<(number_passes+1); k++)
     waddch(win, 0x08); //go backward

   return number_passes;
}

int progress_bar_update(WINDOW *win, int current_pass, int number_passes){
   waddch(win, '=');
   if(current_pass<(number_passes)){
      waddch(win, '>');
      waddch(win, 0x08);
   }
   else{
     waddch(win,'\n'); //new line when progress ends
   }
   return 0;
}

//File selector widget
char * file_selector(WINDOW *win, char *startdir, char *title){
   /* Declare variables. */
   CDKSCREEN *cdkscreen; 
   CDKFSELECT *fSelect;
   char *label		= "File: ";
   char *filename	= 0;
   char *mesg[4];

   /* Set up CDK. */
   cdkscreen = initCDKScreen (win);

   /* Start color. */
   initCDKColor();

   /* Get the filename. */
   if (filename == 0)
   {
      fSelect = newCDKFselect (cdkscreen, CENTER, CENTER, 20, 65,
				title, label, A_NORMAL, '_', A_REVERSE,
				"</5>", "</48>", "</N>", "</N>", TRUE, FALSE);

      /*
       * Set the starting directory. This is not neccessary because when
       * the file selector starts it uses the present directory as a default.
       */
      setCDKFselect (fSelect, startdir, A_NORMAL, '.', A_REVERSE, "</5>", "</48>", "</N>", "</N>", ObjOf(fSelect)->box);

      /* Activate the file selector. */
      filename = copyChar (activateCDKFselect (fSelect, 0));

      /* Check how the person exited from the widget. */
      if (fSelect->exitType == vESCAPE_HIT)
      {
	 /* Pop up a message for the user. */
	 mesg[0] = "<C>Escape hit. No file selected.";
	 mesg[1] = "";
	 mesg[2] = "<C>Press any key to continue.";
	 popupLabel (cdkscreen, mesg, 3);

	 /* Destroy the file selector. */
	 destroyCDKFselect (fSelect);

	 /* Exit CDK. */
	 destroyCDKScreen (cdkscreen);
	 endCDK();

      }
   }

   /* Destroy the file selector. */
   destroyCDKFselect (fSelect);
   return filename;
}
