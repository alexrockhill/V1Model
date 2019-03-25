# V1Model
A model of V1 based of the Paul Bressloff et al.'s paper "What Geometric Visual Hallucinations Tell Us about the Visual Cortex"


(0)  System requirements.  The required C-compiler and libraries are
free and are standard on many Linux and Mac OS X systems.  To compile
the main application, 'V1m', you need,

    i.  C compiler
   ii.  X11 libraries
  iii.  OpenGL libraries

All compile commands are in text files in the ./s/ directory, and can
be edited as needed.


(1)  Compile the libraries and main programs*

  source ./s/compile_lib
  source ./s/compile_main

 *Note, the compile_... and mlm_... scripts in ./s/ contain
  references to paths for OpenGL and X11 libraries and headers.
  Please edit these files and change the paths if needed.


(2)  Test the code by running the model, see ./demo/README


(3)  For help, contact:  aprockhill206@gmail.com

*Adapted from wm by Wyeth Bair* 
