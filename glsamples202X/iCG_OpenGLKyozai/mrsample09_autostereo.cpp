//#include "platform.h" //<GL/glut.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <math.h>

#include <windows.h>
#include "gl/glut.h"

void ProjHMD( float cx, float cy );//◆
void ProjWall(float cx, float cy, float cz );

//ウィンドウ位置・サイズ
const int WINDOW_XO = 50;
const int WINDOW_YO = 50;
const float WINDOW_K = 1;//実サイズに合わせるための調整
const int WINDOW_W = (int)(1024.0 * WINDOW_K);
const int WINDOW_H = (int)( 768.0 * WINDOW_K);
//ビューポートサイズ
const int VIEWPORT_W = WINDOW_W/2;
const int VIEWPORT_H = WINDOW_H;
//両眼瞳間距離
const float IPD = 0.0625;
//◆片眼スクリーン実サイズ
float DISP_W = 1.024;//[m]
float DISP_H = 0.768;//[m]

//投影変換パラメータ
float clip_near = 0.05;//ニアクリップ距離[m]
float clip_far = 10.0;//ファークリップ距離[m]
float fov = 100.0;//垂直視野角[度]
float aspect = (float)VIEWPORT_W / VIEWPORT_H;//横縦比

//視野変換パラメータ
float head_x = 0.0;
float head_y = 0.0;
float head_z = 0.5;
float head_roll = 0.0;
float head_pitch = 0.0;
float head_yaw = 0.0;

//シーンデータ
float rot = 0.0;//オブジェクトを回転させる角度パラメータ

float mousex, mousey;
/*-------------------------------------------------------------- mouseMotion
 * mouse - passive mouse motion callback function
 */
 void mouse(int x, int y)
{
	mousex = (float)x / WINDOW_W * 2.0 - 1.0; //normalize x position
	mousey = (float)y / WINDOW_H * 2.0 - 1.0; //normalize y position
	return;
}
//-------- update: アイドルコールバック
void update(void)
{
	const float da = 2.0;//回転速度パラメータ
	rot += da;
	rot = 45.0;

	glutPostRedisplay();
	Sleep(16);//60FPS程度にインターバル調整

	head_x = mousex * DISP_W / 2.0;
	head_y = -mousey * DISP_H / 2.0;

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
		glutWireCube(0.15);		//キューブ
		//ポリゴン・トーラス描画
		glRotatef(-rot * 0.7, 1.0, 1.0, 1.0);//追加の回転
		glEnable(GL_LIGHTING);//シェーディングON
		glutSolidTorus( 0.01, 0.05, 10, 17);//トーラス
		glDisable(GL_LIGHTING);//シェーディングOFF
	}
	glPopMatrix();//変換マトリクス復元
	glDisable(GL_DEPTH_TEST);//デプステストOFF
	return;
}
//-------- DrawStencil: ステンシルを描画する
void DrawStencil(void)
{
	glDrawBuffer(GL_NONE);//カラーバッファ書込み無効化
	glDepthMask(FALSE);//デプスバッファ書込み無効化
	//
	glClearStencil(0);//ステンシルクリア値
	glClear(GL_STENCIL_BUFFER_BIT);//ステンシルバッファクリア
	glStencilFunc(GL_ALWAYS, 1, 1);//ステンシル書込み有効化
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glEnable(GL_STENCIL_TEST);//ステンシルテスト有効化
	//----ステンシル図形描画
	glViewport(0, 0, WINDOW_W, WINDOW_H);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( 0.0, WINDOW_W-1, 0.0, WINDOW_H-1, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_LINES);
	for (int i = 0; i < WINDOW_W; i += 2) {
		glVertex2d(i, 0);
		glVertex2d(i, WINDOW_H - 1);
	}
	glEnd();
	//
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);//<<◆ステンシル作成終了
	glDrawBuffer(GL_BACK);//カラーバッファ書込み有効化
	glDepthMask(TRUE);//デプスバッファ書込み有効化
}
//-------- display: ディスプレイコールバック
void display(void)
{
	//----全描画領域クリア
	glViewport(0, 0, WINDOW_W, WINDOW_H);
	glClearColor(0.25, 0.25, 0.4, 1.0);//背景色
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//バッファクリア

	//----ビューイング変換(視野変換：視点位置・視方向の逆変換)
	glMatrixMode(GL_MODELVIEW);//モデルビュー変換モード
	glLoadIdentity();//恒等変換

	//--------右眼画像描画
	glStencilFunc(GL_EQUAL, 0, 1);//<<◆ステンシル値0の画素に上描き
	glPushMatrix();
	ProjWall( head_x + IPD / 2.0, head_y, head_z );//右視点透視投影変換
	Draw();//レンダリング
	glPopMatrix();

	//glClear(GL_DEPTH_BUFFER_BIT);
	//--------左眼画像描画
	glStencilFunc(GL_EQUAL, 1, 1);//<<◆ステンシル値1の画素に上描き
	glPushMatrix();
	ProjWall(head_x - IPD / 2.0, head_y, head_z);//左視点透視投影変換
	Draw();//レンダリング
	glPopMatrix();

	//----バッファスワップ
	glutSwapBuffers();	
	return;
}
//-------- config
void config()
{
	DrawStencil();
	glDisable(GL_LINE_SMOOTH);
}
//-------- main関数
int main(int argc, char *argv[])
{
	//---- グラフィックウィンドウ生成
	glutInit(&argc, argv);//GLUT初期化
	glutInitWindowPosition(WINDOW_XO, WINDOW_YO);//ウィンドウ原点(左上)指定
	glutInitWindowSize(WINDOW_W, WINDOW_H);//ウィンドウサイズ指定
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH|GLUT_STENCIL);//バッファ構成≪◆
	glutCreateWindow(argv[0]);//ウィンドウ生成
	//---- penGL描画モード任意諸設定
	glShadeModel(GL_FLAT);//フラットシェーディングモード
	glEnable(GL_LIGHT0);//基本光源ON
	//---- コールバック設定
	glutDisplayFunc(display);//ディスプレイコールバック
	glutIdleFunc(update);//アイドルコールバック
	glutPassiveMotionFunc(mouse);//

	config();

	//---- メインループ開始
	glutMainLoop();
	return 0;
}
