/******************************************************************************   
Title: mainmenu.c
*   Author:Hannah Aizenman
*   Created on:     December 11 2011
*   Description:    Code for menus for usb duplicator
*   Purpose:        Generates menus for main program
*   Usage: Called by main
*   Build with:     make mainmenu
*   Notes: Structure/code taken from: 
    http://www.troubleshooters.com/lpm/200405/200405.htm#_A_Simple_Menu
**/

#include "mainmenu.h"

//manages menu_window
int menu_screen(WINDOW *win){
   int n_choices = 3;
    char *choices[] = {"1) Create Image",
                        "2) Duplicate Image",
                        "3) Exit" 
       					};
   	MENU *my_menu;
   	int my_choice;
   	int i;
   
    mvwprintw(win, 8, 2, "Select an option by pressing a number.\n");
   
   	my_menu = create_menu(choices, n_choices);	
   
   //format window
   	set_menu_win(my_menu, win);
   	set_menu_sub(my_menu, derwin(win, 6, 38, 3, 1));
   	set_menu_format(my_menu, 5, 1);			
   	set_menu_mark(my_menu, " * ");
   
   
   	//post menu to screen
   	post_menu(my_menu);
   
   	wrefresh(win);
   
   	// get choice
   	my_choice = menu_options(my_menu, win);	
   	
   	//unpost and free
   	unpost_menu(my_menu);
   	for (i =0; i<=n_choices; i++)
		free_item(my_menu->items[i]);
   	free_menu(my_menu);
   	return my_choice;
}

//handles user selection
int menu_options(MENU *my_menu, WINDOW *win){
    nodelay(win, 0);
    wrefresh(win);
    int c;  //key
    int offset = 0x30; //ASCII offset for digits
   	//keeps running loop until enter is hit
    while((c = wgetch(win))!= 0x0a){
       switch(c){  
   		case KEY_DOWN:
      		menu_driver(my_menu, REQ_DOWN_ITEM);
      		break;
   		case KEY_UP:
       		menu_driver(my_menu, REQ_UP_ITEM);
      		break;
		default: ///checks if c is one of the #s in the menu
          if (c> offset && (c <= (my_menu->nitems + offset)))	
            return (c-offset) - 1; //converts ASCII option to index
          break;
       }
       wrefresh(win);
    } 
   return item_index(current_item(my_menu));
};

// creates menu
MENU *create_menu(char *choices[], int n_choices){
   ITEM **my_items;        /* list of items on this menu */
   int i;
   /* ALLOCATE ITEM ARRAY AND INDIVIDUAL ITEMS */
   my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
   for(i = 0; i < n_choices; ++i)
	   	my_items[i] = new_item(choices[i], NULL);
   my_items[n_choices] = (ITEM *)NULL; //segfaults without a null item at the end
   /* CREATE THE MENU STRUCTURE AND DISPLAY IT */
   return new_menu((ITEM **)my_items);
}
