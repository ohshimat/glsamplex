#include "gl/glut.h"

//両眼瞳間距離
extern float IPD;//[m]
//◆スクリーン実サイズ
extern float DISP_W;//[m]
extern float DISP_H;//[m]

extern float head_x, head_y, head_z;
extern float head_roll, head_pitch, head_yaw;

//-------- ProjWall: WALL用投影設定
void ProjWall( float cx, float cy, float cz)
{
	//設定値(単位[m])
	float width = DISP_W;//8.0;//片眼実画像領域幅
	float height = DISP_H; //4.5;//実画像領域高さ
	const float front = 3.0, back = 10.0;//基準面からのクリップ面距離

	const float screen = cz; //基準面距離

	//実画像面領域: 左下(x0,y0),右上(x1,y1)
	float x0, y0, x1, y1;

	x0 = -cx - width / 2.0;//画像面左端
	x1 = -cx + width / 2.0;//画像面右端
	//画像領域Y範囲
	y0 = -cy - height / 2.0;
	y1 = -cy + height / 2.0;

	//ビューボリュームパラメータ
	float left, right, bottom, top;//ニアクリップ面領域
	float nearclip, farclip;//ニア・ファークリップ面位置
	nearclip = screen - front;//ニアクリップ面位置
	farclip = screen + back;//ファークリップ面位置
	//ニアクリップ位置・基準面位置比
	float k;

	k = nearclip / screen;
	//実画像領域からニアクリップ領域へのスケール係数

	//ニアクリップ面領域: 左下(left,bottom),右上(right,top)
	left = x0 * k;
	right = x1 * k;
	bottom = y0 * k;
	top = y1 * k;

	//投影変換の適用
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(left, right, bottom, top, nearclip, farclip);
	//ビューイング変換モードに復帰
	glMatrixMode(GL_MODELVIEW);

	glTranslatef(-cx, -cy, -cz);

	return;
}
//-------- 