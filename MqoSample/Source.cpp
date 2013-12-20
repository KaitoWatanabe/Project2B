#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#include "GLMetaseq.h"
#include <gl/freeglut/freeglut.h>
#define WIDTH 640
#define HEIGHT 480

//���C�g�̈ʒu
GLfloat lightpos[] = { 200.0, 150.0, 500.0, 1.0 };
MQO_MODEL model;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//����p,�A�X�y�N�g��(�E�B���h�E�̕�/����),�`�悷��͈�(�ł��߂�����,�ł���������)
	gluPerspective(30.0, (double)WIDTH / (double)HEIGHT, 1.0, 10000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//���_�̐ݒ�
	gluLookAt(150.0, 400.0, 500.0, //�J�����̍��W
		0.0, 100.0, 0.0, // �����_�̍��W
		0.0, 1.0, 0.0); // ��ʂ̏�������w���x�N�g��
	//���C�g�̐ݒ�
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
	glutCreateWindow("(NPR).MQO��ǂݍ���ŕ\��");
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	Init();
	glutMainLoop();
	mqoDeleteModel(model);
	mqoCleanup();
	return 0;
}