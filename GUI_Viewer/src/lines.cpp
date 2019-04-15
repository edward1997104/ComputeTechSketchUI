
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <vector>

#ifdef WIN32
#include <windows.h>
#include <float.h>

#else
#include <limits.h>
#endif
#include "grid.h"
#include "glui_mouse.h"
#include "lines.h"
#include "glui_build.h"

#include "glui.h"
#include <iostream>
#include<fstream>
#include <cstring>
#include<string>
using std::string;
using std::vector;
using namespace std;
//////////////////////////////////////
// External

	extern int winH;
	extern int xySelected;
	extern int yzSelected;
	extern int xzSelected;
	extern int showOBJModel;

	extern LineSet_t * currentLineSet;
	extern GLUI_Listbox * lineSetListbox;
	//extern  Model myModel;

	extern char outputFilename[100];
//////////////////////////////////////
// Internal

	// CONSTANT
	double SELF_EPS = GRID_INTERVAL;

	// line id count
	int id_count = 0;

	// previous modifiication type
	int previousChangeType;
	int previousChangeId;

	// previous selected point
	int selectedFlag = 0;
	Point_t * prevPoint;

	//delete function
	void deleteLine(Line_t * line);

	// copy function
	LineSet_t * copyLineSet(LineSet_t * lineSet);
	Point_t * copyPoint(Point_t * point);

	LineSetHistory_t * lineHistoryStack = newLineSetHistory();


LineSetHistory_t * newLineSetHistory()
{
	LineSetHistory_t * tempHistory = new LineSetHistory_t;
	tempHistory->history = std::vector <LineSet_t *>();
	tempHistory->previousFlag = std::vector <int>();
	tempHistory->previousID = std::vector<int>();

	return tempHistory;
}

void deleteLineSet(LineSet_t * lineSetInput)
{
	for (unsigned int i = 0; i < lineSetInput->lineSet.size(); i++)
	{
		deleteLine(lineSetInput->lineSet[i]);
	}
	delete lineSetInput;
}

LineSet_t * popLineSet()
{
	if (!lineHistoryStack->history.empty())
	{
		LineSet_t * top = lineHistoryStack->history.back();
		lineHistoryStack->history.pop_back();

		previousChangeType = lineHistoryStack->previousFlag.back();
		lineHistoryStack->previousFlag.pop_back();

		previousChangeId = lineHistoryStack->previousID.back();
		lineHistoryStack->previousID.pop_back();

		return top;
	}
	else
		return NULL;
}

void pushLineSet(LineSet_t * lineSetInput, int changeType, int changeId)
{
	lineHistoryStack->history.push_back(
		copyLineSet(lineSetInput)
		);
	lineHistoryStack->previousFlag.push_back(changeType);
	lineHistoryStack->previousID.push_back(changeId);
}

LineSet_t * newLineSet()
{
	LineSet_t * tempLineSet = new LineSet_t;
	tempLineSet->lineSet = std::vector<Line_t *>();

	return tempLineSet;
}

Point_t * newPoint(double x, double y, double z)
{
	///////////////////////////////////////////
	// Constructor of an starting grid
	// args:
	// double x, double y, double z: postion of the input
	// return:
	// struct of the point

	Point_t * tempPoint = new Point_t;

	tempPoint->x = x;
	tempPoint->y = y;
	tempPoint->z = z;

	return tempPoint;
}

Line_t * newLine(Point_t * p1, Point_t * p2, int id)
{
	Line_t * tempLine = new Line_t;

	tempLine->endpoint[0] = copyPoint(p1);

	tempLine->endpoint[1] = copyPoint(p2);

	tempLine->id = id;

	tempLine->intPoint = std::vector<Point_t *>();

	double lineVector[3];
	lineVector[0] = p2->x - p1->x;
	lineVector[1] = p2->y - p1->y;
	lineVector[2] = p2->z - p1->z;

	double lineLen = distance(p1, p2) / (GRID_INTERVAL);

	//printf("length : %lf\n", lineLen);
	for (int i = 0; i < lineLen + FLT_EPSILON; i++)
	{
		Point_t * tempPoint = copyPoint(p1);
		tempPoint->x = p1->x + i / lineLen * lineVector[0];
		tempPoint->y = p1->y + i / lineLen * lineVector[1];
		tempPoint->z = p1->z + i / lineLen * lineVector[2];
		tempLine->intPoint.push_back(tempPoint);
	}

	//printf("array size: %d\n", tempLine->intPoint.size());
	return tempLine;
}

void deletePoint(Point_t * point)
{
	Point_t * temp = point;
	point = NULL;
	delete temp;
}

Point_t * copyPoint(Point_t * point)
{
	return newPoint(point->x, point->y, point->z);
}

void deleteLine(Line_t * line)
{
	deletePoint(line->endpoint[1]);
	deletePoint(line->endpoint[0]);

	for (unsigned int i = 0; i < line->intPoint.size(); i++)
	{
		deletePoint(line->intPoint[i]);
	}

	delete line;
}

Line_t * copyLine(Line_t * line)
{

	Line_t * tempLine = newLine(line->endpoint[0], line->endpoint[1], line->id);


	return tempLine;
}

LineSet_t * copyLineSet(LineSet_t * lineSet)
{
	LineSet_t * tempLineSet = newLineSet();

	for (unsigned int i = 0; i < lineSet->lineSet.size(); i++)
	{
		tempLineSet->lineSet.push_back(
			copyLine(lineSet->lineSet[i])
			);
	}

	return tempLineSet;
}

void selectClosestPoint(int x, int y)
{
	if (!xySelected && !yzSelected && !xzSelected)
		return;


	// get the mosue position in the world coordinate
	double currentMouseWorldPos[3];
	getMouseWorldPos(x, y, currentMouseWorldPos);

	double roundedPos[3];
	getClosestPoint(currentMouseWorldPos[0], currentMouseWorldPos[1], currentMouseWorldPos[2], roundedPos);

	if (selectedFlag)
	{
		Point_t * tempPoint = newPoint(roundedPos[0], roundedPos[1], roundedPos[2]);
		printf("ending selected point : %lf %lf %lf\n", roundedPos[0], roundedPos[1], roundedPos[2]);

		double lineDis = distance(tempPoint, prevPoint) * (GRID_SIZE / GRID_INTERVAL);
		printf("line Dis: %lf %lf\n", lineDis, (GRID_SIZE / GRID_INTERVAL));
		if (abs(lineDis - round(lineDis)) < 1e-5)
		{
			Line_t * tempLine = newLine(prevPoint, tempPoint, id_count++);

			char lineStrBuff[100];
			sprintf(lineStrBuff, "Line: %lf %lf %lf -> %lf %lf %lf\n", tempLine->endpoint[0]->x
				, tempLine->endpoint[0]->y, tempLine->endpoint[0]->z
				, tempLine->endpoint[1]->x, tempLine->endpoint[1]->y, tempLine->endpoint[1]->z);
			resetStatusBar(lineStrBuff);

			pushLineSet(currentLineSet, DRAW_LINE, tempLine->id);

			currentLineSet->lineSet.push_back(tempLine);
			

			prevPoint = copyPoint(tempPoint);

			char buffer[33];
			
			_itoa(tempLine->id, buffer, 10);

			lineSetListbox->add_item(tempLine->id, buffer);
			glutPostRedisplay();

		}
		else
		{
			printf("Not an integer length!\n");
		}
	}
	else
	{
		Point_t * tempPoint = newPoint(roundedPos[0], roundedPos[1], roundedPos[2]);

		char pointStrBuffer[100];
		sprintf(pointStrBuffer, "selected point : %lf %lf %lf\n", roundedPos[0], roundedPos[1], roundedPos[2]);
		resetStatusBar(pointStrBuffer);

		prevPoint = tempPoint;

		selectedFlag = 1;
		glutPostRedisplay();
	}
}


void clearSelectedPoint()
{
	lineSetListbox->do_selection(-1);

	if (selectedFlag == 0)
		return;

	selectedFlag = 0;

	deletePoint(prevPoint);

	glutPostRedisplay();
}

void deleteSelectedLine()
{
	int deleteID = lineSetListbox->int_val;

	if (deleteID == -1)
		return;
	for (unsigned int i = 0; i < currentLineSet->lineSet.size(); i++)
	{
		if (currentLineSet->lineSet[i]->id == deleteID)
		{
			Line_t * temp = currentLineSet->lineSet[i];
			pushLineSet(currentLineSet, DELETE_LINE, deleteID);
			currentLineSet->lineSet.erase(currentLineSet->lineSet.begin() + i);
			deleteLine(temp);
			lineSetListbox->delete_item(deleteID);
			lineSetListbox->set_int_val(-1);
			glutPostRedisplay();
			break;
		}
	}
}

void undoAction()
{
	clearSelectedPoint();

	if (lineHistoryStack->history.empty())
		return;

	LineSet_t * temp = currentLineSet;
	currentLineSet = popLineSet();

	if (previousChangeType == DRAW_LINE)
	{
		lineSetListbox->delete_item(previousChangeId);
	}
	else
	{
		char buffer[33];

		_itoa(previousChangeId, buffer, 10);

		lineSetListbox->add_item(previousChangeId, buffer);
	}

	lineSetListbox->sort_items();

	lineSetListbox->do_selection(-1);

	deleteLineSet(temp);
	
	glutPostRedisplay();
}

int checkIntersection(Point_t * p1, Point_t * p2, double projectVector[2][3], double outputPos[3])
{
	double p13[3];
	double p43[3];
	double p21[3];
	double d1343, d4321, d1321, d4343, d2121;
	double numer, denom;

	p13[0] = p1->x - projectVector[0][0];
	p13[1] = p1->y - projectVector[0][1];
	p13[2] = p1->z - projectVector[0][2];

	p43[0] = projectVector[1][0] - projectVector[0][0];
	p43[1] = projectVector[1][1] - projectVector[0][1];
	p43[2] = projectVector[1][2] - projectVector[0][2];


	if (abs(p43[0]) < FLT_EPSILON && abs(p43[1]) < FLT_EPSILON && abs(p43[2]) < FLT_EPSILON)
		return false;
	
	p21[0] = p2->x - p1->x;
	p21[1] = p2->y - p1->y;
	p21[2] = p2->z - p1->z;

	d1343 = p13[0] * p43[0] + p13[1] * p43[1] + p13[2] * p43[2];
	d4321 = p43[0] * p21[0] + p43[1] * p21[1] + p43[2] * p21[2];
	d1321 = p13[0] * p21[0] + p13[1] * p21[1] + p13[2] * p21[2];
	d4343 = p43[0] * p43[0] + p43[1] * p43[1] + p43[2] * p43[2];
	d2121 = p21[0] * p21[0] + p21[1] * p21[1] + p21[2] * p21[2];

	denom = d2121 * d4343 - d4321 * d4321;
	if (abs(denom) < FLT_EPSILON)
		return(FALSE);
	
	numer = d1343 * d4321 - d1321 * d4343;

	double mua = numer / denom;

	double mub = (d1343 + d4321 * (mua)) / d4343;

	outputPos[0] = p1->x + mua * p21[0];
	outputPos[1] = p1->y + mua * p21[1];
	outputPos[2] = p1->z + mua * p21[2];

	double pb[3];

	pb[0] = projectVector[0][0] + mub * p43[0];
	pb[1] = projectVector[0][1] + mub * p43[1];
	pb[2] = projectVector[0][2] + mub * p43[2];

	if (outputPos[0] - pb[0] < SELF_EPS && outputPos[1] - pb[1] < SELF_EPS && outputPos[2] - pb[2] < SELF_EPS)
	{
		if (distance(pb, p1) + distance(pb, p2) - distance(p1, p2) < SELF_EPS)
			return true;
		else
			return false;
	}

	return false;

}

int checkIntersection(Point_t * p1, Point_t * p2, Point_t * p3, Point_t * p4, Point_t * outputPoint)
{
	double p13[3];
	double p43[3];
	double p21[3];
	double d1343, d4321, d1321, d4343, d2121;
	double numer, denom;

	p13[0] = p1->x - p3->x;
	p13[1] = p1->y - p3->y;
	p13[2] = p1->z - p3->z;

	p43[0] = p4->x - p3->x;
	p43[1] = p4->y - p3->y;
	p43[2] = p4->z - p3->z;


	if (abs(p43[0]) < FLT_EPSILON && abs(p43[1]) < FLT_EPSILON && abs(p43[2]) < FLT_EPSILON)
		return false;

	p21[0] = p2->x - p1->x;
	p21[1] = p2->y - p1->y;
	p21[2] = p2->z - p1->z;

	d1343 = p13[0] * p43[0] + p13[1] * p43[1] + p13[2] * p43[2];
	d4321 = p43[0] * p21[0] + p43[1] * p21[1] + p43[2] * p21[2];
	d1321 = p13[0] * p21[0] + p13[1] * p21[1] + p13[2] * p21[2];
	d4343 = p43[0] * p43[0] + p43[1] * p43[1] + p43[2] * p43[2];
	d2121 = p21[0] * p21[0] + p21[1] * p21[1] + p21[2] * p21[2];

	denom = d2121 * d4343 - d4321 * d4321;
	if (abs(denom) < FLT_EPSILON)
		return(FALSE);

	numer = d1343 * d4321 - d1321 * d4343;

	double mua = numer / denom;

	double mub = (d1343 + d4321 * (mua)) / d4343;

	outputPoint->x = p1->x + mua * p21[0];
	outputPoint->y = p1->y + mua * p21[1];
	outputPoint->z = p1->z + mua * p21[2];

	double pb[3];

	pb[0] = p3->x + mub * p43[0];
	pb[1] = p3->y + mub * p43[1];
	pb[2] = p3->z + mub * p43[2];

	if (outputPoint->x - pb[0] < FLT_EPSILON && outputPoint->y - pb[1] < FLT_EPSILON && outputPoint->z - pb[2] < FLT_EPSILON)
	{
		if (abs(distance(pb, p1) + distance(pb, p2) - distance(p1, p2)) < FLT_EPSILON && abs(distance(pb, p3) + distance(pb, p4) - distance(p3, p4)) < FLT_EPSILON)
			return true;
		else
			return false;
	}

	return false;

}

double linePointDis(Line_t * line, double pt[3])
{
	double U = ((pt[0] - line->endpoint[0]->x) * (line->endpoint[1]->x - line->endpoint[0]->x) +
		(pt[1] - line->endpoint[1]->z) * (line->endpoint[1]->y - line->endpoint[0]->y) +
		(pt[2] - line->endpoint[1]->z) * (line->endpoint[1]->z - line->endpoint[0]->z)) /
		 (distance(line->endpoint[0], line->endpoint[1]) * distance(line->endpoint[0], line->endpoint[1]) );

	double intersection[3];
	intersection[0] = line->endpoint[0]->x + U * (line->endpoint[1]->x - line->endpoint[0]->x);
	intersection[1] = line->endpoint[0]->y + U * (line->endpoint[1]->y - line->endpoint[0]->y);
	intersection[2] = line->endpoint[0]->z + U * (line->endpoint[1]->z - line->endpoint[0]->z);

	return distance(intersection, pt);
}

double getWindowDepth(double pt[3])
{
	double tempDepth;

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	double tempx, tempy;

	gluProject(pt[0], pt[1], pt[2], modelview, projection, viewport, &tempx, &tempy, &tempDepth);

	return tempDepth;
}

void selectLine(int x, int y)
{
	double projectVector[2][3];

	getProjectionVector(x, y, projectVector);

	int tempID = -1;
	double currentMinDIs = 100;

	for (unsigned int i = 0; i < currentLineSet->lineSet.size(); i++)
	{
		double intersectionPos[3];
		if (checkIntersection(currentLineSet->lineSet[i]->endpoint[0]
			, currentLineSet->lineSet[i]->endpoint[1]
			, projectVector
			, intersectionPos)
			)
		{
			if (getWindowDepth(intersectionPos) < currentMinDIs)
			{
				tempID = currentLineSet->lineSet[i]->id;
				currentMinDIs = getWindowDepth(intersectionPos);
				printf("current Selected : %d %lf\n", currentLineSet->lineSet[i]->id, currentMinDIs);

			}
		}
	}

	if (tempID != -1)
	{
		lineSetListbox->do_selection(tempID);
	}
}

int checkPointEqual(Point_t * pt1, Point_t * pt2)
{
	return (abs(pt1->x - pt2->x) + abs(pt1->y - pt2->y) + abs(pt1->z - pt2->z) < FLT_EPSILON * 10);
}

int checkPointEqual(double * pt1, Point_t * pt2)
{
	return (abs(pt1[0] - pt2->x) + abs(pt1[1] - pt2->y) + abs(pt1[2] - pt2->z) < FLT_EPSILON * 10);
}

int CheckIntegerLen(Line_t * inputLine, Point_t * pt)
{
	for (unsigned int i = 0; i < inputLine->intPoint.size(); i++)
	{
		if (checkPointEqual(pt, inputLine->intPoint[i]))
			return true;
	}
	return false;
}
int splitLineSet(LineSet_t * inputLineSet)
{
	int flag = 0;
	for (unsigned int i = 0; i < inputLineSet->lineSet.size(); i++)
	{
		for (unsigned int j = 0; j < inputLineSet->lineSet.size(); j++)
		{
			if (i == j)
				continue;

			Point_t * intersectionPoint = new Point_t;
			if (checkIntersection(inputLineSet->lineSet[i]->endpoint[0],
				inputLineSet->lineSet[i]->endpoint[1],
				inputLineSet->lineSet[j]->endpoint[0],
				inputLineSet->lineSet[j]->endpoint[1], intersectionPoint) && 
				CheckIntegerLen(inputLineSet->lineSet[i], intersectionPoint) &&
				CheckIntegerLen(inputLineSet->lineSet[j], intersectionPoint))
			{
				int count = 0;
				count += checkPointEqual(inputLineSet->lineSet[i]->endpoint[0], intersectionPoint);
				count += checkPointEqual(inputLineSet->lineSet[i]->endpoint[1], intersectionPoint);
				count += checkPointEqual(inputLineSet->lineSet[j]->endpoint[0], intersectionPoint);
				count += checkPointEqual(inputLineSet->lineSet[j]->endpoint[1], intersectionPoint);

				if (count >= 2)
					continue;

				Line_t * tempLine1 = inputLineSet->lineSet[i];
				Line_t * tempLine2 = inputLineSet->lineSet[j];

				if (!checkPointEqual(tempLine1->endpoint[0], intersectionPoint))
				{
					Line_t * splitedLine = newLine(tempLine1->endpoint[0], intersectionPoint, -1);
					inputLineSet->lineSet.push_back(splitedLine);
				}

				if (!checkPointEqual(tempLine1->endpoint[1], intersectionPoint))
				{
					Line_t * splitedLine = newLine(tempLine1->endpoint[1], intersectionPoint, -1);
					inputLineSet->lineSet.push_back(splitedLine);
				}

				if (!checkPointEqual(tempLine2->endpoint[0], intersectionPoint))
				{
					Line_t * splitedLine = newLine(tempLine2->endpoint[0], intersectionPoint, -1);
					inputLineSet->lineSet.push_back(splitedLine);
				}

				if (!checkPointEqual(tempLine2->endpoint[1], intersectionPoint))
				{
					Line_t * splitedLine = newLine(tempLine2->endpoint[1], intersectionPoint, -1);
					inputLineSet->lineSet.push_back(splitedLine);
				}

				if (i > j)
				{
					inputLineSet->lineSet.erase(inputLineSet->lineSet.begin() + i);
					inputLineSet->lineSet.erase(inputLineSet->lineSet.begin() + j);
				}
				else
				{
					inputLineSet->lineSet.erase(inputLineSet->lineSet.begin() + j);
					inputLineSet->lineSet.erase(inputLineSet->lineSet.begin() + i);
				}

				deleteLine(tempLine1);
				deleteLine(tempLine2);
				flag = 1;
			}
		}
	}

	return flag;
}

void printLineSet(LineSet_t * inputLineSet)
{
	printf("lineSet:\n");
	for (unsigned int i = 0; i < inputLineSet->lineSet.size(); i++)
	{
		printf("line : %lf %lf %lf -> %lf %lf %lf\n", inputLineSet->lineSet[i]->endpoint[0]->x
			, inputLineSet->lineSet[i]->endpoint[0]->y,
			inputLineSet->lineSet[i]->endpoint[0]->z,
			inputLineSet->lineSet[i]->endpoint[1]->x,
			inputLineSet->lineSet[i]->endpoint[1]->y,
			inputLineSet->lineSet[i]->endpoint[1]->z);
	}
	printf("lienset Size:%d\n", inputLineSet->lineSet.size());
}

void writeOutputFile(LineSet_t * inputLineSet)
{
	char tempFilename[105];
	strcpy(tempFilename, "./");
	strcat(tempFilename, outputFilename);
	strcat(tempFilename, ".obj");
	if (strcmp(tempFilename, "./") == 0)
	{
		char outputBuffer[1000];
		sprintf(outputBuffer, "Invalid Input!");
		resetStatusBar(outputBuffer);
		return;
	}
	FILE * outputfile = fopen(tempFilename, "w+");
	if (outputfile == NULL)
		return;
	fprintf(outputfile, "# Auto Generated by Program\n");

	int ** index = (int **)malloc(sizeof(int *) * inputLineSet->lineSet.size());
	for (unsigned int i = 0; i < inputLineSet->lineSet.size(); i++)
		index[i] = (int *)malloc(sizeof(int) * 2);

	double ** point = (double **)malloc(sizeof(double *) * inputLineSet->lineSet.size() * 2);
	for (unsigned int i = 0; i < inputLineSet->lineSet.size() * 2; i++)
		point[i] = (double *)malloc(sizeof(double) * 3);

	int index_count = 0;
	for (unsigned int i = 0; i < inputLineSet->lineSet.size(); i++)
	{
		
		for (int j = 0; j < 2; j++)
		{
			int flag = true;
			for (int k = 0; k < index_count; k++)
			{
				if (checkPointEqual(point[k], inputLineSet->lineSet[i]->endpoint[j]))
				{
					index[i][j] = k;
					flag = false;
					break;
				}
			}

			if (flag)
			{
				index[i][j] = index_count;
				point[index_count][0] = inputLineSet->lineSet[i]->endpoint[j]->x;
				point[index_count][1] = inputLineSet->lineSet[i]->endpoint[j]->y;
				point[index_count++][2] = inputLineSet->lineSet[i]->endpoint[j]->z;
			}
		}

	}

	for (int i = 0; i < index_count; i++)
	{
		fprintf(outputfile, "v %lf %lf %lf\n",
			point[i][0] / GRID_INTERVAL,
			point[i][1] / GRID_INTERVAL,
			point[i][2] / GRID_INTERVAL);

	}
	fprintf(outputfile, "\n");

	for (unsigned int i = 0; i < inputLineSet->lineSet.size(); i++)
	{
		fprintf(outputfile, "l %d %d\n", index[i][0] + 1, index[i][1] + 1);
	}
	fclose(outputfile);

	char outputBuffer[1000];
	sprintf(outputBuffer, "Output Sucess as: %s", tempFilename);
	resetStatusBar(outputBuffer);

	for (unsigned int i = 0; i < inputLineSet->lineSet.size(); i++)
		free(index[i]);
	free(index);

	for (unsigned int i = 0; i < inputLineSet->lineSet.size() * 2; i++)
		free(point[i]);
	free(point);
}

void outputLineSet()
{
	LineSet_t * outputSet = copyLineSet(currentLineSet);

	do
	{
		splitLineSet(outputSet);
	} while (splitLineSet(outputSet));

	printLineSet(outputSet);
	writeOutputFile(outputSet);

	//deleteLineSet(outputSet);
}

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
	//remove empty part
	for (unsigned int i = 0; i < v.size(); i++){
		if (v.at(i) == ""){
			v.erase(v.begin() + i);
			i--;
		}
	}
}
void readFromObjFile(string path)
{
	vector<Point_t*>* points = new vector<Point_t*>;
	vector<string> ss;
	double minX = 0.0;
	double maxX = 0.0;
	double minY = 0.0;
	double maxY = 0.0;
	double minZ = 0.0;
	double maxZ = 0.0;

	ifstream fin(path);
	string line;
	bool onceCal = true;
	while (getline(fin, line))
	{
		if (line.find("#Feature") == 0) continue;

		if (!line.empty() && line.at(line.size() - 1) == '\r')
			line.at(line.size() - 1) = ' ';
		if (line.compare("") != 0 && line.at(0) == 'v')
		{
			SplitString(line, ss, " ");
			double x = GRID_INTERVAL* stod(ss.at(1));
			double y = GRID_INTERVAL* stod(ss.at(2));
			double z = GRID_INTERVAL* stod(ss.at(3));
			minX = min(minX,x);
			maxX = max(maxX, x);
			minY = min(minY, y);
			maxY = max(maxY, y);
			minZ = min(minZ, z);
			maxZ = max(maxZ, z);

			points->push_back(newPoint(x, y, z));
			ss.clear();

		}
		else if (line.compare("") != 0 && line.at(0) == 'l')
		{
			SplitString(line, ss, " ");
			for (unsigned int i = 1; i<ss.size() - 1; i++)
			{
				Line_t * tempLine = newLine(points->at(stoi(ss.at(i)) - 1) , points->at(stoi(ss.at(i + 1)) - 1), id_count++);
				char lineStrBuff[100];
				sprintf(lineStrBuff, "Line: %lf %lf %lf -> %lf %lf %lf\n", tempLine->endpoint[0]->x
					, tempLine->endpoint[0]->y, tempLine->endpoint[0]->z
					, tempLine->endpoint[1]->x, tempLine->endpoint[1]->y, tempLine->endpoint[1]->z);
				resetStatusBar(lineStrBuff);

				currentLineSet->lineSet.push_back(tempLine);

				char buffer[33];

				_itoa(tempLine->id, buffer, 10);

				lineSetListbox->add_item(tempLine->id, buffer);
			}
			ss.clear();
		}
	}
	if (points->size() == 0){
		printf("File error!!!\n");
		exit(1);
	}


	fin.close();
	delete points;
}

void reCache(){
	currentLineSet = newLineSet();
}
bool readLineSet(char* objFileName){

	if (objFileName){
		reCache();
		string filename(objFileName);
		cout << filename << endl;

		if (filename.length() == 0)
		{
			char outputBuffer[1000];
			sprintf(outputBuffer, "Invalid Input!");
			resetStatusBar(outputBuffer);
			return false;
		}

		cout << filename << endl;
		readFromObjFile(filename);

		char outputBuffer[1000];
		sprintf(outputBuffer, "Input Sucess as: %s", filename.c_str());
		resetStatusBar(outputBuffer);
		return true;
	}
	return false;
}

void readLineSet2()
{
	reCache();
	string sourceFolder = "data/";//output_1
	string filename(outputFilename);
	if (filename.length() == 0)
	{
		char outputBuffer[1000];
		sprintf(outputBuffer, "Invalid Input!");
		resetStatusBar(outputBuffer);
		return;
	}
	string prefix = ".obj";

	string inputPath = sourceFolder+filename+prefix;

	cout << inputPath << endl;
	readFromObjFile(inputPath);

	char outputBuffer[1000];
	sprintf(outputBuffer, "Input Sucess as: %s", inputPath.c_str());
	resetStatusBar(outputBuffer);
}



