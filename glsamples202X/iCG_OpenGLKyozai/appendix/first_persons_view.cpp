/* simple_behind_view.cpp */

#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

float aspect = (float)WINDOW_WIDTH/WINDOW_HEIGHT; /* アスペクト比 */
  
float mousex0, mousey0;    /*マウスボタン押下時のマウス座標*/
float mousex, mousey;      /*マウス正規化座標*/
int drag = 0;              /*ドラッグスイッチ*/

/*▼オブジェクト位置姿勢*/
float objx = 0.0, objy = 0.0, objz = 0.0;          /*物体の初期位置*/
float objroll = 0.0, objpitch = 0.0, objyaw = 0.0; /*物体の初期姿勢*/
float dobjmove = 0.0; /*移動量*/
float dobjyaw = 0.0;  /*方向変化量*/

#if 0
/*▼視点位置姿勢*/
float viewx = 0.0, viewy = 2.0, viewz = 0.0;            /*視点初期位置*/
float viewroll = 0.0, viewpitch = -10.0, viewyaw = 0.0; /*視線方向初期値*/

/*▼*/
float viewdist = 5.0; /*視点・物体間距離*/
#endif

float obj_dx = 0.0, obj_dy = 0.0, obj_dz = 0.0;          /*物体の移動変化量*/
float obj_droll = 0.0, obj_dpitch = 0.0, obj_dyaw = 0.0; /*物体の回転変化量*/
float k_rot = 1.0;   /*回転角度係数*/
float k_trans = 0.1; /*移動量係数*/

/*----------------------------------------------------------------- DrawGrid */
/* DrawGrid: グリッドを描画する
 */
void DrawGrid( int n, float range )
{
  int i;
  float x, y;
  float d = range/(float)n;
  float min = -0.5 * range;
  float max =  0.5 * range;

  x = min;
  for( i=0; i<n; i++ ){
    glBegin( GL_LINES );
    glVertex3f( x, 0.0, min );
    glVertex3f( x, 0.0, max );
    glEnd();
    x += d;
  }
  y = min;
  for( i=0; i<n; i++ ){
    glBegin( GL_LINES );
    glVertex3f( min, 0.0, y );
    glVertex3f( max, 0.0, y );
    glEnd();
    y += d;
  }
}
/*----------------------------------------------------------------- DrawAxes */
/* DrawAxes: 座標軸を描画する
 */  
void DrawAxes( float l )
{
    /* x軸を赤で描画する */
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(-l, 0.001, 0.0 );
    glVertex3f( l, 0.001, 0.0 );
    glEnd();
    /* y軸を緑で描画する */
    glColor3f(0.0, 1.0, 0.0); 
    glBegin(GL_LINES);
    glVertex3f( 0.0, l, 0.0 );
    glVertex3f( 0.0,-l, 0.0 );
    glEnd();
    /* z軸を青で描画する */
    glColor3f(0.0, 0.0, 1.0); 
    glBegin(GL_LINES);
    glVertex3f( 0.0, 0.001, l );
    glVertex3f( 0.0, 0.001, -l );
    glEnd();
    return;
}
/*--------------------------------------------------------------------- idle */
/* idle: ▼アイドルコールバック関数
 */
void idle( void )
{
//    glutPostRedisplay(); /*描画イベントを発生させる*/
    return;
}
/*----------------------------------------------------------------- initView */
/* initView: ビューイング変換を初期化する
 */
void initView( void )
{
    /* nothing to do */
    return;
}
/*--------------------------------------------------------------- updateView */
/* updateView: ビューイング変換を更新する
 */
void updateView( void )
{
    // nothing to do
    return;
}
/*----------------------------------------------------------------- initObj */
/* initObj: オブジェクト状態を初期化する
 */
void initObj( void )
{
    return;
}
/*---------------------------------------------------------------- updateObj */
/* updateObj: オブジェクトの状態を更新する
 */
void updateObj( void )
{
    objyaw += dobjyaw;
    objx = objx + dobjmove * sinf( objyaw / 180.0 * 3.14 );
    objz = objz + dobjmove * cosf( objyaw / 180.0 * 3.14 );
    return;
}
/*-------------------------------------------------------------- updateScene */
/* UpdateScene: シーンデータ状態を更新する
 */
void UpdateScene( void )
{
    /*▼オブジェクトの状態を更新する*/
    updateObj();
    /*▼ビューイング変換を更新する*/
    updateView();
    return;
}
/*----------------------------------------------------------------- setLight */
/* setLight: 光源を設定する（簡易設定）
 */
void setLight( void )
{
    float light_color[] = { 1.0, 1.0, 1.0, 1.0 };
    float light_pos[] = { 0.0, 1.0, 1.0, 0.0 };
  
    glLightfv( GL_LIGHT0, GL_DIFFUSE, light_color );
    glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
    return;
}
/*-------------------------------------------------------------- setMaterial */
/* setMaterial: 材質感を設定する
 */
void setMaterial( void )
{
    float diffuse[] = { 0.25, 0.5, 0.75, 1.0 };
    float specular[] = { 1.0, 1.0, 1.0, 1.0 };
    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT, GL_SHININESS, 128.0 );
    return;
}
/*--------------------------------------------------------------- drawTeapot */
/* drawTeapot
 */
void drawTeapot()
{
    setMaterial(); /*材質感設定*/
    glShadeModel( GL_FLAT );
    glPushMatrix();
    glRotatef( 90.0, 0.0, 1.0, 0.0 );
    //glutSolidSphere( 0.5, 10, 25 );
    glutSolidTeapot( 0.5 );  
    glPopMatrix();
}
/*---------------------------------------------------------------- DrawScene */
/* DrawScene: オブジェクトを描画する
 */
void DrawScene( void )
{
    setLight(); /*光源設定*/

    /*座標軸を描画する*/
    glColor3f( 0.25, 0.25, 0.25 );
    DrawGrid( 100, 100 );
    DrawAxes( 100 );

    /*▼陰影を付けてソリッド球を描画する*/

    glEnable( GL_LIGHTING ); /*光源計算を有効にする*/
    glEnable( GL_LIGHT0 ); /*光源#0をONにする*/

    glPushMatrix(); /*現在の変換マトリクスを保存*/
    /*★位置と姿勢の指定*/
    glTranslatef( objx, objy, objz );
    glRotatef( objyaw, 0.0, 1.0, 0.0 );
    drawTeapot();
    glPopMatrix(); /*変換マトリクスを復帰*/

    glDisable( GL_LIGHT0 ); /*光源#0をOFFにする*/
    glDisable( GL_LIGHTING ); /*照明計算を無効にする*/
    return;
}
/*------------------------------------------------------------------- display */
/* display: ディスプレイコールバック関数（描画関数)
 */
void display( void )
{
    UpdateScene();

    /*▼透視投影*/
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 30.0, aspect, 1.0, 100.0 );
    glMatrixMode( GL_MODELVIEW );

    /*▼ビューポート設定*/
    glViewport( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT );

    /*▼背景クリア＋Ｚバッファクリア*/
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /*▼モデリング変換初期化*/
    glLoadIdentity();

    /*★ビューイング変換*/
    //gluLookAt( 0.0, 2.0, 10.0, objx, objy, objz, 0.0, 1.0, 0.0 );
    gluLookAt( 0.0, 2.0, 10.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0 );

    glRotatef( 0.0, 0.0, 0.0, 1.0 );
    glRotatef( 0.0, 1.0, 0.0, 0.0 );
    glRotatef( - objyaw, 0.0, 1.0, 0.0 );
    glTranslatef( - objx, - objy, - objz );

    /*▼描画本体*/

    glEnable( GL_DEPTH_TEST ); /*Ｚバッファ処理を有効にする*/

    DrawScene();
  
    glDisable( GL_DEPTH_TEST ); /*Ｚバッファ処理を無効にする*/
  
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
        printf( "[LEFT MOUSE BUTTON]+DRAG: Start Translation\n" );
        printf( "[MIDDLE MOUSE BUTTON]: Stop\n" );
        printf( "[RIGHT MOUSE BUTTON]+DRAG: Start Rotation\n" );
        break;
      case 'q':   /* [Q]がキー入力されたら，プログラムを終了する */
        exit(0);
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
    /*■マウス押下時のマウス座標を正規化して保存*/
    if( state == GLUT_DOWN ){
        mousex0 = (float)x/WINDOW_WIDTH * 2.0 - 1.0;
        mousey0 = (float)y/WINDOW_HEIGHT * 2.0 - 1.0;
    }
    else{
        drag = 0;
        dobjyaw = 0;
        dobjmove = 0;
    }
    switch( button ){
      case GLUT_LEFT_BUTTON: /*左マウスボタン*/
        switch( state ){
          case GLUT_DOWN: /*押す*/
            drag = 1;
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
            obj_dx = 0.0;
            obj_dy = 0.0;
            obj_dz = 0.0;
            obj_dpitch = 0.0;
            obj_droll = 0.0;
            obj_dyaw = 0.0;
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
            drag = 2;
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
/*----------------------------------------------------------------- mouseDrag */
/* mouseDrag: ■マウスドラッグコールバック関数
 * x と y にはマウスの位置が渡される
 */
void mouseDrag( int x, int y)
{
    float dx, dy;
    mousex = (float)x/WINDOW_WIDTH  * 2.0 - 1.0; /*マウスＸ座標を正規化*/
    mousey = (float)y/WINDOW_HEIGHT * 2.0 - 1.0; /*マウスＹ座標を正規化*/
    /*前回マウス座標との差分を変化量として設定*/
    dx = mousex - mousex0;
    dy = mousey - mousey0;
//    mousex0 = mousex;
//    mousey0 = mousey;

    switch( drag ){
      case 1:
        dobjmove = dy * k_trans;
        dobjyaw = -dx * k_rot;
        break;
    }

    return;
}
/*---------------------------------------------------------------- initWindow */
/* initWindow: OpenGLウィンドウの初期化
 */
void initWindow( char *winname )
{
    glutInitWindowPosition( 100, 100 );   /* ウィンドウの左上の位置を(0,0)とする */
    glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT ); /* ウィンドウのサイズ */
    /*RGBAモード＋ダブルバッファモード＋Ｚバッファモード*/
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutCreateWindow( winname ); /* winnameで指定された名前でウィンドウを開く */
    glClearColor( 0.0, 0.1, 0.2, 1.0 ); /* ウィンドウの背景色の指定 */
    return;
}
/*---------------------------------------------------------------------- main */
/* main関数
 */
int main( int argc, char *argv[] )
{
    glutInit( &argc, argv );    /*glutの初期化 */
    initWindow( argv[0] );
    glutDisplayFunc( idle ); /*ディスプレイコールバックの指定 */
    glutIdleFunc( display );       /*アイドルコールバック関数の指定 */
    glutKeyboardFunc(keyboard); /*キーボード入力のコールバック関数の指定*/
    glutMotionFunc(mouseDrag);  /*マウスドラッグ時のコールバック関数*/
    glutMouseFunc(mouse);       /*マウス入力のコールバック関数の指定*/

    initView(); /*ビューイング変換を初期化する*/
    initObj(); /*モデリング変換を初期化する*/

    printf( "[H]:Help\n" ); /*説明*/
    glutMainLoop();             /* イベント待ちの無限ループへ入る */
    return 0;
}
