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

#ifndef _ND_GRID_H
#define _ND_GRID_H

#include <vector>
#include "lines.h"
//////////////////////////////how /////////////////////////////////
// definition of Direction
///////////////////////////////////////////////////////////////

#define XY_DIRECTION 0
#define YZ_DIRECTION 1
#define XZ_DIRECTION 2

///////////////////////////////////////////////////////////////
// public constant
///////////////////////////////////////////////////////////////

const double GRID_SIZE = 2.0;
const double GRID_INTERVAL = 0.1;
const double SNAP_THERSHOLD = 0.05;
const double SNAP_TIME = 0.01;

///////////////////////////////////////////////////////////////
// structure
///////////////////////////////////////////////////////////////



typedef struct Grid_s
{
	///////////////////////////////////////////
	// Gird structure for keeping the coordinate of a point
	// instance:
	// int direction : indicate XY, YZ or XZ direction grid
	// float offset : offset of the plane
	// std::vector<Point> gridPoints : vector of grid points

	int direction;
	int offsetInt;
	float offset;
	double previousSnapPoint[3];
	int snapFlag;
	double previosuSnapTime; 


} Grid_t;


///////////////////////////////////////////////////////////////
// function
///////////////////////////////////////////////////////////////

extern Grid_t * startingGrid(int direction);
extern double distance(double * point1, double * point2);
extern double distance(Point_t * point1, Point_t * point2);
extern double distance(double * point1, Point_t * point2);

void getClosestPoint(double x, double y, double z, double * output);

#endif
