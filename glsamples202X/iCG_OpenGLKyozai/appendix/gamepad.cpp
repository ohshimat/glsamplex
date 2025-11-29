/* gamepad.cpp インタラクション用コールバック関数の設定サンプル */

#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

float aspect = (float)WINDOW_WIDTH/WINDOW_HEIGHT; /* アスペクト比 */
  
float rot = 0.0; /*回転角度*/
int rot_flag = 0;/*回転フラグ 0:停止, 1:反時計回り, -1:時計回り*/

/*カラーテーブル*/
float red[4]  = { 1.0, 1.0, 0.0, 0.0 };
float green[4]= { 1.0, 0.0, 1.0, 0.0 };
float blue[4] = { 1.0, 0.0, 0.0, 1.0 };
int color_index = 0;

int joystick_button[32]; /*■ゲームパッドボタン状態*/
float joystick_lever[2];   /*■左レバー状態*/

/*---------------------------------------------------------------- updateObj */
/* updateObj: オブジェクトの状態を更新する
 */
void updateObj( void )
{
    /*▼回転角度を更新する*/
    if( rot_flag != 0 ){ /*回転フラグが 0 でなければ‥*/
        rot += 1.0 * rot_flag; /*正なら反時計回り、負なら時計回り*/
        if( rot > 360.0 || rot < -360.0 ){ /*一回転したら止める*/
            rot_flag = 0;
            rot = 0.0;
        }
    }
    return;
}/*--------------------------------------------------------------------- idle */
/* idle: ▼アイドルコールバック関数
 */
void idle( void )
{
    glutForceJoystickFunc(); /*■ジョイスティックイベント発生*/
  
    /*▼オブジェクトの状態を更新する*/
    updateObj();
	
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
    float diffuse[] = { 0.6, 0.6, 0.8, 1.0 }; /*拡散反射係数*/
    float specular[] = { 0.9, 0.9, 0.9, 1.0 }; /*鏡面反射係数*/

    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT, GL_SHININESS, 128.0 );
    return;
}
/*------------------------------------------------------------------ drawObj */
/* drawObj: オブジェクトを描画する
 */
void drawObj( void )
{
    /*▼ワイヤキューブを描画する（陰影はつけない）*/
    glColor3f( red[color_index],
               green[color_index],
               blue[color_index] );    /*描画する図形の色を指定 */
    glutWireCube( 1.0 );

    /*▼陰影を付けてソリッド球を描画する*/

    glEnable( GL_LIGHTING ); /*光源計算を有効にする*/
    glEnable( GL_LIGHT0 ); /*光源#0をONにする*/
  
    setMaterial(); /*材質感設定*/
    glShadeModel( GL_FLAT );
    glutSolidSphere( 0.5, 10, 25 );
  
    glDisable( GL_LIGHT0 ); /*光源#0をOFFにする*/
    glDisable( GL_LIGHTING ); /*照明計算を無効にする*/
    return;
}
/*------------------------------------------------------------------- display */
/* display: ディスプレイコールバック関数（描画関数)
 */
void display( void )
{

    /*▼透視投影*/
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45.0, aspect, 0.1, 10.0 );
    glMatrixMode( GL_MODELVIEW );
    /*▼ビューポート設定*/
    glViewport( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT );
    /*▼背景クリア＋Ｚバッファクリア*/
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    /*▼モデリング変換初期化*/
    glLoadIdentity();
    /*▼ビューイング変換*/
    gluLookAt( 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

    /*▼描画本体*/

    setLight(); /*光源設定*/

    glEnable( GL_DEPTH_TEST ); /*Ｚバッファ処理を有効にする*/

    glPushMatrix(); /*現在の変換マトリクスを保存*/
    glRotatef( - 45.0 * joystick_lever[0], 0.0, 1.0, 0.0 ); /*Ｙ軸について回転*/
    glRotatef( - 45.0 * joystick_lever[1], 1.0, 0.0, 0.0 ); /*Ｘ軸について回転*/
    glRotatef( rot, 0.0, 1.0, 0.0 ); /*Ｚ軸について回転*/
    drawObj();
    glPopMatrix(); /*変換マトリクスを復帰*/
  
    glDisable( GL_DEPTH_TEST ); /*Ｚバッファ処理を無効にする*/
  
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
        printf( "[C]:Change color\n" );
        printf( "[R]:Rotate 30 degree\n" );
        printf( "[LEFT MOUSE BUTTON]:Start CCW rotation\n" );
        printf( "[MIDDLE MOUSE BUTTON]:Stop rotation\n" );
        printf( "[RIGHT MOUSE BUTTON]:Start CW rotation\n" );
        break;
      case 'q':   /* [Q]がキー入力されたら，プログラムを終了する */
        exit(0);
      case '0':   /* [0]がキー入力されたら回転が止まる */
        rot_flag = 0; /*回転フラグをオフ*/
        break;
      case '1':   /* [1]がキー入力されたら回転が始まる */
        rot_flag = 1; /*回転フラグをオン*/
        break;
      case 'c':   /* [C]がキー入力されたら色が変わる */
        color_index += 1;
        if( color_index > 3 ) color_index = 0;
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
            rot_flag = 1; /*回転フラグを反時計回りに*/
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
            rot_flag = 0; /*回転フラグをオフに*/
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
            rot_flag = -1; /*回転フラグを時計周りに*/
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
/*----------------------------------------------------------------- joystick */
/*■ joustick: ジョイスティックコールバック関数
 */
void joystick( unsigned int status, int x, int y, int z )
{
    printf( "Joystick(x,y): %d, %d\n", x, y );
    /*レバーの値を-1.0～1.0に正規化する*/
    joystick_lever[0] = x/1000.0;
    joystick_lever[1] =-y/1000.0; /*上下反転*/
    int i;
    for( i=0; i<32; i++ ){
        if( joystick_button[i] = status & 0x01 << i )
          printf( "1" );
        else
          printf( "0" );
    }
    for( i=0; i<32; i++ )
      if( joystick_button[i] ) printf( ":%d", i );
    printf( "\n" );
}
/*---------------------------------------------------------------- initWindow */
/* initWindow: OpenGLウィンドウの初期化
 */
void initWindow( char *winname )
{
    glutInitWindowPosition( 0, 0 );   /* ウィンドウの左上の位置を(0,0)とする */
    glutInitWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT ); /* ウィンドウのサイズ */
    /*RGBAモード＋ダブルバッファモード＋Ｚバッファモード*/
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutCreateWindow( winname ); /* winnameで指定された名前でウィンドウを開く */
    glClearColor( 0.25, 0.25, 0.40, 1.0 ); /* ウィンドウの背景色の指定 */
    glEnable( GL_NORMALIZE ); /*■glScaleを使う場合、法線ベクトル修正に必要*/
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
    glutIdleFunc( idle );       /* アイドルコールバック関数の指定 */
    glutKeyboardFunc(keyboard); /* キーボード入力のコールバック関数の指定*/
    glutMouseFunc(mouse);       /* マウス入力のコールバック関数の指定*/
    /*■ゲームパッド用コールバック関数 ポーリングタイマー設定：33msec */
    glutJoystickFunc( joystick, 33 ); /*■*/
    printf( "[H]:Help\n" ); /*説明*/
    glutMainLoop();             /* イベント待ちの無限ループへ入る */
    return 0;
}
