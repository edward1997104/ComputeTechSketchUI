///////////////////////////////////////////////////////////////
//
// glui_mouse.h
//
// - handle mouse event and idle callback
//
// by Philip Fu (cwfu@acm.org)
//
// 3/4/2003 2:22P
//
//
// 13/9/2006 make compatible with osx by lihw
//
// All rights reserved
// 
///////////////////////////////////////////////////////////////


#include <stdio.h>
#include <limits.h>

#ifdef WIN32
#include <windows.h>
#include <float.h>
#include <GL/gl.h>
#include <GL/glut.h>

#endif

#ifdef __linux
#include <values.h>
#endif

#if defined (__APPLE__) || defined(MACOSX) /* GLUT/ on Mac OS X */
#include <sys/time.h>
#include <sys/resource.h>
#include <OPENGL/gl.h>
#include <GLUT/glut.h>
#endif

#ifdef _WIN32
#include <sys/timeb.h>
#endif
#if !defined(_WIN32) && !defined (__APPLE__) && !defined(MACOSX)
#include <sys/resource.h>
#include <sys/times.h>
#endif

#include "grid.h"
#include "lines.h"
#include "glui.h"

#include "glui_build.h"
#include "glui_template.h"
#include "standard.h"




///////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////


  //////////////////////////////////////
  // Internal

  // mouse status
  int    mouseButton;
  int    mouseModifiers;
  int    preMouseX,preMouseY;
  GLint  preMouse2X,preMouse2Y;			// for idle rotation
  double mouseMotionSensitivity;		// sensitivity in mouse motion

  // idle rotation
  int    idleRotModel;				// rotate the model when idle?
  int    idleRotView;				// rotate the view - modify viewing vector?
  float  idleRot_nx,idleRot_ny,idleRot_nz;	// rotational axis
  float  idleRot_angle;				// angle left for the idle rotation (stop when it is less than zero)
  float  idleRot_speed;				// rotational speed (degree per second)
  int    idleRot_aroundOrigin;			// rotate around origin?
  double idleRot_time;				// time at which we previously make an idle rotation or start to do so
  double timeLastCallMotion;			// time at which we previously call motion func
  double currentMouseWorldPos[3];
  double roundedPos[3];


  //////////////////////////////////////
  // External

  // window
  extern int mainWindowID,winW,winH;

  // GLUI
  extern GLUI *glui;

  // viewing
  extern float currFovy;
  extern float currAspect;

  // selected variable
  extern int   xySelected;
  extern int   yzSelected;
  extern int   xzSelected;

  // external grid

  extern Grid_t * xyGrid;
  extern Grid_t * yzGrid;
  extern Grid_t * xzGrid;


///////////////////////////////////////////////////////////////
// Internal Functions
///////////////////////////////////////////////////////////////

// get world position in world coordinate
void
getMouseWorldPos(int x, int y, double * output)
  {
	  ///////////////////////////////////////////
	  // Function taking mouse position to calculate the world coordinate of mouse
	  // int x, int y: position of mouse
	  // double * output: output world position in 3 dimension (x, y, z)
	  //
	  // define the variable for ray casting to get world coordinate of mouse
	  GLint viewport[4];
	  GLdouble modelview[16];
	  GLdouble projection[16];
	  GLdouble winX, winY, winZ;
	  GLdouble posX_1, posY_1, posZ_1;
	  GLdouble posX_2, posY_2, posZ_2;

	  glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	  glGetDoublev(GL_PROJECTION_MATRIX, projection);
	  glGetIntegerv(GL_VIEWPORT, viewport);

	  winX = (double)x;


	  // hard-coded for tuning projection error
	  winY = (double)y + 25.0f;

	  //printf("windowY: %d\n", winH);

	  // unprojection of mouse in different z-depth
	  gluUnProject(winX, winY, 0.0, modelview, projection, viewport, &posX_1, &posY_1, &posZ_1);
	  gluUnProject(winX, winY, 1.0, modelview, projection, viewport, &posX_2, &posY_2, &posZ_2);

	  /*
	  printf("viewport: %d %d %d %d\n", viewport[0], viewport[1], viewport[2], viewport[3]);

	  printf("model view:\n");
	  printf("[%lf, %lf, %lf, %lf]\n", modelview[0], modelview[1], modelview[2], modelview[3]);
	  printf("[%lf, %lf, %lf, %lf]\n", modelview[4], modelview[5], modelview[6], modelview[7]);
	  printf("[%lf, %lf, %lf, %lf]\n", modelview[8], modelview[9], modelview[10], modelview[11]);
	  printf("[%lf, %lf, %lf, %lf]\n", modelview[12], modelview[13], modelview[14], modelview[15]);

	  printf("projection:\n");
	  printf("[%lf, %lf, %lf, %lf]\n", projection[0], projection[1], projection[2], projection[3]);
	  printf("[%lf, %lf, %lf, %lf]\n", projection[4], projection[5], projection[6], projection[7]);
	  printf("[%lf, %lf, %lf, %lf]\n", projection[8], projection[9], projection[10], projection[11]);
	  printf("[%lf, %lf, %lf, %lf]\n", projection[12], projection[13], projection[14], projection[15]);

	  printf("world coordinate 1: %lf %lf %lf\n", posX_1, posY_1, posZ_1);
	  printf("world coordinate 2: %lf %lf %lf\n", posX_2, posY_2, posZ_2);

	  */

	  // perform ray casting
	  double percent;
	  int flag = 1;

	  if (xySelected)
	     percent = (xyGrid->offset - posZ_1) / (posZ_2 - posZ_1);
	  else if (yzSelected)
		 percent = (yzGrid->offset - posX_1) / (posX_2 - posX_1);
	  else if (xzSelected)
		  percent = (xzGrid->offset - posY_1) / (posY_2 - posY_1);
	  else
		  flag = 0;

	  if (flag)
	  {

		  output[0] = posX_1 * (1 - percent) + posX_2 * percent;
		  output[1] = posY_1 * (1 - percent) + posY_2 * percent;
		  output[2] = posZ_1 * (1 - percent) + posZ_2 * percent;

		  //printf("world coordinate corrected: %lf %lf %lf\n", output[0], output[1], output[2]);
	  }

	  return;
  }

void
getProjectionVector(int x, int y, double outputVector[2][3])
{
	///////////////////////////////////////////
	// Function taking mouse position to calculate the world coordinate of mouse
	// int x, int y: position of mouse
	// double * output: output world position in 3 dimension (x, y, z)
	//
	// define the variable for ray casting to get world coordinate of mouse
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLdouble winX, winY, winZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (double)x;


	// hard-coded for tuning projection error
	winY = (double)y + 25.0f;

	//printf("windowY: %d\n", winH);

	// unprojection of mouse in different z-depth
	gluUnProject(winX, winY, 0.0, modelview, projection, viewport, &outputVector[0][0], &outputVector[0][1], &outputVector[0][2]);
	gluUnProject(winX, winY, 1.0, modelview, projection, viewport, &outputVector[1][0], &outputVector[1][1], &outputVector[1][2]);

}

void
getObjectWinPos(double x, double y, double z, double * output)
{
	///////////////////////////////////////////
	// Function taking world position to calculate the window position of the point
	// double x, double y, double z : position of an input point in world coordinate
	// double * output: output window position in (x, y) and z depth

	// define the variable for geting window position
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	// get the window value
	gluProject(x, y, z, modelview, projection, viewport, &output[0], &output[1], &output[2]);

	// correct error in projection
	output[1] = output[1] - 25.0;

	//printf("window position of point: %lf %lf %lf\n", output[0], output[1], output[2]);

}
// update the fovy

static void
updateFovyBy(double change)
{
    double x;

    #define _FOVY_K 1.0


    ///////////////////////////////////////////
    // Compute currFovy

    // Convert : f -> x
    if (currFovy < _FOVY_K)
	x = log10(currFovy) + _FOVY_K - log(_FOVY_K);
    else
	x = currFovy;

    // adding in the x space
    x += change;

    // Convert : x -> f
    if (x > 0.0) {
	if (x > 179.9)
	    x = 179.9;
    } else {
	x = pow(10.0,x-_FOVY_K+log(_FOVY_K));
	if (x < 1e-7)
	    x = 1e-7;
    }

    currFovy = x;
}


// It returns seconds elapsed since start of program, as a double

double
myTime()
{
  #ifdef _WIN32

    struct _timeb timebuffer;
    _ftime( &timebuffer );
    return (timebuffer.time + timebuffer.millitm * 0.001);

  #else

    struct rusage t;
    double procTime;

    /// (1) Get the rusage data structure at this moment
    getrusage(0,&t);

    // (2) What is the elapsed time?
    //     - CPU time = User time + system time

      // (2a) Get the seconds
      procTime = t.ru_utime.tv_sec + t.ru_stime.tv_sec;

      // (2b) More precisely! Get the microseconds too! :)
      procTime += (t.ru_utime.tv_usec + t.ru_stime.tv_usec) * 1e-6;

    // (3) Return the time!
    return procTime;

  #endif
}




///////////////////////////////////////////////////////////////
// Mouse Button and Motion Callback
///////////////////////////////////////////////////////////////


void
mouse ( int button, int state, int x, int y )
{	

    y = winH - 1 - y;


    /////////////////////////////////////////////////////////
    // since users might just use left/middle button to 
    // close the pop-up menu, we cannot simply use XOR
    // here to compute mouseButton, we have to use two 
    // cases here:

    mouseModifiers = glutGetModifiers();


    // since users might just use left/middle button to 
    // close the pop-up menu, we cannot simply use XOR
    // here to compute mouseButton, we have to use two 
    // cases here:
    if (state == GLUT_DOWN)
        mouseButton = mouseButton | (1<<(button)) ;
    else
        mouseButton = mouseButton & (~(1<<(button))) ;

    mouseModifiers = glutGetModifiers();

	// function for drawing line
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		printf("right button is clicked!\n");
		selectClosestPoint(x, y);
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		printf("left button is clicked!\n");
		selectLine(x, y);
	}
    // for idle rotation
    if (state == GLUT_DOWN) {

	////////////////////////////////////////////
	// no more idle rotation if any

	idleRotModel = _FALSE;
	idleRotView  = _FALSE;
	preMouse2X   = -1;

    } else {

	double diffTime;

	////////////////////////////////////////////
	// set idle rotation?

	diffTime = myTime() - timeLastCallMotion;

	if ( button == GLUT_LEFT_BUTTON
	  && mouseModifiers == 0
	  && mouseButton == 0
	  && diffTime < _IDLE_ROTATION_TIME_LIMIT
	  && preMouse2X != -1 )
	{
	    float scale,nx,ny;

	    nx = -(y - preMouse2Y) * mouseMotionSensitivity;
	    ny =  (x - preMouse2X) * mouseMotionSensitivity;

	    scale = sqrt(nx*nx + ny*ny);

	    if (diffTime <= 1e-3)
		diffTime  = 1e-3;

	    if (scale > 0.0f) {

		idleRot_speed = (scale * 0.2f) / diffTime / 100.0f;
		/*
		if (mouseModifiers & GLUT_ACTIVE_SHIFT)
		    idleRot_speed *= _SHIFT_ACCELERATION;
		if (mouseModifiers & GLUT_ACTIVE_CTRL)
		    idleRot_speed /= _SHIFT_ACCELERATION;
		*/

		// if speed is too small (smaller than 1 degree per second),
		// no need to do idle rotation
		if (idleRot_speed >= 1.0f) {

		    // stop any current idle rotation
		    idleRotView = _FALSE;

		    idleRotModel  = _TRUE;
		    idleRot_nx    = nx / scale;
		    idleRot_ny    = ny / scale;
		    idleRot_nz    = 0.0f;
		    idleRot_time  = myTime();
		    idleRot_angle = FLT_MAX;
		}
	    }
	}
    }

    // record preMouse position
    preMouseX = x;
    preMouseY = y;
	//getMouseWorldPos(preMouseX, preMouseY);
	//printf("preMouseX :%d, preMouseY: %d\n", preMouseX, preMouseY);
}



void
motion ( int x, int y )
{
    GLfloat mat[16];
    GLfloat nx,ny,tx,ty,tz,scale,angle,dx,dy,accelerator;

    int myMouseModifiers,winID;

    extern void resetProj() ;


    ///////////////////////////////////////////
    // Initialize mouse positions

    y = winH - 1 - y;

    if (preMouseX == x && preMouseY == y)
	return;

    dx = (x - preMouseX) * mouseMotionSensitivity;
    dy = (y - preMouseY) * mouseMotionSensitivity;

    // for idle rotation
    timeLastCallMotion = myTime();

    preMouse2X = preMouseX;
    preMouse2Y = preMouseY;

    preMouseX = x;
    preMouseY = y;


    ///////////////////////////////////////////
    // Make sure pointing to the mainWindow

    winID = glutGetWindow();
    if (winID != mainWindowID)
	glutSetWindow(mainWindowID);


    ///////////////////////////////////////////
    // General Mode

	/*
	fprintf(stderr,"mouseModifiers    = [%d]\n",mouseModifiers);
	fprintf(stderr,"GLUT_ACTIVE_SHIFT = [%d]\n",GLUT_ACTIVE_SHIFT);
	fprintf(stderr,"GLUT_ACTIVE_ALT   = [%d]\n",GLUT_ACTIVE_ALT);
	fprintf(stderr,"GLUT_ACTIVE_CTRL  = [%d]\n",GLUT_ACTIVE_CTRL);
	*/

    accelerator = 1.0f;
    if (mouseModifiers & GLUT_ACTIVE_SHIFT)
	accelerator *= _SHIFT_ACCELERATION;
    if (mouseModifiers & GLUT_ACTIVE_CTRL)		// SHIFT + CTRL will cancel each other
	accelerator *= (1.0f / _SHIFT_ACCELERATION);

    myMouseModifiers = mouseModifiers & (~GLUT_ACTIVE_SHIFT) & (~GLUT_ACTIVE_CTRL);


    switch (mouseButton) {


      ////////////////////////////////////////////////////////////
      // LEFT BUTTON (Control ROTATION)
      ////////////////////////////////////////////////////////////

      case 0x1:

	switch (myMouseModifiers) {


	  /////////////////////////////////////////
	  // 1) ROTATION AROUND THE VIEWPOINT

	  case GLUT_ACTIVE_ALT :

	    // Rotation
	    nx = -dy;
	    ny =  dx;
	    scale = sqrt(nx*nx + ny*ny);

	    if (scale > 0.0f) {

		if (accelerator > 1.0001f)
		resetStatusBar("rotation about the viewpoint (accelerated).");
		else
		if (accelerator < 0.9999f)
		resetStatusBar("rotation about the viewpoint (moderate).");
		else
		resetStatusBar("rotation about the viewpoint.");

		glGetFloatv(GL_MODELVIEW_MATRIX,mat);
		glLoadIdentity();

		nx    = nx / scale;
		ny    = ny / scale;
		angle = scale * 0.1f * currFovy/90.0f * accelerator;

		glRotatef(angle,nx,ny,0.0f);
		glMultMatrixf(mat);

		glutPostRedisplay();
	    }

	    break;


	  /////////////////////////////////////////
	  // 2) ROTATING THE MODEL

	  default :

	    // Rotation
	    nx = -dy;
	    ny =  dx;
	    scale = sqrt(nx*nx + ny*ny);

	    if (scale > 0.0f) {

		if (accelerator > 1.0001f)
		resetStatusBar("arcball rotation about the model (accelerated).");
		else
		if (accelerator < 0.9999f)
		resetStatusBar("arcball rotation about the model (moderate).");
		else
		resetStatusBar("arcball rotation about the model.");

		glGetFloatv(GL_MODELVIEW_MATRIX,mat);
		glLoadIdentity();

		nx    = nx / scale;
		ny    = ny / scale;
		angle = scale * 0.2f * accelerator;

		glTranslatef(mat[12],mat[13],mat[14]);
		glRotatef(angle,nx,ny,0.0f); 
		glTranslatef(-mat[12],-mat[13],-mat[14]);
		glMultMatrixf(mat);

		glutPostRedisplay();
	    }
	}

	break;


      ////////////////////////////////////////////////////////////
      // MIDDLE BUTTON (Control TRANSLATION AND FOVY)
      ////////////////////////////////////////////////////////////

      case 0x2:

	switch (myMouseModifiers) {


	  /////////////////////////////////////////
	  // 1) CHANGE FOVY

	  case GLUT_ACTIVE_ALT :

	    if (accelerator > 1.0001f)
	    resetStatusBar("changing fovy (accelerated).");
	    else
	    if (accelerator < 0.9999f)
	    resetStatusBar("changing fovy (moderate).");
	    else
	    resetStatusBar("changing fovy.");

	    // update fovy
	    updateFovyBy(dx*10.0f*accelerator/((double)winH));

	    // update Projection
	    resetProj();

	    // sync currFovy for GLUI
	    glui->sync_live();

	    glutPostRedisplay();
	    break;


	  /////////////////////////////////////////
	  // 2) TRANSLATING (XY)

	  default :

	    if (accelerator > 1.0001f)
	    resetStatusBar("XY translating the model (accelerated).");
	    else
	    if (accelerator < 0.9999f)
	    resetStatusBar("XY translating the model (moderate).");
	    else
	    resetStatusBar("XY translating the model.");

	    // Translation XY
	    tx = 0.05f * dx * accelerator;
	    ty = 0.05f * dy * accelerator;

	    glGetFloatv(GL_MODELVIEW_MATRIX,mat);
	    glLoadIdentity();
	    glTranslatef(tx, ty, 0.0f);
	    glMultMatrixf(mat);

	    glutPostRedisplay();
	    break;
	}

	break;


      ////////////////////////////////////////////////////////////
      // LEFT+MIDDLE BUTTON (Control TRANSLATION AND FOVY)
      ////////////////////////////////////////////////////////////

      case 0x3:

	/////////////////////////////////////////
	// 1) TRANSLATING (XZ)

	if (accelerator > 1.0001f)
	resetStatusBar("XZ translating the model (accelerated).");
	else
	if (accelerator < 0.9999f)
	resetStatusBar("XZ translating the model (moderate).");
	else
	resetStatusBar("XZ translating the model.");

	tx =  0.05f * dx * accelerator;
	tz = -0.05f * dy * accelerator;

	glGetFloatv(GL_MODELVIEW_MATRIX,mat);
	glLoadIdentity();
	glTranslatef(tx, 0.0f, tz);
	glMultMatrixf(mat);

	glutPostRedisplay();
	break;
    }


    if (winID != mainWindowID)
	glutSetWindow(winID);
}

///////////////////////////////////////////////////////////////
// Passive function for Grid Snapping and Drawing
///////////////////////////////////////////////////////////////

void
passiveMotion(int x, int y)
{
	// change back to upright corner
	int temp_y = winH - 1 - y;

	// get the mosue position in the world coordinate
	getMouseWorldPos(x, temp_y, currentMouseWorldPos);


	/*
	double resamplePos[3];
	getObjectWinPos(currentMouseWorldPos[0], currentMouseWorldPos[1], currentMouseWorldPos[2], resamplePos);

	*/

	
	if (xySelected)
	{
		if (xyGrid->snapFlag)
		{
			if (myTime() - xyGrid->previosuSnapTime < SNAP_TIME)
			{
				//printf("different in time: %lf", myTime() - xyGrid->previosuSnapTime < SNAP_TIME);
				double resamplePos[3];
				getObjectWinPos(xyGrid->previousSnapPoint[0], xyGrid->previousSnapPoint[1], xyGrid->previousSnapPoint[2], resamplePos);
				resamplePos[1] = round(winH - 1 - resamplePos[1]);
				glutWarpPointer((int)resamplePos[0], (int)resamplePos[1]);
				return;
			}
		}

		xyGrid->snapFlag = 0;

		getClosestPoint(currentMouseWorldPos[0], currentMouseWorldPos[1], currentMouseWorldPos[2], roundedPos);

		double nearestDis = distance(currentMouseWorldPos, roundedPos);
		//printf("distance from grid: %lf\n",  nearestDis);

		if (nearestDis < SNAP_THERSHOLD && distance(xyGrid->previousSnapPoint, currentMouseWorldPos) > SNAP_THERSHOLD)
		{
			double resamplePos[3];
			getObjectWinPos(roundedPos[0], roundedPos[1], roundedPos[2], resamplePos);
			xyGrid->previousSnapPoint[0] = roundedPos[0];
			xyGrid->previousSnapPoint[1] = roundedPos[1];
			xyGrid->previousSnapPoint[2] = roundedPos[2];

			resamplePos[1] = round(winH - 1 - resamplePos[1]);

			glutWarpPointer((int)resamplePos[0], (int)resamplePos[1]);
			xyGrid->previosuSnapTime = myTime();
			xyGrid->snapFlag = 1;
			return;
		}

	}
	else if (yzSelected)
	{
		if (xyGrid->snapFlag)
		{
			if (myTime() - yzGrid->previosuSnapTime < SNAP_TIME)
			{
				//printf("different in time: %lf", myTime() - yzGrid->previosuSnapTime < SNAP_TIME);
				double resamplePos[3];
				getObjectWinPos(yzGrid->previousSnapPoint[0], yzGrid->previousSnapPoint[1], yzGrid->previousSnapPoint[2], resamplePos);
				resamplePos[1] = round(winH - 1 - resamplePos[1]);
				glutWarpPointer((int)resamplePos[0], (int)resamplePos[1]);
				return;
			}
		}

		yzGrid->snapFlag = 0;

		getClosestPoint(currentMouseWorldPos[0], currentMouseWorldPos[1], currentMouseWorldPos[2], roundedPos);

		double nearestDis = distance(currentMouseWorldPos, roundedPos);
		//printf("distance from grid: %lf\n", nearestDis);

		if (nearestDis < SNAP_THERSHOLD && distance(yzGrid->previousSnapPoint, currentMouseWorldPos) > SNAP_THERSHOLD)
		{
			double resamplePos[3];
			getObjectWinPos(roundedPos[0], roundedPos[1], roundedPos[2], resamplePos);
			yzGrid->previousSnapPoint[0] = roundedPos[0];
			yzGrid->previousSnapPoint[1] = roundedPos[1];
			yzGrid->previousSnapPoint[2] = roundedPos[2];
			
			resamplePos[1] = round(winH - 1 - resamplePos[1]);

			glutWarpPointer((int)resamplePos[0], (int)resamplePos[1]);

			yzGrid->previosuSnapTime = myTime();
			yzGrid->snapFlag = 1;

			return;
		}
	}
	else if (xzSelected)
	{

		if (xzGrid->snapFlag)
		{
			if (myTime() - xzGrid->previosuSnapTime < SNAP_TIME)
			{
				//printf("different in time: %lf", myTime() - xzGrid->previosuSnapTime < SNAP_TIME);
				double resamplePos[3];
				getObjectWinPos(xzGrid->previousSnapPoint[0], xzGrid->previousSnapPoint[1], xzGrid->previousSnapPoint[2], resamplePos);
				resamplePos[1] = round(winH - 1 - resamplePos[1]);
				glutWarpPointer((int)resamplePos[0], (int)resamplePos[1]);
				return;
			}
		}

		yzGrid->snapFlag = 0;

		getClosestPoint(currentMouseWorldPos[0], currentMouseWorldPos[1], currentMouseWorldPos[2], roundedPos);

		double nearestDis = distance(currentMouseWorldPos, roundedPos);
		//printf("distance from grid: %lf\n", nearestDis);

		if (nearestDis < SNAP_THERSHOLD && distance(xzGrid->previousSnapPoint, currentMouseWorldPos) > SNAP_THERSHOLD)
		{
			double resamplePos[3];
			getObjectWinPos(roundedPos[0], roundedPos[1], roundedPos[2], resamplePos);
			xzGrid->previousSnapPoint[0] = roundedPos[0];
			xzGrid->previousSnapPoint[1] = roundedPos[1];
			xzGrid->previousSnapPoint[2] = roundedPos[2];

			resamplePos[1] = round(winH - 1 - resamplePos[1]);

			glutWarpPointer((int)resamplePos[0], (int)resamplePos[1]);

			xzGrid->previosuSnapTime = myTime();
			xzGrid->snapFlag = 1;

			return;
		}
	}

	glutPostRedisplay();

}

///////////////////////////////////////////////////////////////
// Idle Callback
///////////////////////////////////////////////////////////////


void
idle ( void )
{
    ////////////////////////////////////////////////////////////////////
    // If GLUI is used, need to reset window ID

    if ( glutGetWindow() != mainWindowID )
        glutSetWindow(mainWindowID);


    ////////////////////////////////////////////////////////////////////
    // Idle Rotation

    if (idleRotModel) {

	float  mat[16];
	double currTime = myTime();

	// modify the current MV matrix
	{
	    glGetFloatv(GL_MODELVIEW_MATRIX,mat);
	    glLoadIdentity();

	    glTranslatef(mat[12],mat[13],mat[14]);
	    glRotatef( idleRot_speed*(currTime-idleRot_time),
	               idleRot_nx, idleRot_ny, 0.0f );
	    glTranslatef(-mat[12],-mat[13],-mat[14]);
	    glMultMatrixf(mat);

	    glutPostRedisplay();
	}

	// update the timer
	idleRot_time = currTime;

	//fprintf(stderr,"idle rotate by axis (%f,%f,%f) and angle %f\n",
	//    idleRot_nx, idleRot_ny, 0.0f, idleRot_speed*(currTime-idleRot_time) );

    }
    else
    if (idleRotView) {

	double mvmat[16];
	double currTime = myTime();

	// modify the current MV matrix
	if (idleRot_angle > 0.0) {

	    float angle = _MIN( idleRot_angle, idleRot_speed*(currTime-idleRot_time) );

	    glGetDoublev(GL_MODELVIEW_MATRIX,mvmat);

	    // update update view
	    glLoadIdentity();
	    if (idleRot_aroundOrigin) glTranslated(mvmat[12],mvmat[13],mvmat[14]);
	    glRotatef( angle, idleRot_nx, idleRot_ny, idleRot_nz );
	    if (idleRot_aroundOrigin) glTranslated(-mvmat[12],-mvmat[13],-mvmat[14]);

	    glMultMatrixd(mvmat);

	    glutPostRedisplay();

	    // we have rotated this amount of degree
	    idleRot_angle -= angle;

	} else
	    idleRotView = _FALSE;

	// update the timer
	idleRot_time = currTime;

    } else
	if (preMouse2X != -1)
	    preMouse2X  = -1;		// preMouse2X serves as a flag
}


void
stopIdleMotion()
{
    idleRotModel = _FALSE ;
    idleRotView  = _FALSE ;
    preMouse2X   = -1     ;
}
