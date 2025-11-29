/* glsample7_arm.cpp */

#include <math.h> /*数学関数用*/

#include "platform.h" //<GL/glut.h>

#define WINDOW_XO 100
#define WINDOW_YO 100
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

float aspect = (float)WINDOW_WIDTH/WINDOW_HEIGHT; /* アスペクト比 */
  
float rot = 0.0; /*回転角度*/
int rot_flag = 0; /*回転モード 0:停止, 1:反時計回り, -1:時計回り*/

float forearm_angle; /*■下腕回転角度*/
float upperarm_angle; /*■上腕回転角度*/
float t = 0.; /*時間パラメータ*/
#define PI 3.1415926 /*■円周率*/

/*---------------------------------------------------------------- InitScene */
/* InitScene: シーンデータを初期化する
 */
void InitScene( void )
{
	rot = 0.0;
    rot_flag = 0;

    return;
}
/*-------------------------------------------------------------- UpdateScene */
/* UpdateScene: オブジェクトの状態を更新する
 */
void UpdateScene( void )
{
    static float k1 = 360.0; /*下腕周期パラメータ*/
    static float k2 = 180.0; /*上腕周期パラメータ*/
	static float p1 = 0.0; /*下腕位相パラメータ*/
	static float p2 = 0.0; /*上腕位相パラメータ*/
	static float a1 = 60.0; /*下腕振幅パラメータ*/
	static float a2 = 120.0; /*上腕振幅パラメータ*/

	/*▼FPS調整パラメータ*/
	static float k = 0.1; 
	/* 垂直同期機能のあるNVIDIAビデオカードの場合には1.0
	 * その他INTELや垂直同期機能のないビデオカードでは
	 * 更新速度が暴走気味なので 0.1 など小さな値に変更
	 */

    /*回転角度を更新する*/
    if( rot_flag != 0 ){ /*回転フラグが 0 でなければ‥*/
        t += k * rot_flag * 0.0167; /*時間パラメータ更新*/
        forearm_angle = a1 * sinf( ( t * k1 + p1 ) * PI / 180.0 ); /*下腕運動更新*/
        upperarm_angle = a2 * sinf( ( t * k2 + p2 ) * PI / 180.0 ); /*上腕運動更新*/
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
    float diffuse[] = { 0.5, 0.6, 0.8, 1.0 }; /*拡散反射係数*/
    float specular[] = { 0.9, 0.9, 0.9, 1.0 }; /*鏡面反射係数*/

    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT, GL_SHININESS, 64.0 );
    return;
}
/*------------------------------------------------------------------ drawObj */
/* drawWireCube: ワイヤキューブを描画する
 */
void drawWireCube( void )
{
    glColor3f( 1.0, 1.0, 1.0 );
    glutWireCube( 1.0 );
    return;
}
/*------------------------------------------------------------- drawUpperArm */
/* drawUpperArm: 上腕を描画する
 */
void drawUpperArm()
{
    glPushMatrix();
    {
        /*◆プリミティブ調整*/
        glTranslatef( 0.25, 0.0, 0.0 ); /*基準位置調整*/
        glRotatef( 0.0, 0.0, 1.0, 0.0 ); /*ヨー調整*/
        glRotatef( 0.0, 1.0, 0.0, 0.0 ); /*ピッチ調整*/
        glRotatef( 0.0, 0.0, 0.0, 1.0 ); /*ロール調整*/
        glScalef( 1.0, 0.25, 0.25 ); /*サイズ調整*/
        /*▼プリミティブ描画*/
        glutSolidCube( 0.5 );
    }
    glPopMatrix();
    return;
}
/*-------------------------------------------------------------- drawForeArm */
/* drawForeArm: 下腕を描画する
 */
void drawForeArm()
{
    glPushMatrix();
    {
        /*◆プリミティブ調整*/
        glTranslatef( 0.25, 0.0, 0.0 ); /*基準位置調整*/
        glRotatef( 0.0, 0.0, 1.0, 0.0 ); /*ヨー調整*/
        glRotatef( 0.0, 1.0, 0.0, 0.0 ); /*ピッチ調整*/
        glRotatef( 0.0, 0.0, 0.0, 1.0 ); /*ロール調整*/
        glScalef( 1.0, 0.25, 0.25 ); /*サイズ調整*/
        /*▼プリミティブ描画*/
        glutSolidCube( 0.5 );
    }
    glPopMatrix();
    return;
}
/*------------------------------------------------------------------ drawArm */
/* drawArm: 腕を上腕と下腕を組み立てて描画する
 */
void drawArm()
{
    setMaterial();/*◆材質感を設定する*/
    /*●上腕オブジェクトの描画*/
    glPushMatrix();
    {
        /*◆位置姿勢決め*/
        glTranslatef( 0.0, 0.0, 0.0 );  /*位置(x,y,z)*/
        glRotatef( 0.0, 0.0, 1.0, 0.0 ); /*ヨー*/
        glRotatef( 0.0, 1.0, 0.0, 0.0 ); /*ピッチ*/
        glRotatef( upperarm_angle, 0.0, 0.0, 1.0 ); /*ロール*/
        /*▼上腕パーツ描画*/
        drawUpperArm();

        /*●下腕オブジェクトの描画*/
        glPushMatrix();
        {
            /*◆位置姿勢決め*/
            glTranslatef( 0.5, 0.0, 0.0 ); /*位置(x,y,z)*/
            glRotatef( 0.0, 0.0, 1.0, 0.0 ); /*ヨー*/
            glRotatef( 0.0, 1.0, 0.0, 0.0 ); /*ピッチ*/
            glRotatef( forearm_angle, 0.0, 0.0, 1.0 ); /*ロール*/
            /*▼オブジェクト描画*/
            drawForeArm();

        }
        glPopMatrix();
        /*--------*/
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
        /*◆シーン全体の幾何変換*/
        glRotatef( rot, 0.0, 1.0, 0.0 );

        /*▼シェーディングしないオブジェクトの描画*/
        drawWireCube();

        /*▼シェーディングするオブジェクトの描画*/
        glEnable( GL_LIGHTING );/*◆シェーディング有効化*/
        glEnable( GL_LIGHT0 );/*◆０番光源有効化*/

        drawArm();

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
    gluLookAt( 0.0, 1.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
 
    /*▼平行投影*/
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45.0, aspect, 0.1, 10.0 );
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
        printf( "[1]:Start motion\n" );
        printf( "[0]:Stop motion\n" );
        printf( "[MOUSE-LEFT  ]:Rotate CCW\n" );
        printf( "[MOUSE-CENTER]:Stop/stop motion\n" );
        printf( "[MOUSE-RIGHT ]:Rotate CW\n" );
        break;
      case 'q':   /* [Q]がキー入力されたら，プログラムを終了する */
        exit(0);
      case '0':   /* [0]がキー入力されたら運動が止まる */
        rot_flag = 0; /*フラグをオフ*/
        break;
      case '1':   /* [1]がキー入力されたら運動が始まる */
        rot_flag = 1; /*フラグをオン*/
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
            rot += 10.0; /*一定角度回転(CCW)*/
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
            rot_flag = !rot_flag; /*フラグをON/OFF切替*/
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
            rot -= 10.0; /*一定角度回転(CW)*/
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

    glEnable( GL_NORMALIZE ); /*■glScalefを使うとき法線ベクトル修正に必要*/
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

