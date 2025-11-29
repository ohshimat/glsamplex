/******************************************************************************
 * keybdmouse.cpp: keyboard and mouse callback functions 
 */
#include "platform.h"

#include "common.h"

#include "sim.h"
#include "calc.h"

extern SimDataT simdata;
extern WindowDataT window;

//-------- modifier key status
bool isShiftKeyDown( void );
bool isCtrlKeyDown( void );
bool isAltKeyDown( void );

/*-------------------------------------------------- modifier key status
 * Shift, Ctrl, Alt key status
 */
bool isShiftKeyDown( void )
{
	if(glutGetModifiers() & GLUT_ACTIVE_SHIFT)
		return true;
	else
		return false;
}
bool isCtrlKeyDown( void )
{
	if(glutGetModifiers() & GLUT_ACTIVE_CTRL)
		return true;
	else 
		return false;
}
bool isAltKeyDown( void )
{
	if(glutGetModifiers() & GLUT_ACTIVE_ALT)
		return true;
	else
		return false;
}

/*------------------------------------------------------------- charKeyDown/Up
 * charKeyDown/Up - GLUT keyboard callback function
 * key: ASCII code
 * x, y: mouse position
 *--------*/
void charKeyDown( unsigned char key, int x, int y )
{
    switch( key ){
      case 'h': // help
        printf( "Instruction\n" );
        printf( "[H]:Help\n" );
        printf( "[Q]:Quit\n" );
        break;
      case 'q': // quit
        exit(0);
        break;

	  case ' ': //スペースキーをおしたら
		  if (simdata.cube.state == 0) {
			  simdata.cube.state = 1;
			  setObjColor(&simdata.cube, 1.0, 0.0, 0.0);
		  }

		break;

      default:
        break;
    }
    return;
}
void charKeyUp( unsigned char key, int x, int y )
{
    switch( key ){
	  default:
		break;
	}
}
/*--------------------------------------------------------------- funcKeyDown/Up
 * funcKeyDown/Up - GLUT special key callback
 * key: GLUT key code
 * x, y: mouse position
 */
void funcKeyDown( int key, int x, int y )
{
	//ファンクションキー
	//[F*]: GLUT_KEY_F*(*は1～12)
	//[F1]: GLUT_KEY_F1
	//[F12]:GLUT_KEY_F12
	//その他の機能キー
    //[PageUp]  : GLUT_KEY_PAGE_UP:
    //[PageDown]: GLUT_KEY_PAGE_DOWN:
    //[Home]    : GLUT_KEY_HOME:
    //[End]     : GLUT_KEY_END:
    //[Insert]  : GLUT_KEY_INSERT:

	//カーソルキー
	switch( key ){
	  case GLUT_KEY_LEFT: //[←]

		break;
  	  case GLUT_KEY_RIGHT://[→]

		break;
	  case GLUT_KEY_UP: //[↑]

		break;
	  case GLUT_KEY_DOWN://[↓]

		break;
	}
}
void funcKeyUp( int key, int x, int y )
{
	switch( key ){
	  case GLUT_KEY_LEFT: //[←]

		break;
  	  case GLUT_KEY_RIGHT://[→]

		break;
	  case GLUT_KEY_UP: //[↑]

		break;
	  case GLUT_KEY_DOWN://[↓]

		break;
	}
}
/*------------------------------------------------------------- mouseClick
 * mouseClick - GLUT mouse callback function
 * button: mouse button
 * state：press or release
 * x, y：mouse position
 *--------*/
void mouseClick( int button , int state, int x, int y )
{
    switch( button ){
      case GLUT_LEFT_BUTTON:
        switch( state ){
          case GLUT_DOWN: // press
        
            break;
          case GLUT_UP: // release

            break;
          default:
            break;
        }
        break;
      case GLUT_MIDDLE_BUTTON:
        switch( state ){
          case GLUT_DOWN: // press

            break;
          case GLUT_UP: // release

            break;
          default:
            break;
        }
        break;
      case GLUT_RIGHT_BUTTON:
        switch( state ){
          case GLUT_DOWN: // press

            break;
          case GLUT_UP: // release

            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    return;
}
/*---------------------------------------------------------------- mouseDrag 
 * mouseDrag - mouse drag callback function
 */
void mouseDrag( int x, int y )
{
	////////


	////////
    return;
}
/*-------------------------------------------------------------- mouseMotion
 * mouseMotion - passive mouse motion callback function
 */
void mouseMotion( int x, int y )
{
	////////


	////////
    return;
}