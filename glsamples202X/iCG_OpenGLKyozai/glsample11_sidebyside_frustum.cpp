#include "platform.h" //<GL/glut.h>

const int WINDOW_XO = 100;
const int WINDOW_YO = 100;
const int WINDOW_W = 960 * 0.75;
const int WINDOW_H = 540 * 0.75;

float aspect = (float)WINDOW_W / WINDOW_H; //アスペクト比

float rot = 0.0; /*回転角度*/
int rot_mode = 0; /*回転モード 0:停止, 1:反時計回り, -1:時計回り*/

//---------------------------------------------------------------- InitScene
// InitScene: シーンデータを初期化する
//
void InitScene(void)
{
	rot = 0.0;
	rot_mode = 0;
	return;
}
//-------------------------------------------------------------- UpdateScene
// UpdateScene: オブジェクトの状態を更新する
//
void UpdateScene(void)
{
	//▼回転速度パラメータ
	float k = 2.0; //数値を大きくすると早く回転する

	//回転角度を更新する
	switch (rot_mode) {
	case 1:
		rot += k;
		break;
	case 0:
		break;
	case -1:
		rot -= k;
		break;
	}
	return;
}
//------------------------------------------------------------------- update
// update: アイドルコールバック関数
//
void update(void)
{
	UpdateScene();
	/* 垂直同期機能のあるNVIDIAビデオカードを基準として設定：
	 * その他INTELや垂直同期機能のないビデオカードでは
	 * 更新速度が暴走気味なので60Hz=16.7fps程度を設定すること。
	 */
	Sleep(8);//125Hz
	glutPostRedisplay();
	return;
}
//----------------------------------------------------------------- setLight
// setLight: 光源を設定する
//
void setLight(void)
{
	float light_color[] = { 1.0, 1.0, 1.0, 1.0 };
	float light_pos[] = { 0.0, 1.0, 1.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	return;
}
//-------------------------------------------------------------- setMaterial
// setMaterial: 材質感を設定する
//
void setMaterial(void)
{
	float diffuse[] = { 0.6, 0.6, 0.8, 1.0 }; //拡散反射係数
	float specular[] = { 0.9, 0.9, 0.9, 1.0 }; //鏡面反射係数

	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 64.0);
	return;
}
//------------------------------------------------------------------ drawObj
// drawWireCube: ワイヤキューブを描画する
//
void drawWireCube(void)
{
	glColor3f(1.0, 1.0, 1.0);
	glutWireCube(2.0);
	return;
}
//------------------------------------------------------------------ drawObj
// drawSolidSphere: ソリッド球を描画する
//
void drawSolidSphere(void)
{
	setMaterial();//材質感を設定する
	glutSolidSphere(0.5, 20, 25);
	return;
}
//----------------------------------------------------------------- DrawScene
// DrawScene: シーンを描画する
//
void DrawScene(void)
{
	glShadeModel(GL_FLAT); //◆
	setLight();//◆光源を設定する
	glEnable(GL_DEPTH_TEST);
	glPushMatrix(); //現在の変換マトリクスを保存（プッシュ）
	{
		glRotatef(rot * 0.8, 0.0, 0.0, 1.0);
		glRotatef(rot * 1.2, 1.0, 0.0, 0.0);
		glRotatef(rot, 0.0, 1.0, 0.0);

		//▼シェーディングしないオブジェクトの描画
		drawWireCube();

		//▼シェーディングするオブジェクトの描画
		glEnable(GL_LIGHTING);//◆シェーディング有効化
		glEnable(GL_LIGHT0);//◆０番光源有効化

		drawSolidSphere();

		glDisable(GL_LIGHT0);//◆０番光源有効化
		glDisable(GL_LIGHTING);//◆シェーディング無効化

	}
	glPopMatrix(); //変換マトリクスを復元（ポップ）
	glDisable(GL_DEPTH_TEST);
	return;
}
//-------- ProjHMD: HMD Projection
void ProjHMD( bool righteye )
{
	//設定値
	const float A = 0.04;//実画像面距離
	const float B = 2.5;//虚像面距離
	const float width = 0.08;//片眼実画像領域幅
	const float height = 0.09;//実画像領域高さ
	const float ipd = 0.0625;//瞳間距離
	const float refer = B;//基準面距離=虚像面距離
	const float front = 2.25, back = 20.0;//基準面からのクリップ面距離
	//実画像面領域
	float x0, y0, x1, y1;
	if (righteye) {
		x0 = -ipd / 2.0;
		x1 = width - ipd / 2.0;
	}
	else {
		x0 = - width +ipd / 2.0;
		x1 = ipd / 2.0;
	}
	y0 = -height / 2.0;
	y1 = height / 2.0;
	//虚像：実画像倍率
	float m;
	m = B / A;
	//ビューボリュームパラメータ
	float left, right, bottom, top;
	float nearclip, farclip;
	nearclip = refer - front;//ニアクリップ面位置
	farclip = refer + back;//ファークリップ面位置
	//ニアクリップ位置：基準面位置比
	float k;
	k = nearclip / refer;
	//実画像領域からニアクリップ領域へのスケール係数
	float s;
	s = m * k;
	//ニアクリップ面領域
	left = x0 * s;
	right = x1 * s;
	bottom = y0 * s;
	top = y1 * s;
	//投影変換の適用
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(left, right, bottom, top, nearclip, farclip);
	glMatrixMode(GL_MODELVIEW);
	//視差分の投影中心移動
	if( righteye )
		glTranslatef(-ipd / 2.0, 0.0, 0.0);
	else
		glTranslatef(ipd / 2.0, 0.0, 0.0);
	return;
}
/*------------------------------------------------------------------- display */
/* display: ディスプレイコールバック関数（描画関数)
 */
void display(void)
{
	glViewport(0, 0, WINDOW_W, WINDOW_H);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.5, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//// LEFT VIEW
	glViewport(0, 0, WINDOW_W / 2, WINDOW_H);
	glPushMatrix();
	ProjHMD( false );
	DrawScene();
	glPopMatrix();

	//// RIGHT VIEW
	glViewport(WINDOW_W / 2, 0, WINDOW_W / 2, WINDOW_H);
	glPushMatrix();
	ProjHMD( true );
	DrawScene();
	glPopMatrix();

	glutSwapBuffers();
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
		printf("Instruction\n");
		printf("[H]:Help\n");
		printf("[Q]:Quit\n");
		printf("[0]:Stop rotation\n");
		printf("[1]:Start rotation(CCW)\n");
		printf("[2]:Start rotation(CW)\n");
		printf("[MOUSE-LEFT  ]:Start rotation(CCW)\n");
		printf("[MOUSE-CENTER]:Stop rotation\n");
		printf("[MOUSE-RIGHT ]:Start rotation(CW)\n");
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
void mouse(int button, int state, int x, int y)
/* button は押された（離された）ボタン，
  state：押したか離したかの状態，
  x, y：マウスの座標値 */
{
	switch (button) {
	case GLUT_LEFT_BUTTON: /*左マウスボタン*/
		switch (state) {
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
		switch (state) {
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
		switch (state) {
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
void initWindow(char *winname)
{
	glutInitWindowPosition(WINDOW_XO, WINDOW_YO);//ウィンドウ原点(左上角)指定
	glutInitWindowSize(WINDOW_W, WINDOW_H);//ウィンドウサイズ指定

	//RGBAモード＋ダブルバッファモード＋Ｚバッファ
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow(winname); // winnameで指定された名前でウィンドウを開く
	glClearColor(0.25, 0.25, 0.4, 1.0); // ウィンドウの背景色の指定

	glEnable(GL_NORMALIZE); //法線ベクトル補正機能有効化
	return;
}
//---------------------------------------------------------------------- main
// main関数
int main(int argc, char *argv[])
{
	glutInit(&argc, argv); //glutの初期化

	initWindow(argv[0]);

	glutDisplayFunc(update);//シーン更新をディスプレイコールバックに指定
	glutIdleFunc(display);	//描画関数をアイドルコールバックとして指定
	glutKeyboardFunc(keyboard);//キーボード入力のコールバック関数の指定
	glutMouseFunc(mouse);	//マウス入力のコールバック関数の指定
	printf("[H]:Help\n");	//操作説明

	InitScene(); //シーンデータを初期化する
	glutMainLoop(); //イベント待ちの無限ループへ入る
	return 0;
}
