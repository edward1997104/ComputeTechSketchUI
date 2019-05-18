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
#include <SOIL.h>

#define DO_NOT_SELECT

///////////////////////////////////////////////////////////////
// EXTERNAL VARIABLES
///////////////////////////////////////////////////////////////

  extern int showAxes ;

  extern int selectedFlag;
  extern Point_t * prevPoint;

  extern int xySelected;
  extern int yzSelected;
  extern int xzSelected;
  
  extern int innerLighting;

  extern Grid_t * xyGrid;
  extern Grid_t * yzGrid;
  extern Grid_t * xzGrid;

  extern LineSet_t * currentLineSet;

  extern double currentMouseWorldPos[3];
  extern double roundedPos[3];

  extern GLUI_Listbox * lineSetListbox;

  extern unsigned int ANNOT_COUNT;

///////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
///////////////////////////////////////////////////////////////

  const float LINE_WIDTH = 4.0f;
  const float GRID_WIDTH = 2.0f;
  const float SELECTED_SPHERE_SIZE = 0.05f;
  const float POSSIBLE_SPHERE_SIZE = 0.02f;
  GLuint rotation_tex = 0;

  std::vector<Point_t> drawnQuads;

void setRotationGridTexture()
  {
	  GLuint tex_2d = SOIL_load_OGL_texture
		  (
		  "arrow.png",
		  SOIL_LOAD_AUTO,
		  SOIL_CREATE_NEW_ID,
		  SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		  );
	  /* check for an error during the load process */
	  if (0 == tex_2d)
	  {
		  printf("SOIL loading error: '%s'\n", SOIL_last_result());
	  }
	  else
	  {
		  printf("%s\n", SOIL_last_result());
	  }
	  rotation_tex = tex_2d;
  }
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

static void drawPlane(int plane)
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


	//for (float current_line = -GRID_SIZE;
	//	current_line <= GRID_SIZE + 10e-5;
	//	current_line += GRID_INTERVAL)
	//{
	//	if (counter % 5 == 0)
	//		glLineWidth(GRID_WIDTH);
	//	glBegin(GL_LINES);

	//	float alpha = 0.4*(GRID_SIZE-abs(current_line)) / GRID_SIZE;// exp()

	//	glColor4f(0.8f, 0.0f, 0.0f, alpha);
	//	glVertex3f(current_line, GRID_SIZE, xyGrid->offset);
	//	glVertex3f(current_line, -GRID_SIZE, xyGrid->offset);

	//	glColor4f(0.8f, 0.0f, 0.0f, alpha);
	//	glVertex3f(GRID_SIZE, current_line, xyGrid->offset);
	//	glVertex3f(-GRID_SIZE, current_line, xyGrid->offset);
	//	glEnd();
	//	if (counter % 5 == 0)
	//		glLineWidth(1.0);
	//	counter++;
	//}
    //#define GRID_NUM 40
	

	for (int x = -GRID_NUM; x <= GRID_NUM; x++)
	{

		for (int y = -GRID_NUM; y <= GRID_NUM; y++)
		{
			float baseX = x * GRID_INTERVAL;
			float baseY = y * GRID_INTERVAL;

			float alpha = 0.4 * exp(- (baseX*baseX + baseY*baseY) / (GRID_NUM/3.5));// exp()


			glLineWidth(1.0);
			if (abs(y) % 5 == 0)	glLineWidth(2.0);

			glBegin(GL_LINES);

			if (x != GRID_NUM){
				//horizontal line
				glColor4f(0.8f, 0.0f, 0.0f, alpha);
				glVertex3f(baseX, baseY, xyGrid->offset);
				glVertex3f(baseX + GRID_INTERVAL, baseY, xyGrid->offset);
			}
			glEnd();

			glLineWidth(1.0);
			if (abs(x) % 5 == 0)	glLineWidth(2.0);

			glBegin(GL_LINES);

			
			if (y != GRID_NUM){
				//vertical line
				glColor4f(0.8f, 0.0f, 0.0f, alpha);
				glVertex3f(baseX, baseY, xyGrid->offset);
				glVertex3f(baseX, baseY + GRID_INTERVAL, xyGrid->offset);
			}
			glEnd();
		}
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

	//for (float current_line = -GRID_SIZE;
	//	current_line <= GRID_SIZE + 10e-5;
	//	current_line += GRID_INTERVAL)
	//{
	//	if (counter % 5 == 0)
	//		glLineWidth(GRID_WIDTH);
	//	glBegin(GL_LINES);
	//	glColor4f(0.0f, 0.8f, 0.0f, 0.3f);
	//	glVertex3f(yzGrid->offset, GRID_SIZE, current_line);
	//	glVertex3f(yzGrid->offset, -GRID_SIZE, current_line);

	//	glColor4f(0.0f, 0.8f, 0.0f, 0.3f);
	//	glVertex3f(yzGrid->offset, current_line, GRID_SIZE);
	//	glVertex3f(yzGrid->offset, current_line, -GRID_SIZE);
	//	glEnd();
	//	if (counter % 5 == 0)
	//		glLineWidth(1.0);
	//	counter++;

	//}

	for (int x = -GRID_NUM; x <= GRID_NUM; x++)
	{

		for (int y = -GRID_NUM; y <= GRID_NUM; y++)
		{
			float baseX = x * GRID_INTERVAL;//y coord
			float baseY = y * GRID_INTERVAL;//z coord

			float alpha = 0.4 * exp(-(baseX*baseX + baseY*baseY) / (GRID_NUM / 3.5));// exp()


			glLineWidth(1.0);
			if (abs(y) % 5 == 0)	glLineWidth(2.0);

			glBegin(GL_LINES);

			if (x != GRID_NUM){
				//horizontal line
				glColor4f(0.0f, 0.8f, 0.0f, alpha);
				glVertex3f(yzGrid->offset,baseX, baseY );
				glVertex3f(yzGrid->offset, baseX + GRID_INTERVAL, baseY);
			}
			glEnd();

			glLineWidth(1.0);
			if (abs(x) % 5 == 0)	glLineWidth(2.0);

			glBegin(GL_LINES);


			if (y != GRID_NUM){
				//vertical line
				glColor4f(0.0f, 0.8f, 0.0f, alpha);
				glVertex3f(yzGrid->offset,baseX, baseY);
				glVertex3f(yzGrid->offset, baseX, baseY + GRID_INTERVAL);
			}
			glEnd();
		}
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


	/*for (float current_line = -GRID_SIZE;
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

	}*/
	for (int x = -GRID_NUM; x <= GRID_NUM; x++)
	{

		for (int y = -GRID_NUM; y <= GRID_NUM; y++)
		{
			float baseX = x * GRID_INTERVAL;
			float baseY = y * GRID_INTERVAL;

			float alpha = 0.4 * exp(-(baseX*baseX + baseY*baseY) / (GRID_NUM / 3.5));// exp()

			glLineWidth(1.0);
			if (abs(y) % 5 == 0)	glLineWidth(2.0);

			glBegin(GL_LINES);

			if (x != GRID_NUM){
				//horizontal line
				glColor4f(0.0f, 0.0f, 0.8f, alpha);
				glVertex3f(baseX, xzGrid->offset, baseY);
				glVertex3f(baseX + GRID_INTERVAL, xzGrid->offset, baseY);
			}
			glEnd();

			glLineWidth(1.0);
			if (abs(x) % 5 == 0)	glLineWidth(2.0);

			glBegin(GL_LINES);


			if (y != GRID_NUM){
				//vertical line
				glColor4f(0.0f, 0.0f, 0.8f, alpha);
				glVertex3f(baseX, xzGrid->offset,baseY);
				glVertex3f(baseX, xzGrid->offset, baseY + GRID_INTERVAL);
			}
			glEnd();
		}
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
#ifdef DO_NOT_SELECT
		glColor3f(0.0f, 0.0f, 0.0f);
#endif
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

	if (innerLighting){
		for (unsigned int i = 0; i < currentLineSet->lineSet.size(); i++)
		{
			for (unsigned int j = 0; j < currentLineSet->lineSet[i]->intPoint.size(); j++)
			{
				glDisable(GL_LIGHTING);
				{//mahou
					auto absDiff = [](float x, float y) -> float{return fabsf(x - y); };
					bool switchColor = false;
					const auto &currentPoint = *(currentLineSet->lineSet[i]->intPoint[j]);
					for (const auto &point : drawnQuads){
						if (absDiff(point.x, currentPoint.x) + absDiff(point.y, currentPoint.y) + absDiff(point.z, currentPoint.z) < 0.011f){
							switchColor = true;
							break;
						}
					}
					if (switchColor) glColor3f(0.0f, 1.0f, 0.0f);
					else glColor3f(0.802f, 0.802f, 0.880f);
				}
				glPushMatrix();
				glTranslated(currentLineSet->lineSet[i]->intPoint[j]->x,
					currentLineSet->lineSet[i]->intPoint[j]->y,
					currentLineSet->lineSet[i]->intPoint[j]->z);
				glutSolidSphere(POSSIBLE_SPHERE_SIZE, 40, 40);
				glPopMatrix();
				glEnable(GL_LIGHTING);
			}
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

void drawRotationQuad(float x, float y, float z, float size, int axis)
{
	// axis : X : 0 , Y : 1 , Z: 2
	x*=0.1;
	y*=0.1;
	z*=0.1;
	//glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, rotation_tex);
	GLfloat lmodel_ambient[] = { 10.0, 10.0, 10.0, 10.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glDepthMask( GL_FALSE );
	glBegin(GL_QUADS);

	const Point_t tmpPoint = { x, y, z };
	drawnQuads.push_back(tmpPoint);

	if (axis == 0)
	{
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x, y - size, z - size);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x, y - size, z + size);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x, y + size, z + size);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x, y + size, z - size);
	}
	else if (axis == 1)
	{
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x - size, y, z - size);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x - size, y, z + size);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x + size, y, z + size);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x + size, y, z - size);
	}
	else if (axis == 2)
	{
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x - size, y - size, z);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x - size, y + size, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x + size, y + size, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x + size, y - size, z);
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	//glEnable(GL_LIGHTING);     // 启动光照
	glDisable(GL_TEXTURE_2D);
	glDepthMask( GL_TRUE );

}
///////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS FOR OUTSIDE WORLD
///////////////////////////////////////////////////////////////

//#define ANNOT_BIRD
//#define ANNOT_CROSSBOW
//#define ANNOT_CLAW
#define ANNOT_CHAIR_2
//#define ANNOT_SEASAW
//#define ANNOT_LAMP

void
drawFrame()
{
    // draw axes
    if (showAxes) drawAxes();

	// draw plane
	drawSelectedPlane();

	drawLineSet();

	drawSelectedPoint();

	drawnQuads.clear();

#ifdef ANNOT_BIRD
	if(ANNOT_COUNT>0) drawRotationQuad(-7.8,-3, 1, 0.1f, 0);
	if(ANNOT_COUNT>1) drawRotationQuad(-7.8, 3, 1, 0.1f, 0);
	if(ANNOT_COUNT>2) drawRotationQuad(-7.8, 6, 1, 0.1f, 0);
	if(ANNOT_COUNT>3) drawRotationQuad(-7.8,-6, 1, 0.1f, 0);
	if(ANNOT_COUNT>4) drawRotationQuad(-3.8, 3, 1, 0.1f, 0);
	if(ANNOT_COUNT>5) drawRotationQuad(-3.8,-3, 1, 0.1f, 0);
#endif

#ifdef ANNOT_CROSSBOW
	if(ANNOT_COUNT>0) drawRotationQuad(8 , -2 ,3,  0.1f, 1);
	if(ANNOT_COUNT>1) drawRotationQuad(12, -2 ,3,  0.1f, 1);
	if(ANNOT_COUNT>2) drawRotationQuad(12, -2,10,  0.1f, 1);
	if(ANNOT_COUNT>3) drawRotationQuad(8 ,-2 ,10, 0.1f, 1);
	if(ANNOT_COUNT>4) drawRotationQuad(8 , 2  ,3,   0.1f, 1);
	if(ANNOT_COUNT>5) drawRotationQuad(12, 2 ,3,   0.1f, 1);
	if(ANNOT_COUNT>6) drawRotationQuad(8,  2, 10, 0.1f, 1);
	if(ANNOT_COUNT>7) drawRotationQuad(12, 2 ,10,  0.1f, 1);
#endif

//////////////////////////////// Thanks to Prof. FU //////////////////////////////////

#ifdef ANNOT_CLAW
	if(ANNOT_COUNT>0) drawRotationQuad(1.947100f, -1.900000f, -2.276200f, 0.1f, 1);
	if(ANNOT_COUNT>1) drawRotationQuad(2.898100f, -1.900000f, -5.120300f, 0.1f, 1);
	if(ANNOT_COUNT>2) drawRotationQuad(6.742300f, -1.900000f, -7.175000f, 0.1f, 1);
	if(ANNOT_COUNT>3) drawRotationQuad(10.586400f, -1.900000f, -9.229500f, 0.1f, 1);
	if(ANNOT_COUNT>4) drawRotationQuad(14.433100f, -1.900000f, -11.267500f, 0.1f, 1);
#endif

#ifdef ANNOT_CHAIR_1
	if(ANNOT_COUNT>0) drawRotationQuad(0.000000f, 9.000000f, 2.900000f, 0.1f, 2);
	if(ANNOT_COUNT>1) drawRotationQuad(0.000000f, 1.000000f, 2.900000f, 0.1f, 2);
	if(ANNOT_COUNT>2) drawRotationQuad(6.000000f, 1.000000f, 2.900000f, 0.1f, 2);
	if(ANNOT_COUNT>3) drawRotationQuad(0.000000f, -1.000000f, 2.900000f, 0.1f, 2);
	if(ANNOT_COUNT>4) drawRotationQuad(6.000000f, -1.000000f, 2.900000f, 0.1f, 2);
	if(ANNOT_COUNT>5) drawRotationQuad(0.000000f, 9.000000f, -3.100000f, 0.1f, 2);
	if(ANNOT_COUNT>6) drawRotationQuad(0.000000f, 1.000000f, -3.100000f, 0.1f, 2);
	if(ANNOT_COUNT>7) drawRotationQuad(6.000000f, 1.000000f, -3.100000f, 0.1f, 2);
	if(ANNOT_COUNT>8) drawRotationQuad(0.000000f, -1.000000f, -3.100000f, 0.1f, 2);
	if(ANNOT_COUNT>9) drawRotationQuad(6.000000f, -1.000000f, -3.100000f, 0.1f, 2);
#endif

#ifdef ANNOT_CHAIR_2
	if(ANNOT_COUNT>0) drawRotationQuad(0.100000f, 9.000000f, 3.000000f, 0.1f, 0);
	if(ANNOT_COUNT>1) drawRotationQuad(0.100000f, 1.000000f, 3.000000f, 0.1f, 0);
	if(ANNOT_COUNT>2) drawRotationQuad(6.100000f, 1.000000f, 3.000000f, 0.1f, 0);
	//if(ANNOT_COUNT>3) drawRotationQuad(0.100000f, -1.000000f, 3.000000f, 0.1f, 0);
	//if(ANNOT_COUNT>4) drawRotationQuad(6.100000f, -2.000000f, 3.000000f, 0.1f, 0);
	if(ANNOT_COUNT>3) drawRotationQuad(0.100000f, 9.000000f, -3.000000f, 0.1f, 0);
	if(ANNOT_COUNT>4) drawRotationQuad(0.100000f, 1.000000f, -3.000000f, 0.1f, 0);
	if(ANNOT_COUNT>5) drawRotationQuad(6.100000f, 1.000000f, -3.000000f, 0.1f, 0);
	//if(ANNOT_COUNT>7) drawRotationQuad(0.100000f, -1.000000f, -3.000000f, 0.1f, 0);
	//if(ANNOT_COUNT>9) drawRotationQuad(6.100000f, -2.000000f, -3.000000f, 0.1f, 0);
#endif

#ifdef ANNOT_SEASAW
	if(ANNOT_COUNT>0) drawRotationQuad(0.000000f, 1.900000f, 0.000000f, 0.1f, 1);
	if(ANNOT_COUNT>1) drawRotationQuad(0.000000f, -2.100000f, 0.000000f, 0.1f, 1);
#endif 

#ifdef ANNOT_LAMP
	if (ANNOT_COUNT>0) drawRotationQuad(-1.000000f, -0.100000f, 23.000000f, 0.1f, 1);
	if (ANNOT_COUNT>1) drawRotationQuad(1.000000f, -0.100000f, 23.000000f, 0.1f, 1);
	if (ANNOT_COUNT>2) drawRotationQuad(-1.000000f, -0.100000f, -1.000000f, 0.1f, 1);
	if (ANNOT_COUNT>3) drawRotationQuad(1.000000f, -0.100000f, -1.000000f, 0.1f, 1);
	if (ANNOT_COUNT>4) drawRotationQuad(-1.000000f, -0.100000f, 11.000000f, 0.1f, 1);
	if (ANNOT_COUNT>5) drawRotationQuad(1.000000f, -0.100000f, 11.000000f, 0.1f, 1);
#endif


	glutPostRedisplay();
}
