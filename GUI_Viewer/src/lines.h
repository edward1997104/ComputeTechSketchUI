
#ifndef _ND_LINES_H
#define _ND_LINES_H

#include <vector>


#define DRAW_LINE 0
#define DELETE_LINE 1

typedef struct Point_s
{
	///////////////////////////////////////////
	// Point structure for keeping the coordinate of a point
	// instance:
	// double x, double y, double z: coordinate of the points

	double x;
	double y;
	double z;
} Point_t;

typedef struct Line_s
{
	///////////////////////////////////////////
	// line structure for keeping the endpoint of the line
	// instance:
	// Point_t [2] : endpoint of the line
	// int id :

	Point_t * endpoint[2];
	std::vector <Point_t *> intPoint;
	int id;

} Line_t;

typedef struct LineSet_s
{
	std::vector <Line_t *> lineSet;

}LineSet_t;

typedef struct LineSetHistory_s
{
	std::vector < LineSet_t *> history;
	std::vector < int > previousFlag;
	std::vector < int > previousID;

}LineSetHistory_t;

///////////////////////////////////////////////////////////////
// function
///////////////////////////////////////////////////////////////

extern LineSet_t * newLineSet();
extern void selectClosestPoint(int x, int y);
extern void clearSelectedPoint();
extern void deleteSelectedLine();
extern void undoAction();
extern void selectLine(int x, int y);
extern void outputLineSet();

Point_t * newPoint(double x, double y, double z);
LineSetHistory_t * newLineSetHistory();

#endif