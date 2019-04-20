#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h> 
#include  <X11/Xlib.h>
#include  <GL/glx.h>
#include  <GL/gl.h>
#include  <png.h>  //http://www.libpng.org/pub/png/libpng.html brew install libpng
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

// https://stackoverflow.com/questions/3191978/how-to-use-glut-opengl-to-render-to-a-file
/* Adapted from https://github.com/cirosantilli/cpp-cheat/blob/19044698f91fefa9cb75328c44f7a487d336b541/png/open_manipulate_write.c */
static png_byte *png_bytes = NULL;
static png_byte **png_rows = NULL;
static void screenshot_png(const char *filename, unsigned int width, unsigned int height,
        GLubyte **pixels, png_byte **png_bytes, png_byte ***png_rows) {
    size_t i, nvals;
    const size_t format_nchannels = 4;
    FILE *f = fopen(filename, "wb");
    nvals = format_nchannels * width * height;
    *pixels = realloc(*pixels, nvals * sizeof(GLubyte));
    *png_bytes = realloc(*png_bytes, nvals * sizeof(png_byte));
    *png_rows = realloc(*png_rows, height * sizeof(png_byte*));
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, *pixels);
    for (i = 0; i < nvals; i++)
        (*png_bytes)[i] = (*pixels)[i];
    for (i = 0; i < height; i++)
        (*png_rows)[height - i - 1] = &(*png_bytes)[i * width * format_nchannels];
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();
    png_infop info = png_create_info_struct(png);
    if (!info) abort();
    if (setjmp(png_jmpbuf(png))) abort();
    png_init_io(png, f);
    png_set_IHDR(
        png,
        info,
        width,
        height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);
    png_write_image(png, *png_rows);
    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    fclose(f);
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
  FILE                  *avconv = NULL;
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
  delta = 2.0/nw.args.dim;
  cmap = XCreateColormap(dpy,RootWindow(dpy,vi->screen),vi->visual,AllocNone);
  swa.colormap = cmap;
  swa.border_pixel = 0;
  swa.event_mask = ExposureMask | ButtonPressMask | StructureNotifyMask;
  win = XCreateWindow(dpy,RootWindow(dpy,vi->screen), 0, 0, size, size,
    0, vi->depth,InputOutput, vi->visual,
    CWBorderPixel | CWColormap | CWEventMask,
    &swa);
  XSetStandardProperties(dpy, win,"V1 Model", "V1 Model", None, None, 0, NULL);
  glXMakeCurrent(dpy, win, cx);
  XMapWindow(dpy, win);

  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0,1.0,0.0,0.0,0.0,1.0);

  char fname[300];
  unsigned char *pixels = malloc(size * size * 4 * sizeof(GL_UNSIGNED_BYTE));

  t_ind = 0;
  while(t_ind < nw.args.n_steps) {
    while (XPending(dpy)) {
      XNextEvent(dpy, &event);
    }
    sprintf(fname, "dpy/%i.png", t_ind);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
    for(int i=0;i<nw.args.dim;i++){
        for(int j=0;j<nw.args.dim;j++){
            float v = 0;
            for (int k=0; k < nw.args.oris; k++) {
                v += nw.cols[i][j].ns[k].v[t_ind];
            }
            v /= (float) nw.args.oris;
            float x = nw.cols[i][j].ns[0].x*delta-1;
            float y = nw.cols[i][j].ns[0].y*delta-1;
            draw_box(red(v), green(v), blue(v), x, y, delta, delta);
        }
    }
    if(doubleBuffer)
      glXSwapBuffers(dpy, win); 
    else
      glFlush();
    if (t_ind > 1)
      screenshot_png(fname, size, size, &pixels, &png_bytes, &png_rows);
    printf("Time: %i\n", t_ind);
    usleep(500000);
    t_ind++;
  }
  XCloseDisplay(dpy);
  free(pixels);
  free(png_bytes);
  free(png_rows);
  return(0);
}

/*
sprintf(fname, "dpy/%i.%i.%i.%i.%.2f.%.2f.%.2f.%.2f.%.2f.%s.%s.%i.png",
            size, nw.args.dim, nw.args.oris, nw.args.n_steps,
            nw.args.loc_mu, nw.args.lat_mu, nw.args.loc_sig, nw.args.lat_sig,
            nw.args.leak, nw.args.loc, nw.args.lat, t_ind);
*/

