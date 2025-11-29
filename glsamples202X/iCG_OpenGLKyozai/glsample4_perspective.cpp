/* glsample4_perspective.cpp */

#include "platform.h" //<GL/glut.h>

#define WINDOW_XO 100
#define WINDOW_YO 100
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

float aspect = (float)WINDOW_WIDTH/WINDOW_HEIGHT; /* アスペクト比 */
  
float rot = 0.0; /*回転角度*/
int rot_mode = 0; /*回転モード 0:停止, 1:反時計回り, -1:時計回り*/

//★0610　キーボードで変化させたいパラメータ
float obj_x = 0.0;
float obj_z = 0.0;

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

//★0610光源を設定する関数を追加
void setMainLight( void )
{
	float light_color[] = { 0.8, 0.8, 0.8, 1.0 }; //光源の色（白）
	float light_pos[] = { 0.2, 1.0, 0.2, 0.0 }; //光源の方向（最後の０は、無限遠にある光源＝真上から）

	glLightfv( GL_LIGHT0, GL_DIFFUSE, light_color ); //光源色の設定
	glLightfv( GL_LIGHT0, GL_POSITION, light_pos ); //光源位置（方向）の設定
	return;
}

void setSubLight( void )
{
	float light_color[] = { 0.1, 0.1, 0.2, 1.0 }; //光源の色（白）
	float light_pos[] = { 0.0, -1.0, 0.5, 0.0 }; //光源の方向（最後の０は、無限遠にある光源＝真上から）
	float ambient[] = { 0.8, 0.8, 0.8, 1.0 }; //環境光反射係数

	glLightfv( GL_LIGHT1, GL_AMBIENT, ambient );

	glLightfv( GL_LIGHT1, GL_DIFFUSE, light_color ); //光源色の設定
	glLightfv( GL_LIGHT1, GL_POSITION, light_pos ); //光源位置（方向）の設定
	return;
}

//★0610質感を設定する関数を追加
void setMaterialColor( float r, float b, float g )
{
	//float diffuse[] = { 0.6, 0.6, 0.8, 1.0 }; //拡散反射係数
	float diffuse[4]; //配列の宣言の仕方に注意
	//float specular[] = { 0.9, 0.9, 0.9, 1.0 }; //鏡面反射係数
	float specular[] = { 0.4, 0.4, 0.4, 1.0 }; //鏡面反射係数
	float ambient[] = { 0.0, 0.8, 0.8, 1.0 }; //環境光反射係数

	diffuse[0] = r;
	diffuse[1] = g;
	diffuse[2] = b;
	glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse ); //拡散反射係数の設定
	glMaterialfv( GL_FRONT, GL_SPECULAR, specular ); //鏡面反射係数の設定
	glMaterialf( GL_FRONT, GL_SHININESS, 16.0 );//鏡面反射の度合い（0.0～128.0）
	glMaterialfv( GL_FRONT, GL_AMBIENT, ambient );//環境光反射係数の設定
	return;
}

/*------------------------------------------------------------------ drawObj */
/* drawCube: オブジェクトを描画する
 */
void drawCube( void )
{
    //glColor3f( 1.0, 1.0, 1.0 ); /*描画する図形の色を白に指定*/
	//setMaterialColor( 1.0, 1.0, 1.0 ); //★0610マテリアル設定
    glutSolidCube( 0.5 );        /*ワイヤキューブを描画する*/
    return;
}
void drawSphere( void )
{
	//glColor3f( 1.0, 0.0, 0.0 );
	glutSolidSphere( 0.5, 32, 16 ); //半径、分割数、スライス数
}
/*----------------------------------------------------------------- DrawScene */
/* DrawScene: シーンを描画する
 */
void DrawScene( void )
{
	setMainLight(); //★0610　照明環境の設定
	setSubLight();

    glPushMatrix(); /*現在の変換マトリクスを保存（プッシュ）*/
    {
        glRotatef( rot, 0.0, 1.0, 0.0 );
		////////////////////////////////////////

		glPushMatrix();
		glTranslatef( 1.0, 0.0, 0.0 );
	    setMaterialColor( 0.0, 1.0, 1.0 ); //★0610マテリアル設定
        drawCube();
		glPopMatrix();

		glPushMatrix();
		glTranslatef( -1.0, 0.0, 0.0 );
	    setMaterialColor( 1.0, 1.0, 0.0 ); //★0610マテリアル設定
		drawCube();
		glPopMatrix();

		glPushMatrix();
		glTranslatef( obj_x, 0.0, obj_z );
		for( int i=0; i<10; i++ ){
			glPushMatrix();
			float r = (float)i/10.0;
			float x = (float)i/100.0;
	        setMaterialColor( r, 0.0, 0.0 ); //★0610マテリアル設定
			glTranslatef( x, 0.0, 0.0 );
			glScalef( 0.1, 0.1, 0.1 );
		    drawSphere();
			glPopMatrix();
		}
		glPopMatrix();

		////////////////////////////////////////
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
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); //★Zバッファもクリア

    /*▼モデルビュー変換初期化*/
    glLoadIdentity();

    /*▼ビューイング変換設定*/
    gluLookAt( 0.0, 1.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );//★後半戦
 
    /*▼透視投影変換*/
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 40.0, aspect, 0.1, 10.0 );//★後半戦
    glMatrixMode( GL_MODELVIEW );

    /*▼描画本体*/
	glEnable( GL_LIGHTING ); //★0610シェーディング機能有効化
	glEnable( GL_LIGHT0 ); //★0610　0番ライトON
	glEnable( GL_LIGHT1 ); //★0610　1番ライトON
	glEnable( GL_DEPTH_TEST ); //★

	glShadeModel( GL_FLAT ); //★ポリゴンポリゴンしている絵が好きな人
	//glShadeModel( GL_SMOOTH ); //滑らかな感じが好きな人

    DrawScene();
	glDisable( GL_DEPTH_TEST ); //★
	glDisable( GL_LIGHT0 ); //★0610　0番ライトOFF
	glDisable( GL_LIGHT1 ); //★0610　1番ライトOFF
	glDisable( GL_LIGHTING ); //★0610シェーディング機能無効化

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
        printf( "[MOUSE-RIGHT ]:Start rotation(CW)\n" );        break;
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

	  case 'a': //左に移動
		  obj_x -= 0.05; //5cm左へ
		  break;
		 
	  case 'd': //右に移動
		  obj_x += 0.05; //5cm右へ
		  break;

	  case 's': //前に移動
		  obj_z += 0.05;
		  break;

	  case 'w': //後ろに移動
		  obj_z -= 0.05;
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
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );//★

    glutCreateWindow( winname ); /* winnameで指定された名前でウィンドウを開く */
    glClearColor( 0.25, 0.25, 0.4, 1.0 ); /* ウィンドウの背景色の指定 */

	glEnable( GL_NORMALIZE ); //★0610法線ベクトル補正機能有効化
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
