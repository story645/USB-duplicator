/******************************************************************************
*   Title: 	    usbscan.h
*   Author:	    Hannah Aizenman
*   Created on:     December 17 2011
*   Description:    header for usbscan
*   Purpose:        handles usb interaction 
*   Notes:          http://www.signal11.us/oss/udev/
**/

#include <libudev.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <ncurses.h>

//describes which system to filter on
#define UDEV_SUBSYSTEM "block"

//sets up udev
struct udev *get_udev();
//grabs the moniter
struct udev_monitor *setup_udev_mon(struct udev *udev);
//grabs the device if added
struct udev_device *scan_for_usb(struct udev_monitor *mon, int fd);
//prints device info
void print_dev_info(struct udev_device *dev);

