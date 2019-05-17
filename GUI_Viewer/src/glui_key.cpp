///////////////////////////////////////////////////////////////
//
// glui_key.cpp
//
// - handle keyboard and idle events
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
#endif

#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#ifdef __APPLE__
#include <OPENGL/gl.h>
#include <GLUT/glut.h>
#endif // __APPLE__

#include "lines.h"
#include "standard.h"

#include "glui_build.h"



///////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////

  extern int mainWindowID ;

  extern int showAxes ;

  unsigned int ANNOT_COUNT = 0;




///////////////////////////////////////////////////////////////
// Internal Functions
///////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////
// Keyboard Callbacks
///////////////////////////////////////////////////////////////

void
keyboard ( unsigned char key, int x, int y )
{
    static int waitForSecondKey = _FALSE;
    static char str[_MAX_STR_SIZE];
    int winID,r,i,l;

    // rollout list
    extern int  nRolloutList;
    extern char rolloutChar[_MAX_ROLLOUT];

    // external function
    void myExit ( int ) ;
    void myHelp (     ) ;


    ///////////////////////////////////////////
    // Make sure pointing to the mainWindow

    winID = glutGetWindow();
    if (winID != mainWindowID)
	glutSetWindow(mainWindowID);


    ///////////////////////////////////////////
    // For second key

    if (waitForSecondKey) {

	// rotate the panels
	if ('1' <= key && key <= '1'+nRolloutList-1)
	    r = rotatePanelsTo(key-'1');
	else
	    r = rotatePanelsTo(key);

	waitForSecondKey = _FALSE;

	// if success, just return
	if (r) {
	    resetStatusBar("control panel rearranged.");
	    return;
	}
    }


	printf("key: %d\n", key);
    switch (key) {


      ////////////////////////////////////////////////////////////////////
      // Quit (27 -> ESC)

      case 'q' : case 'Q' :
	myExit(EXIT_SUCCESS);
	break;

	  case 27:
		  clearSelectedPoint();
		  break;

      case 'h' : case 'H' :
	myHelp();
	break;

      case 'm' :
	if (strncmp(str,"Please",6)) {

	    // init. str once only
	    sprintf(str,"Rearrange subpanels by keys: %c",rolloutChar[0]);
	    l = strlen(str);
	    for (i=1; i<nRolloutList; i++) {
		str[l++] = ',';
		str[l++] = ' ';
		str[l++] = rolloutChar[i];
	    }
	    str[l++] = '.';
	    str[l]   = '\0';
	    strcat(str, " ( M - close all subpanels )");
	}
	resetStatusBar(str);
	waitForSecondKey = _TRUE;	// wait for the next key
	break;

      case 'M' :
	closeAllRollouts();
	resetStatusBar("all panels closed.");
	break;
	


      ////////////////////////////////////////////////////////////////////
      // basic control

      case 'r' : case 'R' :
	callbackGLUI(ID_RESETVIEW);
	break;

      case 'd' : case 'D' :
	callbackGLUI(ID_DUMPSCREEN);
	break;


      ////////////////////////////////////////////////////////////////////
      // status

      case 'i' : case 'I' :
	//float mat[16];
	/*
	fprintf(stderr,"Mouse button = %d\n",mouseButton);
	glGetFloatv(GL_MODELVIEW_MATRIX,mat);
	fprintf(stderr,"mvmat = [\n");
	fprintf(stderr,"  %6.3f %6.3f %6.3f %6.3f\n",  mat[0],mat[4],mat[8], mat[12]);
	fprintf(stderr,"  %6.3f %6.3f %6.3f %6.3f\n",  mat[1],mat[5],mat[9], mat[13]);
	fprintf(stderr,"  %6.3f %6.3f %6.3f %6.3f\n",  mat[2],mat[6],mat[10],mat[14]);
	fprintf(stderr,"  %6.3f %6.3f %6.3f %6.3f ]\n",mat[3],mat[7],mat[11],mat[15]);
	*/
	break;

      case 'a' : case 'A' :
	showAxes = 1 - showAxes;
	callbackGLUI(ID_SHOWAXES);
	break;
	  
	  case 'o': case 'O':
		  outputLineSet();
		  break;
	  case 127:
		  printf("delete is pressed!\n");
		  deleteSelectedLine();
		  break;
	  case 26:
		  undoAction();
		  break;

	  case 54: //'6'
		  printf("666666666\n");
		  ANNOT_COUNT++;
		  break;
	  case 52: //'4'
		  printf("444444444\n");
		  ANNOT_COUNT = 0;
		  break;

      default:
	;

	//fprintf(stderr,"Unhandled key: %c [%d] \n",key,key);
    }


    if (winID != mainWindowID)
	glutSetWindow(winID);
}

void
specKey ( int key, int x, int y )
{
}
