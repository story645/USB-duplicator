#Makefile for final project

CC = gcc
CFLAGS = -Wall 
LDFLAGS = -lncurses -lmenu -ludev -lcdk
OBJECTS = dispatch.o mainmenu.o wintemp.o usbscan.o
HEADERS = mainmenu.h wintemp.h usbscan.h

all: usbdup

usbdup: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) ${LDFLAGS} -o usbd

dispatch: dispatch.c $(HEADERS)
	$(CC) $(CFLAGS) -c dispatch.c

mainmenu: mainmenu.c mainmenu.h
	$(CC) $(CFLAGS) -c mainmenu.c
    
wintemp: wintemp.c wintemp.h
	$(CC) $(CFLAGS) -c wintemp.c

usbscan: usbscan.c usbscan.h
	$(CC) $(CFLAGS) -c usbscan.c
    
clean:
	rm -rf *.o *~
	rm -rf /tmp/*.img
