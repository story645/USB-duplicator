/******************************************************************************
*   Title:          dispatch.c
*   Author:         Hannah Aizenman
*   Created on:     December 13 2011
*   Description:    
*   Purpose:        manages the logic of each screen 
*   Usage:        
*   Build with:     make dispatch
*   Notes: 
* 
**/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <sys/stat.h>
#include <errno.h>

#include "mainmenu.h"
#include "usbscan.h" 
#include "wintemp.h"

#define IMG_FOLDER "/tmp"
#define RCFILE ".usbdrc"
#define COPY_BLOCK_SIZE 32*1024 //32kb
#define SHEIGHT 10 
#define SWIDTH 50
#define SSTARTX 10
#define SSTARTY 10

//manages main menu screen 
int main_menu_screen(WINDOW *win);
//manages create image window
int create_image_screen(WINDOW *win);
//manages duplicate screen 
int duplicate_screen(WINDOW *win);
//manages confirm disc screen
int confirm_usb_screen(WINDOW *win, struct udev *udev, struct udev_device *dev);
//select location screen
int copy_image_screen(WINDOW *win, const char *devident, char *devpath, char *mediapath);
//copies image to usb
int duplicate_image_screen(WINDOW *win);
//handles error messages and exits
int error_exit(WINDOW *win, char * message);
//window for copy dialog
int block_copy(WINDOW *win, WINDOW *sbwin, char *src, char *dest);

int main(){
    WINDOW *win; 
    win = setup_window();
    set_border(win);
    main_menu_screen(win);
    teardown_window(win);
    exit(0);
}

//displays menu and dispatches choices
int main_menu_screen(WINDOW *win){
   WINDOW *sbwin;
   sbwin = subwin(win, SHEIGHT, SWIDTH, SSTARTX, SSTARTY);
   touchwin(win);
   wrefresh(win);
   switch(menu_screen(sbwin)){ 
    case 0:
      return create_image_screen(win);
    case 1:
      return duplicate_image_screen(win);
    case 2:
      return (0);
    default:
      error_exit(win, "invalid main menu choice");
   }
   return(0);
};

//screen waits on usb insertion
int create_image_screen(WINDOW *win){
   WINDOW *sbwin;
   sbwin = subwin(win, SHEIGHT, SWIDTH, SSTARTX, SSTARTY);
   
   touchwin(win);
   wrefresh(win);
   wprintw(sbwin, "Insert USB \n");
   wrefresh(sbwin);
   
   struct udev *udev;
   struct udev_device *dev;
   struct udev_monitor *mon;
   int fd;
   //revolving bar
   int i = 0;
   char barspin[5] = {'-', '\\', '|', '/', '-'};
   waddch(sbwin, 0x20); //adds space
   
   udev = get_udev();
   mon  = setup_udev_mon(udev);
   fd  = udev_monitor_get_fd(mon); 
   nodelay(win, 1);
   nodelay(sbwin, 1); //turns off delay on input
   curs_set(0);
   
   while(!(dev=scan_for_usb(mon, fd))){
     dev = scan_for_usb(mon, fd);
     waddch(sbwin, 0x08); //backspace
     waddch(sbwin, barspin[i%5] | A_BOLD);
     waddch(sbwin, 0x08);
     wrefresh(sbwin);
     i++;
   }
   waddch(sbwin, 0x20);
   werase(sbwin);
   delwin(sbwin);
   return confirm_usb_screen(win, udev, dev);
};
                  
//screen for verifiying the usb to pull the image off of
int confirm_usb_screen(WINDOW *win, struct udev *udev, struct udev_device *dev){
   struct stat buf;
   const char *label;
   const char *uuid;
   const char *devident;
   const char *ident;
   const char *devname;
   char mediapath[FILENAME_MAX + 1];
   char devpath[FILENAME_MAX +1];
   
   WINDOW *sbwin;
   sbwin = subwin(win, SHEIGHT, SWIDTH, SSTARTX, SSTARTY);
   touchwin(win);   
   wrefresh(win);
   wprintw(sbwin, "Confirm USB: Press Enter \n");
   wrefresh(sbwin);
   devname = udev_device_get_property_value(dev, "DEVNAME");
   label = udev_device_get_property_value(dev, "ID_FS_LABEL");
   uuid = udev_device_get_property_value(dev, "ID_FS_UUID");
   int LEN = strlen(devname);
  
   //currently only handles usbs with one parition
   if (devname[LEN-1]!='1')
      error_exit(win, "unknown, check if drive has multiple partitions");
   
   strncpy(devpath, devname, LEN-1); //strips off the one for the block 
   devident = (label)? label : uuid;
   ident = (label)? "Label" : "UUID";
   //assumes device gets mounted on media either using label or uuid
   wprintw(sbwin, "%s: %s\n", ident, devident);
   sprintf(mediapath, "/media/%s", devident);
   wprintw(sbwin, "Device path: %s\n", devpath);
   wrefresh(sbwin);
   if (stat(mediapath, &buf)!=1){      
      wprintw(sbwin, "Mount Path: %s\n", mediapath);
   }
   else{
      udev_unref(udev);
      delwin(sbwin);
      error_exit(win, "couldn't find device"); 
   }
   wrefresh(sbwin);
   
   if (wgetch(sbwin) == 0x0a){
      werase(sbwin);
      delwin(sbwin);
      return copy_image_screen(win, devident, devpath, mediapath);
   }
   udev_unref(udev);
   werase(sbwin);
   delwin(sbwin);
   
  return(0);
};

//select destination screen
int copy_image_screen(WINDOW *win, const char *devident,  char *devpath, char *mediapath){
   char dfname[FILENAME_MAX + 1];
   time_t rawtime;
   struct tm * timeinfo;
   int CUR_LEN = 80;
   char curtime[CUR_LEN];
   time(&rawtime);
   timeinfo = localtime(&rawtime);
   
   strftime(curtime, CUR_LEN, "%Y%m%d%H%M%S", timeinfo);
   sprintf(dfname, "%s/%s_%s.img", IMG_FOLDER, curtime, devident);
   
   WINDOW *sbwin;
   wrefresh(win);
   sbwin = subwin(win, SHEIGHT, SWIDTH, SSTARTX, SSTARTY);
   touchwin(win);
   wprintw(sbwin, "Copying Image\nPress ESC to exit\n" );
   block_copy(win, sbwin, devpath, dfname);
   return (0);
};

//screen for copying image to screen
//calls two file selectors
int duplicate_image_screen(WINDOW *win){
   char *src;
   char *dest;
   WINDOW *sbwin;
   wrefresh(win);
   sbwin = subwin(win, SHEIGHT, SWIDTH, SSTARTX, SSTARTY);
   touchwin(win);  
   src = file_selector(sbwin, IMG_FOLDER, "<C>Pick an image file.");
   werase(sbwin);
   wrefresh(sbwin);
   dest = file_selector(sbwin, "/dev", "<C>Pick a disk to write to.");
   werase(sbwin);
   wrefresh(sbwin);
   block_copy(win, sbwin, src, dest);
   return (0);
}

//screen for doing block copies and handles revolving bar
//revolving bar is used 'cause I couldn't get progress bar to work
//'cause I was getting bad filesizes
int block_copy(WINDOW *win, WINDOW *sbwin, char *src, char *dest){
   FILE *ifile;
   FILE *ofile;
   char * buffer;
   int bytesread;
   int byteswritten;
   int i = 0;
   curs_set(0);
   char barspin[5] = {'-', '\\', '|', '/', '-'};
   
   
   if ((ifile = fopen(src, "rb")) == NULL) {
     error_exit(win, "Error opening input file");
   }
   //ofile = fopen(dfname, "wa"); //file to write image to 
   if ((ofile = fopen(dest, "w")) == NULL) {
     error_exit(win, "Error opening output file");
   }

   buffer = (char*) malloc (sizeof(char)*COPY_BLOCK_SIZE);
   
   wprintw(sbwin, "Copying from %s to %s \n", src, dest);
   wprintw(sbwin, "Please wait: \n"); 
   
   wrefresh(sbwin);
   do{
      bytesread = fread(buffer, 1, COPY_BLOCK_SIZE, ifile);
      byteswritten = fwrite(buffer, 1, bytesread, ofile);
      if (byteswritten != bytesread)
           perror("Copy mismatch: read =/= write");
      
      waddch(sbwin, 0x08);
      waddch(sbwin, barspin[i%5] | A_BOLD);
      waddch(sbwin, 0x08);
      wrefresh(sbwin);
      i++;
      
   }while(bytesread !=0);
   wprintw(sbwin, "Finished Copying\n");
   werase(sbwin);
   delwin(sbwin);
   curs_set(1);      
   
   fclose(ifile);
   fclose(ofile);   
   return (0);
}

//ends ncurses then prints error exit
int error_exit(WINDOW *win, char *message){
   teardown_window(win);
   perror(message);
   exit(-1);
}
