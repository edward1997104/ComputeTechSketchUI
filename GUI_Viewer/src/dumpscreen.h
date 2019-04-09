///////////////////////////////////////////////////////////////
//
// dumpscreen.h
//
// - dump screen utility to grad screen from OpenGL
//
// by Philip Fu (cwfu@acm.org)
//
// 3/4/2003 2:22P
//
// All rights reserved
// 
///////////////////////////////////////////////////////////////




#ifndef _DUMPSCREEN_H
#define _DUMPSCREEN_H


///////////////////////////////////////////////////////////////
//
// Input Parameters:
//
//  - viewport is the screen region
//	- x ( window coordinates x )
//	- y ( window coordinates y )
//	- w ( width  )
//	- h ( height )
//	- will be clamped by the current viewport internally
//  - mode
//	- GL_FRONT, GL_BACK, GL_LEFT, GL_RIGHT, etc.
//	  (see glReadBuffer)
//  - buf
//	- IF input buf == NULL
//	  - dumpscreen routines will allocate memory internally
//	- ELSE
//	  - assume intput buf has enough space to hold the data
//
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
// (1) Dump to the allocated buf
///////////////////////////////////////////////////////////////

// return the buffer on success else NULL

unsigned char * dumpScreenToBuf ( unsigned char *buf, int mode, int x, int y, int w, int h ) ;
unsigned char * dumpScreenToBuf ( unsigned char *buf, int mode                             ) ;


///////////////////////////////////////////////////////////////
// (2) Dump to a File
///////////////////////////////////////////////////////////////

// return 0 on success else 1 on error

int dumpScreenToFile ( const char *filename, unsigned char *buf, int mode, int x, int y, int w, int h ) ;
int dumpScreenToFile ( const char *filename, unsigned char *buf, int mode                             ) ;


#endif
