#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#include "GLMetaseq.h"
#include <gl/freeglut/freeglut.h>
#define WIDTH 640
#define HEIGHT 480

//ライトの位置
GLfloat lightpos[] = { 200.0, 150.0, 500.0, 1.0 };
MQO_MODEL model;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//視野角,アスペクト比(ウィンドウの幅/高さ),描画する範囲(最も近い距離,最も遠い距離)
	gluPerspective(30.0, (double)WIDTH / (double)HEIGHT, 1.0, 10000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//視点の設定
	gluLookAt(150.0, 400.0, 500.0, //カメラの座標
		0.0, 100.0, 0.0, // 注視点の座標
		0.0, 1.0, 0.0); // 画面の上方向を指すベクトル
	//ライトの設定
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	mqoCallModel(model);

	glutSwapBuffers();
}

void idle(void)
{
	glutPostRedisplay();
}

void Init(){
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	mqoInit();
	model = mqoCreateModel("mikumikoto.mqo", 2.0);
}

int main(int argc, char *argv[])
{
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInit(&argc, argv);
	glutCreateWindow("(NPR).MQOを読み込んで表示");
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	Init();
	glutMainLoop();
	mqoDeleteModel(model);
	mqoCleanup();
	return 0;
}