///////////////////////////////////////////////////////////////
//
// dumpscreen.cpp
//
// - dump screen utility to grad screen from OpenGL
//   - dump to a buf
//     OR
//   - dump to a ppm file
//
// by Philip Fu (cwfu@acm.org)
//
// 3/4/2003 2:22P
//
// 13/9/2006 make compatible with osx by lihw
//
// All rights reserved
// 
///////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
#endif

#ifdef __APPLE__
#include <OPENGL/gl.h>
#endif // __APPLE__

#include "pnm.h"
#include "standard.h"




///////////////////////////////////////////////////////////////
// Internal functions
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// Upside Down the Image

static void
upsidedownImage(unsigned char *img, int w, int h)
{
    unsigned char * ptrUp   ;
    unsigned char * ptrDown ;

    int nPixelLine, linesize, count ;


    ////////////////////////////////////////////////////////////////////////
    // Initialization

    // number of pixels in one scanline
    nPixelLine = w * 3 ;

    // number of bytes for one scanline
    linesize = sizeof(unsigned char) * nPixelLine ;

    // counter
    count = h / 2 ;

    // pointers
    ptrUp   = img                      ;	// first line
    ptrDown = img + nPixelLine * (h-1) ;	// last  line


    ////////////////////////////////////////////////////////////////////////
    // Speed up upside-down image when w <= 2048

    if ( w <= 2048 ) {

	unsigned char buf [ 2048*3 ] ;		// <= 2048 pixels

	while ( count-- ) {

	    memcpy( buf,     ptrUp,   linesize ) ;
	    memcpy( ptrUp,   ptrDown, linesize ) ;
	    memcpy( ptrDown, buf,     linesize ) ;

	    ptrUp   += nPixelLine ;
	    ptrDown -= nPixelLine ;
	}
    }


    ////////////////////////////////////////////////////////////////////////
    // Have to allocate memory if w > 2048

    else {

	unsigned char *buf;

	buf = (unsigned char *) malloc ( linesize );

	if (!buf) {
	    fprintf(stderr,"upsidedownImage : Not enough memory to allocate for buf!\n\n");
	    exit(1);
	}

	while ( count-- ) {

	    memcpy( buf,     ptrUp,   linesize ) ;
	    memcpy( ptrUp,   ptrDown, linesize ) ;
	    memcpy( ptrDown, buf,     linesize ) ;

	    ptrUp   += nPixelLine ;
	    ptrDown -= nPixelLine ;
	}

	free(buf);
    }
}


///////////////////////////////////////////////////////
// Read the OpenGL rendering buffer into a buf

static void
readGLBuffer ( GLenum mode, int x, int y, int w, int h, unsigned char *imgbuf )
{
    ////////////////////////////////////////////
    // (1) Read Image at corresponding buffer

    glReadBuffer  ( mode ) ;
    glPixelStorei ( GL_PACK_ALIGNMENT, 1 ) ;
    glReadPixels  ( x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, imgbuf ) ;


    ////////////////////////////////////////////
    // (2) Swap it upside down

    upsidedownImage ( imgbuf, w, h ) ;
}


///////////////////////////////////////////////////////
// Clamp the requested viewport against the current 
// OpenGL viewport to make sure it is not outside the 
// boundary of the possible viewport
//
// - return true  if the requested viewport is valid
// - return false if the requested viewport is invalid
//  (i.e. outside the current viewport)

static bool
clampViewport ( int *x, int *y, int *w, int *h )
{
    int x0,x1,y0,y1,viewport[4];


    ////////////////////////////////////////////
    // (1) Get current Viewport

    glGetIntegerv(GL_VIEWPORT, (GLint*)viewport);

    x0 = viewport[0]               ;
    x1 = viewport[0] + viewport[2] ;
    y0 = viewport[1]               ;
    y1 = viewport[1] + viewport[3] ;


    ////////////////////////////////////////////
    // (2) clamp x and width

    if ( *x < x0 ) {

	// shrink the width
	*w -= ( x0 - *x ) ;
	if ( *w <= 0 )
	    return false ;

	// move x to x0
	*x = x0 ;
    }
    else
    if ( *x > x1 )
	return false ;


    ////////////////////////////////////////////
    // (3) clamp y and height

    if ( *y < y0 ) {

	// shrink the height
	*h -= ( y0 - *y ) ;
	if ( *h <= 0 )
	    return false ;

	// move y to y0
	*y = y0 ;
    }
    else
    if ( *y > y1 )
	return false ;

    return true ;
}


///////////////////////////////////////////////////////
// get current viewport

static void
getCurrViewport ( int *inViewport )
{
    glGetIntegerv ( GL_VIEWPORT, (GLint*)inViewport ) ;
}


///////////////////////////////////////////////////////
// allocate memory

static unsigned char *
allocateImgBuf ( int w, int h )
{
    unsigned char *img = (unsigned char *) malloc(sizeof(unsigned char) * w * h * 3);

    if (!img) {
	printf("readImg : Not enough memory to allocate for img!\n\n");
	return NULL;
    }

    return img ;
}



///////////////////////////////////////////////////////////////
// External functions
///////////////////////////////////////////////////////////////



////////////////////////////////////////////////////
// (1) Dump to the allocated buf
////////////////////////////////////////////////////

// return the buffer on success else NULL


unsigned char * 
dumpScreenToBuf ( unsigned char *buf, int mode, int x, int y, int w, int h )
{
    /////////////////////////////////////////
    // Clamp the Viewport

    if ( clampViewport ( &x, &y, &w, &h ) == false )
	return NULL ;


    /////////////////////////////////////////
    // Allocate memory if necessary

    if ( buf == NULL ) {

	buf = allocateImgBuf ( w, h ) ;

	if ( buf == NULL ) return NULL ;
    }


    /////////////////////////////////////////
    // Read the OpenGL Buffer

    readGLBuffer ( mode, x, y, w, h, buf ) ;


    return buf ;
}


unsigned char * 
dumpScreenToBuf ( unsigned char *buf, int mode )
{
    /////////////////////////////////////////
    // Get the Viewport

    int viewport[4] ;

    getCurrViewport ( viewport ) ;


    /////////////////////////////////////////
    // Allocate memory if necessary

    if ( buf == NULL ) {

	buf = allocateImgBuf ( viewport[2], viewport[3] ) ;

	if ( buf == NULL ) return NULL ;
    }


    /////////////////////////////////////////
    // Read the OpenGL Buffer

    readGLBuffer ( mode, viewport[0], viewport[1], viewport[2], viewport[3], buf ) ;


    return buf ;
}



////////////////////////////////////////////////////
// (2) Dump to a File
////////////////////////////////////////////////////

// return 0 on success else 1 on error


int
dumpScreenToFile ( const char *filename, unsigned char *buf, int mode, int x, int y, int w, int h )
{
    /////////////////////////////////////////
    // dump screen to a buffer

    unsigned char * imgbuf = dumpScreenToBuf ( buf, mode, x, y, w, h ) ;

    if ( imgbuf == NULL ) return 1 ;	// error


    /////////////////////////////////////////
    // write to a file

    if ( writePNM( filename, "P6", w, h, 255, imgbuf ) == _ERROR )
	return 1 ;


    /////////////////////////////////////////
    // free the buffer?

    if ( buf == NULL )
	free( imgbuf ) ;


    return 0 ;
}


int
dumpScreenToFile ( const char *filename, unsigned char *buf, int mode )
{
    /////////////////////////////////////////
    // dump screen to a buffer

    unsigned char * imgbuf = dumpScreenToBuf ( buf, mode ) ;

    if ( imgbuf == NULL ) return 1 ;	// error


    /////////////////////////////////////////
    // write to a file

    int viewport[4];

    getCurrViewport ( viewport ) ;

    if ( writePNM( filename, "P6", viewport[2], viewport[3], 255, imgbuf ) == _ERROR )
	return 1 ;


    /////////////////////////////////////////
    // free the buffer?

    if ( buf == NULL )
	free( imgbuf ) ;


    return 0 ;
}

