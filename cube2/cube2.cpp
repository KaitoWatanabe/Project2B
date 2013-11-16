// cube2.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//


#include "stdafx.h"
#include <gl\freeglut\glut.h>
#include <gl\freeglut\freeglut.h>

#define WIDTH 320
#define HEIGHT 240


//回転用
float angle = 0.0f;
//白
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
//黄色
GLfloat yellow[] = { 1.0, 1.0, 0.0, 1.0 };
float Radius = 80.0f;
float posX = 120.0f;
float posY = 0.0f;
float posZ = 80.0f;

void Line3D(float x1, float y1, float z1, float x2, float y2, float z2){
	//線幅
	glLineWidth(1.0);
	//線
	glBegin(GL_LINES);
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);
	glEnd();
}

void DrawMeasure(int measure, float size){
	glDisable(GL_LIGHTING);
	glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
	for (int x = 0; x <= measure; x++){ Line3D(x*size - (size*measure / 2), 0, -(size*measure / 2), x*size - (size*measure / 2), 0, measure*size - (size*measure / 2)); }
	for (int y = 0; y <= measure; y++){ Line3D(-(size*measure / 2), 0, y*size - (size*measure / 2), measure*size - (size*measure / 2), 0, y*size - (size*measure / 2)); }
	glDisable(GL_DEPTH_TEST);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	Line3D(0, 0, 0, (measure / 2 + 2)*size, 0, 0);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	Line3D(0, 0, 0, 0, (measure / 2 + 2)*size, 0);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	Line3D(0, 0, 0, 0, 0, (measure / 2 + 2)*size);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(30.0, (double)WIDTH / (double)HEIGHT, 1.0, 5000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(300.0, 800.0, 800.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);

	glColor3f(1, 1, 1);
	DrawMeasure(16, 40);

	//回転基準の目印
	glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
	glTranslatef(posX, 50.0f, posZ);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glutSolidCone(10.0f, 50.0f, 4, 3);

	//円運動
	glLoadIdentity();
	gluLookAt(300.0, 800.0, 800.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);
	//回転基準位置に移動
	glTranslatef(posX, posY, posZ);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);//Y軸を回転
	//回転半径を足す
	glTranslatef(Radius, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
	glutSolidSphere(10.0, 16, 16);

	glutSwapBuffers();
}
void idle(void)
{
	angle += 0.2f;
	glutPostRedisplay();
}
void Init(){
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}
int main(int argc, char *argv[])
{
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInit(&argc, argv);
	glutCreateWindow("指定座標で円運動");
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	Init();
	glutMainLoop();
	return 0;
}
