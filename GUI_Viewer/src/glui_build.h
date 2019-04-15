///////////////////////////////////////////////////////////////
//
// glui_build.cpp
//
// - building the generic GLUI interface
//
// Philip Fu
// 
///////////////////////////////////////////////////////////////


#ifndef _GLUI_BUILD
#define _GLUI_BUILD


  #define _MAX_ROLLOUT	32

	///////////////////////////////////////////////////////////////
	// INTERNAL CONSTANT
	///////////////////////////////////////////////////////////////
	#define XY_SELECTED  0
	#define YZ_SELECTED  1
	#define XZ_SELECTED  2

  // ID for GLUI callback
  #define ID_QUIT			10
  #define ID_HELP			11
  #define ID_ABOUT			12
  #define ID_ROTATE_PANELS		13
  #define ID_CLOSE_PANELS		14

  // ID for basic panel
  #define ID_RESETVIEW			20
  #define ID_DUMPSCREEN			21
  #define ID_FOVY			22
  #define ID_CHANGE_BGCOLOR		23
  #define ID_SHOWAXES			24
  #define ID_XY_SELECT			25
  #define ID_YZ_SELECT			26
  #define ID_XZ_SELECT			27
  #define ID_XY_OFFSET			28
  #define ID_YZ_OFFSET			29
  #define ID_XZ_OFFSET			30
  #define ID_LINE_SET           31
  #define ID_UNDO               32
  #define ID_OUTPUT             33
  #define ID_PERSPECTIVE        34
  #define ID_TOGGLE_DRAW        35
  #define ID_READ_SKETCH		36

  // Build the GLUI interface
  extern void initGLUI();

  // rotate the rollout panels in main panel
  extern void closeAllRollouts();
  extern void rotatePanelsOffset(int value);
  extern int  rotatePanelsTo(char key);

  // Reset file, Views, and Textures
  extern void resetStatusBar(const char * fmt, ... );

  // functions
  extern void callbackGLUI(int id);


#endif
