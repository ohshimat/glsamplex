/* depthview.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

float aspect = (float)WINDOW_WIDTH/WINDOW_HEIGHT; /* アスペクト比 */
  
float rot = 0.0; /*回転角度*/
int rot_mode = 0;/*回転モード 0:停止, 1:反時計回り, -1:時計回り*/

int zbits;
unsigned char *image;
unsigned int  *depth;
float *fdepth;

/*---------------------------------------------------------------- updateObj */
/* updateObj: オブジェクトの状態を更新する
 */
void updateObj( void )
{
    /*▼回転角度を更新する*/
    if( rot_mode != 0 ){ /*回転モードが 0 でなければ‥*/
        rot += 1.0 * rot_mode; /*正なら反時計回り、負なら時計回り*/
        if( rot > 360.0 || rot < -360.0 ){ /*■一回転したら止める*/
            rot_mode = 0;
            rot = 0.0;
        }
    }
    return;
}
/*--------------------------------------------------------------------- idle */
/* idle: ▼アイドルコールバック関数
 */
void idle( void )
{
    /*▼オブジェクトの状態を更新する*/
    updateObj();
    return;
}
/*------------------------------------------------------------------ drawObj */
/* drawObj: オブジェクトを描画する
 */
void drawObj( void )
{
    /*▼ワイヤキューブを描画する*/
    glColor3f( 1.0, 1.0, 1.0 ); /*描画する図形の色を指定 */
    glutWireCube( 1.0 );
    /*▼ソリッド球を描画する*/
    glColor3f( 0.6, 0.6, 0.6 ); /*描画する図形の色を指定 */
    glutSolidSphere( 0.5, 10, 25 );
    return;
}
/*--------------------------------------------------------------- DrawDepthview */
void DrawDepthview()
{
    unsigned char *p;
    unsigned int *d;
    float *fd;
    int i, j;

    /* Read from depth burrer */
    /*
	glReadPixels( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_DEPTH_COMPONENT,
                  GL_UNSIGNED_INT,
                  (void *)depth );
	 */

    glReadPixels( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_DEPTH_COMPONENT,
                  GL_FLOAT,
                  (void *)fdepth );

    /*
    glReadPixels( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE,
                 (void *)image );
     */

    p = image;
    fd = fdepth;
    for( i=WINDOW_HEIGHT*WINDOW_WIDTH; i>0; i-- ){
        *p++ = *fd * 255.0;
        *p++ = *fd * 255.0;
        *p++ = *fd * 255.0;
        *p++ = 255;
        fd++;
    }

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0, WINDOW_WIDTH, 0, WINDOW_HEIGHT );
    glMatrixMode( GL_MODELVIEW );

    glDrawPixels( WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE,
                  (void *)image );

    return;
}
/*------------------------------------------------------------------- display */
/* display: ディスプレイコールバック関数（描画関数)
 */
void display( void )
{
    glViewport( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45.0, aspect, 1.0, 3.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    /*▼ビューイング変換*/
    gluLookAt( 0.0, 1.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

    /*▼描画本体*/

    glEnable( GL_DEPTH_TEST ); /*■Ｚバッファ処理を有効にする*/

    glPushMatrix(); /*現在の変換マトリクスを保存*/
    glRotatef( rot, 0.0, 1.0, 0.0 ); /*Ｙ軸について回転*/
    drawObj();
    glPopMatrix(); /*変換マトリクスを復帰*/

    glDisable( GL_DEPTH_TEST ); /*■Ｚバッファ処理を無効にする*/

    DrawDepthview(); /*Zバッファの内容をカラーバッファに複写する*/

	glutPostRedisplay(); /*描画イベントを発生させる*/

    glutSwapBuffers(); /*バッファスワップ*/
    return;
}
/*----------------------------------------------------------------- keyboard */
/* keyboard: キーボードイベントコールバック関数
 * 引数 key にはタイプされたキーの ASCII コードが，
 * x と y にはキー入力が検出されたときのマウスの位置が渡される
 */
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
      case 'h':   /*[H]がキー入力されたら，操作説明を表示する */
        printf( "Instruction\n" );
        printf( "[H]:Help\n" );
        printf( "[Q]:Quit\n" );
        printf( "[1]:Start counter-clockwise rotation\n" );
        printf( "[0]:Stop rotation\n" );
        printf( "[R]:Rotate 30 degree\n" );
        printf( "[LEFT MOUSE BUTTON]:Start CCW rotation\n" );
        printf( "[MIDDLE MOUSE BUTTON]:Start CCW rotation\n" );
        printf( "[RIGHT MOUSE BUTTON]:Stop CW rotation\n" );
        break;
      case 'q':   /* [Q]がキー入力されたら，プログラムを終了する */
        exit(0);
      case '0':   /* [0]がキー入力されたら回転が止まる */
        rot_mode = 0; /*回転フラグをオフ*/
        break;
      case '1':   /* [1]がキー入力されたら回転が始まる */
        rot_mode = 1; /*回転フラグをオン*/
        break;
      case 'r':   /* [R]がキー入力されたら30度回転する */
        rot += 30.0;
        if( rot > 360.0 ) rot -= 360.0;
        break;
      default:
        break;
    }
    return;
}
/*-------------------------------------------------------------------- mouse */
/* mouse: マウスイベントコールバック関数 */
void mouse( int button , int state, int x, int y )
/* button は押された（離された）ボタン，
  state：押したか離したかの状態，
  x, y：マウスの座標値 */
{
    switch( button ){
      case GLUT_LEFT_BUTTON: /*左マウスボタン*/
        switch( state ){
          case GLUT_DOWN: /*押す*/
            rot_mode = 1; /*回転モードを反時計回りに*/
            break;
          case GLUT_UP: /*離す*/
            break;
          default:
            break;
        }
        break;
      case GLUT_MIDDLE_BUTTON: /*中マウスボタン */
        switch( state ){
          case GLUT_DOWN: /*押す*/
            rot_mode = 0; /*回転モードをオフに*/
            break;
          case GLUT_UP: /*離す*/
            break;
          default:
            break;
        }
        break;
      case GLUT_RIGHT_BUTTON: /*右マウスボタン */
        switch( state ){
          case GLUT_DOWN: /*押す*/
            rot_mode = -1; /*回転モードを時計周りに*/
            break;
          case GLUT_UP: /*離す*/
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
/*---------------------------------------------------------------- initWindow */
/* initWindow: OpenGLウィンドウの初期化
 */
void initWindow( char *winname )
{
    glutInitWindowPosition( 0, 0 );   /* ウィンドウの左上の位置を(0,0)とする */
    glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT ); /* ウィンドウのサイズ */
    /*■RGBAモード＋ダブルバッファモード＋Ｚバッファモード*/
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutCreateWindow( winname ); /* winnameで指定された名前でウィンドウを開く */
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); /* ウィンドウの背景色の指定 */

    glGetIntegerv( GL_DEPTH_BITS, &zbits );
    printf( "DEPTH_BITS = %d\n", zbits );
    glGetIntegerv( GL_ALPHA_BITS, &zbits );
    printf( "ALPHA_BITS = %d\n", zbits );
    glGetIntegerv( GL_RED_BITS, &zbits );
    printf( "RED_BITS = %d\n", zbits );
    glGetIntegerv( GL_GREEN_BITS, &zbits );
    printf( "GREEN_BITS = %d\n", zbits );
    glGetIntegerv( GL_BLUE_BITS, &zbits );
    printf( "BLUE_BITS = %d\n", zbits );
  
    image = (unsigned char*)malloc( WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(char) * 4 );
    depth = (unsigned int*)malloc( WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(char) * 4 );
    fdepth = (float *)malloc( WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(float ) );
    return;
}
/*---------------------------------------------------------------------- main */
/* main関数
 */
int main( int argc, char *argv[] )
{
    glutInit( &argc, argv );    /* glutの初期化 */
    initWindow( argv[0] );
    glutDisplayFunc( idle ); /* ディスプレイコールバックの指定 */
    glutIdleFunc( display );       /* アイドルコールバック関数の指定 */
    glutKeyboardFunc(keyboard); /* キーボード入力のコールバック関数の指定*/
    glutMouseFunc(mouse);       /* マウス入力のコールバック関数の指定*/
    printf( "[H]:Help\n" ); /*説明*/
    glutMainLoop();             /* イベント待ちの無限ループへ入る */
    return 0;
}
