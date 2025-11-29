#include "platform.h"
#include "warptex.h"
extern WarpTex leftview, rightview;
/*----------------------------------------------------------------- keyboard */
/* keyboard: キーボードイベントコールバック関数
 * 引数 key にはタイプされたキーの ASCII コードが，
 * x と y にはキー入力が検出されたときのマウスの位置が渡される
 */
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':   /* [Q]がキー入力されたら，プログラムを終了する */
		exit(0);
	case 'r':
		leftview.kR -= 0.0001;
		rightview.kR -= 0.0001;
		leftview.updateMesh();
		rightview.updateMesh();
		break;
	case 'R':
		leftview.kR += 0.0001;
		rightview.kR += 0.0001;
		leftview.updateMesh();
		rightview.updateMesh();
		break;
	case 'g':
		leftview.kG -= 0.0001;
		rightview.kG -= 0.0001;
		leftview.updateMesh();
		rightview.updateMesh();
		break;
	case 'G':
		leftview.kG += 0.0001;
		rightview.kG += 0.0001;
		leftview.updateMesh();
		rightview.updateMesh();
		break;
	case 'b':
		leftview.kB -= 0.0001;
		rightview.kB -= 0.0001;
		leftview.updateMesh();
		rightview.updateMesh();
		break;
	case 'B':
		leftview.kB += 0.0001;
		rightview.kB += 0.0001;
		leftview.updateMesh();
		rightview.updateMesh();
		break;
	case 't':
		leftview.grid = !leftview.grid;
		rightview.grid = !rightview.grid;
		leftview.updateMesh();
		rightview.updateMesh();
		break;
	default:
		break;
	}
	return;
}
