/* kadai6-2.cpp */
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

float param = 0.0; /* 回転角度変数 */
int rotate_flag = 0; /* 回転スイッチ */
int color_index = 0; /* ■カラーインデックス（再利用）*/
int shade_flag = 0; /* シェーディングフラグ 0:FLAT, 1:SMOOTH */

/*マテリアル設定*/
float diffuse_white[] = {0.8, 0.8, 0.8, 1.0};  /* ■拡散反射成分の反射強度*/
float diffuse_red[] = {0.8, 0.0, 0.0, 1.0};    /* ■拡散反射成分の反射強度*/
float diffuse_green[] = {0.0, 0.8, 0.0, 1.0};  /* ■拡散反射成分の反射強度*/
float diffuse_blue[] = {0.0, 0.0, 0.8, 1.0};   /* ■拡散反射成分の反射強度*/
float specular[] = {1.0, 1.0, 1.0, 1.0};       /* 鏡面反射成分の反射強度 */
float ambient[] = {0.2, 0.2, 0.2, 1.0};        /* 環境光成分の反射強度*/
float shininess = 128.0;

/*--------------------------------------------------------------------- idle */
/* idle: アイドルコールバック関数
 */
void idle(void)
{
    if( rotate_flag == 1 ){ /* 回転スイッチが1ならば‥ */
        param += 1.0;
        if( param > 360.0 ){
            param = 0.0;
        }
    }
    glutPostRedisplay();
}
/*------------------------------------------------------------------ display */
/* display: ディスプレイコールバック関数（描画関数)
 */
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix(); /* 現在のマトリクス状態をセーブ（スタックにプッシュ）*/

    glEnable(GL_DEPTH_TEST); /*■■陰面処理開始*/
    
    /* 座標軸を描画する */
    /* x軸を赤で描画する */
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(-10.0, 0.0, 0.0 );
    glVertex3f( 10.0, 0.0, 0.0 );
    glEnd();
    /* y軸を緑で描画する */
    glColor3f(0.0, 1.0, 0.0); 
    glBegin(GL_LINES);
    glVertex3f( 0.0, 10.0, 0.0 );
    glVertex3f( 0.0,-10.0, 0.0 );
    glEnd();
    /* z軸を青で描画する */
    glColor3f(0.0, 0.0, 1.0); 
    glBegin(GL_LINES);
    glVertex3f( 0.0, 0.0, 10.0 );
    glVertex3f( 0.0, 0.0,-10.0 );
    glEnd();

    glRotatef( param, 0.0, 1.0, 0.0 ); /* z軸周りにparam度回転する */
    /*■マテリアル設定*/
    switch( color_index ){
      case 0:
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_white);/* 拡散反射の設定 */
        break;
      case 1:
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_red);  /* 拡散反射の設定 */
        break;
      case 2:
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_green);/* 拡散反射の設定 */
        break;
      case 3:
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_blue);  /* 拡散反射の設定 */
        break;
      default:
        break;
    }
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);   /* 鏡面反射の設定 */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);     /* 環境光の設定 */
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);  /* 鏡面反射の鋭さの設定 */
    /*----*/
    glEnable(GL_LIGHTING); /* シェーディング計算の開始 */
    /* ■オブジェクトの描画開始 */
    /* 頭部 */
    glTranslatef(0.0, 1.0, 0.0);
    glRotatef(param, 0.0, 1.0, 0.0 );
    glutSolidCube(2.0);
    /* 右目 */
    glPushMatrix();
    glTranslatef(-0.5, 0.5, 1.0);
    glutSolidSphere(0.2, 10, 10);
    glPopMatrix();
    /* 左目 */
    glPushMatrix();
    glTranslatef(0.5, 0.5, 1.0);
    glutSolidSphere(0.2, 10, 10);
    glPopMatrix();
    /* 鼻 */
    glPushMatrix();
    glTranslatef(0.0, 0.0, 1.1);
    glutSolidCube(0.2);
    glPopMatrix(); 
   /* ■オブジェクトの描画終了*/
    
    glDisable(GL_LIGHTING); /* シェーディング計算の終了 */
    glPopMatrix(); /* マトリクス状態をリストア（スタックからポップ）*/

    glEnable(GL_DEPTH_TEST); /* 陰面処理終了 */

    glutSwapBuffers(); /* フロントバッファとバックバッファをスワップ */
}
/*-------------------------------------------------------------------- mouse */
/* mouse: マウスイベントコールバック関数 */
void mouse( int button , int state, int x, int y )
/* button は押された（離された）ボタン，
  state：押したか離したかの状態，
  x, y：マウスの座標値 */
{
    switch( button ){
      case GLUT_LEFT_BUTTON: /* 左マウスボタン*/
        switch( state ){
          case GLUT_DOWN: /* プレスダウン */
            rotate_flag = 1;
            break;
          case GLUT_UP:   /* リリースアップ */
            break;
          default:
            break;
        }
        break;
      case GLUT_MIDDLE_BUTTON: /* 右マウスボタン */
        break;
      case GLUT_RIGHT_BUTTON: /* 中マウスボタン */
        switch( state ){
          case GLUT_DOWN: /* プレスダウン */
            rotate_flag = 0;
            break;
          case GLUT_UP:   /* リリースアップ */
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    return;
}
/*----------------------------------------------------------------- keyboard */
/* keyboard: ■■キーボードイベントコールバック関数 */
void keyboard(unsigned char key, int x, int y)
/*引数 key にはタイプされたキーの ASCII コードが，
x と y にはキーがタイプされたときのマウスの位置が渡される */
{
    switch (key) {
      case 'q':                   /* qが入力されたら，プログラムを終了する */
        exit(0);
      case '0':
        rotate_flag = 0;
        break;
      case '1':
        rotate_flag = 1;
        break;
      case 'r':
        param += 30.;
        break;
      case 'c': /* ■カラーインデックス（再利用） */
        color_index++;
        if( color_index > 3 ) color_index = 0;
        break;
      case 's': /* ■シェーディングフラグ */
        if( shade_flag ){
            glShadeModel( GL_FLAT );
            shade_flag = 0;
        }
        else{
            glShadeModel( GL_SMOOTH );
            shade_flag = 1;
        }
        break;
      default:
        break;
    }
    return;
}
/*--------------------------------------------------------------------- init */
/* init: OpenGLウィンドウの初期化
 */
void init(char *winname)
{
    glutInitWindowPosition(0, 0); /* ウィンドウの左上の位置を(0,0)とする */
    glutInitWindowSize(500, 500); /* ウィンドウのサイズを500×500とする */

    /* RGBAカラーバッファとダブルバッファを設定 */
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutCreateWindow(winname); /* winnameで指定された名前でウィンドウを開く */

    glClearColor(0.0, 0.125, 0.25, 1.0);/* ウィンドウの背景色の指定 */
    
    /* ■■透視投影の設定 */
    glMatrixMode(GL_PROJECTION); /* ■投影変換に関する座標変換を開始 */
    glLoadIdentity(); /* 投影変換行列を初期化 */
    gluPerspective( 45.0, 1., 1.0, 10.0 );
    glMatrixMode(GL_MODELVIEW);  /* ■視点の移動やモデルの移動など投影以外の
                                   幾何変換モードに戻す */

    /* ■■視点の位置と視線の方向を設定する */
    glLoadIdentity(); /* 幾何変換行列を初期化 */
    gluLookAt( 3.0, 4.0, 5.0, 0., 0., 0., 0., 1., 0. );

    /*■■シェーディング計算方式をフラットシェーディングに設定 */
    glShadeModel(GL_FLAT);
    /*■■光源LIGHT0の照明効果を有効にする */
    glEnable(GL_LIGHT0);
}
/*--------------------------------------------------------------------- main */
/* main関数
 */
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);           /* glutの初期化 */
    init(argv[0]);
    glutDisplayFunc(display);        /* ディスプレイコールバック関数の指定 */
    glutKeyboardFunc(keyboard);/* ■■キーボード入力のコールバック関数の指定*/
    glutMouseFunc(mouse);           /* ■■マウス入力のコールバック関数の指定*/
    glutIdleFunc(idle); /* アイドルコールバック関数の指定 */

    glutMainLoop();                  /* イベント待ちの無限ループへ入る */
    exit( 0 ); /* ループが終わったらプロセスからシェルに0を返して終了 */
}
