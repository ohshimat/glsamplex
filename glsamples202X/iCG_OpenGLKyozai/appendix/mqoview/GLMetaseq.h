
#ifndef __GLMETASEQ_H__
#define __GLMETASEQ_H__

/*=========================================================================================

	GLMetaseq.h

	メタセコイアで作成したモデル(*.mqo)をOpenGL上に読み込む関数をまとめたヘッダです．
	このヘッダを使う上で以下の点に注意してください．

　	　・扱えるテクスチャは24bitビットマップ画像
	　・テクスチャ画像のサイズは「一辺が2のn乗サイズ(64,128,256…)の正方形」に限る
	
	また，最低限必要な機能しか実装していないので他にもいろいろ制約があります．


	再配布・改変は自由です．
　　Copyright (c) 工学ナビ, 2007-. ver.07/06/01
	website: http://www1.bbiq.jp/kougaku/

=========================================================================================*/


////////////////////////////////////////////////////////////////////////////////////////////
// ユーザが任意で設定
#define MAX_MATERIAL	100			// 最大マテリアル数
#define MAX_VERTEX		10000		// 最大頂点数
#define MAX_FACE		10000		// 最大面数


////////////////////////////////////////////////////////////////////////////////////////////
// ヘッダ
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>


////////////////////////////////////////////////////////////////////////////////////////////
// マクロ定義

typedef GLuint MQO_OBJECT;	// MQO_OBJECTはディスプレイリストと同じ扱い

#define SIZE_STR		256	// 文字列長
#define NOUSE_TEXTURE	0	// テクスチャ未使用
#define USE_TEXTURE		1	// テクスチャ使用
#define MQO_FAILED		-1	// 処理失敗
#define MQO_OK			0	// 処理成功

// 最大値マクロ
#ifndef MAX
	#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif


////////////////////////////////////////////////////////////////////////////////////////////
// 構造体定義

// OpenGL用色構造体
#ifndef DEFINE_GLCOLOR
#define DEFINE_GLCOLOR
	typedef struct tag_glCOLOR{
		double r;
		double g;
		double b;
		double a;
	} glCOLOR;
#endif


// 2次元座標構造体
#ifndef DEFINE_POINT2D
#define DEFINE_POINT2D
	typedef struct tagPOINT2D {
		double x;
		double y;
	} POINT2D;
#endif


// 3次元座標構造体
#ifndef DEFINE_GLPOINT
#define DEFINE_GLPOINT
	typedef struct tag_glPOINT{
		double x;
		double y;
		double z;
	} glPOINT;
#endif


// メタセコイア面情報構造体
#ifndef DEFINE_MQOFACE
#define DEFINE_MQOFACE
	typedef struct tagMQOFACE{
		int		n;		// 1つの面を構成する頂点の数（3～4）
		int		m;		// 面の材質番号
		int		v[4];	// 頂点番号を格納した配列
		POINT2D uv[4];	// UVマップ
	} MQOFACE;
#endif


// メタセコイア材質構造体
#ifndef DEFINE_MQOMATERIAL
#define DEFINE_MQOMATERIAL
	typedef struct tagMQOMATERIAL{
		glCOLOR col;				// 色
		int		useTex;				// テクスチャの有無
		char	texFile[SIZE_STR];	// テクスチャファイル
		GLuint  texName;			// テクスチャ名
		GLubyte *texImage;			// テクスチャ画像
	} MQOMATERIAL;
#endif

////////////////////////////////////////////////////////////////////////////////////////////



/*=========================================================================
【関数】mqoLoadTexture
【用途】ビットマップファイルからテクスチャ画像を作成する
【引数】
		*filename	ファイル名
		*tex_size	テクスチャのサイズ（一辺の長さ）を返す

【戻値】テクスチャ画像へのポインタ（失敗時はNULL）
【仕様】24bitビットマップ限定
		サイズは「一辺が2のn乗の正方形」に限定
=========================================================================*/

GLubyte* mqoLoadTexture(char *filename,int *tex_size)
{
	FILE *fp;
	int	y,x,size;
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	GLubyte	*pImage, *pRead;

	if ( (fp=fopen(filename,"rb"))==NULL ) return NULL;

	// ヘッダのロード
	fread(&bmfh,sizeof(BITMAPFILEHEADER),1,fp);
	fread(&bmih,sizeof(BITMAPINFOHEADER),1,fp);	
	size = bmih.biWidth;

	// メモリの確保
	pImage = (GLubyte*)malloc(sizeof(unsigned char)*size*size*4);
	if (pImage==NULL) return NULL;

	for (y=0; y<size; y++){
		pRead = pImage + (size-1-y)*4*size;
		for (x=0; x<size; x++) {
			fread(&pRead[2],1,1,fp);	// B
			fread(&pRead[1],1,1,fp);	// G	
			fread(&pRead[0],1,1,fp);	// R
			pRead[3] = 255;				// A
			pRead+=4;
		}
	}
	fclose(fp);
	*tex_size = size;

	return pImage;
}



/*=========================================================================
【関数】mqoRegistTexture
【用途】テクスチャの登録
【引数】
		*tex_name	テクスチャ名
		*tex_img	テクスチャ画像へのポインタ
		tex_size	テクスチャのサイズ（一辺の長さ）

【戻値】なし
=========================================================================*/

void mqoRegistTexture(GLuint* tex_name,GLubyte* tex_img,int tex_size)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glDeleteTextures(1, tex_name);			// 前回のテクスチャ情報を削除
	glGenTextures(1,tex_name);				// テクスチャを生成
	glBindTexture(GL_TEXTURE_2D,*tex_name);	// テクスチャの割り当て

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_size, tex_size,
					0, GL_RGBA, GL_UNSIGNED_BYTE, tex_img);
}



/*=========================================================================
【関数】mqoReleaseTexture
【用途】テクスチャ画像の開放
【引数】
		*pImage　テクスチャ画像へのポインタ

【戻値】なし
【仕様】free()してるだけ
=========================================================================*/

void mqoReleaseTexture(void *pImage)
{
	free(pImage);
}



/*=========================================================================
【関数】mqoSnormal
【用途】法線ベクトルを求める
【引数】
		A		3次元座標上の点A
		B		3次元座標上の点B
		C		3次元座標上の点C
		*normal	ベクトルCAとベクトルCBの法線ベクトル（右ねじ方向）

【戻値】なし
=========================================================================*/

void mqoSnormal(glPOINT A,glPOINT B,glPOINT C,glPOINT *normal)
{
	double norm;
	glPOINT vec0,vec1;

	vec0.x = A.x-C.x;  vec0.y = A.y-C.y;  vec0.z = A.z - C.z;
	vec1.x = B.x-C.x;  vec1.y = B.y-C.y;  vec1.z = B.z - C.z;

	normal->x = vec0.y * vec1.z - vec0.z * vec1.y;
	normal->y = vec0.z * vec1.x - vec0.x * vec1.z;
	normal->z = vec0.x * vec1.y - vec0.y * vec1.x;

	// 正規化する
	norm = normal->x * normal->x + normal->y * normal->y + normal->z * normal->z;
	norm = sqrt ( norm );
	
	normal->x /= norm; normal->y /= norm;  normal->z /= norm;
}



/*=========================================================================
【関数】mqoReadMaterial
【用途】メタセコイアファイルからマテリアル情報を読み込む
【引数】
		*fp		現在開いているファイルのファイルポインタ
		mat[]	マテリアル情報を格納する配列
		max_mat	マテリアル数の上限

【戻値】マテリアル情報の数
【仕様】col属性とtex属性のみに対応
		mqoLoadFileの子関数なのでこれ単体で使うことはない
=========================================================================*/

int mqoReadMaterial(FILE *fp, MQOMATERIAL mat[],int max_mat)
{
	int n_mat,i;
	char buf[SIZE_STR];
	int len;
	char *pStrEnd;
	char *pStr;

	// マテリアル数を読み込む
	fgets(buf,SIZE_STR,fp);
	sscanf(buf,"%d",&n_mat);
	if ( n_mat > max_mat ) n_mat = max_mat;		// 色数の上限設定

	// Materialの読み込み
	for (i=0; i<n_mat; i++) {

		fgets(buf,SIZE_STR,fp);		// 行読み込み

		// col
		if ( (pStr = strstr(buf,"col")) != NULL ) {
			sscanf(pStr,"col(%lf %lf %lf %lf)", 
				   &mat[i].col.r, &mat[i].col.g, &mat[i].col.b, &mat[i].col.a);
		}

		// tex
		if ( (pStr = strstr(buf,"tex")) != NULL ) {
			mat[i].useTex = USE_TEXTURE;
			pStrEnd = strstr(pStr,"\")");
			len = pStrEnd - (pStr+5);
			strncpy(mat[i].texFile,pStr+5,len);
			mat[i].texFile[len] = '\0';
		} else {
			mat[i].useTex = NOUSE_TEXTURE;
			mat[i].texFile[0] = '\0';
			mat[i].texImage = NULL;
		}
	}

	return n_mat;
}



/*=========================================================================
【関数】mqoReadVertex
【用途】メタセコイアファイルから頂点情報を読み込む

【引数】*fp			現在オープンしているメタセコイアファイルのファイルポインタ
		V[]			頂点を格納する配列
		max_vertex	頂点数の上限

【戻値】頂点数
【仕様】mqoLoadFileの子関数なのでこれ単体で使うことはない
=========================================================================*/

int mqoReadVertex(FILE *fp,glPOINT V[],int max_vertex)
{
	int n_vertex,i;

	fscanf(fp,"%d",&n_vertex);							// 頂点数を読み込む
	if (n_vertex>max_vertex) n_vertex = max_vertex;		// 頂点数の上限設定

	while (fgetc(fp)!='{');								// 余分な文字を読み飛ばす
	for (i=0; i<n_vertex; i++) {
		fscanf(fp,"%lf %lf %lf",&V[i].x,&V[i].y,&V[i].z);
	}

	return n_vertex;
}



/*=========================================================================
【関数】mqoReadFace
【用途】メタセコイアファイルから面情報を読み込む

【引数】*fp			現在オープンしているメタセコイアファイルのファイルポインタ
		face[]		面情報を格納する配列
		max_face	面数の上限

【戻値】面数
【仕様】mqoLoadFileの子関数なのでこれ単体で使うことはない
=========================================================================*/

int mqoReadFace(FILE *fp,MQOFACE face[],int max_face)
{
	int n_face,i;
	char *pStr;
	char buf[SIZE_STR];

	// 面数を読み込む
	fgets(buf,SIZE_STR,fp);
	sscanf(buf,"%d",&n_face);
	if ( n_face > max_face ) n_face = max_face;		// 面数の上限設定
	
	for (i=0; i<n_face; i++) {
	
		// 行読み込み
		fgets(buf,SIZE_STR,fp);

		// 面を構成する頂点数
		sscanf(buf,"%d",&face[i].n);

		// 頂点(V)の読み込み
		if ( (pStr = strstr(buf,"V(")) != NULL ) {
			switch (face[i].n) {
				case 3:
					sscanf(pStr,"V(%d %d %d)",&face[i].v[0],&face[i].v[1],&face[i].v[2]);
					break;
				case 4:
					sscanf(pStr,"V(%d %d %d %d)",&face[i].v[0],&face[i].v[1],&face[i].v[2],&face[i].v[3]);
					break;
				default:
					break;
			}		
		}

		// マテリアル(M)の読み込み
		face[i].m = 0;
		if ( (pStr = strstr(buf,"M(")) != NULL ) {
			sscanf(pStr,"M(%d)",&face[i].m);
		}

		// UVマップ(UV)の読み込み
		if ( (pStr = strstr(buf,"UV(")) != NULL ) {
			switch (face[i].n) {
				case 3:
					sscanf(pStr,"UV(%lf %lf %lf %lf %lf %lf)",
									&face[i].uv[0].x, &face[i].uv[0].y,
									&face[i].uv[1].x, &face[i].uv[1].y,
									&face[i].uv[2].x, &face[i].uv[2].y
									);
					break;
				case 4:
					sscanf(pStr,"UV(%lf %lf %lf %lf %lf %lf %lf %lf)",
									&face[i].uv[0].x, &face[i].uv[0].y,
									&face[i].uv[1].x, &face[i].uv[1].y,
									&face[i].uv[2].x, &face[i].uv[2].y,
									&face[i].uv[3].x, &face[i].uv[3].y
									);
					break;
				default:
					break;
			}		
		}
	}

	return n_face;
}



/*=========================================================================
【関数】mqoMakePolygon
【用途】ポリゴンの作成
【引数】
		F		面情報
		V[]		頂点配列
		M[]		マテリアル配列
		scale	スケール (1.0で等倍)		
【戻値】なし
【仕様】この関数で１枚のポリゴンが作られる
=========================================================================*/

void mqoMakePolygon(MQOFACE F,glPOINT V[],MQOMATERIAL M[],double scale)
{
	glPOINT normal;	// 法線ベクトル
	int i;

	glColor4f(M[F.m].col.r, M[F.m].col.g, M[F.m].col.b, M[F.m].col.a);	// 面の色
	mqoSnormal(V[F.v[0]],V[F.v[1]],V[F.v[2]],&normal);					// 法線ベクトルを計算

	if (M[F.m].useTex==USE_TEXTURE) {

		glShadeModel(GL_FLAT);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0,1.0,1.0,1.0);

		glBindTexture(GL_TEXTURE_2D,M[F.m].texName);
			if (F.n==3) glBegin(GL_TRIANGLES);
			if (F.n==4) glBegin(GL_QUADS);
			for (i=0; i<F.n; i++) {
				glTexCoord2f( F.uv[i].x, F.uv[i].y);
				glVertex3f(V[F.v[i]].x*scale, V[F.v[i]].y*scale, V[F.v[i]].z*scale);
			}
		glEnd();

		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);

	} else {

		if (F.n==3) glBegin(GL_TRIANGLES);
		if (F.n==4) glBegin(GL_QUADS);
			glNormal3f(-normal.x, -normal.y, -normal.z);
			for (i=F.n-1; i>=0; i--) {
				glTexCoord2f( F.uv[i].x, F.uv[i].y);
				glVertex3f(V[F.v[i]].x*scale, V[F.v[i]].y*scale, V[F.v[i]].z*scale);
			}
		glEnd();
	}

}



/*=========================================================================
【関数】mqoLoadFile
【用途】メタセコイアファイル(*.mqo)からモデルデータをOpenGLに読み込む
【引数】
		filename	ファイルのパス
		scale		拡大率

【戻値】成功：MQO_OK ／ 失敗：MQO_FAILED
=========================================================================*/

int mqoLoadFile(char *filename,double scale)
{
	static glPOINT		V[MAX_VERTEX];	 // 頂点
	static MQOMATERIAL	M[MAX_MATERIAL]; // マテリアル
	static MQOFACE		F[MAX_FACE];	 // 面

	FILE *fp;
	char *pStr;
	char buff[SIZE_STR];		// 行バッファ
	char path_dir[SIZE_STR];	// ディレクトリのパス
	char path_tex[SIZE_STR];	// テクスチャファイルのパス
	int	i, n_face, n_mat, len, tex_size;


	// ファイルを開く
	if ((fp = fopen(filename, "r")) == NULL) return MQO_FAILED;

	// パスの取得
	pStr = MAX( strrchr(filename,'\\'), strrchr(filename,'/') );
	len = MAX((int)(pStr-filename)+1,0);
	strncpy(path_dir,filename,len);
	path_dir[len] = '\0';
	

	//glEnable(GL_DEPTH_TEST);

	while (!feof(fp)) {
		fscanf(fp, "%s", buff);

		if (!strcmp(buff,"Material")) {
			n_mat = mqoReadMaterial(fp,M,MAX_MATERIAL);

			for (i=0; i<n_mat; i++) {
				if (M[i].useTex == USE_TEXTURE)	{
					sprintf(path_tex,"%s%s",path_dir,M[i].texFile);
					M[i].texImage = mqoLoadTexture(path_tex,&tex_size);
					mqoRegistTexture(&(M[i].texName),M[i].texImage,tex_size);
				}
			}
		}

		if (!strcmp(buff,"vertex"))	{
			mqoReadVertex(fp,V,MAX_VERTEX);
		}

		if (!strcmp(buff,"face")) {
			n_face = mqoReadFace(fp,F,MAX_FACE);
			for (i=0; i<n_face; i++) {
				mqoMakePolygon(F[i],V,M,scale);
			}
		}
	}
	fclose(fp);

	//glDisable(GL_DEPTH_TEST);


	for (i=0; i<n_mat; i++) {
		mqoReleaseTexture(M[i].texImage);
	}

	return MQO_OK;
}



/*=========================================================================
【関数】mqoCreateObject
【用途】メタセコイアファイル(*.mqo)からMQOオブジェクトを作成する
【引数】filename	ファイルのパス
		scale		拡大率

【戻値】MQOオブジェクト
【仕様】MQOオブジェクト（MQO_OBJECT）はただのディスプレイリストです．
=========================================================================*/

MQO_OBJECT mqoCreateObject(char *filename,double scale)
{
	MQO_OBJECT displist;

	displist = glGenLists(1);
	glNewList(displist, GL_COMPILE_AND_EXECUTE);
		glPushMatrix();
			glRotatef(90.0f, 1.f, 0.f, 0.f); // ARToolKitではZ軸が上方向になるので
			if (mqoLoadFile(filename,scale)==MQO_FAILED) return 0; 
		glPopMatrix();
	glEndList();

	return displist;
}



/*=========================================================================
【関数】mqoCallObject
【用途】MQOオブジェクトをOpenGLの画面上に呼び出す
【引数】
		object		MQOオブジェクト

【戻値】なし
=========================================================================*/

void mqoCallObject(MQO_OBJECT object)
{
	glCallList(object);
}

#endif