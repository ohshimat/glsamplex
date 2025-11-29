/******************************************************************************
 * glsample_frameLv2.cpp 
 * Level 2 - introducing structure */

#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

//-------- common prototypes for GLUT callback functions
void display( void );
void update( void );
void mouse( int button , int state, int x, int y );
void keyboard( unsigned char key, int x, int y );
void reshape( int width, int height );

//-------- common prototypes for user functions
void InitScene( void );
void UpdateScene( void );
void DrawScene( void );
void initWindow( char *title );

//-------- common window parameters
typedef struct {
    int xo;
    int yo;
    int width;
    int height;
    float aspect;
	char *title;
} WindowDataT;

WindowDataT window;

//-------- prototypes for user functions
void setLight( void );
void setMaterial( void );
void drawWireCube( void );
void drawSolidSphere( void );

//-------- simulation data
typedef struct {
    float t; // time parameter [sec]
    float dt; // frame time [sec]

    float rot; // rotation angle
    int rot_mode; // rotation mode - 0:stop, 1:ccw, -1:cw
} SimDataT;

SimDataT simdata;

/*----------------------------------------------------------------- setLight
 * setLight:
 *--------*/
void setLight( void )
{
    float light_color[] = { 1.0, 1.0, 1.0, 1.0 };
    float light_pos[] = { 0.0, 1.0, 1.0, 0.0 };
  
    glLightfv( GL_LIGHT0, GL_DIFFUSE, light_color );
    glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
    return;
}
/*-------------------------------------------------------------- setMaterial
 * setMaterial:
 *--------*/
void setMaterial( void )
{
    float diffuse[] = { 0.6, 0.6, 0.8, 1.0 };
    float specular[] = { 0.9, 0.9, 0.9, 1.0 };

    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT, GL_SHININESS, 64.0 );
    return;
}
/*---------------------------------------------------------------- InitScene
 * InitScene:
 *--------*/
void InitScene( void )
{
    simdata.t = 0.0;
    simdata.dt = 0.0167;

    simdata.rot = 0.0;
    simdata.rot_mode = 0;
    return;
}
/*-------------------------------------------------------------- UpdateScene
 * UpdateScene:
 *--------*/
void UpdateScene( void )
{
	simdata.t += simdata.dt;

    switch( simdata.rot_mode ){
      case 1:
        simdata.rot += 1.0;
        break;
      case 0:
        break;
      case -1:
        simdata.rot -= 1.0;
        break;
    }
    return;
}
/*------------------------------------------------------------------ drawObj
 * drawWireCube:
 *--------*/
void drawWireCube( void )
{
    glColor3f( 1.0, 1.0, 1.0 );
    glutWireCube( 1.0 );
    return;
}
/*------------------------------------------------------------------ drawObj
 * drawSolidSphere:
 *--------*/
void drawSolidSphere( void )
{
    setMaterial();
    glutSolidSphere( 0.5, 20, 25 );
    return;
}
/*---------------------------------------------------------------- DrawScene
 * DrawScene:
 *--------*/
void DrawScene( void )
{
    setLight();

	glEnable( GL_DEPTH_TEST ); // ---- begin: 
    glPushMatrix();
    {
		//---- begin: global transformation
        glRotatef( simdata.rot, 0.0, 1.0, 0.0 );
		//---- end

		//---- begin: draw non-shaded objects
        drawWireCube();
		//---- end

		//---- begin: shaded objects
        glEnable( GL_LIGHTING );
        glEnable( GL_LIGHT0 );
        
        drawSolidSphere();

        glDisable( GL_LIGHT0 );
        glDisable( GL_LIGHTING );
		//---- end
    }
    glPopMatrix();
    glDisable( GL_DEPTH_TEST );

    return;
}
/*------------------------------------------------------------------- update
 * update - GLUT idle callback function
 *--------*/
void update( void )
{
    UpdateScene();

	//-------- time count

	return;
}
/*---------------------------------------------------------------- initWindow
 * initWindow: initializes OpenGL window
 *--------*/
void initWindow( char *winname )
{
	window.xo = 100;
	window.yo = 100;
	window.width = 800;
	window.height = 600;
	window.aspect = (float)window.width/window.height; // aspect ratio
	window.title = winname;

	//-------- window properties --------
	glutInitWindowPosition( window.xo, window.yo );
    glutInitWindowSize( window.width, window.height );

    //-------- config buffers --------
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	//-------- open window --------
	glutCreateWindow( window.title );

	//-------- background color --------
    glClearColor( 0.25, 0.25, 0.4, 1.0 );

	glEnable( GL_NORMALIZE );
	glShadeModel( GL_FLAT );
    return;
}
/*------------------------------------------------------------------- display
 * display - GLUT display callback function
 *--------*/
void display( void )
{
    //-------- viewport --------
	glViewport( 0, 0, window.width, window.height );

    //-------- clear buffers --------
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//-------- projection transformation --------
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
	//---- begin projection transformation
    gluPerspective( 45.0, window.aspect, 0.1, 10.0 );
	//---- end
    glMatrixMode( GL_MODELVIEW );

	//-------- viewing transformation --------
    glLoadIdentity();
	//---- begin viewing transformation
    gluLookAt( 0.0, 1.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
	//---- end

    //-------- draw --------
    DrawScene();

	//-------- ready to redraw
	glutPostRedisplay();

	//-------- swapbuffers --------
    glutSwapBuffers();

    return;
}
/*----------------------------------------------------------------- keyboard
 * keyboard - GLUT keyboard callback function
 * key: ASCII code
 * x, y: mouse position
 *--------*/
void keyboard( unsigned char key, int x, int y )

{
    switch( key ){
      case 'h': // help
        printf( "Instruction\n" );
        printf( "[H]:Help\n" );
        printf( "[Q]:Quit\n" );
        printf( "[0]:Stop rotation\n" );
        printf( "[1]:Start rotation(CCW)\n" );
        printf( "[2]:Start rotation(CW)\n" );
        printf( "[MOUSE-LEFT  ]:Start rotation(CCW)\n" );
        printf( "[MOUSE-CENTER]:Stop rotation\n" );
        printf( "[MOUSE-RIGHT ]:Start rotation(CW)\n" );
        break;
      case 'q': // quit
        exit(0);
      case '0': // stop rotation
        simdata.rot_mode = 0;
        break;
      case '1': // CCW rotation
        simdata.rot_mode = 1;
        break;
      case '2': // CW rotation
        simdata.rot_mode = -1;
        break;
	  default:
        break;
    }
    return;
}
/*-------------------------------------------------------------------- mouse
 * mouse - GLUT mouse callback function
 * button: mouse button
 * stateÅFpress or release
 * x, yÅFmouse position
 *--------*/
void mouse( int button , int state, int x, int y )

{
    switch( button ){
      case GLUT_LEFT_BUTTON:
        switch( state ){
          case GLUT_DOWN: // press
            simdata.rot_mode = 1;
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
            simdata.rot_mode = 0;
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
            simdata.rot_mode = -1;
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
/*--------------------------------------------------------------------- reshape
 * reshape - GLUT reshape callback function
 *--------*/
void reshape( int width, int height )
{
	window.width = width;
	window.height = height;
}
/*---------------------------------------------------------------------- main
 * main
 *--------*/
int main( int argc, char *argv[] )
{
    glutInit( &argc, argv );

    initWindow( argv[0] );

    glutDisplayFunc( update );  // display callback function
    glutIdleFunc( display );    // idle callback function
	glutReshapeFunc( reshape ); // reshape callback function
    glutKeyboardFunc(keyboard); // keyboard callback function
    glutMouseFunc(mouse);       // mouse callback function
    printf( "[H]:Help\n" );     // indicate help instruction

    InitScene();

    glutMainLoop(); // run main loop

    return 0;
}
/******************************************************************************
 * end of source code */
