///////////////////////////////////////////////////////////////
//
// glui_draw.cpp
//
// - drawing routines
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
#include <vector>
#include <GL/glut.h>

#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
#endif

#ifdef __APPLE__
#include <OPENGL/gl.h>
#endif // __APPLE__

#include "glui_template.h"
#include "grid.h"
#include "lines.h"
#include "glui_build.h"
#include "glui.h"

///////////////////////////////////////////////////////////////
// EXTERNAL VARIABLES
///////////////////////////////////////////////////////////////

  extern int showAxes ;

  extern int selectedFlag;
  extern Point_t * prevPoint;

  extern int xySelected;
  extern int yzSelected;
  extern int xzSelected;
    
  extern Grid_t * xyGrid;
  extern Grid_t * yzGrid;
  extern Grid_t * xzGrid;

  extern LineSet_t * currentLineSet;

  extern double currentMouseWorldPos[3];
  extern double roundedPos[3];

  extern GLUI_Listbox * lineSetListbox;

///////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
///////////////////////////////////////////////////////////////

  const float LINE_WIDTH = 4.0f;
  const float GRID_WIDTH = 2.0f;
  const float SELECTED_SPHERE_SIZE = 0.05f;
  const float POSSIBLE_SPHERE_SIZE = 0.02f;

static void
drawAxes()
{
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
	glColor3f(1.0f,0.0f,0.0f);
	glVertex3f(GRID_SIZE, 0.0f, 0.0f);
	glVertex3f(0.0f,0.0f,0.0f);

	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(0.0f, GRID_SIZE, 0.0f);
	glVertex3f(0.0f,0.0f,0.0f);

	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(0.0f, 0.0f, GRID_SIZE);
	glVertex3f(0.0f,0.0f,0.0f);
    glEnd();
    glEnable(GL_LIGHTING);
}

static void
drawPlane(int plane)
{
	int counter = 0;

	switch (plane)
	{

	///////////////////////////////////////////////////////////////
	// show selected xy plane

	case XY_SELECTED:
	glDisable(GL_LIGHTING);
    /*glBegin(GL_QUADS);

	glColor3f(0.8f,0.0f,0.0f);

	glVertex3f(-SELECTED_PLANE_SIZE, -SELECTED_PLANE_SIZE, xyOffset);
	glVertex3f(SELECTED_PLANE_SIZE, -SELECTED_PLANE_SIZE, xyOffset);
	glVertex3f(SELECTED_PLANE_SIZE, SELECTED_PLANE_SIZE, xyOffset);
	glVertex3f(-SELECTED_PLANE_SIZE, SELECTED_PLANE_SIZE, xyOffset);

    glEnd();*/

	///////////////////////////////////////////////////////////////
	// show selected xy plane grid with interval delta


	for (float current_line = -GRID_SIZE;
		current_line <= GRID_SIZE + 10e-5;
		current_line += GRID_INTERVAL)
	{
		if (counter % 5 == 0)
			glLineWidth(GRID_WIDTH);
		glBegin(GL_LINES);
		glColor4f(0.8f, 0.0f, 0.0f, 0.3f);
		glVertex3f(current_line, GRID_SIZE, xyGrid->offset);
		glVertex3f(current_line, -GRID_SIZE, xyGrid->offset);

		glColor4f(0.8f, 0.0f, 0.0f, 0.3f);
		glVertex3f(GRID_SIZE, current_line, xyGrid->offset);
		glVertex3f(-GRID_SIZE, current_line, xyGrid->offset);
		glEnd();
		if (counter % 5 == 0)
			glLineWidth(1.0);
		counter++;
	}


    glEnable(GL_LIGHTING);
	break;

	///////////////////////////////////////////////////////////////
	// show selected yz plane

	case YZ_SELECTED:
	glDisable(GL_LIGHTING);
    /*glBegin(GL_QUADS);

	glColor3f(0.0f,0.8f,0.0f);

	glVertex3f(yzOffset, -SELECTED_PLANE_SIZE, -SELECTED_PLANE_SIZE);
	glVertex3f(yzOffset, -SELECTED_PLANE_SIZE, SELECTED_PLANE_SIZE);
	glVertex3f(yzOffset, SELECTED_PLANE_SIZE, SELECTED_PLANE_SIZE);
	glVertex3f(yzOffset, SELECTED_PLANE_SIZE, -SELECTED_PLANE_SIZE);


    glEnd();*/

	///////////////////////////////////////////////////////////////
	// show selected yz plane grid with interval delta

	for (float current_line = -GRID_SIZE;
		current_line <= GRID_SIZE + 10e-5;
		current_line += GRID_INTERVAL)
	{
		if (counter % 5 == 0)
			glLineWidth(GRID_WIDTH);
		glBegin(GL_LINES);
		glColor4f(0.0f, 0.8f, 0.0f, 0.3f);
		glVertex3f(yzGrid->offset, GRID_SIZE, current_line);
		glVertex3f(yzGrid->offset, -GRID_SIZE, current_line);

		glColor4f(0.0f, 0.8f, 0.0f, 0.3f);
		glVertex3f(yzGrid->offset, current_line, GRID_SIZE);
		glVertex3f(yzGrid->offset, current_line, -GRID_SIZE);
		glEnd();
		if (counter % 5 == 0)
			glLineWidth(1.0);
		counter++;

	}

    glEnable(GL_LIGHTING);
	break;

	///////////////////////////////////////////////////////////////
	// show selected xz plane

	case XZ_SELECTED:
	glDisable(GL_LIGHTING);
    /*glBegin(GL_QUADS);

	glColor3f(0.0f,0.0f,0.8f);

	glVertex3f(-SELECTED_PLANE_SIZE, xzOffset, -SELECTED_PLANE_SIZE);
	glVertex3f(SELECTED_PLANE_SIZE, xzOffset, -SELECTED_PLANE_SIZE);
	glVertex3f(SELECTED_PLANE_SIZE, xzOffset, SELECTED_PLANE_SIZE);
	glVertex3f(-SELECTED_PLANE_SIZE, xzOffset, SELECTED_PLANE_SIZE);


    glEnd();*/

	///////////////////////////////////////////////////////////////
	// show selected xz plane grid with interval delta


	for (float current_line = -GRID_SIZE;
		current_line <= GRID_SIZE + 10e-5;
		current_line += GRID_INTERVAL)
	{
		if (counter % 5 == 0)
			glLineWidth(GRID_WIDTH);
		glBegin(GL_LINES);

		glColor4f(0.0f, 0.0f, 0.8f, 0.3f);
		glVertex3f(-GRID_SIZE, xzGrid->offset, current_line);
		glVertex3f(GRID_SIZE, xzGrid->offset, current_line);

		glColor4f(0.0f, 0.0f, 0.8f, 0.3f);
		glVertex3f(current_line, xzGrid->offset, GRID_SIZE);
		glVertex3f(current_line, xzGrid->offset, -GRID_SIZE);

		glEnd();
		if (counter % 5 == 0)
			glLineWidth(1.0);
		counter++;

	}
    glEnable(GL_LIGHTING);
	break;

	}
}

// draw the selected plane
static void drawSelectedPlane()
{
	if (xySelected) drawPlane(XY_SELECTED);
	if (yzSelected) drawPlane(YZ_SELECTED);
	if (xzSelected) drawPlane(XZ_SELECTED);
}

static void
drawLineSet()
{
	glDisable(GL_LIGHTING);
	glLineWidth(LINE_WIDTH);
	glBegin(GL_LINES);


	double delta = 0.001;
	for (unsigned int i = 0; i < currentLineSet->lineSet.size(); i++)
	{
		if (currentLineSet->lineSet[i]->id != lineSetListbox->int_val)
			glColor3f(0.0f, 0.0f, 0.0f);
		else
			glColor3f(1.0f, 0.843f, 0.0f);
		glVertex3f(currentLineSet->lineSet[i]->endpoint[0]->x + delta, currentLineSet->lineSet[i]->endpoint[0]->y, currentLineSet->lineSet[i]->endpoint[0]->z);
		glVertex3f(currentLineSet->lineSet[i]->endpoint[1]->x + delta, currentLineSet->lineSet[i]->endpoint[1]->y, currentLineSet->lineSet[i]->endpoint[1]->z);

		glVertex3f(currentLineSet->lineSet[i]->endpoint[0]->x - delta, currentLineSet->lineSet[i]->endpoint[0]->y, currentLineSet->lineSet[i]->endpoint[0]->z);
		glVertex3f(currentLineSet->lineSet[i]->endpoint[1]->x - delta, currentLineSet->lineSet[i]->endpoint[1]->y, currentLineSet->lineSet[i]->endpoint[1]->z);

		glVertex3f(currentLineSet->lineSet[i]->endpoint[0]->x, currentLineSet->lineSet[i]->endpoint[0]->y + delta, currentLineSet->lineSet[i]->endpoint[0]->z);
		glVertex3f(currentLineSet->lineSet[i]->endpoint[1]->x, currentLineSet->lineSet[i]->endpoint[1]->y + delta, currentLineSet->lineSet[i]->endpoint[1]->z);

		glVertex3f(currentLineSet->lineSet[i]->endpoint[0]->x, currentLineSet->lineSet[i]->endpoint[0]->y - delta, currentLineSet->lineSet[i]->endpoint[0]->z);
		glVertex3f(currentLineSet->lineSet[i]->endpoint[1]->x, currentLineSet->lineSet[i]->endpoint[1]->y - delta, currentLineSet->lineSet[i]->endpoint[1]->z);

		glVertex3f(currentLineSet->lineSet[i]->endpoint[0]->x, currentLineSet->lineSet[i]->endpoint[0]->y, currentLineSet->lineSet[i]->endpoint[0]->z + delta);
		glVertex3f(currentLineSet->lineSet[i]->endpoint[1]->x, currentLineSet->lineSet[i]->endpoint[1]->y, currentLineSet->lineSet[i]->endpoint[1]->z + delta);

		glVertex3f(currentLineSet->lineSet[i]->endpoint[0]->x, currentLineSet->lineSet[i]->endpoint[0]->y, currentLineSet->lineSet[i]->endpoint[0]->z - delta);
		glVertex3f(currentLineSet->lineSet[i]->endpoint[1]->x, currentLineSet->lineSet[i]->endpoint[1]->y, currentLineSet->lineSet[i]->endpoint[1]->z - delta);
	}

	glEnd();
	glLineWidth(1.0);
	glEnable(GL_LIGHTING);

	for (unsigned int i = 0; i < currentLineSet->lineSet.size(); i++)
	{
		for (unsigned int j = 0; j < currentLineSet->lineSet[i]->intPoint.size(); j++)
		{
			glDisable(GL_LIGHTING);
			glColor3f(0.902f, 0.902f, 0.980f);
			glPushMatrix();
			glTranslated(currentLineSet->lineSet[i]->intPoint[j]->x,
				currentLineSet->lineSet[i]->intPoint[j]->y,
				currentLineSet->lineSet[i]->intPoint[j]->z);
			glutSolidSphere(POSSIBLE_SPHERE_SIZE, 50, 50);
			glPopMatrix();
			glEnable(GL_LIGHTING);
		}
	}

}

void drawSelectedPoint()
{
	if (selectedFlag)
	{
		glPushMatrix();
		glTranslated(prevPoint->x, prevPoint->y, prevPoint->z);
		glutSolidSphere(SELECTED_SPHERE_SIZE, 50, 50);
		glPopMatrix();

		glDisable(GL_LIGHTING);
		glLineWidth(LINE_WIDTH);
		glBegin(GL_LINES);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(prevPoint->x, prevPoint->y, prevPoint->z);
		glVertex3f(roundedPos[0], roundedPos[1], roundedPos[2]);

		glEnd();
		glLineWidth(1.0);
		glEnable(GL_LIGHTING);
	}
}
///////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS FOR OUTSIDE WORLD
///////////////////////////////////////////////////////////////

void
drawFrame()
{
    // draw axes
    if (showAxes) drawAxes();

	// draw plane
	drawSelectedPlane();

	drawLineSet();

	drawSelectedPoint();
}
