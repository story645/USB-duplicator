/******************************************************************************
*   Title: mainmenu.h
*   Author:Hannah Aizenman
*   Created on:     December 11 2011
*   Description:    header file for main menu 
*   Purpose:        Generates menus for main program
**/


#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>

//returns selected option
int menu_options(MENU *my_menu, WINDOW *win);
//creates window for menu, passes value back to main
int menu_screen(WINDOW *win);
//creates menu
MENU *create_menu(char *choices[], int n_choices);
