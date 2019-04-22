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


#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h> 
#include  <X11/Xlib.h>
#include  <GL/glx.h>
#include  <GL/gl.h>
#include  "network.h"

static int  sngBuf[] = {GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1,
                        GLX_BLUE_SIZE, 1, GLX_DEPTH_SIZE, 12, None};

static int  dblBuf[] = {GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1,
                        GLX_BLUE_SIZE, 1, GLX_DEPTH_SIZE, 12, GLX_DOUBLEBUFFER,
                        None};

void fatalError(char *estr)
{
  fprintf(stderr, "%s", estr);
  exit(1);
}

// glplot_util.c, wm
void draw_box(r,g,b,x,y,w,h)
     float r,g,b;      
     float x,y,w,h;    
{
  glBegin(GL_QUADS);
  glColor3f(r,g,b);
  glVertex2f(x,y);
  glVertex2f(x+w,y);
  glVertex2f(x+w,y+h);
  glVertex2f(x,y+h);
  glEnd();
}

// https://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale
double interpolate( double val, double y0, double x0, double y1, double x1 ) {
  return (val-x0)*(y1-y0)/(x1-x0) + y0;
}
double blue( double grayscale ) {
  if ( grayscale < -0.33 ) return 1.0;
  else if ( grayscale < 0.33 ) return interpolate( grayscale, 1.0, -0.33, 0.0, 0.33 );
  else return 0.0;
}
double green( double grayscale ) {
  if ( grayscale < -1.0 ) return 0.0; // unexpected grayscale value
  if  ( grayscale < -0.33 ) return interpolate( grayscale, 0.0, -1.0, 1.0, -0.33 );
  else if ( grayscale < 0.33 ) return 1.0;
  else if ( grayscale <= 1.0 ) return interpolate( grayscale, 1.0, 0.33, 0.0, 1.0 );
  else return 1.0; // unexpected grayscale value
}
double red( double grayscale ) {
  if ( grayscale < -0.33 ) return 0.0;
  else if ( grayscale < 0.33 ) return interpolate( grayscale, 0.0, -0.33, 1.0, 0.33 );
  else return 1.0;
}

int plot_network(nw)
  struct network nw;
{
  Display               *dpy;
  Window                win;
  Bool                  doubleBuffer = True;
  XVisualInfo           *vi = NULL;
  Colormap              cmap;
  XSetWindowAttributes  swa;
  GLXContext            cx;
  XEvent                event;
  int                   screen_num, t_ind, ori_ind;
  unsigned int          display_width, display_height, size; 
  float                 delta;
  static Bool           displayListInited = False;

  if(!(dpy = XOpenDisplay(NULL)))
    fatalError("could not open display");
  if(!(vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf))) {
    if(!(vi = glXChooseVisual(dpy, DefaultScreen(dpy), sngBuf)))
      fatalError("no RGB visual with depth buffer");
    doubleBuffer = False;
  }
  if(vi->class != TrueColor)
    fatalError("TrueColor visual required for this program");
  if(!(cx = glXCreateContext(dpy, vi, None, True)))
    fatalError("could not create rendering context");
  screen_num = DefaultScreen(dpy);
  display_width = DisplayWidth(dpy, screen_num);
  display_height = DisplayHeight(dpy, screen_num);
  if (display_width < display_height) {
    size = display_width*0.8;
  } else {
    size = display_height*0.8;
  }
  delta = 2.0/nw.dim;
  cmap = XCreateColormap(dpy,RootWindow(dpy,vi->screen),vi->visual,AllocNone);
  swa.colormap = cmap;
  swa.border_pixel = 0;
  swa.event_mask = ExposureMask | ButtonPressMask | StructureNotifyMask;
  win = XCreateWindow(dpy,RootWindow(dpy,vi->screen),0,0,size,size,
    0,vi->depth,InputOutput,vi->visual,
    CWBorderPixel | CWColormap | CWEventMask,
    &swa);
  XSetStandardProperties(dpy,win,"V1 Model","V1 Model",None,None,0,NULL);
  glXMakeCurrent(dpy, win, cx);
  XMapWindow(dpy, win);

  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0,1.0,0.0,0.0,0.0,1.0);

  t_ind = 0;
  //ori_ind = 0;
  //printf("Displaying orientation %.2f\n", nw.cols[0][0].ns[ori_ind].ori);
  while(t_ind < nw.n_steps) {
    XNextEvent(dpy, &event);
    XClearWindow(dpy, win);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(int i=0;i<nw.dim;i++){
        for(int j=0;j<nw.dim;j++){
            float v = 0;
            for (int k=0; k < nw.oris; k++) {
                v += nw.cols[i][j].ns[k].v[t_ind];
            }
            v /= (float) nw.oris;
            float x = nw.cols[i][j].ns[ori_ind].x*delta-1;
            float y = nw.cols[i][j].ns[ori_ind].y*delta-1;
            draw_box(red(v), green(v), blue(v), x, y, delta, delta);
        }
    }
    if(doubleBuffer)
      glXSwapBuffers(dpy, win); 
    else
      glFlush();
    //printf("Time: %i\n", t_ind);
    usleep(500000);
    t_ind++;
    /*if (t_ind == nw.n_steps && ori_ind < nw.oris) {
      printf("%i\n", t_ind);
      t_ind = 0;
      ori_ind++;
      printf("Displaying orientation %f\n", nw.cols[0][0].ns[ori_ind].ori);
    }*/
  }
  XCloseDisplay(dpy);
  return(0);
}


printf("%.2f\n", (45.0/(float)nw.args.oris));
    for (int i=0; i < nw.args.dim; i++) {
        for (int j=0; j < nw.args.dim; j++) {
            for (int i1=0; i1 < nw.args.dim; i1++) {
                for (int j1=0; j1 < nw.args.dim; j1++) {
                    float angle = (180.0/M_PI)*atan2(i - i1, j - j1);
                    if (angle < 0) {
                        angle += 180;
                    }
                    for (int k=0; k < nw.args.oris; k++) {
                        if (fabsf(angle - nw.cols[i][j].ns[k].ori) < (45.0/(float)nw.args.oris)) {
                            printf("%i %i %i %i %.2f %.2f\n", i, j, i1, j1, angle, nw.cols[i][j].ns[k].ori);
                        }
                    }
                }
            }
        }
    }

    int lat_n = 0;
    while ((*lat_f)(0, lat_n, 0, 0, nw.args.lat_sig) > 0.01) {
        lat_n++;
    }

    int *get_ij(int i, int j, int lat_n, int dim) {
    static int indices[4];
    if ((i - lat_n) < 0) { 
        indices[0] = 0;
    } else {
        indices[0] = i - lat_n;
    }
    if ((i + lat_n) > dim) { 
        indices[1] = dim;
    } else {
        indices[1] = i + lat_n;
    }
    if ((j - lat_n) < 0) { 
        indices[2] = 0;
    } else {
        indices[2] = j - lat_n;
    }
    if ((j + lat_n) > dim) { 
        indices[3] = dim;
    } else {
        indices[3] = j + lat_n;
    }
    return indices;
}

float gaussian1D(float x0, float x1, float sigma) {
    return exp(-((pow(x1 - x0, 2) / (2 * pow(sigma, 2)))));
}

float mexican_hat1D(float x0, float x1, float sigma) {
    return (1-pow((x1 - x0) / sigma, 2)) * exp(-((pow(x1 - x0, 2) / (2 * pow(sigma, 2))))); //(2/(pow(3 * sigma, 0.5) * pow(M_PI, 0.25)))*
}

float gaussian2D(float x0, float x1, float y0, float y1, float sigma) {
    return exp(-(((pow(x1 - x0, 2) / (2 * pow(sigma, 2)) + (pow(y1 - y0, 2) / (2 * pow(sigma, 2)))))));
}

float mexican_hat2D(float x0, float x1, float y0, float y1, float sigma) {
    return (1-(((pow(x1 - x0, 2) + pow(y1 - y0, 2)) / pow(sigma, 2))/2))*
            exp(-((pow(x1 - x0, 2) + pow(y1 - y0, 2)) / (2*pow(sigma, 2)))); //(1/(M_PI*pow(sigma,2)))
}

for (int i=0; i < nw.args.dim; i++) {
        for (int i1=0; i1 < nw.args.dim; i1++) {
            for (int j=0; j < nw.args.dim; j++) {
                for (int j1=0; j1 < nw.args.dim; j1++) {
                    delta_loc = pow((pow(i - i1, 2) + pow(j - j1, 2)), 0.5);
                    for (int x_wrap; x_wrap < 3; x_wrap++) {
                        for (int y_wrap; y_wrap < 3; y_wrap++) {
                            delta_loc = fminf(delta_loc, 
                                pow((pow(i - i1 + wrap[x_wrap], 2) + pow(j - j1 + wrap[y_wrap], 2)), 0.5));
                        }
                    }
                    printf("%i %i %i %i %.2f\n", i, i1, j, j1, delta_loc);
                }
            }
        }
    }

