#include "platform.h"

#include "WarpTex.h"
#include <stdarg.h>

WarpTex::WarpTex(void)
{
	//デフォルト設定
	setCenter(0, 0);
	setParams(-0.0005, -0.0005, -0.0005);
}
WarpTex::~WarpTex(void)
{
}
void WarpTex::copy(const WarpTex *src)
{
}
//
void WarpTex::allocTex(GLenum pixel, int x, int y, GLenum format )
{
	//テクスチャ登録
	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexImage2D(GL_TEXTURE_2D, 0, pixel, x, y, 0, format, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//テクスチャ領域サイズ
	texwidth = x;
	texheight = y;
	//GLオブジェクトリスト生成
	objlist = glGenLists(3);
}
//フレームバッファからテクスチャを更新する
void WarpTex::updateTex(unsigned char *image)
{
	glBindTexture(GL_TEXTURE_2D, texid);
	if (image!=nullptr) {
		//指定のバッファからテクスチャを更新する
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, subwidth, subheight, GL_BGR_EXT, GL_UNSIGNED_BYTE, image);	
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, subwidth, subheight, 
			GL_BGRA_EXT, GL_UNSIGNED_BYTE, image);
	}
	else {
		//バックバッファからテクスチャメモリにコピー
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, xo, yo, subwidth, subheight);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}
//フレームバッファからテクスチャを更新する場合の初期化処理
void WarpTex::config(void)
{
	allocTex( GL_RGB, 1024, 1024, GL_RGB );

	//ビューポートサイズ(width,height)が
	//サブイメージサイズ(subwidth,subheight)と同じ
	subwidth = width;
	subheight = height;
}
//メインメモリからテクスチャを更新する場合の初期化処理
void WarpTex::configExt(void)
{
	//カメラを名前なしでopenした場合
	//allocTex(GL_RGB, 1024, 512, GL_BGR_EXT);
	//カメラを名前指定でOpenした場合
	allocTex(GL_RGBA, 1024, 512, GL_BGRA_EXT);//////////
	//(1024, 512)がOpenGL的には正しそうだが
	//(640,480)でも動作することが確認できた

	//カメラ画像サイズがサブイメージサイズ
	subwidth = CAM_W;
	subheight = CAM_H;
}
//歪を適用するビューポートの設定
void WarpTex::setViewport(int x, int y, int w, int h)
{
	xo = x;
	yo = y;
	width = w;
	height = h;
}
//歪みの中心設定
void WarpTex::setCenter(float x, float y)
{
	cx = x;
	cy = y;
}
//テスト用格子描画指定
void WarpTex::switchGrid(bool sw)
{
	grid = sw;
}
//歪みパラメータ設定
void WarpTex::setParams(float kr, float kg, float kb)
{
	kR = kr;
	kB = kb;
	kG = kg;
}
//ビューポートのサイズを取得
void WarpTex::getViewport( void )
{
	GLint v[4];
	glGetIntegerv(GL_VIEWPORT, v);
	xo = v[0];
	yo = v[1];
	width = v[2];
	height = v[3];
}
//頂点更新
void WarpTex::makeMesh(void)
{
	float dw = width / (float)MESH_W;
	float dh = height / (float)MESH_H;
	float x, y, u, v, s, t;
	for (int i = 0; i<MESH_W + 1; i++) {
		x = dw*i - width/2.0;
		u = x - cx;
		for (int j = 0; j<MESH_H + 1; j++) {
			y = dh*j - height/2.0;
			v = y - cy;
			s = u * (u*u + v * v)/ (float)width;
			t = v * (u*u + v * v)/ (float)height;
			vR[i][j].x = x + kR * s;
			vR[i][j].y = y + kR * t;
			vG[i][j].x = x + kG * s;
			vG[i][j].y = y + kG * t;
			vB[i][j].x = x + kB * s;
			vB[i][j].y = y + kB * t;
		}
	}
}
//メッシュ更新
void WarpTex::updateMesh()
{
	//頂点データ更新
	makeMesh();
	//Rメッシュ更新
	glNewList(objlist, GL_COMPILE);
	drawMesh(vR);
	glEndList();
	//Gメッシュ更新
	glNewList(objlist+1, GL_COMPILE);
	drawMesh(vG);
	glEndList();
	//Bメッシュ更新
	glNewList(objlist+2, GL_COMPILE);
	drawMesh(vB);
	glEndList();
}
//ビューポート適用
void WarpTex::applyViewport()
{
	glViewport(xo, yo, width, height);
}
//画像描画
void WarpTex::show( float x, float y, float sx, float sy)
{
	//投影変換
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( -width/2.0, width/2.0, -height/2.0, height/2.0, -1, 1);

	//モデルビュー変換
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(x, y, 0.0);
	glScalef(sx, sy, 1.0);

	//歪み補正画像描画
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glColor3f(1.0, 1.0, 1.0);
	//Rメッシュ描画
	glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);//Rプレーンのみ描き込み可
	glCallList(objlist);
	//Gメッシュ描画
	glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_FALSE);//Gプレーンのみ描き込み可
	glCallList(objlist+1);
	//Bメッシュ描画
	glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);//Bプレーンのみ描き込み可
	glCallList(objlist+2);

	glBindTexture(GL_TEXTURE_2D, 0);//テクスチャオフ
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);//カラーマスク解除

	//補助情報表示
	if (grid) {
		drawCross();
		drawParams();
	}
	glPopMatrix();
}
//各画像描画
void WarpTex::drawMesh(v2f v[MESH_W+1][MESH_H+1])
{
	glEnable(GL_TEXTURE_2D);

	/////////////////////////
	float dw = (float)subwidth / (float)texwidth / (float)MESH_W;
	float dh = (float)subheight / (float)texheight / (float)MESH_H;

	//float dw = 1.0 / (float)MESHWIDTH;
	//float dh = 1.0 / (float)MESHHEIGHT;
	/////////////////////////

	for (int j = 0; j<MESH_H; j++) {
		glBegin(GL_TRIANGLE_STRIP);
		for (int i = 0; i <= MESH_W; i++) {
			glTexCoord2f(dw*i, dh*j);
			glVertex2f(v[i][j].x, v[i][j].y);
			glTexCoord2f(dw*i, dh*(j + 1));
			glVertex2f(v[i][j + 1].x, v[i][j + 1].y);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	if (!grid) return;
	for (int j = 0; j <= MESH_H; j++) {
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i <= MESH_W; i++ ) {
			glVertex2f(v[i][j].x, v[i][j].y);
		}
		glEnd();
	}
	for (int i = 0; i <= MESH_W; i++) {
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j <= MESH_H; j++) {
			glVertex2f(v[i][j].x, v[i][j].y);
		}
		glEnd();
	}
}
//デバッグ表示
void WarpTex::drawCross(void)
{
	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex2f( -width/8.0 + cx, 0.0);
	glVertex2f( width/8.0 +cx, 0.0);
	glVertex2f( cx, -height/8.0);
	glVertex2f( cx, height/8.0);
	glEnd();
	glLineWidth(1.0);
}
//パラメータ表示
void WarpTex::drawParams(void)
{
	glColor3d(0.0, 1.0, 0.5);//文字色
	glLineWidth(2.0);
	float x = cx + 10;//先頭行開始X位置
	float y = cy - 20;//先頭行開始Y位置
	float line = -20;//改行幅
	glPushMatrix();
	drawText(x, y, "(%5.2f,%5.2f)", cx, cy);//歪中心
	drawText(0, line, "%d x %d", width, height );//ビューポートサイズ
	drawText(0, line, "kR: %7.4f", kR);//R歪係数
	drawText(0, line, "kG: %7.4f", kG);//G歪係数
	drawText(0, line, "kB: %7.4f", kB);//B歪係数
	glPopMatrix();
	glLineWidth(1.0);
}
//文字表示
void WarpTex::drawText(float x, float y, const char *format, ...)
{
	char text[64];
	//引数から文字列に変換
	va_list ap;
	va_start(ap, format);
	vsprintf(text, format, ap);
	va_end(ap);
	//文字列をストロークフォントで描画
	glTranslatef(x, y, 0.0);//描画開始位置移動
	glPushMatrix();
	glScalef(0.08, 0.12, 1.0);//文字サイズ調整
	char *p = text;
	while (*p!=NULL) {
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *p);
		++p;
	}
	glPopMatrix();
}
