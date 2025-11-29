//#include "platform.h" //<GL/glut.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <math.h>

#include <windows.h>
#include "gl/glut.h"

//ウィンドウ位置・サイズ
const int WINDOW_XO = 100;
const int WINDOW_YO = 100;
const float WINDOW_K = 0.375;//実サイズに合わせるための調整
const int WINDOW_W = (int)(1920.0 * WINDOW_K); 
const int WINDOW_H = (int)(1080.0 * WINDOW_K);
//ビューポートサイズ
const int VIEWPORT_W = WINDOW_W;
const int VIEWPORT_H = WINDOW_H;

//ヘッドトラッキングパラメータ
float head_x = 0.0;
float head_y = 0.0;
float head_z = 2.0;
float head_roll = 0.0;
float head_pitch = 0.0;
float head_yaw = 0.0;

//クリッピング距離
float clip_near = 0.05;
float clip_far = 10.0;
float fov = 60.0;//垂直視野角
float aspect = (float)VIEWPORT_W / VIEWPORT_H;

float rot = 0.0;//オブジェクトを回転させる角度パラメータ

//-------- update: アイドルコールバック
void update(void)
{
	const float da = 2.0;//回転速度パラメータ
	rot += da;
	glutPostRedisplay();
	Sleep(16);//60FPS程度にインターバル調整
	return;
}
//-------- Draw: シーンを描画する
void Draw(void)
{
	glEnable(GL_DEPTH_TEST);//デプステストON
	glPushMatrix();//変換マトリクス保存
	{
		//回転（モデリング変換）
		glRotatef(rot * 0.2, 0.0, 1.0, 0.0);//ヨー
		glRotatef(rot * 0.3, 1.0, 0.0, 0.0);//ピッチ
		glRotatef(rot * 0.5, 0.0, 0.0, 1.0);//ロール
		//ワイヤフレーム・キューブ描画
		glColor3f(1.0, 1.0, 1.0);
		glutWireCube(2.0);		//キューブ
		//ポリゴン・トーラス描画
		glRotatef(-rot * 0.7, 1.0, 1.0, 1.0);//追加の回転
		glEnable(GL_LIGHTING);//シェーディングON
		glutSolidTorus( 0.2, 0.5, 10, 17);//トーラス
		glDisable(GL_LIGHTING);//シェーディングOFF
	}
	glPopMatrix();//変換マトリクス復元
	glDisable(GL_DEPTH_TEST);//デプステストOFF
	return;
}
//-------- display: ディスプレイコールバック
void display(void)
{
	//----全描画領域クリア
	glViewport(0, 0, WINDOW_W, WINDOW_H);
	glClearColor(0.25, 0.25, 0.4, 1.0);//背景色
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//----ビューイング変換(視野変換：視点位置・視方向の逆変換)
	//視点位置(0.0,0.0,2.0),視方向(0.0,0.0,0.0)
	glMatrixMode(GL_MODELVIEW);//モデルビュー変換モード
	glLoadIdentity();//恒等変換
	glRotatef( -head_roll,  0.0, 0.0, 1.0);//回転逆変換：ロール
	glRotatef( -head_pitch, 1.0, 0.0, 0.0);//ピッチ
	glRotatef( -head_yaw,   0.0, 1.0, 0.0);//ヨー
	glTranslatef( -head_x, -head_y, -head_z );//視点位置逆変換

	//--------単眼画像描画
	//ビューポート設定
	glViewport(0, 0, VIEWPORT_W, VIEWPORT_H);
	//透視投影変換
	glMatrixMode(GL_PROJECTION);//投影変換モード
	glLoadIdentity();
	gluPerspective(fov, aspect, clip_near, clip_far);
	//相対ビューイング変換(基準位置からの差分)
	glMatrixMode(GL_MODELVIEW);//モデルビュー変換モードに復帰
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	//レンダリング
	Draw();
	glPopMatrix();
	//----バッファスワップ
	glutSwapBuffers();	
	return;
}
//-------- main関数
int main(int argc, char *argv[])
{
	//---- グラフィックウィンドウ生成
	glutInit(&argc, argv);//GLUT初期化
	glutInitWindowPosition(WINDOW_XO, WINDOW_YO);//ウィンドウ原点(左上)指定
	glutInitWindowSize(WINDOW_W, WINDOW_H);//ウィンドウサイズ指定
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);//バッファ構成
	glutCreateWindow(argv[0]);//ウィンドウ生成
	//---- penGL描画モード任意諸設定
	glShadeModel(GL_FLAT);//フラットシェーディングモード
	glEnable(GL_LIGHT0);//基本光源ON
	//---- コールバック設定
	glutDisplayFunc(display);//ディスプレイコールバック
	glutIdleFunc(update);//アイドルコールバック
	//---- メインループ開始
	glutMainLoop();
	return 0;
}
