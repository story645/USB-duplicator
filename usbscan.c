/******************************************************************************
*   Title: 	        usbscan.c
*   Author:	        Hannah Aizenman
*   Created on:     December 11 2011
*   Description:    Functions for detecting and managing usb events
*   Purpose:        handles usb interaction 
*   Build with:     make usbscan
*   Notes:          http://www.signal11.us/oss/udev/
*                   https://github.com/lunaryorn/pyudev/
**/

#include "usbscan.h" 

//Create the udev object 
struct udev *get_udev(){
   struct udev *udev;
   udev = udev_new();
   if (!udev){
      fprintf(stderr, "Can't create udev \n");
      exit(1);
   }
   return udev;
}

//set up monitor
struct udev_monitor *setup_udev_mon(struct udev *udev){
   struct udev_monitor *mon;
   mon = udev_monitor_new_from_netlink(udev, "udev");
   udev_monitor_filter_add_match_subsystem_devtype(mon, UDEV_SUBSYSTEM , NULL);
   udev_monitor_enable_receiving(mon);
   return mon;
}   
   
/**scan for devices and return added usb devices
this is always called inside a loop and expects that
either it gets a device or the program has been exited out of
 **/
struct udev_device *scan_for_usb(struct udev_monitor *mon, int fd){	
   struct udev_device *dev;
   const char *label;
   const char *uuid;
   const char *action;
   int ret;
   
   
   fd_set fds;
   FD_ZERO(&fds);
   FD_SET(fd, &fds);
   
   struct timeval tv;
   tv.tv_sec = 0; //otherwise wgetch is blocked
   tv.tv_usec = 0;
   

   //used to ensure non blocking io
   ret = select(fd+1, &fds, NULL, NULL, &tv); 
   
   // Check if our file descriptor has received data.
   if (ret > 0 && FD_ISSET(fd, &fds)){
	  dev = udev_monitor_receive_device(mon);
	  if (dev) {
         action = udev_device_get_action(dev);
         if ((action) && !strcmp(action, "add")){            
            //scans for usbs with labels or uuids 'cause that  it in media
            label = udev_device_get_property_value(dev, "ID_FS_LABEL");
            uuid = udev_device_get_property_value(dev, "ID_FS_UUID");
            if (label || uuid){
               return dev;
            }
            
         }
      }
   }

   usleep(250*1000);
   fflush(stdout);
   return 0;       
}


