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
  int                   screen_num, t_ind;
  unsigned int          display_width, display_height, size; 
  float                 delta;

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

  static Bool displayListInited = False;

  t_ind = 0;
  while(t_ind < nw.n_steps) {
    XNextEvent(dpy, &event);
    if(displayListInited)
      glCallList(1);
    else {
      glNewList(1,GL_COMPILE_AND_EXECUTE); 
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      for(int i=0;i<nw.dim;i++){
          for(int j=0;j<nw.dim;j++){
              float this_v = nw.cols[i][j].ns[0].v;
              float x = nw.cols[i][j].ns[0].x*delta-1;
              float y = nw.cols[i][j].ns[0].y*delta-1;
              draw_box(this_v+0.5, 0.5-this_v, 0, x, y, delta, delta);
          }
      }
      glEndList();
      displayListInited = True;
    }
    if(doubleBuffer)
      glXSwapBuffers(dpy, win); 
    else
      glFlush();
    usleep(500000);
    t_ind++;
  }
  XCloseDisplay(dpy);
  return(0);
}

