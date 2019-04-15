///////////////////////////////////////////////////////////////
//
// glui_build.cpp
//
// - building the generic GLUI interface
//
// Philip Fu
// 
///////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef WIN32
#include <windows.h>
#include <float.h>
#else
#include <limits.h>
#endif

#include "grid.h"
#include "lines.h"

#include "glui_build.h"
#include "glui_template.h"
#include "glui.h"
#include "standard.h"




////////////////////////////////////////////////////////
// (1) Global Variables


  #define _UNKNOWN	"---"


  //////////////////////////////////////////////
  // (A) External from driver program

  // window
  extern int winW,winH;
  extern int mainWindowID;
  extern int backgndColor[3];

  // external state variables
  extern float currFovy;
  extern int   showAxes;
  extern int   showStatusBar;

  extern int   draw2D;
  extern int   xySelected;
  extern int   yzSelected;
  extern int   xzSelected;

  // external grid

  extern Grid_t * xyGrid;
  extern Grid_t * yzGrid;
  extern Grid_t * xzGrid;

  // perspective
  extern int perspective;

  // line history
  extern LineSetHistory_t * lineHistoryStack;

  //////////////////////////////////////////////
  // (B) Internal

  // using GLUI
  GLUI *glui          = NULL;
  GLUI *gluiStatusBar = NULL;

  // list of rollout
  GLUI_Rollout *rolloutList[_MAX_ROLLOUT];
  int  rolloutStart;
  int  nRolloutList  = 0;
  int  rolloutOffset = 0;
  char rolloutChar[_MAX_ROLLOUT];

  // Status Bar
  GLUI_StaticText *statusBar;
  char             statusStr[_MAX_STR_SIZE];

  // Checkbox
  GLUI_Checkbox * xyCheckbox;
  GLUI_Checkbox * yzCheckbox;
  GLUI_Checkbox * xzCheckbox;

  // listbox
  GLUI_Listbox * lineSetListbox;
  int listboxVal;

  int interval = (int)((GRID_SIZE + 1e-5) / GRID_INTERVAL);

  char outputFilename[100]; 

////////////////////////////////////////////////////////
// ROTATING THE ROLLOUTS in Main Panel
////////////////////////////////////////////////////////


void
closeAllRollouts()
{
    int i;

    for (i=0; i<nRolloutList; i++)
	rolloutList[i]->close();
}


static void
resetPanels()
{
    int offset,i;

    // remove all the rollouts
    while ( glui->remove_control(glui->get_main_panel(),rolloutStart,false) ) ;

    // current starting point
    offset = rolloutOffset;

    // add all the rollouts
    for (i=0; i<nRolloutList; i++) {

	glui->add_separator(false,0);

	glui->add_control(glui->get_main_panel(),rolloutList[offset],true);
	offset = (offset+1) % nRolloutList;
    }
}


void
rotatePanelsOffset(int value)
{
    // any rollout?
    if (!nRolloutList)
	return;

    // rollout offset
    rolloutOffset += value;

    // make sure it is positive
    if (rolloutOffset >= nRolloutList)
        rolloutOffset -= ((rolloutOffset-nRolloutList) / nRolloutList + 1) * nRolloutList;
    if (rolloutOffset < 0)
        rolloutOffset += ((-rolloutOffset) / nRolloutList + 1) * nRolloutList;

    // rotate the rollout panels
    resetPanels();
}


int
rotatePanelsTo(char key)
{
    GLUI_Rollout *tmpControl,*tmp2Control;

    int index,i,from,tmpCH,tmp2CH;


    // any rollout?
    if (!nRolloutList)
	return _FALSE;


    // 1. find the index

    // to lowercase
    if ('A' <= key && key <= 'Z')
	key = key-'A'+'a';

    // index to the selected stuff
    index = -1;
    for (i=0; i<nRolloutList; i++) 
	if (rolloutChar[i] == key)
	    index = i;

    // cannot find the key?
    if (index == -1)
	return _FALSE;


    // 2. close the rollout if it is currently open

    if (rolloutList[index]->is_open) {
	rolloutList[index]->close();
	return _TRUE;
    }


    // 3. rearrange rolloutList and rolloutChar consistently

    tmpControl = rolloutList[index];
    tmpCH      = rolloutChar[index];
    from       = rolloutOffset;

    if (from != index) {

	from--;

	do {

	    from = (from+1) % nRolloutList;

	    // swap(tmpControl,rolloutList[from]);
	    tmp2Control       = tmpControl;
	    tmpControl        = rolloutList[from];
	    rolloutList[from] = tmp2Control;

	    // swap(tmpCH,rolloutList[from]);
	    tmp2CH            = tmpCH;
	    tmpCH             = rolloutChar[from];
	    rolloutChar[from] = tmp2CH;

	} while (from != index);
    }


    // 4. reset the panel

    resetPanels();


    // 5. open this panel

    rolloutList[rolloutOffset]->open();


    return _TRUE;
}




////////////////////////////////////////////////////////
// ADD PANELS
////////////////////////////////////////////////////////

static void
addBasicPanel(GLUI_Panel *basicAdjPanel)
{
    glui->add_separator_to_panel(basicAdjPanel,false,1);


    //////////////////////////////////////////////
    // Button

    glui->add_separator_to_panel(basicAdjPanel,false,-10);

    GLUI_Panel *subPanel = glui->add_panel_to_panel(basicAdjPanel,"",GLUI_PANEL_NONE);

    GLUI_Button *resetViewButton
	= glui->add_button_to_panel(subPanel,"(R)esetView",ID_RESETVIEW,callbackGLUI);
    glui->add_column_to_panel(subPanel,false);
    GLUI_Button *dumpscrnButton
	= glui->add_button_to_panel(subPanel,"(D)umpscreen",ID_DUMPSCREEN,callbackGLUI);

    resetViewButton->set_w(74,true);
    resetViewButton->set_alignment(GLUI_ALIGN_LEFT);
    dumpscrnButton->set_w(83,true);
    dumpscrnButton->set_alignment(GLUI_ALIGN_LEFT);


    //////////////////////////////////////////////
    // Change FOVY

    glui->add_separator_to_panel(basicAdjPanel,false,-8);
    GLUI_Slider *fovy_slider
	= glui->add_slider_to_panel( basicAdjPanel,
	                             "Fovy:",
	                             GLUI_SLIDER_FLOAT,
	                             &currFovy,
	                             (float) 0.0f, 179.9f,
	                             ID_FOVY,
	                             callbackGLUI );
    fovy_slider->set_speed( 1.0f );
    fovy_slider->set_w(160,true);
    fovy_slider->set_alignment( GLUI_ALIGN_CENTER );

	glui->add_separator_to_panel(basicAdjPanel, false, 2);
	yzCheckbox = glui->add_checkbox_to_panel(basicAdjPanel,
		"perspective:",
		&perspective,
		ID_PERSPECTIVE,
		callbackGLUI);

    //////////////////////////////////////////////
    // Show/Hide stuff

    glui->add_separator_to_panel(basicAdjPanel,false,2);

    glui->add_checkbox_to_panel( basicAdjPanel,
	                         "show axes (a)",
	                         &showAxes,
	                         ID_SHOWAXES,
	                         callbackGLUI );


    //////////////////////////////////////////////
    // Change Background Colors

    glui->add_separator_to_panel(basicAdjPanel,false,2);

    GLUI_Rollout *subRollout = glui->add_rollout_to_panel(basicAdjPanel,"Background Color",GLUI_PANEL_NONE);
    subRollout->set_w(160,true);

    GLUI_Spinner *backgndR_spinner =
	glui->add_spinner_to_panel( subRollout,
	                            "  backgnd red",
	                            GLUI_SPINNER_INT,
	                            &(backgndColor[0]),
	                            ID_CHANGE_BGCOLOR,
	                            callbackGLUI );
    backgndR_spinner->set_alignment( GLUI_ALIGN_CENTER );
    backgndR_spinner->set_int_limits( 0, 255 );

    GLUI_Spinner *backgndG_spinner =
	glui->add_spinner_to_panel( subRollout,
	                            "backgnd green",
	                            GLUI_SPINNER_INT,
	                            &(backgndColor[1]),
	                            ID_CHANGE_BGCOLOR,
	                            callbackGLUI );
    backgndG_spinner->set_alignment( GLUI_ALIGN_CENTER );
    backgndG_spinner->set_int_limits( 0, 255 );

    GLUI_Spinner *backgndB_spinner =
	glui->add_spinner_to_panel( subRollout,
	                            " backgnd blue",
	                            GLUI_SPINNER_INT,
	                            &(backgndColor[2]),
	                            ID_CHANGE_BGCOLOR,
	                            callbackGLUI );
    backgndB_spinner->set_alignment( GLUI_ALIGN_CENTER );
    backgndB_spinner->set_int_limits( 0, 255 );


    glui->add_separator_to_panel(basicAdjPanel,false,0);
}

static void
addAppPanel(GLUI_Panel *panel)
{
    glui->add_separator_to_panel(panel,false,1);


	///////////////////////////////////////////
	// select xy-plane checkbox
	glui->add_separator_to_panel(panel, false, 2);
	xyCheckbox = glui->add_checkbox_to_panel(panel,
		"Using 2D Drawing",
		&draw2D,
		ID_TOGGLE_DRAW,
		callbackGLUI);

	glui->add_separator_to_panel(panel,false,2);
	xyCheckbox = glui->add_checkbox_to_panel( panel,
	                         "select xy-plane (xy)",
	                         &xySelected,
	                         ID_XY_SELECT,
							 callbackGLUI);
	

	///////////////////////////////////////////
	// xy-plane offset slider


	glui->add_separator_to_panel(panel,false,-8);
    GLUI_Slider *xy_slider
	= glui->add_slider_to_panel( panel,
	                             "xy-plane offset:",
	                             GLUI_SLIDER_INT,
								 &(xyGrid->offsetInt),
								 -interval, interval,
	                             ID_XY_OFFSET,
	                             callbackGLUI );
	xy_slider->set_int_limits(-interval, interval);
	xy_slider->set_int_val(0);
    xy_slider->set_speed( 1 );
    xy_slider->set_w(160,true);
    xy_slider->set_alignment( GLUI_ALIGN_CENTER );


	///////////////////////////////////////////
	// select yz-plane checkbox


	glui->add_separator_to_panel(panel,false,2);
	yzCheckbox = glui->add_checkbox_to_panel(panel,
	                         "select yz-plane (yz)",
	                         &yzSelected,
	                         ID_YZ_SELECT,
	                         callbackGLUI );


	///////////////////////////////////////////
	// yz-plane offset slider


	glui->add_separator_to_panel(panel,false,-8);
    GLUI_Slider *yz_slider
	= glui->add_slider_to_panel( panel,
	                             "yz-plane offset:",
								 GLUI_SLIDER_INT,
								 &(yzGrid->offsetInt),
								 -interval, interval,
	                             ID_YZ_OFFSET,
	                             callbackGLUI );
	yz_slider->set_int_limits(-interval, interval);
	yz_slider->set_int_val(0);
	yz_slider->set_speed(1);
    yz_slider->set_w(160,true);
    yz_slider->set_alignment( GLUI_ALIGN_CENTER );


	///////////////////////////////////////////
	// select xz-plane checkbox


	glui->add_separator_to_panel(panel,false,2);
	xzCheckbox = glui->add_checkbox_to_panel(panel,
	                         "select xz-plane (xz)",
	                         &xzSelected,
	                         ID_XZ_SELECT,
	                         callbackGLUI );


	///////////////////////////////////////////
	// xz-plane offset slider


	glui->add_separator_to_panel(panel,false,-8);
    GLUI_Slider *xz_slider
	= glui->add_slider_to_panel( panel,
	                             "xz-plane offset:",
								 GLUI_SLIDER_INT,
								 &(xzGrid->offsetInt),
								 -interval, interval,
	                             ID_XZ_OFFSET,
	                             callbackGLUI );
	xz_slider->set_int_limits(-interval, interval);
	xz_slider->set_int_val(0);
	xz_slider->set_speed(1);
    xz_slider->set_w(160,true);
    xz_slider->set_alignment( GLUI_ALIGN_CENTER );


    glui->add_separator_to_panel(panel,false,2);

	///////////////////////////////////////////
	// line set list

	glui->add_separator_to_panel(panel, false, 2);

	lineSetListbox = glui->add_listbox_to_panel(panel, "line drawn:", &listboxVal, ID_LINE_SET, callbackGLUI);

	lineSetListbox->add_item(-1, "NULL");

	///////////////////////////////////////////
	// undo button

	glui->add_separator_to_panel(panel, false, 2);

	glui->add_button_to_panel(panel, "UNDO", ID_UNDO, callbackGLUI);

	///////////////////////////////////////////
	// edit text
	glui->add_separator_to_panel(panel, false, 2);

	glui->add_edittext_to_panel(panel, "Output filename:", GLUI_EDITTEXT_TEXT, outputFilename, -1, callbackGLUI);

	GLUI_Button *readSketchButton = glui->add_button_to_panel(panel, "Read Sketch", ID_READ_SKETCH, callbackGLUI);
	glui->add_separator_to_panel(panel, false, 2);

	///////////////////////////////////////////
	// edit text
	glui->add_separator_to_panel(panel, false, 2);

	glui->add_button_to_panel(panel, "Output", ID_OUTPUT, callbackGLUI);

}




////////////////////////////////////////////////////////
// MAIN INTERFACE BUILDER
////////////////////////////////////////////////////////

static void
buildInterface()
{
    ///////////////////////////////////////////
    // (1) This is a control panel

    glui->add_separator(false,2);
    glui->add_separator();

    GLUI_StaticText *infoText
	= glui->add_statictext( "CONTROL PANEL" );
    infoText->set_alignment( GLUI_ALIGN_CENTER );

    glui->add_separator();


    // Quit / Help / About

    glui->add_separator(false,-5);

    GLUI_Panel *lastPanel
	= glui->add_panel( "", GLUI_PANEL_NONE );

    glui->add_column_to_panel(lastPanel,false);
    GLUI_Button *quitButton
	= glui->add_button_to_panel(lastPanel,"Quit",ID_QUIT,callbackGLUI);
    glui->add_column_to_panel(lastPanel,false);
    GLUI_Button *helpButton
	= glui->add_button_to_panel(lastPanel,"Help",ID_HELP,callbackGLUI);
    glui->add_column_to_panel(lastPanel,false);
    GLUI_Button *aboutButton
	= glui->add_button_to_panel(lastPanel,"About",ID_ABOUT,callbackGLUI);
    glui->add_column_to_panel(lastPanel,false);
    GLUI_Button *menuButton_down
	= glui->add_button_to_panel(lastPanel,"-",ID_CLOSE_PANELS,callbackGLUI);
    glui->add_column_to_panel(lastPanel,false);
    GLUI_Button *menuButton_up
	= glui->add_button_to_panel(lastPanel,"m",ID_ROTATE_PANELS,callbackGLUI);

    quitButton->set_w(41,41);
    helpButton->set_w(41,41);
    aboutButton->set_w(41,41);
    menuButton_up->set_w(12,12);
    menuButton_down->set_w(12,12);

    quitButton->set_alignment(GLUI_ALIGN_CENTER);
    helpButton->set_alignment(GLUI_ALIGN_CENTER);
    aboutButton->set_alignment(GLUI_ALIGN_CENTER);
    menuButton_up->set_alignment(GLUI_ALIGN_CENTER);
    menuButton_down->set_alignment(GLUI_ALIGN_CENTER);

    glui->add_separator(false,-14);

    glui->add_separator(false,0);

    rolloutStart = (glui->get_main_panel())->get_num_childs();


    ///////////////////////////////////////////
    // (2) Basic Panel

    glui->add_separator(false,0);

    GLUI_Rollout *rollout1 = glui->add_rollout("[B]asic Control",false);
    rollout1->set_w( 177, true );

    addBasicPanel(rollout1);

    // add to rollout list
    rolloutChar[nRolloutList] = 'b';
    rolloutList[nRolloutList++] = (GLUI_Rollout *) rollout1;


    ///////////////////////////////////////////
    // (3) Application Panel

    glui->add_separator(false,0);

    GLUI_Rollout *rollout2 = glui->add_rollout("[A]pplication Control",false);
    rollout2->set_w( 177, true );

    addAppPanel(rollout2);

    // add to rollout list
    rolloutChar[nRolloutList] = 'a';
    rolloutList[nRolloutList++] = (GLUI_Rollout *) rollout2;


    ///////////////////////////////////////////
    // (4) Status Bar

    statusBar = gluiStatusBar->add_statictext(statusStr);

    statusBar->set_w( winW-10 );
    statusBar->set_alignment(GLUI_ALIGN_LEFT);
}


void
initGLUI()
{
    int tx,ty,tw,th;

    // create the GLUI panel
    glui = GLUI_Master.create_glui_subwindow(mainWindowID,GLUI_SUBWINDOW_RIGHT);
    glui->bkgd_color.set(GLUI_BGCOLOR);
    glui->fogd_color.set(GLUI_FGCOLOR);

    // create the status bar
    gluiStatusBar = GLUI_Master.create_glui_subwindow(mainWindowID,GLUI_SUBWINDOW_BOTTOM);
    gluiStatusBar->bkgd_color.set(STATUSBAR_BGCOLOR);
    gluiStatusBar->fogd_color.set(STATUSBAR_FGCOLOR);

    // create the controls/widgets
    buildInterface();

    // who is the main window
    glui->set_main_gfx_window( mainWindowID );
    gluiStatusBar->set_main_gfx_window( mainWindowID );

    // reset the window size
    GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
    //winW += (winW - tw);
    glutReshapeWindow(winW*2-tw,winH*2-th);
}

bool GetOpenFileName2(HWND hWnd, LPSTR szFile, int StringSize)
{
	OPENFILENAME   ofn;
	char   szFileTitle[256];
	strcpy(szFileTitle, "Open File");
	szFile[0] = 0;

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = "All\0*.*\0.obj\0*.obj\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = StringSize;
	ofn.lpstrTitle = szFileTitle;
	ofn.Flags = OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) != TRUE)
	{
		DWORD   Errval;
		char   Errstr[50] = "Common   Dialog   Error:   ";
		char   buf[5];
		Errval = CommDlgExtendedError();
		if (Errval != 0)
		{
			wsprintf(buf, "%ld ", Errval);
			strcat(Errstr, buf);
			MessageBox(NULL, Errstr, "Warning ", MB_OK | MB_ICONSTOP);
		}
		return false;
	}

	return   true;
}

char* readFile()
{
	HWND hWnd = GetForegroundWindow();
	char objFileName[256];

	if (GetOpenFileName2(hWnd, objFileName, 256))
	{
		return objFileName;
	}
}

void doReadSketch(){
	char* filename = readFile();
	bool result = readLineSet(filename);
}


///////////////////////////////////////////////////////////////
// Reset file, Views, and Textures
///////////////////////////////////////////////////////////////

void
resetStatusBar(const char * fmt, ... )
{
    va_list argp;
    char    str[_MAX_STR_SIZE];


    // 1. string to be put onto the status bar

    va_start(argp, fmt);
    vsprintf(str, fmt, argp);
    va_end(argp);


    // 2. put it on the status bar if it is not on the status bar now

    if (strcmp(str,statusStr)) {
	strcpy(statusStr,str);
	if (statusBar) statusBar->set_text(statusStr);
    }
}




////////////////////////////////////////////////////////
// GLUI CALLBACK
////////////////////////////////////////////////////////

void
callbackGLUI(int id)
{
    void myExit(int exitCode);
    void myHelp();
    void myAbout();
    void resetStatusBar(const char * fmt, ... );
    void resetProj();
    void resetView();
    void dumpScreen();
	void checkboxCallBack(int selectedPlane);
	void sliderCallBack(int selectedPlane);

    int winID;


    // Make sure the view is mainWindow
    winID = glutGetWindow();
    if (winID != mainWindowID)
	glutSetWindow(mainWindowID);

    switch(id) {


      ////////////////////////////////////////////////////////////////////
      // QUIT / HELP / ABOUT
      ////////////////////////////////////////////////////////////////////

      case ID_QUIT :
	myExit(EXIT_SUCCESS);
	break;

      case ID_HELP :
	myHelp();
	break;

      case ID_ABOUT :
	myAbout();
	break;

      case ID_ROTATE_PANELS :
	rotatePanelsOffset(1);
	resetStatusBar("cycle control panel.");
	break;

      case ID_CLOSE_PANELS :
	closeAllRollouts();
	resetStatusBar("all panels closed.");
	break;


      ////////////////////////////////////////////////////////////////////
      // BASIC ADJUSTMENT PANEL
      ////////////////////////////////////////////////////////////////////

      case ID_RESETVIEW :
	resetStatusBar("reset view.");
	resetView();
	glutPostRedisplay();
	break;

      case ID_DUMPSCREEN :
	dumpScreen();
	break;

      case ID_FOVY :
	resetStatusBar("changing fovy.");
	resetProj();
	glutPostRedisplay();
	break;

      case ID_CHANGE_BGCOLOR :
	resetStatusBar("Background Color changed");
	glClearColor( backgndColor[0]/255.0f, 
	              backgndColor[1]/255.0f, 
	              backgndColor[2]/255.0f, 0.0f );
	glutPostRedisplay();
	break;

      case ID_SHOWAXES :
	if (showAxes)
	    resetStatusBar("show axes");
	else
	    resetStatusBar("hide axes");
	glutPostRedisplay();
	break;


      ////////////////////////////////////////////////////////////////////
      // APPLICATION PANEL
      ////////////////////////////////////////////////////////////////////
	  case ID_XY_SELECT:
		  checkboxCallBack(XY_SELECTED);
		  break;
	  case ID_XY_OFFSET:
		  sliderCallBack(XY_SELECTED);
		  break;
	  case ID_YZ_SELECT:
		  checkboxCallBack(YZ_SELECTED);
		  break;
	  case ID_YZ_OFFSET:
		  sliderCallBack(YZ_SELECTED);
		  break;
	  case ID_XZ_SELECT:
		  checkboxCallBack(XZ_SELECTED);
		  break;
	  case ID_XZ_OFFSET:
		  sliderCallBack(XZ_SELECTED);
		  break;
	  case ID_UNDO:
		  undoAction();
		  break;
	  case ID_OUTPUT:
		  outputLineSet();
		  break;
	  case ID_PERSPECTIVE:
		  resetView();
		  glutPostRedisplay();
		  break;
	  case ID_TOGGLE_DRAW:
		  if (draw2D)
		  {
			  resetView();
//			  while (!lineHistoryStack->history.empty())
//				  undoAction();
		  }
		  break;
	case ID_READ_SKETCH:
		doReadSketch();
		glutPostRedisplay();
		break;
    }


    if (winID != mainWindowID)
	glutSetWindow(winID);
}


void checkboxCallBack(int selectedPlane)
{
	switch (selectedPlane)
	{
	case XY_SELECTED:
		printf("xy is selected!\n");
		clearSelectedPoint();

		yzSelected = 0;
		yzCheckbox->draw_unchecked();

		xzSelected = 0;
		xzCheckbox->draw_unchecked();
		break;
	case YZ_SELECTED:
		printf("yz is selected!\n");
		clearSelectedPoint();

		xySelected = 0;
		xyCheckbox->draw_unchecked();

		xzSelected = 0;
		xzCheckbox->draw_unchecked();
		break;
	case XZ_SELECTED:
		printf("xz is selected!\n");
		clearSelectedPoint();

		xySelected = 0;
		xyCheckbox->draw_unchecked();

		yzSelected = 0;
		yzCheckbox->draw_unchecked();
		break;
	}
}

void sliderCallBack(int selectedPlane)
{
	switch (selectedPlane)
	{
	case XY_SELECTED:
		clearSelectedPoint();
		xyGrid->offset = (GRID_INTERVAL * (xyGrid->offsetInt));
		printf("xy offset: %lf\n", xyGrid->offset);
		break;
	case YZ_SELECTED:
		clearSelectedPoint();
		yzGrid->offset = (GRID_INTERVAL * (yzGrid->offsetInt));
		printf("yz offset: %lf\n", yzGrid->offset);
		break;
	case XZ_SELECTED:
		clearSelectedPoint();
		xzGrid->offset = (GRID_INTERVAL * (xzGrid->offsetInt));
		printf("xz offset: %lf\n", xzGrid->offset);
		break;
	default:
		break;
	}
}