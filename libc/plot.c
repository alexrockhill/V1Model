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
    glNewList(1, GL_COMPILE_AND_EXECUTE); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(int i=0;i<nw.dim;i++){
        for(int j=0;j<nw.dim;j++){
            float v = 0;
            for (int k=0; k < nw.oris; k++) {
                v += nw.cols[i][j].ns[k].v[t_ind];
            }
            v /= (float) nw.oris;
            float x = nw.cols[i][j].ns[0].x*delta-1;
            float y = nw.cols[i][j].ns[0].y*delta-1;
            draw_box(red(v), green(v), blue(v), x, y, delta, delta);
        }
    }
    glEndList();
    if(doubleBuffer)
      glXSwapBuffers(dpy, win); 
    else
      glFlush();
    printf("Time: %i\n", t_ind);
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

