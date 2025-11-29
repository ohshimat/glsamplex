#ifndef WARPTEX_H
#define WARPTEX_H
#pragma once
//#include "platform.h"

class WarpTex
{
public:
	static const int MESH_W = 16;
	static const int MESH_H = 16;

	static const int CAM_W = 640;
	static const int CAM_H = 480;

	float cx = 0, cy = 0;//歪中心
	float kR, kG, kB;//歪補正パラメータ
	bool grid;//テストグリッド表示フラグ

	WarpTex(void);
	~WarpTex(void);
	void copy(const WarpTex *src);

	void config(void);
	void configExt(void);

	void setViewport(int x, int y, int w, int h);
	void setCenter(float cx, float cy);
	void setParams(float kR, float kG, float kB);
	void switchGrid(bool sw);

	void allocTex( GLenum pixel, int x, int y, GLenum format );

	void updateTex(unsigned char *image = nullptr);
	void updateMesh();

	void applyViewport();
	void show( float x = 0.0, float y = 0.0, float sx = 1.0, float sy = 1.0);

private:
	typedef struct{
		float x;
		float y;
	}v2f;

	void getViewport(void);

	void makeMesh(void);

	void drawCross(void);
	void drawParams(void);

	void drawMesh(v2f v[MESH_W + 1][MESH_H + 1]);

	void drawText(float x, float y, const char *_Format, ...);

	int width, height;//ビューポートサイズ
	int xo, yo;//ビューポート原点
	int texwidth, texheight; //テクスチャ領域サイズ
	int subwidth, subheight; //サブイメージサイズ
	int mexhwidth, meshheight;//メッシュ分割数

	GLuint texid;
	GLuint objlist;
	v2f vR[MESH_W + 1][MESH_H + 1];
	v2f vG[MESH_W + 1][MESH_H + 1];
	v2f vB[MESH_W + 1][MESH_H + 1];
};
#endif