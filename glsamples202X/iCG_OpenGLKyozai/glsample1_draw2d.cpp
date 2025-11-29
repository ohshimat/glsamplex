/************************
 * glsample_draw2d.cpp *
 ************************/

#include "platform.h" //<GL/glut.h> /* glutヘッダファイルのインクルード */

#define WINDOW_WIDTH 500  /* 定数マクロ定義：ウィンドウの幅（ピクセル数）*/
#define WINDOW_HEIGHT 500 /* 定数マクロ定義：ウィンドウ高さ（ピクセル数）*/

/*------------------------------------------------------------------ drawObj */
/* drawObj: オブジェクト描画関数
 */
void drawObj( void )
{
    glColor3f( 1.0, 1.0, 1.0 ); /* 色指定：描画する図形の色を白に指定 */

    /*▼図形描画コード本体：四角形を描画する*/
    glBegin( GL_POLYGON ); /* 多角形を描画するモード開始 */
    /*頂点座標を指定(x, y, z)*/
    glVertex3f( -0.5, -0.5, 0.0 ); /*頂点①*/
    glVertex3f(  0.5, -0.5, 0.0 ); /*頂点②*/
    glVertex3f(  0.5,  0.5, 0.0 ); /*頂点③*/
    glVertex3f( -0.5,  0.5, 0.0 ); /*頂点④*/
    glEnd();                       /* 描画モード終了 */
  
    return;
}
/*------------------------------------------------------------------ display */
/* display: ディスプレイコールバック関数（描画関数
 */
void display( void )
{
    /*▼投影変換の設定*/
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 ); /* 平行投影変換 */
    glMatrixMode( GL_MODELVIEW );

    /*▼ビューポート設定*/
    glViewport( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT );

    /*▼背景クリア、描画準備*/
    glClear( GL_COLOR_BUFFER_BIT ); /* ウィンドウの背景を塗りつぶす */

    /*▼モデリング変換初期化*/
    glLoadIdentity();

    /*▼図形描画コード本体：四角形を描画する*/
    drawObj();
    
    glFlush();                     /* OpenGLコマンド強制実行 */

    return;
}
/*--------------------------------------------------------------- initWindow */
/* initWindow: OpenGLウィンドウの初期化
 */
void initWindow( char *winname )
{
    /*--- -ウィンドウの左上の位置を 100,100)とする */
    glutInitWindowPosition( 100, 100 );
 
    /*---- ウィンドウのサイズを幅WINDOW_WIDTH,高さWINDOW_HEIGHTとする */
    glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT );     

    /*---- 色の指定にRGBAモードを用いる */
    glutInitDisplayMode( GLUT_RGBA );   

    /*---- winnameで指定された名前でウィンドウを開く */
    glutCreateWindow( winname );      

    glClearColor( 0.25, 0.25, 0.4, 1.0 ); /* ウィンドウの背景色の指定 */
    /* R(赤),G(緑),B(青),A(透明度)の順で指定（値域は[0.0, 0.1]）*/

    return;
}
/*--------------------------------------------------------------------- main */
/* main関数
 */
int main( int argc, char *argv[] )
{
    glutInit( &argc, argv ); /* glutの初期化 */

    initWindow( argv[0] );

    glutDisplayFunc( display ); /* ディスプレイコールバック関数の指定 */

    glutMainLoop(); /* イベント待ちの無限ループへ入る */

    return 0;
}
/* end of gl1_square.cpp（注：ファイルの最後に改行を入れるのを忘れずに↓）*/
