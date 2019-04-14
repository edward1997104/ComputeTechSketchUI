///////////////////////////////////////////////////////////////
//
// grid.h
//
// - handle grid related struct and function
//
// by Edward Hui 
//
// 9/2/2018 23:00
//
// All rights reserved
// 
///////////////////////////////////////////////////////////////

#include "glui_mouse.h"
#include "grid.h"
#include "glui_build.h"

#include "lines.h"

#include <stdio.h>
#include <limits.h>
#include <vector>

//////////////////////////
// INTERNAL

int xySelected = 1;
int yzSelected;
int xzSelected;

extern Grid_t * xyGrid;
extern Grid_t * yzGrid;
extern Grid_t * xzGrid;

extern LineSet_t * currentLineSet;

Grid_t * startingGrid(int direction)
{
	///////////////////////////////////////////
	// Constructor of an starting grid
	// create a grid with an offset of 0
	// args:
	// int direction: Direction of grid
	// return:
	// a struct of grid

	// malloc the grid

	Grid_t * tempGrid = new Grid_t;

	tempGrid->direction = direction;

	tempGrid->offset = 0.0;

	tempGrid->snapFlag = 0;

	tempGrid->previosuSnapTime = myTime();
	tempGrid->previousSnapPoint[0] = 0.0;
	tempGrid->previousSnapPoint[1] = 0.0;
	tempGrid->previousSnapPoint[2] = 0.0;

	return tempGrid;

}


void getClosestPoint(double x, double y, double z, double * output)
{
	///////////////////////////////////////////
	// get the closest point on the grid
	// args:
	// Grid_t * inputGrid : the grid struct created
	// double x, y, z : the coordinate
	// return:
	// closest point structure

	// set the rounded output


	if (xySelected)
	{
		output[0] = round((x + GRID_SIZE) / GRID_INTERVAL) * GRID_INTERVAL - GRID_SIZE;
		output[1] = round((y + GRID_SIZE) / GRID_INTERVAL) * GRID_INTERVAL - GRID_SIZE;
		output[2] = z;
		//printf("rounded coordinate: %lf %lf %lf\n", output[0], output[1], output[2]);
	}
	else if (yzSelected)
	{
		output[0] = x;
		output[1] = round((y + GRID_SIZE) / GRID_INTERVAL) * GRID_INTERVAL - GRID_SIZE;
		output[2] = round((z + GRID_SIZE) / GRID_INTERVAL) * GRID_INTERVAL - GRID_SIZE;
		//printf("rounded coordinate: %lf %lf %lf\n", output[0], output[1], output[2]);
	}
	else if (xzSelected)
	{
		output[0] = round((x + GRID_SIZE) / GRID_INTERVAL) * GRID_INTERVAL - GRID_SIZE;
		output[1] = y;
		output[2] = round((z + GRID_SIZE) / GRID_INTERVAL) * GRID_INTERVAL - GRID_SIZE;
		//printf("rounded coordinate: %lf %lf %lf\n", output[0], output[1], output[2]);
	}
	else
		return;
	
	double mousePos[3] = { x, y, z };
	double currentMinDis = distance(mousePos, output);
	for (unsigned int i = 0; i < currentLineSet->lineSet.size(); i++)
	{
		for (unsigned int j = 0; j < currentLineSet->lineSet[i]->intPoint.size(); j++)
		{
			if (xySelected)
			{
				if ( abs (currentLineSet->lineSet[i]->intPoint[j]->z -z) < FLT_EPSILON)
				{
					double tempDis = distance(mousePos, currentLineSet->lineSet[i]->intPoint[j]);
					if (tempDis < currentMinDis)
					{
						tempDis = currentMinDis;
						output[0] = currentLineSet->lineSet[i]->intPoint[j]->x;
						output[1] = currentLineSet->lineSet[i]->intPoint[j]->y;
						output[2] = currentLineSet->lineSet[i]->intPoint[j]->z;
					}
				}
			}
			else if (yzSelected)
			{
				if (abs(currentLineSet->lineSet[i]->intPoint[j]->x - x) < FLT_EPSILON)
				{
					double tempDis = distance(mousePos, currentLineSet->lineSet[i]->intPoint[j]);
					if (tempDis < currentMinDis)
					{
						tempDis = currentMinDis;
						output[0] = currentLineSet->lineSet[i]->intPoint[j]->x;
						output[1] = currentLineSet->lineSet[i]->intPoint[j]->y;
						output[2] = currentLineSet->lineSet[i]->intPoint[j]->z;
					}
				}
			}
			else if (xzSelected)
			{
				if (abs(currentLineSet->lineSet[i]->intPoint[j]->y - y) < FLT_EPSILON)
				{
					double tempDis = distance(mousePos, currentLineSet->lineSet[i]->intPoint[j]);
					if (tempDis < currentMinDis)
					{
						tempDis = currentMinDis;
						output[0] = currentLineSet->lineSet[i]->intPoint[j]->x;
						output[1] = currentLineSet->lineSet[i]->intPoint[j]->y;
						output[2] = currentLineSet->lineSet[i]->intPoint[j]->z;
					}
				}
			}
		}
	}
}

double distance(double * point1, double * point2)
{
	return sqrt((point1[0] - point2[0]) * (point1[0] - point2[0]) + (point1[1] - point2[1]) * (point1[1] - point2[1]) + (point1[2] - point2[2]) * (point1[2] - point2[2]));
}

double distance(Point_t * point1, Point_t * point2)
{
	return sqrt((point1->x - point2->x) * (point1->x - point2->x) + (point1->y - point2->y) * (point1->y - point2->y) + (point1->z - point2->z) * (point1->z - point2->z));
}

double distance(double * point1, Point_t * point2)
{
	return sqrt((point1[0] - point2->x) * (point1[0] - point2->x) + (point1[1] - point2->y) * (point1[1] - point2->y) + (point1[2] - point2->z) * (point1[2] - point2->z));
}