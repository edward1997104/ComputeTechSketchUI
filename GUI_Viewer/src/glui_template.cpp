///////////////////////////////////////////////////////////////
//
// glui_template.c
//
// Example to build a standard GLUI interface
//
// Philip Fu
// 
// 13/9/2006 make compatible with osx by lihw
//
///////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#ifdef WIN32
#include <windows.h>
#include <float.h>
#else
#include <unistd.h>
#endif

#ifdef __linux
#include <values.h>
#endif

#include <string.h>

#if defined (__APPLE__) || defined(MACOSX) /* GLUT/ on Mac OS X */
#include <OPENGL/gl.h>
#include <GLUT/glut.h>                     /* for GLUT, GL, GLU */
#else                                      /* GL/ on IRIX etc. */
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "grid.h"
#include "lines.h"

#include "standard.h"
#include "pnm.h"
#include "glui.h"
#include "dumpscreen.h"

#include "glui_build.h"
#include "glui_draw.h"
#include "glui_key.h"
#include "glui_mouse.h"
#include "glui_template.h"


///////////////////////////////////////////////////////////////
// Global Definitions
///////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////


  //////////////////////////
  // INTERNAL

  // window stuff
  int  mainWindowID,winW,winH;
  char programName[_MAX_STR_SIZE],winTitle[_MAX_STR_SIZE],aboutStr[_MAX_STR_SIZE];
  int  backgndColor[3];

  // various flag
  int showAxes;
  int showStatusBar;
  int perspective = 1;

  // viewing
  float currFovy;
  float currAspect;

  // Grid variable
  Grid_t * xyGrid;
  Grid_t * yzGrid;
  Grid_t * xzGrid;

  // LineSet Variable
  LineSet_t * currentLineSet;
  //////////////////////////
  // EXTERNAL

  extern GLUI * glui, *gluiStatusBar   ;
  extern double mouseMotionSensitivity ;




///////////////////////////////////////////////////////////////
// Function Declarations
///////////////////////////////////////////////////////////////

  void myInit();
  void glInit();
  void myExit(int exitCode);
  void myHelp();
  void myAbout();

  void resetProj();
  void resetView();
  void reshape(int w, int h);

  void display(void);
  void dumpScreen();




///////////////////////////////////////////////////////////////
// 1. Initialization and Cleanup
///////////////////////////////////////////////////////////////

void
myInit()
{
    ////////////////////////////////////////////////////////////////////
    // Note: No OpenGL initialization should be put here, glInit instead
    ////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////
    // Setup the window

    winW = WIN_W;
    winH = WIN_H;

    // window title
    sprintf(programName,"%s %s.%s",PROGRAM_NAME,VERSION_MAJOR,VERSION_MINOR);
    sprintf(winTitle,"%s",programName);

    // about string
    strcat(aboutStr, PROGRAM_ABOUT);


    ////////////////////////////////////////////////////////////////////
    // Initial Viewing

    currFovy   = _DEFAULT_FOVY;
    currAspect = (float) winW / (float) winH;


    ////////////////////////////////////////////////////////////////////
    // Status

    showAxes = _FALSE;


    ////////////////////////////////////////////////////////////////////
    // Mouse motion sensitivity

    mouseMotionSensitivity = 1.0;


    ////////////////////////////////////////////////////////////////////
    // Status bar

    showStatusBar = _TRUE;
    resetStatusBar("Ready.");


    ////////////////////////////////////////////////////////////////////
    // background color

    backgndColor[0] = 255 ;
    backgndColor[1] = 255 ;
    backgndColor[2] = 255 ;


    ///////////////////////////////////////
    // no idle rotation or motion

    stopIdleMotion();
}


void
glInit()
{
    GLfloat ambient[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuse[]  = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat position[] = { 0.0f, 0.0f, 1.0f, 0.0f };

    ////////////////////////////////////////////////////////////////////
    // 1. Color and Lighting

    glClearColor( backgndColor[0]/255.0f, 
                  backgndColor[1]/255.0f, 
                  backgndColor[2]/255.0f, 0.0f );


    ////////////////////////////////////////////////////////////////////
    // 2. various status

    // may not work on all machines: improves TEXTURE specular shading
    #ifdef GL_VERSION_1_2
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
    #endif
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // Anti-aliasing
    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    glEnable(GL_LINE_SMOOTH);

    // Auto-Normalization
    //glEnable(GL_NORMALIZE);

    // Cull the back face (speedup and transparency)
    glCullFace( GL_BACK );


    ////////////////////////////////////////////////////////////////////
    // 3. set the projection and modelview

    resetView();


    ////////////////////////////////////////////////////////////////////
    // 4. Lighting

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glEnable(GL_LIGHT0);

    glPopMatrix();

	////////////////////////////////////////////////////////////////////
	// 5. Initialise grid
	
	xyGrid = startingGrid(XY_DIRECTION);
	yzGrid = startingGrid(YZ_DIRECTION);
	xzGrid = startingGrid(XZ_DIRECTION);

	////////////////////////////////////////////////////////////////////
	// 5. Initialise lineSet

	currentLineSet = newLineSet();

}


#ifdef WIN32

DWORD WINAPI
openHelpMessageBox()
{
    MessageBox(NULL,_HELP_MESSAGE,programName,MB_ICONINFORMATION|MB_SETFOREGROUND|MB_TASKMODAL);
    return 0;
}

DWORD WINAPI
openAboutMessageBox()
{
    MessageBox(NULL,aboutStr,programName,MB_SETFOREGROUND|MB_TASKMODAL);
    return 0;
}

#endif


void
myExit(int exitCode)
{
    ///////////////////////////////////////////
    // really quit?

    #ifdef WIN32

    int r = MessageBox(NULL,"Really Quit?",programName,MB_ICONEXCLAMATION|MB_YESNO|MB_DEFBUTTON2);
    if (r == IDNO)
	return;

    #endif

    exit(exitCode);
}


void
myHelp()
{
    // feedback
    #ifdef _WIN32

	HANDLE hThread;
	DWORD  IDThread;

	// Create a thread for the message box window
	hThread = CreateThread( NULL,						// no security attributes 
	                        0,						// use default stack size 
	                        (LPTHREAD_START_ROUTINE) openHelpMessageBox,	// thread function 
	                        NULL,						// no thread function argument 
	                        0,						// use default creation flags 
	                        &IDThread );					// returns thread identifier 
 
	// Check the return value for success.
	if (hThread == NULL) fprintf(stderr,"Error: creation menu.\n");

    #else

	fprintf(stderr,_HELP_MESSAGE);

    #endif
}


void
myAbout()
{
    #ifdef _WIN32

	HANDLE hThread;
	DWORD  IDThread;

	// Create a thread for the message box window
	hThread = CreateThread( NULL,						// no security attributes 
	                        0,						// use default stack size 
	                        (LPTHREAD_START_ROUTINE) openAboutMessageBox,	// thread function 
	                        NULL,						// no thread function argument 
	                        0,						// use default creation flags 
	                        &IDThread );					// returns thread identifier 

	// Check the return value for success.
	if (hThread == NULL) fprintf(stderr,"Error: creation menu.\n");

    #else

	// feedback
	resetStatusBar(PROGRAM_ABOUT_ONE_LINE);

    #endif
}




///////////////////////////////////////////////////////////////
// 2. Reset view / projection / Reshape
///////////////////////////////////////////////////////////////


void
resetProj()
{
	if (perspective)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(currFovy, currAspect, _Z_NEAR, _Z_FAR);
		glMatrixMode(GL_MODELVIEW);
	}
	else
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(1.0, currAspect, _Z_NEAR, _Z_FAR);
		glMatrixMode(GL_MODELVIEW);
	}
}


void
resetView()
{
    // reset projection matrix
    resetProj();

    // reset modelview matrix
	if (perspective)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0, -5.0f);
	}
	else
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0, -300.0f);
	}
    // stop idle motion
    stopIdleMotion();
}




///////////////////////////////////////////////////////////////
// 3. Display and Reshape Functions
///////////////////////////////////////////////////////////////


void
display(void)
{
    ////////////////////////////////////////////////////////////////////
    // (1) Normal Rendering

    {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the frame
	drawFrame();

	// Refresh
	glutSwapBuffers();
	glFlush();
    }
}


void
reshape(int w, int h)
{
    int tx,ty,tw,th;

    GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
    glViewport(tx,ty,tw,th);

    winW = tw;
    winH = th;

//    if (statusBar)
//	statusBar->set_w( winW-10 );

    if (showStatusBar)
	glViewport(0,25,tw,th);
    else
	glViewport(0,0,tw,th+25);

    currAspect = (float) tw / (float) th;

    resetProj();
}




///////////////////////////////////////////////////////////////
// 4. Auxillary Functions
///////////////////////////////////////////////////////////////

void
dumpScreen()
{
    static int seqNum = 0;

    GLint viewport[4],w,h;
    char  file[_MAX_PATH_SIZE];
    int   r,winID;
    

    // window ID
    winID = glutGetWindow();
    if (winID != mainWindowID)
	glutSetWindow(mainWindowID);

    // get sizes
    glGetIntegerv(GL_VIEWPORT, viewport);
    w = viewport[2];
    h = viewport[3];

    // write out image
    sprintf(file,"%s.%04d.ppm",DUMPSCREEN_FILE,seqNum++);

    // dump screen
    r = dumpScreenToFile ( file, NULL, GL_FRONT, 0, 25, w, h ) ;

    // any error?
    if (r == _ERROR)
	resetStatusBar("Failed to dump screen to file [%s].\n",file);
    else
	resetStatusBar("Screen dumped to file [%s].\n",file);

    // window ID
    if (winID != mainWindowID)
	glutSetWindow(winID);
}




///////////////////////////////////////////////////////////////
// Main Program
///////////////////////////////////////////////////////////////


void
usage(char *cmd)
{
    fprintf(stderr,"Usage : %s\n\n",cmd);
}


int
main(int argc, char **argv)
{
    ////////////////////////////////////////////////////////////////////
    // 1. Initialization

    // initialize various non-GL stuff
    myInit();


    ////////////////////////////////////////////////////////////////////
    // 2. GLUT Initialization

    // initialization
    glutInit(&argc,argv);
    glutInitWindowSize(winW, winH);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    // create a new window and give a title to it
    mainWindowID = glutCreateWindow(winTitle);

    // initialize OpenGL stuff
    glInit();


    ////////////////////////////////////////////////////////////////////
    // 3. GLUI Creation

    initGLUI();


    ////////////////////////////////////////////////////////////////////
    // 4. GLUT Callbacks

    glutDisplayFunc(display);

    GLUI_Master.set_glutReshapeFunc(reshape);
    GLUI_Master.set_glutMouseFunc(mouse);
    GLUI_Master.set_glutKeyboardFunc(keyboard);
    GLUI_Master.set_glutSpecialFunc(specKey);

    GLUI_Master.set_glutIdleFunc(idle);

    glutMotionFunc(motion);
	glutPassiveMotionFunc(passiveMotion);

    glutMainLoop();

    return(EXIT_SUCCESS);
}


// Note:
// -----
// If we are running in Windows non-console mode, the starting point for 
// for program is WinMain instead of main().

#if (!defined(_CONSOLE)) && defined(WIN32)

int WINAPI
WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, PSTR cmd, int showmode)
{
    char  *args[1024];			/* arbitrary limit, but should suffice */
    char  *p, *q, **argv = args;
    int    argc = 0;

    argv[argc++] = programName;
    p = cmd;
    for (;;) {
	if (*p == ' ')
	    while (*++p == ' ')
		;
	/* now p points at the first non-space after some spaces */
	if (*p == '\0')
	    break;    /* nothing after the spaces:  done */
	argv[argc++] = q = p;
	while (*q && *q != ' ')
	    ++q;
	/* now q points at a space or the end of the string */
	if (*q == '\0')
	    break;    /* last argv already terminated; quit */
	*q = '\0';    /* change space to terminator */
	p = q + 1;
    }
    argv[argc] = NULL;   /* terminate the argv array itself */

    return main(argc,argv);
}

#endif
