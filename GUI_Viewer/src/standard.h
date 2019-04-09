///////////////////////////////////////////////////////////////////////
//
//  standard.h
//
//  written by Philip Fu (cwfu@acm.org)
//
//  - copyright
//  - please notify me for any modification
//
//  Mon Jan 22 7:54:04 EST 2001
//
///////////////////////////////////////////////////////////////////////




#ifndef _STANDARD
#define _STANDARD


#include <math.h>


///////////////////////////////////////////////////////////////////////
//
//  Debug Flag
//

//#define _DEBUG


///////////////////////////////////////////////////////////////////////
//
//  General Definition
//

#define errexit(s)		{ fprintf(stderr,s);     exit(EXIT_FAILURE); }
#define errexit2(s1,s2)		{ fprintf(stderr,s1,s2); exit(EXIT_FAILURE); }

#define _MAX_STR_SIZE		512
#define _MAX_LINE_SIZE		512
#define _MAX_PATH_SIZE		512

#define _ERROR			-1
#define _ERROR1			-2
#define _ERROR2			-3
#define _OKAY			 1

#define _TRUE			1
#define _FALSE			0

#define _TOGGLE(bool)		( ((bool) == _TRUE) ? _FALSE : _TRUE )

#define _ToRadian(X)		((X)/180.0*M_PI)
#define _ToDegree(X)		((X)*180.0/M_PI)

#ifndef _EPSILON
#define _EPSILON		1e-7
#endif

#ifndef _MAX
#define _MAX(a,b)		( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef _MIN
#define _MIN(a,b)		( ((a) < (b)) ? (a) : (b) )
#endif

#ifndef _IN_BETWEEN
#define _IN_BETWEEN(v,a,b)	( ((v) >= (a)) && ((v) <= (b)) )
#endif

#ifndef M_PI
#define M_PI			3.1415926535897932384626433832795
#endif

#if defined (__APPLE__) || defined(MACOSX) /* GLUT/ on Mac OS X */
#define FLT_MAX			MAXFLOAT
#endif


///////////////////////////////////////////////////////////////////////
// All Path Information should reference to _TOP_DIR (default value)
//
// If we can getenv("_TOP_DIR") from the environment, the return string
// will be the _TOP_DIR, else the following will be used by default
//

/*
#ifdef _WIN32
  #define _TOP_DIR	""
#else
  #define _TOP_DIR	""
#endif
*/


#endif
