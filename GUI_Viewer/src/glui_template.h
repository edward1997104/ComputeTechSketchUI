///////////////////////////////////////////////////////////////
//
// glui_example.c
//
// Example to build a standard GLUI interface
//
// Philip Fu
// 
///////////////////////////////////////////////////////////////




#ifndef _GLUI_EXAMPLE
#define _GLUI_EXAMPLE


  #include <math.h>


  // Version
  #define PROGRAM_NAME			"GLUI Template Program"
  #define VERSION_MAJOR			"1"
  #define VERSION_MINOR			"1"

  // About this Program
  #define PROGRAM_ABOUT			"Program by Philip Chi-Wing Fu\n \
- cwfu@acm.org\n \
\n \
22nd Jan. 2003 -"
  #define PROGRAM_ABOUT_ONE_LINE	"Program by Philip Chi-Wing"	// for status bar

  // Dumpscreen File
  #define DUMPSCREEN_FILE		"glui_template"

  // Window Color
  #define GLUI_BGCOLOR			 50, 30, 75
  #define GLUI_FGCOLOR			220,220,220
  #define STATUSBAR_BGCOLOR		 50, 30, 75
  #define STATUSBAR_FGCOLOR		220,220,220

  // window size and title
  #define WIN_W				640
  #define WIN_H				640

  // Z range
  #define _Z_NEAR			   1.0f
  #define _Z_FAR			5000.0f

  // default fovy
  #define _DEFAULT_FOVY			45.0f

  // acceleration factor on SHIFT key
  #define _SHIFT_ACCELERATION		5.0f

  // if the action between motion and mouse's up is within this limit (in seconds),
  // may set idle rotation (depends on other condition as well)
  #define _IDLE_ROTATION_TIME_LIMIT	0.2f

  // minmax of fovy
  #define FOVY_MIN			  1.0f
  #define FOVY_MAX			150.0f

  // Help Menu
  #ifdef _WIN32
  #define _HELP_MESSAGE "  Mouse Control\n\n\
left        : rotate the world\n\
  + ALT    : rotate at viewpoint\n\
middle      : xy-translation\n\
   + ALT    : changing fovy\n\
left+mid    : xz-translation\n\
\n\
 + shift    : acceleration key\n\
 + ctrl     : deceleration key\n\
right click : select as line end point\n\
 Ctrl + Z   : undo draw(delete) action\n\
 DEL        : delete selected line\n\
 left click : select a line\n\
 O          : Output line set\n"
  #else
  #define _HELP_MESSAGE "Mouse Control\n\n \
left     : rotate the world\n \
   + ALT : rotate at viewpoint\n \
middle   : xy-translation\n \
   + ALT : changing fovy\n \
left+mid : xz-translation\n \
\n \
 + shift : acceleration key\n \
 + ctrl  : deceleration key"
  #endif



#endif
