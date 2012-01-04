/******************************************************************************
*   Title: 	    wintemp.h
*   Author:	    Hannah Aizenman
*   Created on:     December 11 2011
*   Description:    header file for windows function
*   Purpose:        Generates main window border
**/


#include <ncurses.h>
#include <cdk/cdk.h>
#include <string.h>

// sets up main window
WINDOW *setup_window();
//creates border heading and the like
void set_border(WINDOW *win);
//floats window in middle of screen
void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);
//cleans up main window
void teardown_window();
//creates progress bar
int progress_bar_init(WINDOW *win, int block_size, int total_size);
//updates progress bar
int progress_bar_update(WINDOW *win, int current_pass, int number_passes);
//gets fileviewer
char * file_selector(WINDOW *win, char *startdir, char *title);