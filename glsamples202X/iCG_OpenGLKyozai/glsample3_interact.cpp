/* glsample3_interact.cpp */

#include "platform.h" //<GL/glut.h>

#define WINDOW_XO 100
#define WINDOW_YO 100
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

float aspect = (float)WINDOW_WIDTH/WINDOW_HEIGHT; /* アスペクト比 */
  
float rot = 0.0; /*回転角度*/
int rot_mode = 0; /*回転モード 0:停止, 1:反時計回り, -1:時計回り*/

/*---------------------------------------------------------------- InitScene */
/* InitScene: シーンデータを初期化する
 */
void InitScene( void )
{
    rot = 0.0;
    rot_mode = 0;
    return;
}
/*-------------------------------------------------------------- UpdateScene */
/* UpdateScene: オブジェクトの状態を更新する
 */
void UpdateScene( void )
{
    /*回転角度を更新する*/
    switch( rot_mode ){
      case 1: /*反時計回り CCW*/
        rot += 1.0; /*数値を大きくすると早く回転する*/
        break;
      case 0:
        break;
      case -1: /*時計回り CW*/
        rot -= 1.0; /*数値を大きくすると早く回転する*/
        break;
    }
    return;
}
/*------------------------------------------------------------------- update */
/* update: アイドルコールバック関数
 */
void update( void )
{
    UpdateScene();
	Sleep(16);
    return;
}
/*------------------------------------------------------------------ drawObj */
/* drawObj: オブジェクトを描画する
 */
void drawObj( void )
{
    /*四角形を描画する*/
    glColor3f( 1.0, 1.0, 1.0 );    /* 描画する図形の色を白に指定 */
    glBegin( GL_POLYGON );         /* 多角形を描画するモード開始 */
    glVertex3f( -0.5, -0.5, 0.0 ); /* 頂点座標を指定 (x, y, z) */
    glVertex3f(  0.5, -0.5, 0.0 );
    glVertex3f(  0.5,  0.5, 0.0 );
    glVertex3f( -0.5,  0.5, 0.0 );
    glEnd(); /* 描画モード終了 */
    return;
}
/*----------------------------------------------------------------- DrawScene */
/* DrawScene: シーンを描画する
 */
void DrawScene( void )
{
    glPushMatrix(); /*現在の変換マトリクスを保存（プッシュ）*/
	{
        glRotatef( rot, 0.0, 0.0, 1.0 );
        drawObj();
	}
    glPopMatrix(); /*変換マトリクスを復元（ポップ）*/
    return;
}
/*------------------------------------------------------------------- display */
/* display: ディスプレイコールバック関数（描画関数)
 */
void display( void )
{
    /*▼ビューポート設定*/
    glViewport( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT );

    /*▼背景クリア、描画準備*/
    glClear( GL_COLOR_BUFFER_BIT );

    /*▼モデルビュー変換初期化*/
    glLoadIdentity();

    /*▽ビューイング変換設定*/
    /**デフォルト設定**/
 
    /*▼平行投影*/
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -1.0, 1.0 );
    glMatrixMode( GL_MODELVIEW );

    /*▼描画本体*/
    DrawScene();

    glutPostRedisplay(); /*■updateをトリガするイベントを発生させる*/
  
    glutSwapBuffers(); /*■バッファスワップ*/

    return;
}
/*----------------------------------------------------------------- keyboard */
/* keyboard: ■キーボードイベントコールバック関数
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
        printf( "[0]:Stop rotation\n" );
        printf( "[1]:Start rotation(CCW)\n" );
        printf( "[2]:Start rotation(CW)\n" );
        printf( "[MOUSE-LEFT  ]:Start rotation(CCW)\n" );
        printf( "[MOUSE-CENTER]:Stop rotation\n" );
        printf( "[MOUSE-RIGHT ]:Start rotation(CW)\n" );
        break;
      case 'q':   /* [Q]がキー入力されたら，プログラムを終了する */
        exit(0);
      case '0':   /* [0]がキー入力されたら回転が止まる */
        rot_mode = 0; /*回転モードをオフ*/
        break;
      case '1':   /* [1]がキー入力されたら反時計回り */
        rot_mode = 1; /*回転モードを反時計回りに*/
        break;
      case '2':   /* [1]がキー入力されたら時計回り */
        rot_mode = -1; /*回転モードを時計回りに*/
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
      case GLUT_MIDDLE_BUTTON: /*中マウスボタン*/
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
      case GLUT_RIGHT_BUTTON: /*右マウスボタン*/
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
    glutInitWindowPosition( WINDOW_XO, WINDOW_YO );/*ウィンドウの左上の位置*/
    glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT ); /*ウィンドウのサイズ*/

    /*RGBAモード＋ダブルバッファモード */
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE);

    glutCreateWindow( winname ); /* winnameで指定された名前でウィンドウを開く */
    glClearColor( 0.25, 0.25, 0.4, 1.0 ); /* ウィンドウの背景色の指定 */
    return;
}
/*---------------------------------------------------------------------- main */
/* main関数
 */
int main( int argc, char *argv[] )
{
    glutInit( &argc, argv );   /*glutの初期化*/

    initWindow( argv[0] );

    glutDisplayFunc( update ); /*シーン更新をディスプレイコールバックに指定*/
    glutIdleFunc( display );   /*描画関数をアイドルコールバックとして指定*/
    glutKeyboardFunc(keyboard);/*キーボード入力のコールバック関数の指定*/
    glutMouseFunc(mouse);       /*マウス入力のコールバック関数の指定*/
    printf( "[H]:Help\n" ); /*操作説明*/

    InitScene(); /*シーンデータを初期化する*/
    glutMainLoop();            /*イベント待ちの無限ループへ入る*/
    return 0;
}

