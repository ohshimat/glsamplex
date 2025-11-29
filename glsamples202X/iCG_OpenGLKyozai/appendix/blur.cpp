/* blur.cpp */

#include <stdlib.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

float aspect = (float)WINDOW_WIDTH/WINDOW_HEIGHT; /* アスペクト比 */
  
float rot = 0.0; /*■回転角度*/

/*--------------------------------------------------------------------- idle */
/* idle: ▼アイドルコールバック関数
 */
void idle( void )
{
    glutPostRedisplay(); /*■描画イベントを発生させる*/
    return;
}
/*---------------------------------------------------------------- updateObj */
/* updateObj: オブジェクトの状態を更新する
 */
void updateObj( void )
{
    /*▼回転角度を更新する*/
    rot += 1.0;
  
    return;
}
/*------------------------------------------------------------------ drawObj */
/* drawObj: オブジェクトを描画する
 */
void drawObj( void )
{
    /*▼四角形を描画する*/
    glColor3f( 1.0, 1.0, 1.0 );    /* 描画する図形の色を白に指定 */
    glBegin( GL_POLYGON );         /* 多角形を描画するモード開始 */
    glVertex3f( -0.5, -0.5, 0.0 ); /* 頂点座標を指定 (x, y, z) */
    glVertex3f(  0.5, -0.5, 0.0 );
    glVertex3f(  0.5,  0.5, 0.0 );
    glVertex3f( -0.5,  0.5, 0.0 );
    glEnd(); /* 描画モード終了 */

    return;
}
/*------------------------------------------------------------------- display */
/* display: ディスプレイコールバック関数（描画関数)
 */
void display( void )
{
    /*▼オブジェクトの状態を更新する*/
    updateObj();
  
    /*▼平行投影*/
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -1.0, 1.0 );
    glMatrixMode( GL_MODELVIEW );

    /*▼ビューポート設定*/
    glViewport( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT );

    /*▼背景クリア、描画準備*/
    glClear( GL_COLOR_BUFFER_BIT );

    /*▼モデリング変換初期化*/
    glLoadIdentity();
  
    /*▼描画本体*/
    glPushMatrix(); /*現在の変換マトリクスを保存*/
    glRotatef( rot, 0.0, 0.0, 1.0 );
    drawObj();
    glPopMatrix(); /*変換マトリクスを復帰*/

    /*■■アキュムレーションバッファ*/
    glAccum( GL_MULT, 0.9 );
    glReadBuffer( GL_BACK );
    glAccum( GL_ACCUM, 0.1 );
    glAccum( GL_RETURN, 1.0 );
  
    glutSwapBuffers(); /*■バッファスワップ*/
    return;
}
/*---------------------------------------------------------------- initWindow */
/* initWindow: OpenGLウィンドウの初期化
 */
void initWindow( char *winname )
{
    glutInitWindowPosition( 0, 0 );   /* ウィンドウの左上の位置を(0,0)とする */
    glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT ); /* ウィンドウのサイズ */
    /*■RGBAモード＋ダブルバッファモード */
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_ACCUM ); /*■■*/
    glutCreateWindow( winname ); /* winnameで指定された名前でウィンドウを開く */
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); /* ウィンドウの背景色の指定 */
    glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT); /*■■*/

    return;
}
/*---------------------------------------------------------------------- main */
/* main関数
 */
int main( int argc, char *argv[] )
{
    glutInit( &argc, argv );    /* glutの初期化 */
    initWindow( argv[0] );
    glutDisplayFunc( display ); /* ディスプレイコールバックの指定 */
    glutIdleFunc( idle );       /*■アイドルコールバック関数の指定 */
    glutMainLoop();             /* イベント待ちの無限ループへ入る */
    return 0;
}
