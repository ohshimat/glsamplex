/*############################################################################
 * インタラクティブCGプログラミング演習Ⅰ
 * 教材用ソースコード
 * mqoview.cpp: メタセコイアモデルデータ簡易ビュア
 * OpenGL+GLUT 学習用のサンプルプログラムであり，
 * 最適なソースコードの事例ではありません
 * 機能の改良や追加，他の書き方などを考えてみましょう
 * 書き直したソースコードは自由に使用してかまいません．
 * なお，本プログラムに使用しているGLMetaseq.hファイルは，
 * 橋本なおき氏の著作物ですが，改変・再配布は作者により許可されています．
 *############################################################################
 */

#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <windows.h>

#include "GLMetaseq.h" /* mqoローダ */
MQO_OBJECT mqoObj; /* mqoオブジェクト */

#define XMIN 128
#define YMIN 128
#define WIDTH 512
#define HEIGHT 512

int moveflag = 0;

float dx = 0.;
float dy = 0.;
float d = 0.;
float ab = 0.;
float scalefactor = 1.0;
float distance = 10.;

float mcx = 0., mcy = 0.; /* マウスボタンダウン時の座標 */
float mmx = 0., mmy = 0.; /* マウス移動中の座標 */
float mdx = 0., mdy = 0.; /* マウスドラッグ中の座標 */
float mdx0 = 0., mdy0 = 0.; /* マウスドラッグ中の座標 */
float diffuse[] = {0.8, 0.8, 0.8, 1.0};  /* 拡散反射成分の反射強度*/
float gray[] = {0.2, 0.2, 0.2, 1.0};  /* 拡散反射成分の反射強度*/
float specular[] = {1.0, 1.0, 1.0, 1.0}; /* 鏡面反射成分の反射強度 */
float ambient[] = {0.2, 0.2, 0.2, 1.0};  /* 環境光成分の反射強度*/
float red[] = { 1.0, 0., 0., 1.0};  /* 拡散反射成分の反射強度*/
float black[] = {0., 0., 0., 1.0};  /* 拡散反射成分の反射強度*/
float shininess = 128.0;

float lightdir[] = { 0.0, -1.0, 0.0, 0.0 };

float trackball_matrix[16] = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
  };

void display(void)
{
    /* フレームバッファとデプスバッファをクリア */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* オブジェクトを描画 */
    glPushMatrix();
    glMatrixMode(GL_PROJECTION); /* 投影法に関する座標変換を開始 */
    glLoadIdentity();

    gluPerspective( 45.0, 1.0, distance * 0.1, distance * 2.0 );

    glMatrixMode(GL_MODELVIEW); /* 視点やモデルの座標変換を開始 */
    gluLookAt( 0.0, 0.0, distance, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );       /* LIGHT0の光源を有効にする */
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );    
    glLightfv( GL_LIGHT0, GL_POSITION, lightdir );

    switch( moveflag ){
      case 1: /* rotate */
        glPushMatrix();
        glLoadIdentity();
        glRotatef( 10.0 * d, 0.0, 0.0, 1.0 );
        glRotatef( 10.0 * dx, 0.0, 1.0, 0.0 );
        glRotatef( - 10.0 * dy, 1.0, 0.0, 0.0 );
        glMultMatrixf( trackball_matrix );
        glGetFloatv( GL_MODELVIEW_MATRIX, trackball_matrix );
        glPopMatrix();
        break;
      case 2: /* dolly */
        break;
    }
    glPushMatrix();
    {
        glMultMatrixf( trackball_matrix );
        mqoCallObject( mqoObj );
    }
    glPopMatrix();

    glPopMatrix();

    glDisable( GL_LIGHTING );
    /* ラバーバンドを描画 */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glPushMatrix();
    glMatrixMode( GL_MODELVIEW );
    /* ラバーバンド描画用に平行投影に設定 */
    glPushMatrix();
    glLoadIdentity();
    glOrtho( 0.0, 1.0, 0.0, 1.0, -1.0, 1.0 );

    switch( moveflag ){
      case 2:
        glPointSize(10.0);
        glColor3f( 0.0, 0.0, 1.0 );
        glBegin( GL_POINTS ); /* カーソル位置に点を描画 */
        {
            glVertex2f( mdx, mdy );
        }
        glEnd();
        glPointSize(1.0);
        break;
      case 1:
        glPointSize(10.0);
        glColor3f( 0.0, 1.0, 0.0 );
        glBegin( GL_POINTS ); /* カーソル位置に点を描画 */
        {
            glVertex2f( mdx, mdy );
        }
        glEnd();
        glColor3f( 0.0, 0.0, 1.0 );
        glBegin( GL_POINTS ); /* カーソル位置に点を描画 */
        {
            glVertex2f( mmx, mmy );
        }
        glEnd();
        glColor3f( 0.0, 1.0, 0.0 );
        /* 前回のクリック位置から現在のカーソル位置まで線を引く */
        glBegin( GL_LINES );
        {
            glVertex2f( mcx, mcy );
            glVertex2f( mdx, mdy );
        }
        glEnd();
        glColor3f( 1.0, 0.0, 0.0 );
        glBegin( GL_POINTS ); /* カーソル位置に点を描画 */
        {
            glVertex2f( mcx, mcy );
        }
        glEnd();
        glPointSize(1.0);
        break;
    }
    glPopMatrix();

    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    glutSwapBuffers(); /* バッファをスワップ */
}

void idle(void)
{
    glutPostRedisplay();
}
/*----------------------------------------------------------------- mouse ----*/
/* mouseコールバック
 * マウスボタンがクリックされたときの動作を定義する
 */
void mouse(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN){     /* マウスがクリックされて */
        mcx = x / (float)WIDTH;
        mcy = ((float)HEIGHT - y )/(float)HEIGHT;
        mdx0 = mdx = mcx;
        mdy0 = mdy = mcy;
//        printf( "mouse: [x,y]=[%6.3f, %6.3f]\n", mcx, mcy ); fflush( stdout );
        switch(button){
          case GLUT_LEFT_BUTTON:    /* クリックされたのが左ボタンだったら */
            moveflag = 1;
            break;
          case GLUT_RIGHT_BUTTON:   /*クリックされたのが右ボタンだったら */
            moveflag = 2;
            break;
        }
    }
    else{
        moveflag = 0;
        mdx0 = mcx = mdx;
        mdy0 = mcy = mdy;
        dx = 0.;
        dy = 0.;
        switch(button){
          case GLUT_RIGHT_BUTTON:
          case GLUT_LEFT_BUTTON:
            break;
        }
    }
}
/*----------------------------------------------------------- mousemotion ----*/
void mousemotion( int x, int y )
{
    mmx = x / (float)WIDTH;
    mmy = ((float)HEIGHT - y )/(float)HEIGHT;
}
/*------------------------------------------------------------- mousedrag ----*/
void mousedrag( int x, int y )
{
    mdx0 = mdx;
    mdy0 = mdy;
    mdx = x / (float)WIDTH;
    mdy = 1.0 - y / (float)HEIGHT;

    dx = mdx - mcx;
    dy = mdy - mcy;

    float ax = mcx - 0.5;
    float ay = mcy - 0.5;
    float bx = mdx - 0.5;
    float by = mdy - 0.5;
    float a2 = ax*ax + ay*ay;
    float b2 = bx*bx + by*by;
    float aXb = sqrtf( (ax*by-ay*bx)*(ax*by-ay*bx) );
    ab = sqrtf( (ax-bx)*(ax-bx) + (ay-by)*(ay-by) );
    if( ab == 0.0 ) d = 0.0;
    else
      //d = aXb * 2.0 / ab;
      d = aXb * 2.0;
    float C;
    C = - (ay-by)/(ax-bx)*ax + ay;
    if( C < 0.0 ) d *= -1.0;
    if( ax < bx ) d *= -1.0;
//    printf( "d = %f\r", d ); fflush( stdout );
//    printf( "mousedrag: [x,y]=[%6.3f, %6.3f]\r", mdx, mdy ); fflush( stdout );
    if( moveflag == 2 ){
        if( b2 - a2 > 0. ){
            distance *= 1. - ( mdy - mdy0 );
        }
        else{
            distance *= 1. - ( mdy - mdy0 );
        }
//        printf( "%f  %f\n", mdy, mdy0 ); fflush(stdout );
    }
  
    return;
}
/*-------------------------------------------------------------- keyboard ----*/
void keyboard(unsigned char key, int x, int y)
{
    static int smooth = 0;
    switch( key ){
      case 'q': /* [q]が入力されたら，プログラムを終了する */
        exit(0);
      case 'h': /* [h]が押されたら，操作説明を表示する */
        printf( "▼操作説明：\n" );
        printf( "■オブジェクトの回転\n" );
        printf( "画面の中で左マウスボタンを押したまま\n" );
        printf( "オブジェクトを回転したい方向にゆっくりドラッグして下さい\n" );
        printf( "左マウスボタンを押している間，回転を続けます\n" );
        printf( "■観察距離\n" );
        printf( "画面の中で右マウスボタンを押したまま\n" );
        printf( "上にドラッグすると接近し、下にドラッグすると遠くなります\n" );
        printf( "■観察距離リセット\n" );
        printf( "[r]キーを押下すると観察距離をリセットします\n" );
        printf( "■終了\n" );
        printf( "[q]キーを押下するとプログラムを終了します\n" );
        printf( "▲\n" );
        break;
      case 'r':
        distance = 10.;
        break;
      case 's':
        if( smooth )
          glShadeModel(GL_SMOOTH);
        else
          glShadeModel(GL_FLAT);
        smooth = !smooth;
        break;
      default:
        break;
    }
}
/*------------------------------------------------------------------ init ----*/
void init(char *winname)
{
    glutInitWindowPosition( XMIN, YMIN );
    glutInitWindowSize( WIDTH, HEIGHT );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow(winname);

    glClearColor(0.2, 0.4, 0.4, 1.0);

    glEnable(GL_DEPTH_TEST);   /* 隠面処理の開始を宣言 */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); /* zバッファを初期化 */

    /* シェーディングの種類をフラットシェーディングに設定 */
    glShadeModel(GL_FLAT);

    glEnable( GL_COLOR_MATERIAL );

    glLoadIdentity();
}/*---- init */
/*================================================================== main ====*/
int main(int argc, char *argv[])
{
    if( argc == 1 ){
        printf( "使い方> %s mqoファイル名（※拡張子なし） 拡大率\n", argv[0] );
        printf( "例> %s tachikoma 0.001\n", argv[0] );
        exit( 0 );
    }
    if( argc == 3 ) scalefactor = atof( argv[2] );
    else scalefactor = 1.0;
    printf( "■操作説明はウィンドウをクリックしてから[h]キーを押してください\n" );
    
    glutInit(&argc, argv);
    init(argv[0]);

    char *model;
    model = (char *)malloc( strlen( argv[1] ) + 4 );
    strcpy( model, argv[1] );
    strcat( model, ".mqo" );

    mqoObj = mqoCreateObject( model, scalefactor );

    glutDisplayFunc(display);

    /* キーボード入力のコールバック関数の指定 */
    glutKeyboardFunc(keyboard);

    /* マウス入力のコールバック関数の指定 */
    glutMouseFunc(mouse);

    /* マウスドラッグ時のコールバック関数の指定 */
    glutMotionFunc(mousedrag);

    /* マウス移動時のコールバック関数の指定 */
    glutPassiveMotionFunc(mousemotion);

    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
/*=========================================================================*/
