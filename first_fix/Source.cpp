/*
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
�����Ƃ̃T���v���R�[�h�ɂ������L�q
*/

#include <iostream>
#include <stdio.h>

#include <gl\freeglut\glut.h>
//#include <cmath>

using namespace std;


void display(){
	static GLfloat vertices[8][3] =
	{
		{ -0.5f, -0.5f, 0.5f },
		{ 0.5f, -0.5f, 0.5f },
		{ 0.5f, 0.5f, 0.5f },
		{ -0.5f, 0.5f, 0.5f },
		{ 0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f, 0.5f, -0.5f },
		{ 0.5f, 0.5f, -0.5f },
	};

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*�O*/
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3fv(vertices[0]);
	glVertex3fv(vertices[1]);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3fv(vertices[2]);
	glVertex3fv(vertices[3]);
	glEnd();

	/*��*/
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3fv(vertices[4]);
	glVertex3fv(vertices[5]);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3fv(vertices[6]);
	glVertex3fv(vertices[7]);
	glEnd();

	/*�E*/
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3fv(vertices[1]);
	glVertex3fv(vertices[4]);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3fv(vertices[7]);
	glVertex3fv(vertices[2]);
	glEnd();

	/*��*/
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3fv(vertices[5]);
	glVertex3fv(vertices[0]);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3fv(vertices[3]);
	glVertex3fv(vertices[6]);
	glEnd();

	/*��*/
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3fv(vertices[3]);
	glVertex3fv(vertices[2]);
	glVertex3fv(vertices[7]);
	glVertex3fv(vertices[6]);
	glEnd();

	/*��*/
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3fv(vertices[1]);
	glVertex3fv(vertices[0]);
	glVertex3fv(vertices[5]);
	glVertex3fv(vertices[4]);
	glEnd();

	glFlush();
}

//(1)���_�̐ݒ�
void reshape(int width, int height){
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)width / (double)height, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.5, 1.5, 2.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

}
int main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutCreateWindow("3D Cube");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

 cout << "Hello!" <<  endl;
	
	glutMainLoop();

	return 0;
}
