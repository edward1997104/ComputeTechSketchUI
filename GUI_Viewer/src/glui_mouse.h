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
// All rights reserved
// 
///////////////////////////////////////////////////////////////




#ifndef _ND_MOUSE_H
#define _ND_MOUSE_H

////////////////////////////////////////////
// External coordinate function

extern void getMouseWorldPos(int x, int y, double * output);
extern void getProjectionVector(int x, int y, double outputVector[2][3]);

////////////////////////////////////////////
// Mouse Button and Motion Callback

extern void mouse  ( int button, int state, int x, int y ) ;
extern void motion ( int x, int y ) ;
extern void passiveMotion(int x, int y);
extern double myTime();

////////////////////////////////////////////
// Idle Callback

extern void idle();
extern void stopIdleMotion();



#endif
