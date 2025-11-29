#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <time.h>
#include "calc.h"
#include "Vector.h"
#include "perlinNoise.h"

//#define WIDTH 100
//#define HEIGHT 100

const int cwidth = 256;
const int cheight = 256;

float terrain[cwidth][cheight];
float tu[cwidth][cheight]; //テクスチャ座標u
float tv[cwidth][cheight]; //テクスチャ座標v

const int permutation_size = 256;
//#define PERMUTATION_SIZE 256
/*
PERMUTATION_SIZE は、順列テーブルのサイズ（通常256）を表します。
p[] 配列は、0〜255の整数をランダムに並び替えた順列を格納するための配列です。
この配列は、ノイズの「ランダム性」を制御するために使われます。
*/

int p[permutation_size * 2];

#define NA 180
#define NR 90
float web_x[NR + 1][NA + 1];
float web_y[NR + 1][NA + 1];
float web_z[NR + 1][NA + 1];

float web_u[NR + 1][NA + 1];
float web_v[NR + 1][NA + 1];

vector_t web_n[NR + 1][NA + 1];

//int projmode = 0;

static float stereographic(float r) { return tanf(r / 2.0); } //立体射影
static float equidistance(float r) { return r / (3.14 / 2.0); } //等距離射影
static float equisolidangle(float r) {
	return 2.0 / sqrtf(2.0) * sinf(r / 2.0);
} //等立体角射影
static float orthogonal(float r) { return sinf(r); } //正射影

//-------------------------------------------------------------- makeWeb
/* makeWeb: クモの巣状の格子点を生成する
 */
static void makeWeb_old(float(*func)(float), float freq, float fractal)
{
	float x, y, a, r, da, dr, l;
	da = 360.0 / NA;
	dr = 90.0 / NR;
	for (int i = 0; i <= NR; i++) {
		r = dr * i;
		r = r / 180.0 * 3.14159;
		for (int j = 0; j <= NA; j++) {
			a = da * j;
			a = a / 180.0 * 3.14159;

			l = func(r);

			web_z[i][j] = l * cosf(a);
			web_x[i][j] = l * sinf(a);

			web_u[i][j] =   web_x[i][j] / 2.0 + 0.5;
			web_v[i][j] = - web_z[i][j] / 2.0 + 0.5;

			//高さスケール
			//float k = sqrtf(web_z[i][j] * web_z[i][j] + web_x[i][j] * web_x[i][j]);
			//float k = l * l;
			float k = 1.0f;
			web_y[i][j] = k * perlinFractal(web_z[i][j], web_x[i][j], freq, fractal);


		}
	}
}
static void makeWeb(float(*func)(float), float freq, float fractal )
{
	float x, y, a, r, da, dr, l;
	da = 360.0 / NA;
	dr = 90.0 / NR;
	for (int i = 0; i <= NR; i++) {
		r = dr * i;
		r = r / 180.0 * 3.14159;
		for (int j = 0; j <= NA; j++) {
			a = da * j;
			a = a / 180.0 * 3.14159;

			l = func(r);

			web_z[i][j] = l * cosf(a);
			web_x[i][j] = l * sinf(a);

			//高さスケール
			//float k = l * l;
			float k = 1.0f;
			//web_y[i][j] = k * perlinFractal(web_z[i][j], web_x[i][j], freq, fractal);
			//web_y[i][j] = k * perlinNoise(web_z[i][j], web_x[i][j]);
			//web_y[i][j] = k * perlinFractal((float)j / NA, (float)i/NR, freq, fractal);
			web_y[i][j] = k * perlinFractalCircle( a, r, freq, fractal);
		}
	}

	// --- 法線ベクトルの計算 ---
	// 各頂点の法線を初期化
	for (int i = 0; i <= NR; ++i) {
		for (int j = 0; j <= NA; ++j) {
			web_n[i][j].x = 0.0f;
			web_n[i][j].y = 0.0f;
			web_n[i][j].z = 0.0f;
		}
	}

	// クアッドごとに面法線を計算して周辺頂点に加算する
	for (int i = 0; i < NR; ++i) {
		for (int j = 0; j < NA; ++j) {
			int j1 = (j + 1) % NA; // 周方向はループ

			vector_t v00 = vector_t(web_x[i][j],     web_y[i][j],     web_z[i][j]);
			vector_t v10 = vector_t(web_x[i+1][j],   web_y[i+1][j],   web_z[i+1][j]);
			vector_t v11 = vector_t(web_x[i+1][j1],  web_y[i+1][j1],  web_z[i+1][j1]);
			vector_t v01 = vector_t(web_x[i][j1],    web_y[i][j1],    web_z[i][j1]);

			// 三角形1: v00, v10, v11
			vector_t e1 = vector_t(v10.x - v00.x, v10.y - v00.y, v10.z - v00.z);
			vector_t e2 = vector_t(v11.x - v00.x, v11.y - v00.y, v11.z - v00.z);
			vector_t fn;
			fn.x = e1.y * e2.z - e1.z * e2.y;
			fn.y = e1.z * e2.x - e1.x * e2.z;
			fn.z = e1.x * e2.y - e1.y * e2.x;
			web_n[i][j].x     += fn.x; web_n[i][j].y     += fn.y; web_n[i][j].z     += fn.z;
			web_n[i+1][j].x   += fn.x; web_n[i+1][j].y   += fn.y; web_n[i+1][j].z   += fn.z;
			web_n[i+1][j1].x  += fn.x; web_n[i+1][j1].y  += fn.y; web_n[i+1][j1].z  += fn.z;

			// 三角形2: v00, v11, v01
			e1 = vector_t(v11.x - v00.x, v11.y - v00.y, v11.z - v00.z);
			e2 = vector_t(v01.x - v00.x, v01.y - v00.y, v01.z - v00.z);
			fn.x = e1.y * e2.z - e1.z * e2.y;
			fn.y = e1.z * e2.x - e1.x * e2.z;
			fn.z = e1.x * e2.y - e1.y * e2.x;
			web_n[i][j].x     += fn.x; web_n[i][j].y     += fn.y; web_n[i][j].z     += fn.z;
			web_n[i+1][j1].x  += fn.x; web_n[i+1][j1].y  += fn.y; web_n[i+1][j1].z  += fn.z;
			web_n[i][j1].x    += fn.x; web_n[i][j1].y    += fn.y; web_n[i][j1].z    += fn.z;
		}
	}

	// 正規化とシームコピー (j==NA は j==0 と同じ)
	for (int i = 0; i <= NR; ++i) {
		for (int j = 0; j < NA; ++j) {
			float nx = web_n[i][j].x;
			float ny = web_n[i][j].y;
			float nz = web_n[i][j].z;
			float len = sqrtf(nx*nx + ny*ny + nz*nz);
			if (len > 1e-6f) {
				web_n[i][j].x = nx / len;
				web_n[i][j].y = ny / len;
				web_n[i][j].z = nz / len;
			} else {
				web_n[i][j].x = 0.0f;
				web_n[i][j].y = 1.0f;
				web_n[i][j].z = 0.0f;
			}
			if (i == 0) {
				// 北極点の法線は上向きに固定
				web_n[i][j].x = 0.0f;
				web_n[i][j].y = 1.0f;
				web_n[i][j].z = 0.0f;
			}
		}
		// シームをコピー
		web_n[i][NA] = web_n[i][0];
	}
}
//-------------------------------------------------------------- drawWeb3
/* drawWeb: クモの巣状の格子点を描画する
 */
static void drawWeb(void)
{
	//glShadeModel(GL_FLAT);
	//glColor3f( 0.0, 1.0, 1.0 );
	for (int i = 0; i < NR; i++) {
		glBegin(GL_TRIANGLE_STRIP); //
		//glBegin( GL_LINE_LOOP );
		for (int j = 0; j <= NA; j++) {
			int jj = (j == NA) ? 0 : j;
			// 頂点(i,jj)
			glNormal3f(web_n[i][jj].x, web_n[i][jj].y, web_n[i][jj].z);
			glTexCoord2f(web_u[i][jj], web_v[i][jj]);
			glVertex3f(web_x[i][jj],     web_y[i][jj]  , web_z[i][jj]);
			// 頂点(i+1,jj)
			glNormal3f(web_n[i+1][jj].x, web_n[i+1][jj].y, web_n[i+1][jj].z);
			glTexCoord2f(web_u[i + 1][jj], web_v[i + 1][jj]);
			glVertex3f(web_x[i + 1][jj], web_y[i + 1][jj], web_z[i + 1][jj]);
		}
		glEnd();
	}
}
//-------------------------------------------------------------------------------
float perlinNoise(float x, float y);
//-------------------------------------------------------------------------------
// Fade function as defined by Ken Perlin. This eases coordinate values
static float fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}
//-------------------------------------------------------------------------------
// Linear interpolation function
static float lerp(float t, float a, float b) {
	return a + t * (b - a);
}
//-------------------------------------------------------------------------------
// Gradient function
static float grad(int hash, float x, float y) {
	switch (hash & 3) {
	case 0: return  x + y;
	case 1: return -x + y;
	case 2: return  x - y;
	case 3: return -x - y;
	default: return 0; // 保険
	}
}
/*
static float grad(int hash, float x, float y) {
	int h = hash & 7;
	float u = h < 4 ? x : y;
	float v = h < 4 ? y : x;
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}
*/
//-------------------------------------------------------------------------------
// Initialize permutation array
void perlinInit() {
	for (int i = 0; i < permutation_size; i++) {
		p[i] = i;
	}
	// Shuffle
	for (int i = 0; i < permutation_size; i++) {
		int j = rand() % permutation_size;
		int tmp = p[i];
		p[i] = p[j];
		p[j] = tmp;
	}
	// Duplicate the permutation array
	for (int i = 0; i < permutation_size; i++) {
		p[permutation_size + i] = p[i];
	}
}
//-------------------------------------------------------------------------------
// Perlin noise function
float perlinNoise(float x, float y) {
	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;
	x -= floor(x);
	y -= floor(y);
	float u = fade(x);
	float v = fade(y);
	int aa = p[p[X] + Y];
	int ab = p[p[X] + Y + 1];
	int ba = p[p[X + 1] + Y];
	int bb = p[p[X + 1] + Y + 1];
	float res = lerp(v,
		lerp(u, grad(aa, x, y), grad(ba, x - 1, y)),
		lerp(u, grad(ab, x, y - 1), grad(bb, x - 1, y - 1))
	);
	return (res + 1.0f) / 2.0f - 0.5; // Normalize to [0,1]
}
//-------------------------------------------------------------------------------
// Perlin noise on a circle
float perlinNoiseCircle(float angle, float radius) {
	// 入力角度が度単位の可能性がある場合の自動変換（度->ラジアン）
	/*
	const float TWO_PI = 6.28318530717958647692f;
	if (fabsf(angle) > TWO_PI) {
		angle = angle * (3.14159265358979323846f / 180.0f); // deg -> rad
	}
	*/

	// 極座標 -> デカルト (角度はラジアン)
	float ux = cosf(angle) * radius;
	float uy = sinf(angle) * radius;

	// 周波数調整（必要に応じてこの値を変更）
	// const float scale = 8.0f;    // 周波数を上げると細かいノイズに、下げると粗いノイズに
	// const float offset = 100.0f; // 負の座標を避けたい場合のオフセット（任意）

	// マッピングした座標で Perlin ノイズをサンプリング
	return perlinNoise( ux, uy );
}
//-------------------------------------------------------------------------------
// Perlin Fractal noise function
float perlinFractal(float x, float y, float freq, float fractal ) 
{
	const int octaves = 6;
	float scale = powf(2.0, freq - 1.0);
	float amp = 1.0f / (scale + 1);
	float offset = amp * 0.5;
	float sum = 0.0f;
	float norm = 0.0f;

	for (int o = 0; o < octaves; o++) {
		sum += amp * perlinNoise( x * scale, y * scale);
		norm += amp;
		amp *= fractal;      // 振幅を半減
		scale *= 2.0f;      // 周波数を倍増
	}
	return sum / norm / powf(2.0, freq - 1.0) + offset; // 正規化
}
//-------------------------------------------------------------------------------
// Perlin Fractal noise function
float perlinFractalCircle(float angle, float radius, float freq, float fractal)
{
	const int octaves = 6;
	float scale = powf(2.0, freq - 1.0);
	float amp = 1.0f / (scale + 1);
	float offset = amp * 0.5;
	float sum = 0.0f;
	float norm = 0.0f;

	for (int o = 0; o < octaves; o++) {
		sum += amp * perlinNoiseCircle( angle * scale, radius * scale);
		norm += amp;
		amp *= fractal;      // 振幅を半減
		scale *= 2.0f;      // 周波数を倍増
	}
	return sum / norm / powf(2.0, freq - 1.0) + offset; // 正規化
}
//-------------------------------------------------------------------------------
// Generate Perlin noise terrain
void perlinMake( float freq, float fractal ) {

	srand((unsigned int)time(NULL));

	perlinInit();

	float scale = 1.0f;
	//float terrain[HEIGHT][WIDTH];
	for (int y = 0; y < cheight; y++) {
		for (int x = 0; x < cwidth; x++) {
			//terrain[y][x] = perlin(x * scale, y * scale);
			//terrain[y][x] = perlin(x * scale, y * scale) * 10.0f; // 高さを大きくする場合
			//terrain[y][x] = (perlin(x * scale, y * scale) + 1.0f) * 5.0f; // -1～1を0～10に変換する場合
			//terrain[y][x] = perlin(x * scale, y * scale) * perlin(x * scale * 2, y * scale * 2); // 複数周波数の組み合わせ
			//terrain[y][x] = (perlin(x * scale, y * scale) + 0.5f * perlin(x * scale * 2, y * scale * 2) + 0.25f * perlin(x * scale * 4, y * scale * 4)); // フラクタルノイズ
			//terrain[y][x] = pow(perlin(x * scale, y * scale), 3); // 高さの強調
			//terrain[y][x] = sqrt(perlin(x * scale, y * scale)); // 高さの平坦化
			//terrain[y][x] = perlin(x * scale, y * scale) > 0.5f ? 1.0f : 0.0f; // 二値化
			/*
			float k0, k1, k2;
			k0 = 1.0f; // powf(1.0f, fractal);
			k1 = powf( 1.0f, 1.0f * fractal);
			k2 = powf( 1.0f, 0.5f * fractal);
			terrain[y][x] = k0 * perlin((float)x / cwidth * freq, (float)y / cheight * freq)
				+ k1 * perlin((float)x / cwidth * freq * 2.0f, (float)y / cheight * freq * 2.0f)
				+ k2 * perlin((float)x / cwidth * freq * 4.0f, (float)y / cheight * freq * 4.0f);
			// 正規化座標
			*/

			terrain[y][x] = perlinFractal((float)x / cwidth, (float)y / cheight, freq, fractal); // 正規化
		}
	}
	return;
}
//-------------------------------------------------------------------------------
// Draw terrain using triangle strips
static void perlinDraw(void)
{
	float dw = 1.0f / (float)cwidth;
	float dh = 1.0f / (float)cheight;
	float cx = 0.5f;
	float cy = 0.5f;

	glEnable(GL_NORMALIZE);
	for (int j = 0; j < cheight - 1; j++) {
		glBegin(GL_TRIANGLE_STRIP);
		for (int i = 0; i < cwidth; i++) {
			// 法線計算
			float dx1 = dw;
			float dy1 = 0.0f;
			float dz1 = terrain[j][i + 1 < cwidth ? i + 1 : i] - terrain[j][i];

			float dx2 = 0.0f;
			float dy2 = dh;
			float dz2 = terrain[j + 1 < cheight ? j + 1 : j][i] - terrain[j][i];

			// 外積
			float nx = dy1 * dz2 - dz1 * dy2;
			float ny = dz1 * dx2 - dx1 * dz2;
			float nz = dx1 * dy2 - dy1 * dx2;

			// 正規化
			float len = sqrt(nx * nx + ny * ny + nz * nz);
			if (len > 0.0f) {
				nx /= len; ny /= len; nz /= len;
			}
			glNormal3f(nx, nz, ny);
			// 頂点1
			glTexCoord2f(dw * i, dh * j);
			glVertex3f(dw * i - cx, terrain[j][i],dh * j - cy);
			// 頂点2
			glTexCoord2f(dw * i, dh * (j + 1));
			glVertex3f(dw * i-cx, terrain[j + 1][i], dh * (j + 1) -cy);
		}
		glEnd();
	}
}
//------------------------------------------------------------------------
//-------- ezGround_Make: 地面オブジェクトを生成する
GLuint ezGround_Make(float freq, float fractal)
{
	GLuint list;
	list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	perlinMake(freq, fractal);
	perlinDraw();
	glEndList();
	return list;
}
//------------------------------------------------------------------------
//-------- ezGround_MakeSquare: 四角形の地面オブジェクトを生成する
GLuint ezGround_MakeSquare(float freq, float fractal, float height, float size)
{
	GLuint list;
	list = glGenLists(1);
	perlinMake(freq, fractal);
	glNewList(list, GL_COMPILE);
	perlinMake(freq, fractal);
	glPushMatrix();
	glScalef( size, height, size);
	glEnable(GL_NORMALIZE);
	perlinDraw();
	glPopMatrix();
	glEndList();
	return list;
}
//------------------------------------------------------------------------
//-------- ezGround_MakeCircle: 円形の地面オブジェクトを生成する
GLuint ezGround_MakeCircle(float freq, float fractal, float height, float radius)
{
	GLuint list;
	list = glGenLists(1);
	glNewList(list, GL_COMPILE);
	makeWeb( stereographic, freq, fractal);
	glPushMatrix();
	glScalef(radius, height, radius);
	glEnable(GL_NORMALIZE);
	drawWeb();
	glPopMatrix();
	glEndList();
	return list;
}
//------------------------------------------------------------------------
//-------- ezGround_Draw: 地面オブジェクトを描画する
void ezGround_Draw(GLuint list)
{
	glCallList(list);
	return;
}
//------------------------------------------------------------------------