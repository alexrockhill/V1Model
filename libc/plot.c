#import <stdio.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <assert.h>   
#include <unistd.h>   

#define NIL (0)

void plot_line()
{
    int     screen;
    Window  root;
    Display *dpy = XOpenDisplay(NIL);
    assert(dpy);
    int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
    int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));
    screen = XDefaultScreen(dpy);
    root = XRootWindow(dpy,screen);
    Window w = XCreateSimpleWindow(dpy, root, 0, 0, 600, 600, 0, blackColor, blackColor);
    XSelectInput(dpy, w, StructureNotifyMask);
    XMapWindow(dpy, w);
    GC gc = XCreateGC(dpy, w, 0, NIL);
    XSetForeground(dpy, gc, whiteColor);
    for(;;) {
        XEvent e;
        XNextEvent(dpy, &e);
        if (e.type == MapNotify)
            break;
    }      
    XDrawLine(dpy, w, gc, 10, 60, 180, 20);
    XFlush(dpy);
    sleep(10);
}