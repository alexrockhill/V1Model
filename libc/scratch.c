void plot_network(nw)
    struct network nw;
{
    Display *dpy = XOpenDisplay(NIL);
    assert(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, DefaultScreen(dpy), attributeList);
    assert(vi);
    GLXContext gc = glXCreateContext(dpy, vi, 0, GL_TRUE);
    XSetWindowAttributes swa;
    swa.colormap = XCreateColormap(dpy, RootWindow(dpy, vi->screen),
                                   vi->visual, AllocNone);
    int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
    int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));
    int screen = XDefaultScreen(dpy);
    Window root = XRootWindow(dpy, vi->screen);
    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask;
    Window win = XCreateWindow(dpy, root, 0, 0, 600, 
                               600, 0, vi->depth, InputOutput, vi->visual,
                               CWBorderPixel|CWColormap|CWEventMask, &swa);
    XStoreName(dpy, win, “simplest”);
    XMapWindow(dpy, win);
    //GC gc = XCreateGC(dpy, win, 0, NIL);
    XSetForeground(dpy, gc, whiteColor);
    XSelectInput(dpy, win, StructureNotifyMask);
    glXMakeCurrent(dpy, win, gc);
    /*for(;;) {
        XEvent e;
        XNextEvent(dpy, &e);
        if (e.type == MapNotify)
            break;
    }*/
    XDrawLine(dpy, win, gc, 10, 60, 180, 20);
    XFlush(dpy);
    sleep(10);
}


int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
    int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));
    int screen = XDefaultScreen(dpy);
    int depth = XDefaultDepthOfScreen(screen);
    int width = XWidthOfScreen(screen);
    int height = XHeightOfScreen(screen);
    Colormap colormap = DefaultColormapOfScreen(screen);
    Window root = XRootWindow(dpy, screen);
    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask;
    Window win = XCreateSimpleWindow(dpy, root, *width/4, *height/4, *width/2, *height/2,
                                     0, NIL, blackColor);
    //XStoreName(dpy, win, “simplest”);
    XMapWindow(dpy, win);
    GC gc = XCreateGC(dpy, win, 0, NIL);
    XSetForeground(dpy, gc, whiteColor);
    XSelectInput(dpy, win, StructureNotifyMask);
    /*for(;;) {
        XEvent e;
        XNextEvent(dpy, &e);
        if (e.type == MapNotify)
            break;
    }*/



XVisualInfo *vi;
GLXContext ctx;
Colormap cmap;
XWindowAttributes xwa;
XSetWindowAttributes swa;
vi = glXChooseVisual(dpy,DefaultScreen(dpy),doubleBuffer);
ctx = glXCreateContext(dpy,vi,None,True);
cmap = XCreateColormap(dpy,RootWindow(dpy,vi->screen),vi->visual,AllocNone);
swa.colormap = cmap;
swa.border_pixel = 0;
swa.event_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask | 
                 StructureNotifyMask | KeyPressMask;
Window win = XCreateWindow(dpy,RootWindow(dpy,vi->screen),0,0,600,600,0,vi->depth,
                    InputOutput,vi->visual,
                    CWBorderPixel | CWColormap | CWEventMask,&swa);
glXMakeCurrent(dpy,win,ctx);
XMapWindow(dpy,win);

unsigned long gc_valuemask;
XGCValues gc_values;        
gc_values.foreground = 0;
gc_values.background = 1;
gc_valuemask         = GCForeground | GCBackground;
GC gc = XCreateGC(dpy, win, gc_valuemask, &gc_values);
glColor3f(1.0,0.0,0.0);
XDrawPoint(dpy, win, gc, 100, 20);
XDrawLine(dpy, win, gc, 10, 60, 180, 20);
XFlush(dpy);
sleep(10);



#import <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <assert.h>   
#include <unistd.h>
#include "network.h" 

#define NIL (0)

void plot_network();
void redraw(void);

static int single_buffer[] = {GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1,
                              GLX_BLUE_SIZE, 1, GLX_DEPTH_SIZE, 12, None};

static int double_buffer[] = {GLX_RGBA,GLX_RED_SIZE,1,GLX_GREEN_SIZE,1,
                              GLX_BLUE_SIZE,1,GLX_DEPTH_SIZE,12,GLX_DOUBLEBUFFER,
                              None};

Display   *dpy;
Window    win;
Bool    use_double_buffer = True;

GLfloat   xAngle = 42.0, yAngle = 82.0, zAngle = 112.0;

void plot_network(nw)
    struct network nw;
{
    XVisualInfo   *vi = NULL;
    Colormap    cmap;
    XSetWindowAttributes  swa;
    GLXContext    cx;
    XEvent      event;
    Bool      need_redraw = False, recalc_model_view = True;
    int     dummy;
    Display *dpy = XOpenDisplay(NIL);
    if(!(dpy = XOpenDisplay(NULL)))
        printf("could not open display"); return;
    if(!glXQueryExtension(dpy, &dummy, &dummy))
        printf("X server has no OpenGL GLX extension"); return;
    if(!(vi = glXChooseVisual(dpy, DefaultScreen(dpy), double_buffer))) {
        if(!(vi = glXChooseVisual(dpy, DefaultScreen(dpy), single_buffer)))
          printf("no RGB visual with depth buffer"); return;
        use_double_buffer = False;
    }
    if(vi->class != TrueColor)
        printf("TrueColor visual required for this program"); return;
    if(!(cx = glXCreateContext(dpy, vi, None, True)))
        printf("could not create rendering context"); return;
    cmap = XCreateColormap(dpy,RootWindow(dpy,vi->screen),vi->visual,AllocNone);
    swa.colormap = cmap;
    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | ButtonPressMask | StructureNotifyMask;
    win = XCreateWindow(dpy,RootWindow(dpy,vi->screen),0,0,600,600,
                        0,vi->depth,InputOutput,vi->visual,
                        CWBorderPixel | CWColormap | CWEventMask,
                        &swa);
    XSetStandardProperties(dpy,win,"glxsimple","glxsimple",None,None,0,NULL);
    glXMakeCurrent(dpy, win, cx);
    XMapWindow(dpy, win);
    glEnable(GL_DEPTH_TEST); 
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();
    glFrustum(-1.0,1.0,-1.0,1.0,1.0,10.0);

    while(1) {
    do {
      XNextEvent(dpy, &event);
      switch(event.type) {
        case ButtonPress:
          recalc_model_view = True;
          switch(event.xbutton.button) {
            case 1: xAngle += 10.0;   break;
            case 2: yAngle += 10.0;   break;
            case 3: zAngle += 10.0;   break;
          }
          break;
        case ConfigureNotify:
          glViewport(0,0,event.xconfigure.width,event.xconfigure.height);
        case Expose:
          need_redraw = True;
          break;
      }
    } while(XPending(dpy));

    if(recalc_model_view) {
      glMatrixMode(GL_MODELVIEW); /* switch to model matrix stack */
      glLoadIdentity();     /* reset modelview matrix to identity */
      glTranslatef(0.0,0.0,-3.0); /* move camera back 3 units */
      glRotatef(xAngle,0.1,0.0,0.0);  /* rotate by X angle */
      glRotatef(yAngle,0.0,0.1,0.0);  /* rotate by Y angle */
      glRotatef(zAngle,0.0,0.0,1.0);  /* rotate by Z angle */
      recalc_model_view = False;
      need_redraw = True;
    }

    if(need_redraw) {
      redraw();
      need_redraw = False;
    }
  }
}

void redraw(void)
{
  static Bool display_list_inited = False;

  if(display_list_inited)
    glCallList(1);        /* if list exists, execute it */
  else {
    glNewList(1,GL_COMPILE_AND_EXECUTE);  /* else create and execute it */

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_QUADS);

      /* Front face */
      glColor3f(0.0,0.7,0.1);     /* green */
      glVertex3f(-1.0, 1.0, 1.0);
      glVertex3f( 1.0, 1.0, 1.0);
      glVertex3f( 1.0,-1.0, 1.0);
      glVertex3f(-1.0,-1.0, 1.0);

      /* Back face */
      glColor3f(0.9,1.0,0.0);     /* yellow */
      glVertex3f(-1.0, 1.0,-1.0);
      glVertex3f( 1.0, 1.0,-1.0);
      glVertex3f( 1.0,-1.0,-1.0);
      glVertex3f(-1.0,-1.0,-1.0);

      /* Top side face */
      glColor3f(0.2,0.2,1.0);     /* blue */
      glVertex3f(-1.0, 1.0, 1.0);
      glVertex3f( 1.0, 1.0, 1.0);
      glVertex3f( 1.0, 1.0,-1.0);
      glVertex3f(-1.0, 1.0,-1.0);

      /* Bottom side face */
      glColor3f(0.7,0.0,0.1);     /* red */
      glVertex3f(-1.0,-1.0, 1.0);
      glVertex3f( 1.0,-1.0, 1.0);
      glVertex3f( 1.0,-1.0,-1.0);
      glVertex3f(-1.0,-1.0,-1.0);

    glEnd();

    glEndList();
    display_list_inited = True;
  }

  if(use_double_buffer)
    glXSwapBuffers(dpy, win);    /* buffer swap does implicit glFlush. */
  else
    glFlush();        /* explicit flush for single buf case */
}




#import <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <assert.h>   
#include <unistd.h>
#include "network.h" 

#define NIL (0)

void plot_network();
void redraw(void);

static int single_buffer[] = {GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1,
                              GLX_BLUE_SIZE, 1, GLX_DEPTH_SIZE, 12, None};

static int double_buffer[] = {GLX_RGBA,GLX_RED_SIZE,1,GLX_GREEN_SIZE,1,
                              GLX_BLUE_SIZE,1,GLX_DEPTH_SIZE,12,GLX_DOUBLEBUFFER,
                              None};

Display   *dpy;
Window    win;
Bool    use_double_buffer = True;

/* Initial 3d box orientation. */

GLfloat   xAngle = 42.0, yAngle = 82.0, zAngle = 112.0;

void plot_network(nw)
    struct network nw;
{
    XVisualInfo   *vi = NULL;
    Colormap    cmap;
    XSetWindowAttributes  swa;
    GLXContext    cx;
    XEvent      event;
    Bool      need_redraw = False, recalc_model_view = True;
    int     dummy;
    Display *dpy = XOpenDisplay(NIL);
    if(!(dpy = XOpenDisplay(NULL)))
        fprintf(stderr, "could not open display");
    if(!glXQueryExtension(dpy, &dummy, &dummy))
        fprintf(stderr, "X server has no OpenGL GLX extension");
    if(!(vi = glXChooseVisual(dpy, DefaultScreen(dpy), double_buffer))) {
        if(!(vi = glXChooseVisual(dpy, DefaultScreen(dpy), single_buffer)))
          fprintf(stderr, "no RGB visual with depth buffer");
        use_double_buffer = False;
    }
    if(vi->class != TrueColor)
       fprintf(stderr, "TrueColor visual required for this program");
    if(!(cx = glXCreateContext(dpy, vi, None, True)))
        fprintf(stderr, "could not create rendering context");
    cmap = XCreateColormap(dpy,RootWindow(dpy,vi->screen),vi->visual,AllocNone);
    swa.colormap = cmap;
    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | ButtonPressMask | StructureNotifyMask;
    win = XCreateWindow(dpy,RootWindow(dpy,vi->screen),0,0,600,600,
                        0,vi->depth,InputOutput,vi->visual,
                        CWBorderPixel | CWColormap | CWEventMask,
                        &swa);
    XSetStandardProperties(dpy,win,"V1 Model","V1 Model",None,None,0,NULL);
    glXMakeCurrent(dpy, win, cx);
    XMapWindow(dpy, win);
    glEnable(GL_DEPTH_TEST); 
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();
    glFrustum(-1.0,1.0,-1.0,1.0,1.0,10.0);

    while(1) {
    do {
      XNextEvent(dpy, &event);
      switch(event.type) {
        case ButtonPress:
          recalc_model_view = True;
          switch(event.xbutton.button) {
            case 1: xAngle += 10.0;   break;
            case 2: yAngle += 10.0;   break;
            case 3: zAngle += 10.0;   break;
          }
          break;
        case ConfigureNotify:
          glViewport(0,0,event.xconfigure.width,event.xconfigure.height);
        case Expose:
          need_redraw = True;
          break;
      }
    } while(XPending(dpy));

    if(recalc_model_view) {
      glMatrixMode(GL_MODELVIEW); /* switch to model matrix stack */
      glLoadIdentity();     /* reset modelview matrix to identity */
      glTranslatef(0.0,0.0,-3.0); /* move camera back 3 units */
      glRotatef(xAngle,0.1,0.0,0.0);  /* rotate by X angle */
      glRotatef(yAngle,0.0,0.1,0.0);  /* rotate by Y angle */
      glRotatef(zAngle,0.0,0.0,1.0);  /* rotate by Z angle */
      recalc_model_view = False;
      need_redraw = True;
    }

    if(need_redraw) {
      redraw(nw);
      need_redraw = False;
    }
  }
}

void redraw(nw)
   struct network nw;
{
    for(int i=0;i<nw.dim;i++){
        for(int j=0;j<nw.dim;j++){
            XDrawPoint(dpy, win, gc, fx[i][j],height-fy[i][j]);
            //for(int k=0;k<nw.oris;k++){
        //printf("%i\n", nw.cols[i][j].ns[k].x);
        //printf("%i\n", nw.cols[i][j].ns[k].y);
        //printf("%f\n", nw.cols[i][j].ns[k].ori);
        //printf("%f\n", nw.cols[i][j].ns[k].v);
           //}
    }
  }
  XFlush(dpy);
}



#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include "network.h" 

#define NIL (0)

static int  sngBuf[] = {GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1,
                        GLX_BLUE_SIZE, 1, GLX_DEPTH_SIZE, 12, None};

static int  dblBuf[] = {GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1,
                        GLX_BLUE_SIZE, 1, GLX_DEPTH_SIZE, 12, GLX_DOUBLEBUFFER,
                        None};

Window create_simple_window(Display* display, int width, int height, int x, int y)
{
  int screen_num = DefaultScreen(display);
  int win_border_width = 0;
  Window win;
  win = XCreateSimpleWindow(display, RootWindow(display, screen_num),
                            x, y, width, height, win_border_width,
                            BlackPixel(display, screen_num),
                            WhitePixel(display, screen_num));
  XMapWindow(display, win);
  XFlush(display);
  return win;
}

GC create_gc(Display* display, Window win)
{
  GC gc;
  unsigned long valuemask = 0; 
  XGCValues values; 
  unsigned int line_width = 2;
  int line_style = LineSolid;
  int cap_style = CapButt; 
  int join_style = JoinBevel;
  int screen_num = DefaultScreen(display);

  gc = XCreateGC(display, win, valuemask, &values);
  if (gc < 0) {
      fprintf(stderr, "XCreateGC: \n");
  }
  XSetForeground(display, gc, BlackPixel(display, screen_num));
  XSetBackground(display, gc, WhitePixel(display, screen_num));
  XSetLineAttributes(display, gc, line_width, line_style, cap_style, join_style);
  XSetFillStyle(display, gc, FillSolid);
  return gc;
}

int plot_network(nw)
    struct network nw;
{
  Display* display;
  int screen_num; 
  Window win; 
  unsigned int display_width, display_height, size; 
  GC gc;
  Colormap screen_colormap; 
  XColor red, brown, blue, yellow, green;
  Status rc;

  display = XOpenDisplay(NIL);
  if (display == NULL) {
    fprintf(stderr, "cannot connect to X server\n");
    exit(1);
  }
  if(!(vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf))) {
    if(!(vi = glXChooseVisual(dpy, DefaultScreen(dpy), sngBuf)))
      printf(stderr, "no RGB visual with depth buffer");
      exit(1);
    doubleBuffer = False;
  }
  screen_num = DefaultScreen(display);
  display_width = DisplayWidth(display, screen_num);
  display_height = DisplayHeight(display, screen_num);
  if (display_width < display_height) {
    size = display_width*0.8;
  } else {
    size = display_height*0.8;
  }
  win = create_simple_window(display, size, size, 0, 0);
  gc = create_gc(display, win);
  XSync(display, False);
  screen_colormap = DefaultColormap(display, DefaultScreen(display));
  for(int i=0;i<nw.dim;i++){
      for(int j=0;j<nw.dim;j++){
          float this_v = nw.cols[i][j].ns[0].v;
          XSetForeground(display, gc, color(this_v+0.5,0.5-this_v,0));
          XDrawPoint(display, win, gc, nw.cols[i][j].ns[0].x, nw.cols[i][j].ns[0].y);
      }
  }
  sleep(4);
  XCloseDisplay(display);
  return(0);
}
