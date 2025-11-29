#include <stdio.h>

#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <GL/glut.h>

typedef struct{
	float x, y;
} dot_t;

float param = 0.0;
//float _x0 = 4.0, _y0 = 0.0;
dot_t p0 = {1.0, 0.0};
dot_t p1;
float xx1, yy1;
#define XMIN 32
#define YMAX 600
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
  glClear(GL_COLOR_BUFFER_BIT);

  glPushMatrix();
  i++;
  glPointSize( 2.0 );
  glBegin(GL_POINTS);
  glColor3f(0.0, 0.0, 0.0);
  //glVertex3f( _x1+0.1, _y1-0.1, 0.0);
  glVertex3f( 0., 0., 0.);
  glColor3f(.0, .75, .5);
//  glVertex3f( _x1, _y1, 0.0);
  glVertex3f( p1.x + p1.y, - p1.x + p1.y, 0.0); //*************
  glEnd();
  //glLineWidth( 1.0 );
  glPopMatrix();
  if( i%100000 == 0 ) printf( "%d\n", i );

  static int tone;
  midi->Send( 0x90, 64 - tone * 2 + 32, 0 );  
  tone = (int)sqrtf( (float)(p1.x*p1.x) + (float)(p1.y*p1.y) );
 // printf( "%d\n", tone );
  midi->Send( 0x90, 64 - tone * 2 + 32, 0x7f );
  
  glAccum( GL_MULT, 0.999 );
  //glAccum( GL_MULT, 0.99 );
  glReadBuffer( GL_BACK);
  glAccum( GL_ACCUM, 1.0 );
  glDrawBuffer( GL_BACK);
  glAccum( GL_RETURN, 1.0 );
  glReadBuffer( GL_FRONT);

  glBegin(GL_LINES);
  //glColor3f(.0, .0, .0);
  glColor3f(1., .0, .5);
  glVertex3f( 0.0, 0.0, 0.0);
  //glVertex3f( 5., 5., 0.0);
  //glColor3f(.0, .75, .5);  
//  glVertex3f( _x1, _y1, 0.0);
  glVertex3f( p1.x + p1.y, - p1.x + p1.y, 0.0); //*************
  glEnd();


  glutSwapBuffers();
}

void idle(void)
{
  p1.x = p0.y + 0.2 * p0.x + (5.0 * p0.x )/( 1.0 + p0.x * p0.x );
  p1.y = - p0.x;
  p0.x = p1.x;
  p0.y = p1.y;
  
  glutPostRedisplay();
}

void idle2(void)
{
  p1.x = p0.y - 0.97 * p0.x + 5.0/(1.0+p0.x*p0.x) - 5.0;
  p1.y = - 0.995 * p0.x;
  p0.x = p1.x;
  p0.y = p1.y;
  
  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
/*引数 key にはタイプされたキーの ASCII コードが，
x と y にはキーがタイプされたときのマウスの位置が渡される */
{
  switch (key) {
  case 'q':
    exit(0);
  case 'c':
    glClear(GL_COLOR_BUFFER_BIT);
    break;
  case 'd':
    display();
    break;
  default:
    break;
  }
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
  //glutInitDisplayMode( GLUT_RGBA);
  glutCreateWindow(winname);

//  glClearColor(0.2, 0.4, 0.4, 1.0);
    glClearColor(0., 0., 0., 1.0);

  glMatrixMode(GL_PROJECTION); /* 投影法に関する座標変換を開始 */
  glLoadIdentity();

//  glOrtho( -25.0, 25.0, -25.0, 25.0, -1.0, 1.0 );
  glOrtho( -35.0, 35.0, -15.0, 25.0, -1.0, 1.0 );

  glMatrixMode(GL_MODELVIEW); /* 視点の移動やモデルの移動など投影法
                                 以外の座標変換を開始 */

  //gluLookAt( 3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
  glLoadIdentity();
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
}

int main(int argc, char *argv[])
{

  midi->Open(0);
//  midi->Send( 0xc0, 0x5b, 0 );
//  midi->Send( 0xc0, 0x5f, 0 );
    midi->Send( 0xc0, 0x36, 0 );
  glutInit(&argc, argv);
  init(argv[0]);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMotionFunc(mousedrag);
  glutIdleFunc(idle2);
  glutMainLoop();
  midi->Close();
  return 0;
}
