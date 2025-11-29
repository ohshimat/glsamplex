/* glsample9_random.cpp */

#include "platform.h" //<GL/glut.h>

#include "GLMetaseq.h" /* mqoローダ */
MQO_MODEL mqoObj; /* mqoオブジェクト */

#define WINDOW_XO 100
#define WINDOW_YO 100
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define PI 3.1415926 /*円周率*/

float aspect = (float)WINDOW_WIDTH/WINDOW_HEIGHT; /* アスペクト比 */
  
float posx = 0.0;
float posz = 0.0;
float dir; /*移動方向*/

/*------------------------------------------------------------------ Random */
/* rminからrmaxまでの範囲の値をランダムに返す
 */
float Random( float rmin, float rmax ){
    double r;
    r = (double)rand()/(double)RAND_MAX;
    r = r * ( rmax - rmin ) + rmin;
    return r;
}
/*---------------------------------------------------------------- InitScene */
/* InitScene: シーンデータを初期化する
 */
void InitScene( void )
{
    mqoObj = mqoCreateModel( "tachikoma.mqo", 0.001 );

    return;
}
/*-------------------------------------------------------------- UpdateScene */
/* UpdateScene: オブジェクトの状態を更新する
 */
void UpdateScene( void )
{
    float dt = 0.0167; /*1フレームあたりの経過時間（概算）*/
	float d = 1.0; /*移動速度[m/sec]*/
 
    dir += Random( -10.0, 10.0 );
    posx += d * dt * (   sinf( - dir / 180.0 * PI ) );
    posz += d * dt * ( - cosf(   dir / 180.0 * PI ) );
    return;
}
/*------------------------------------------------------------------- update */
/* update: アイドルコールバック関数
 */
void update( void )
{
    UpdateScene();
    return;
}
/*----------------------------------------------------------------- setLight */
/* setLight: 光源を設定する◆
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
/* setMaterial: 材質感を設定する◆
 */
void setMaterial( void )
{
    float diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    float specular[] = { 1.0, 1.0, 1.0, 1.0 };

    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT, GL_SHININESS, 64.0 );
    return;
}

/*----------------------------------------------------------------- DrawGrid */
/*DrawGrid: グリッドを描画する
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
/*DrawAxes: 座標軸を描画する
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
/*------------------------------------------------------------ drawTachikoma */
/* drawTachikoma: タチコマを描画する
 */
void drawTachikoma()
{
    glPushMatrix();
    {
        /*モデル位置姿勢補正*/
        glTranslatef( 0.0, 0.5, 0.0 );  /*位置(x,y,z)*/
        glRotatef( 180.0, 0.0, 1.0, 0.0 ); /*ヨー*/
        glRotatef( 0.0, 1.0, 0.0, 0.0 ); /*ピッチ*/
        glRotatef( 0.0, 0.0, 0.0, 1.0 ); /*ロール*/

        mqoCallModel( mqoObj );
    }
    glPopMatrix();
    return;
}
/*----------------------------------------------------------------- DrawScene */
/* DrawScene: シーンを描画する
 */
void DrawScene( void )
{
    glShadeModel( GL_FLAT ); /*◆*/
    setLight();/*◆光源を設定する*/

    glPushMatrix(); /*現在の変換マトリクスを保存（プッシュ）*/
    {
        /*▼シェーディングしないオブジェクトの描画*/
        DrawGrid( 100, 100 );
        DrawAxes( 100.0 );

        /*▼シェーディングするオブジェクトの描画*/
        glEnable( GL_LIGHTING );/*◆シェーディング有効化*/
        glEnable( GL_LIGHT0 );/*◆０番光源有効化*/

        glTranslatef( posx, 0.0, posz );  /*位置(x,y,z)*/
        glRotatef( dir, 0.0, 1.0, 0.0 ); /*ヨー*/
        glRotatef( 0.0, 1.0, 0.0, 0.0 ); /*ピッチ*/
        glRotatef( 0.0, 0.0, 0.0, 1.0 ); /*ロール*/

        drawTachikoma();

        glDisable( GL_LIGHT0 );/*◆０番光源有効化*/   
        glDisable( GL_LIGHTING );/*◆シェーディング無効化*/

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

    /*▼背景クリア，Ｚバッファクリア*/
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); /*◆*/

    /*▼モデルビュー変換初期化*/
    glLoadIdentity();

    /*▽ビューイング変換設定*/
    gluLookAt( 5.0, 10.0, 5.0, posx, 0.0, posz, 0.0, 1.0, 0.0 );
 
    /*▼平行投影*/
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 15.0, aspect, 1.0, 200.0 );
    glMatrixMode( GL_MODELVIEW );

    /*▼描画本体*/
    glEnable( GL_DEPTH_TEST ); /*◆陰面処理有効化*/

    /*▼シーン描画*/
    DrawScene();

    glDisable( GL_DEPTH_TEST ); /*◆陰面処理無効化*/

    glutPostRedisplay(); /*updateをトリガするイベントを発生させる*/

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
        break;
      case 'q':   /* [Q]がキー入力されたら，プログラムを終了する */
        exit(0);
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

    /*RGBAモード＋ダブルバッファモード＋Ｚバッファ */
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH ); /*◆*/

    glutCreateWindow( winname ); /* winnameで指定された名前でウィンドウを開く */
    glClearColor( 0.25, 0.25, 0.4, 1.0 ); /* ウィンドウの背景色の指定 */

    glEnable( GL_NORMALIZE ); /*glScalefを使うとき法線ベクトル修正に必要*/
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

