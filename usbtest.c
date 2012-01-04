/******************************************************************************
*   Title: 	        usbtest.c
*   Author:	        Hannah Aizenman
*   Created on:     December 19 2011
*   Description:    Functions for detecting and managing usb events
*   Purpose:        tests whether usbscan works
*   Build with:     make tusb
*   Notes:          http://www.signal11.us/oss/udev/
**/
#include <stdio.h>
#include <sys/stat.h>

#include "usbscan.h"

int main(){
   struct udev *udev;
   struct udev_device *dev;
   struct udev_monitor *mon;
   const char *label;
   const char *uuid;
   int fd;
   struct stat buf;
   char mediapath[FILENAME_MAX + 1];
   
   udev = get_udev();
   mon  = setup_udev_mon(udev);
   fd  = udev_monitor_get_fd(mon); 
   
   while(!(dev = scan_for_usb(mon, fd)));
   label = udev_device_get_property_value(dev, "ID_FS_LABEL");
   uuid = udev_device_get_property_value(dev, "ID_FS_UUID");
   
   if (label){
      fprintf(stdout, "label: %s\n", label);
      sprintf(mediapath, "/media/%s", label);
   }
   else{
      fprintf(stdout, "UUID: %s\n", uuid);
      sprintf(mediapath, "/media/%s", uuid);
   }
   
   if (stat(mediapath, &buf)!=1){
      fprintf(stdout, "Device Path: %s\n", mediapath);
      int size = buf.st_size;
      fprintf(stdout, "Device Size: %il bytes\n", size);
   }
   else
     fprintf(stdout, "Device Name: %s\n", udev_device_get_property_value(dev, "DEVNAME"));
             
   return(0);
};
