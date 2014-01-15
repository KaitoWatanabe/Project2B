#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#include <GL/freeglut/freeglut.h>

#define WIDTH 500
#define HEIGHT 500

//���s�ړ��p
float y = 150.0f;
bool flag = false;
//��
GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
//���C�g�̈ʒu
GLfloat lightpos[] = { 200.0, 150.0, -500.0, 1.0 };

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//����p,�A�X�y�N�g��(�E�B���h�E�̕�/����),�`�悷��͈�(�ł��߂�����,�ł���������)
	gluPerspective(30.0, (double)WIDTH / (double)HEIGHT, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//���_�̐ݒ�
	gluLookAt(0.0, 0.0, -1000.0, //�J�����̍��W
		0.0, 0.0, 0.0, // �����_�̍��W
		0.0, 1.0, 0.0); // ��ʂ̏�������w���x�N�g��
	//���C�g�̐ݒ�
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	//�}�e���A���̐ݒ�
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
	//���s�ړ�
	glTranslatef(0.0f, y, 0.0f);
	glutSolidSphere(10.0, 16, 16);

	glutSwapBuffers();
}
void idle(void)
{
	if (flag){ y -= 0.05f; }
	else{ y += 0.05f; }
	if (y>300.0f)flag = true;
	if (y<-300.0f)flag = false;
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
	glutCreateWindow("����\�����ĕ��s�ړ�");
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	Init();
	glutMainLoop();
	return 0;
}