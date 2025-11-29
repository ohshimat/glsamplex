#include <stdio.h>

#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <GL/glut.h>

typedef struct{
	float x, y;
} dot_t;

typedef struct{
	float min, max;
} range_t;

float param = 0.0;

bool preview = false;

//func0
dot_t p0 = {4.0, 0.0};
dot_t p1;
dot_t p;

/*
//idle2
dot_t p0 = {1.0, 0.0};
dot_t p1;
*/

range_t xrange = {-35.0, 35.0};
range_t yrange = {-15.0, 25.0};

float width = 600;
float height = 600;

float xx1, yy1;
#define XMIN 32
#define YMAX 600

void func0( bool init = false, float a = 0.0 );
/*-----------------------------------------------------------------*/
class ezMIDI{
public:
	/*----------------*/
	ezMIDI(){
		_channel = 0;
		_tone = 0x60;
		_volume = 0x60;
	}
	~ezMIDI(){};
	/*----------------*/
	/* midiデバイスオープン*/
	int Open( int ch ){
		int ret;
		_channel = ch;
		ret = midiOutOpen( &_hMidi, MIDIMAPPER, 0, 0, 0 );
		if( ret != 0 ) printf("error to open midi\n");
		return ret;
	}
	/*----------------*/
	/* midiデバイスクローズ*/
	int Close(){
		int ret;
		ret = midiOutClose( _hMidi);
		if( ret != 0 ) printf("error to close midi\n");
		return ret;
	}
	/*----------------*/
	DWORD PackMIDI( int stat, int data1, int data2 ){
		return (DWORD)( stat|_channel |(data1 << 8)|(data2 << 16));
	}
	/*----------------*/
	/* 音を鳴らす　midiデバイスにmidiコードを転送 */
	int Send( int stat, int data1, int data2 ){
		midiOutShortMsg( _hMidi, PackMIDI( stat, data1 ,data2 ));
		return 0;
	}
	/*----------------*/
	void SetInstrument( int code ){
		_instrument = code;
	}

private:
	HMIDIOUT _hMidi;
	int _channel;
	int _tone;
	int _volume;
	int _instrument;
};

/*-----------------------------------------------------------------*/

ezMIDI *midi = new ezMIDI;

int i = 0;

void display(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( xrange.min, xrange.max, yrange.min, yrange.max, -1.0, 1.0 );
	glMatrixMode(GL_MODELVIEW);

	//gluLookAt( 3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
	glLoadIdentity();

	if( preview ){
		glDrawBuffer( GL_FRONT );
		//glDrawBuffer( GL_BACK );
		//glReadBuffer( GL_FRONT);
	}
	else{
		glDrawBuffer( GL_BACK );
		glClear(GL_COLOR_BUFFER_BIT);
	}

	glPushMatrix();
	i++;
	glPointSize( 2.0 );
	glBegin(GL_POINTS);
	glColor3f(0.0, 0.0, 0.0);
	//glVertex3f( _x1+0.1, _y1-0.1, 0.0);
	glVertex3f( 0., 0., 0.);
	glColor3f(.0, .75, .5);
	//  glVertex3f( _x1, _y1, 0.0);
	glVertex3f( p1.x + p1.y, p1.x - p1.y, 0.0); //*************
	glEnd();
	//glLineWidth( 1.0 );
	glPopMatrix();

	if( preview ){
		glFlush();
		//glReadBuffer( GL_BACK );
		//glutSwapBuffers();
		return;
	}

	if( i%100000 == 0 ) printf( "%d\n", i );

	static int tone;
	midi->Send( 0x90, 64 - tone * 2 + 32, 0 );  
	tone = (int)sqrtf( (float)(p1.x*p1.x) + (float)(p1.y*p1.y) );
	// printf( "%d\n", tone );
	midi->Send( 0x90, 64 - tone * 2 + 32, 0x7f );

	glAccum( GL_MULT, 0.999 ); //0.99
	glReadBuffer( GL_BACK);
	glAccum( GL_ACCUM, 1.0 );
	glDrawBuffer( GL_BACK);
	glAccum( GL_RETURN, 1.0 );
	//glReadBuffer( GL_FRONT);

	glLineWidth( 1.0 );

	glBegin(GL_LINES);
	//glColor3f(.0, .0, .0);
	glColor3f(1., .0, .5);
	glVertex3f( 0.0, 0.0, 0.0);
	//glVertex3f( 5., 5., 0.0);
	//glColor3f(.0, .75, .5);  
	//  glVertex3f( _x1, _y1, 0.0);
	glVertex3f( p1.x + p1.y, p1.x - p1.y, 0.0); //*************
	glEnd();

	glBegin(GL_LINES);
	glColor3f( 1., .5, 0.0);
	glVertex3f( xrange.min, p.x - p.y, 0.0);
	glVertex3f( xrange.max, p.x - p.y, 0.0);

	glVertex3f( p.x + p.y, yrange.min, 0.0);
	glVertex3f( p.x + p.y, yrange.max, 0.0);
	glEnd();

	glutSwapBuffers();
}

static void func( dot_t *p0, dot_t *p1, float a )
{

}

//網の目カオス
void func0( bool init, float a_init )
{
	static float a;
	if( init ){
		a = a_init;
		xrange.min = -15.0;
		xrange.max = 15.0;
		yrange.min = -20.0;
		yrange.max = 20.0;
	}

	p1.x = p0.y + a * p0.x + (5.0 * p0.x )/( 1.0 + p0.x * p0.x );
	p1.y = - p0.x;
	p0.x = p1.x;
	p0.y = p1.y;
}

//三つの翼
void func1( float a, float b )
{
	xrange.min = -35.0;
	xrange.max = 35.0;
	yrange.min = -15.0;
	yrange.max = 25.0;

	// a = 0.97;
	// b = 0.995;

	p1.x = p0.y - 0.97 * p0.x + 5.0/(1.0+p0.x*p0.x) - 5.0;
	p1.y = - 0.995 * p0.x;
	p0.x = p1.x;
	p0.y = p1.y;
}

void idle(void)
{
	//func0( 1.02 );
	func0();

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
		exit(0);
	case 'c':
		glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
		break;
	case 'd':
		display();
		break;
	case 'x':
		preview = !preview;
		//static dot_t p;
		if( preview ){
			//p.x = p0.x;
			//p.y = p0.y;
			glDrawBuffer( GL_FRONT | GL_BACK );
			glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
		}
		else{
			p0.x = p.x;
			p0.y = p.y;
		}
		break;
	case 'z':
		glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
		break;
	default:
		break;
	}
}

void specialKey( int key, int x, int y )
{
	switch( key ){
	case GLUT_KEY_UP:
		param += 0.01;
		func0( true, param );
		break;
	case GLUT_KEY_DOWN:
		param -= 0.01;
		func0( true, param );
		break;
	}
	printf( "param = %f\n", param );
}

void mouseClick( int button, int state, int x, int y )
{
	dot_t q;
	if( state == GLUT_DOWN ){

		switch( button ){
		case GLUT_LEFT_BUTTON:
			q.x = (float)x / width * ( xrange.max - xrange.min ) + xrange.min;
			q.y = (1.0 - (float)y / height )* ( yrange.max - yrange.min ) + yrange.min;
			p.x = (q.x + q.y) / 2.0;
			p.y = (q.x - q.y) / 2.0;
			printf( "%f,%f\n", p.x, p.y );
			//glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
			break;
		case GLUT_MIDDLE_BUTTON:
			q.x = (float)x / width * ( xrange.max - xrange.min ) + xrange.min;
			q.y = (1.0 - (float)y / height )* ( yrange.max - yrange.min ) + yrange.min;
			p.x = (q.x + q.y) / 2.0;
			p.y = (q.x - q.y) / 2.0;
			printf( "%f,%f\n", p.x, p.y );
			preview = true;
			break;
		case GLUT_RIGHT_BUTTON:

			break;
		}

	}
	else{
		switch( button ){
		case GLUT_LEFT_BUTTON:
			break;
		case GLUT_MIDDLE_BUTTON:
			preview = false;
			break;
		case GLUT_RIGHT_BUTTON:
			break;
		}

	}

}

void mouseMotion( int x, int y )
{
	dot_t p, q;
	q.x = (float)x / width * ( xrange.max - xrange.min ) + xrange.min;
	q.y = (1.0 - (float)y / height )* ( yrange.max - yrange.min ) + yrange.min;
	p.x = (q.x + q.y) / 2.0;
	p.y = (q.x - q.y) / 2.0;
}

void mousedrag( int x, int y )
{
	xx1 = x + XMIN;
	yy1 = YMAX - y;
	//printf( "%d\t%d\n", x, y );
}

void init(char *winname)
{
	glutInitWindowPosition( 32, 32);
	glutInitWindowSize( 600, 600 );
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ACCUM );
	glutCreateWindow(winname);

	glClearColor(0., 0., 0., 1.0);

	//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);

	func0( true, 0.2 );
}

int main(int argc, char *argv[])
{

	midi->Open(0);
	//  midi->Send( 0xc0, 0x5b, 0 );
	//  midi->Send( 0xc0, 0x5f, 0 );
	midi->Send( 0xc0, 0x36, 0 );
	glutInit(&argc, argv);
	init(argv[0]);
	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutSpecialFunc( specialKey );
	//glutMotionFunc(mousedrag);
	glutMouseFunc( mouseClick );
	glutPassiveMotionFunc( mouseMotion );
	glutIdleFunc(idle);
	glutMainLoop();
	midi->Close();
	return 0;
}
