#include "platform.h" //<GL/glut.h>
#include "warptex.h"
#include "webcam.h"

void keyboard(unsigned char key, int x, int y);

WarpTex leftview, rightview;
WarpTex leftcam, rightcam;
WebCam camL, camR;

const float DISP_W = 0.16;//表示領域幅[m]
const float DISP_H = 0.09;//表示領域高[m]
const float IPD = 0.0625; //両眼瞳間距離[m]

const int WINDOW_XO = 100;
const int WINDOW_YO = 100;
const int WINDOW_W = 1920 * 0.375; //実サイズに合わせるための調整
const int WINDOW_H = 1080 * 0.375;//実サイズに合わせるための調整
const float DPI = WINDOW_W/2 /0.08; //片眼画像幅80mmのときの解像度dot/m

float aspect = (float)WINDOW_W / WINDOW_H; //アスペクト比（未使用）

float rot = 0.0; //回転角度

bool grid = true;

//--------* update: アイドルコールバック
//
void update(void)
{
	const float da = 2.0;//回転速度パラメータ
	rot += da;
	Sleep(16);
	glutPostRedisplay();
}
//--------* Draw: シーンを描画する
//
void Draw(void)
{
	glEnable(GL_DEPTH_TEST);//デプスオン
	glPushMatrix();//変換マトリクス保存
	{
		//回転（モデリング変換）
		glRotatef(rot * 0.5, 0.0, 1.0, 0.0);//ヨー
		glRotatef(rot * 0.3, 1.0, 0.0, 0.0);//ピッチ
		glRotatef(rot * 0.2, 0.0, 0.0, 1.0);//ロール
		//ワイヤフレーム・キューブ描画
		glColor3f(1.0, 1.0, 1.0);
		glutWireCube(2.0);		//キューブ
		//ポリゴン・トーラス描画
		glRotatef(rot * 0.7, 1.0, 1.0, 1.0);//追加の回転
		glEnable(GL_LIGHTING);	//シェーディングオン
		glEnable(GL_LIGHT0);	//光源#0オン
		glutSolidTorus( 0.2, 0.5, 10, 17);	//トーラス
		glDisable(GL_LIGHT0);	//光源#0オフ
		glDisable(GL_LIGHTING);	//シェーディングオフ
	}
	glPopMatrix();//変換マトリクス復元
	glDisable(GL_DEPTH_TEST);//デプスオフ
	return;
}
//-------- ProjHMD: HMD用投影設定
void ProjHMD( bool righteye )
{
	//設定値(単位[m])
	const float A = 0.04;//実画像面距離
	const float B = 2.5;//虚像面距離
	const float width = 0.08;//片眼実画像領域幅
	const float height = 0.09;//実画像領域高さ
	const float ipd = 0.0625;//瞳間距離
	const float refer = B;//基準面距離=虚像面距離
	const float front = 2.25, back = 20.0;//基準面からのクリップ面距離

	//実画像面領域: 左下(x0,y0),右上(x1,y1)
	float x0, y0, x1, y1;
	if (righteye) { //右眼画像領域X範囲
		x0 = -ipd / 2.0;
		x1 = width - ipd / 2.0;
	}
	else { //左眼画像領域X範囲
		x0 = - width +ipd / 2.0;
		x1 = ipd / 2.0;
	}
	//画像領域Y範囲
	y0 = -height / 2.0;
	y1 = height / 2.0;
	//虚像：実画像倍率
	float m;
	m = B / A;
	//ビューボリュームパラメータ
	float left, right, bottom, top;//ニアクリップ面領域
	float nearclip, farclip;//ニア・ファークリップ面位置
	nearclip = refer - front;//ニアクリップ面位置
	farclip = refer + back;//ファークリップ面位置
	//ニアクリップ位置・基準面位置比
	float k;
	k = nearclip / refer;
	//実画像領域からニアクリップ領域へのスケール係数
	float s;
	s = m * k;
	//ニアクリップ面領域: 左下(left,bottom),右上(right,top)
	left = x0 * s;
	right = x1 * s;
	bottom = y0 * s;
	top = y1 * s;
	//投影変換の適用
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(left, right, bottom, top, nearclip, farclip);
	glMatrixMode(GL_MODELVIEW);
	//視差分の投影中心移動（補正のビューイング変換）
	if( righteye )
		glTranslatef(-ipd / 2.0, 0.0, 0.0);//右眼の場合
	else 
		glTranslatef(ipd / 2.0, 0.0, 0.0);//左眼の場合
	return;
}
//--------* display: ディスプレイコールバック
//
void display(void)
{
	//--------全描画領域クリア
	glViewport(0, 0, WINDOW_W, WINDOW_H);
	glClearColor(0.25, 0.25, 0.4, 1.0);//背景色設定
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//--------ビューイング変換
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.5, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	//--------右眼画像生成
	rightview.applyViewport();//描画・歪補正領域設定
	glPushMatrix();
	if (camR.isLatest()) {
		rightcam.updateTex(camR.buffer());
	}
	rightcam.show(-(0.08 / 2.0 - IPD / 2.0) * WINDOW_W / 0.16, 0, 0.65, -0.65 * 3 / 4);
	ProjHMD( true );//右眼投影変換
	Draw();//シーンレンダリング
	glPopMatrix();
	rightview.updateTex();//右眼歪補正画像生成
	//--------左眼画像生成
	leftview.applyViewport();//描画・歪補正領域設定
	glPushMatrix();
	if (camL.isLatest()) {
		leftcam.updateTex(camL.buffer());
	}
	leftcam.show((0.08 / 2.0 - IPD / 2.0) * WINDOW_W / 0.16, 0, 0.65, -0.65*3/4 );
	ProjHMD( false );//左眼投影変換
	Draw();//シーンレンダリング
	glPopMatrix();
	leftview.updateTex();//左眼歪補正画像生成
	//--------歪補正画像提示
	glViewport(0, 0, WINDOW_W, WINDOW_H);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	//右眼歪補正画像提示
	rightview.applyViewport();//右眼画像領域指定
	rightview.show();//歪補正画像提示

	//左眼歪補正画像提示
	leftview.applyViewport();//左眼画像領域指定
	leftview.show();//歪補正画像提示

	glutSwapBuffers();
	return;
}
//
//--------* Conifg: 諸設定
//
void Config(void)
{
	const char CAM_NAME[] = "IPEVO V4K";
	const int CAM_FPS = 60;

	//右眼側
	rightview.setViewport(WINDOW_W / 2, 0, WINDOW_W / 2, WINDOW_H);
	rightview.config();
	rightview.setCenter(-(0.08 / 2.0 - IPD / 2.0) * WINDOW_W / 0.16, 0.0);
	rightview.updateMesh();

	rightcam.setViewport(WINDOW_W / 2, 0, WINDOW_W / 2, WINDOW_H);
	rightcam.configExt();
	rightcam.setCenter(0.0, 0.0);
	rightcam.updateMesh();

	camR.config(WarpTex::CAM_W, WarpTex::CAM_H, CAM_FPS);
	camR.open(0, CAM_NAME);

	//左眼側
	leftview.setViewport(0, 0, WINDOW_W / 2, WINDOW_H);
	leftview.config();
	leftview.setCenter(  (0.08 / 2.0 - IPD / 2.0) * WINDOW_W / 0.16, 0.0);
	leftview.updateMesh();

	leftcam.setViewport(0, 0, WINDOW_W / 2, WINDOW_H);
	leftcam.configExt();
	leftcam.setCenter(0.0, 0.0);
	leftcam.updateMesh();

	camL.config(WarpTex::CAM_W, WarpTex::CAM_H, CAM_FPS);
	camL.open(1, CAM_NAME);

	return;
}
//--------* main関数
//
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);		//glutの初期化
	glutInitWindowPosition(WINDOW_XO, WINDOW_YO);//ウィンドウ原点(左上角)指定
	glutInitWindowSize(WINDOW_W, WINDOW_H);//ウィンドウサイズ指定
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(argv[0]);
	glEnable(GL_NORMALIZE);//法線ベクトル補正機能有効化
	glShadeModel(GL_FLAT);//

	Config();

	glutDisplayFunc(display);	//ディスプレイコールバック
	glutIdleFunc(update);		//アイドルコールバック
	glutKeyboardFunc(keyboard);	//キーボードコールバック
	glutMainLoop();				//メインループ
	return 0;
}
