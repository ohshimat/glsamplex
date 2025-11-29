#include "gl/glut.h"

//両眼瞳間距離
extern float IPD;//[m]
//◆スクリーン実サイズ
extern float DISP_W;//[m]
extern float DISP_H;//[m]

//-------- ProjHMD: HMD用投影設定
void ProjHMD( float cx, float cy )
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

	x0 = cx - width / 2.0;//画像面左端
	x1 = cx + width / 2.0;//画像面右端
	//画像領域Y範囲
	y0 = cy - height / 2.0;
	y1 = cy + height / 2.0;
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
	//ビューイング変換モードに復帰
	glMatrixMode(GL_MODELVIEW);
	return;
}
