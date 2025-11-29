/* glsample3_interact.cpp */

#include "platform.h" //<GL/glut.h>

#define WINDOW_XO 100
#define WINDOW_YO 100
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

float aspect = (float)WINDOW_WIDTH/WINDOW_HEIGHT; /* アスペクト比 */

#define NA 180
#define NR 64
float web_x[NR+1][NA+1];
float web_y[NR+1][NA+1];
float web_u[NR+1][NA+1];
float web_v[NR+1][NA+1];

int projmode = 0;

float stereographic( float r ){ return tanf( r/2.0 ); } //立体射影
float equidistance( float r ){ return r / (3.14/2.0); } //等距離射影
float equisolidangle( float r ){
	return 2.0 / sqrtf( 2.0 ) * sinf( r/2.0 ); 
} //等立体角射影
float orthogonal( float r ){ return sinf( r ); } //正射影
//-------------------------------------------------------------- makeWeb
/* makeWeb: クモの巣状の格子点を生成する
 */
void makeWeb( float(*func)(float) )
{
	float x, y, a, r, da, dr, l;
	da = 360.0 / NA;
	dr = 90.0 / NR;
	for( int i = 0; i <= NR; i++ ){
		r = dr * i;
		r = r / 180.0 * 3.14159;
		for( int j = 0; j <= NA; j++ ){
			a = da * j;
			a = a / 180.0 * 3.14159;

			l = func( r );

			web_x[i][j] = l * cosf( a );
			web_y[i][j] = l * sinf( a );
		}
	}
}
//-------------------------------------------------------------- drawWeb1
/* drawWeb1: クモの巣状の格子点を描画する
 */
void drawWeb1( void )
{
	glPointSize( 2.0 );
	glBegin( GL_POINTS );
	glColor3f( 0.0, 1.0, 0.0 );
	for( int i = 0; i < NR; i++ ){
		for( int j = 0; j < NA; j++ ){
			glVertex2f( web_x[i][j], web_y[i][j] );
		}
	}
	glEnd();

	glColor3f( 0.0, 1.0, 0.5 );
	for( int i = 0; i <= NR; i++ ){
		glBegin( GL_LINE_LOOP );
		for( int j = 0; j < NA; j++ ){
			glVertex2f( web_x[i][j], web_y[i][j] );
		}
		glEnd();
	}

	glColor3f( 0.0, 1.0, 0.0 );
	glBegin( GL_LINES );
	for( int j = 0; j <= NA; j++ ){
		glVertex2f( web_x[0][j], web_y[0][j] );
		glVertex2f( web_x[NR][j], web_y[NR][j] );
	}
	glEnd();
}
//-------------------------------------------------------------- drawWeb2
/* drawWeb2: クモの巣状の格子点を描画する
 */
void drawWeb2( void )
{
	glColor3f( 0.0, 1.0, 1.0 );
	for( int i = 0; i < NR; i++ ){
		//glBegin( GL_TRIANGLE_STRIP );
		glBegin( GL_LINE_LOOP );
		for( int j = 0; j <= NA; j++ ){
			glVertex2f( web_x[i][j], web_y[i][j] );
			glVertex2f( web_x[i+1][j], web_y[i+1][j] );
		}
		glEnd();
	}

}
//-------------------------------------------------------------- drawWeb3
/* drawWeb3: クモの巣状の格子点を描画する
 */
void drawWeb3( void )
{
	glShadeModel( GL_FLAT );
	//glColor3f( 0.0, 1.0, 1.0 );
	for( int i = 0; i < NR; i++ ){

		glBegin( GL_TRIANGLE_STRIP ); //
		//glBegin( GL_LINE_LOOP );
		for( int j = 0; j <= NA; j++ ){
			//
			if( (i+j)%2 ) glColor3f( 1.0, 1.0, 1.0 );
			else glColor3f( 0.0, 0.0, 0.0 );
			//
			glVertex2f( web_x[i][j], web_y[i][j] );
			glVertex2f( web_x[i+1][j], web_y[i+1][j] );
		}
		glEnd();
	}

}
/*---------------------------------------------------------------- InitScene */
/* InitScene: シーンデータを初期化する
 */
void InitScene( void )
{
	makeWeb( stereographic );

    return;
}
/*-------------------------------------------------------------- UpdateScene */
/* UpdateScene: オブジェクトの状態を更新する
 */
void UpdateScene( void )
{

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
	//drawWeb1();
    //drawWeb2();
    drawWeb3();
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
      case '1':
		  makeWeb( stereographic );
        break;
      case '2':
		  makeWeb( equidistance );
        break;
      case '3':
		  makeWeb( equisolidangle );
        break;
	  case '4':
		  makeWeb( orthogonal );
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

