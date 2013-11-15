#include <glm/glm.hpp>
#include <gl/freeglut/glut.h>

//----------- �v���g�^�C�v�錾 --------------//
void display();
void reshape(int w, int h);
void timer(int value);
void idle();

void DRAW_XYZ();
void DRAW_TRI();

//------------- OpenGL�̏����ݒ� ------------------//
void GLUT_INIT()
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); //�_�u���o�b�t�@�AZ�o�b�t�@
	glutInitWindowSize(640, 480);
	glutCreateWindow("Basic Animation with GLM");
}

void GLUT_CALL_FUNC()
{
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

}

void MY_INIT()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
}

//------------- ���C���֐� ----------------//
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	GLUT_INIT();
	GLUT_CALL_FUNC();
	MY_INIT();

	glutMainLoop();

	return 0;
}

//------------ ��������R�[���o�b�N�֐� ------------------//
void display()
{
	static int r = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	DRAW_XYZ();

	glPushMatrix();
	glRotatef(float(r), 0.f, 1.f, 0.f);
	glTranslatef(1.f, 0.f, 0.f);
	glColor3f(0.f, 1.f, 0.f);
	DRAW_TRI();
	glPopMatrix();

	glColor3d(1, 1, 1); //�F�̃��Z�b�g


	glutSwapBuffers();


	if (++r >= 360)
	{
		r = 0;
	}
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION); //�s�񃂁[�h�ؑ�
	glLoadIdentity();  //�s�񏉊���
	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW); //�s�񃂁[�h�ؑ�
	glLoadIdentity();
	gluLookAt(3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

}


void idle()
{
	glutPostRedisplay();
}

//--------------- ��������e��֐� ----------------//
void DRAW_XYZ()
{
	glBegin(GL_LINES);

	glColor3f(0, 1, 0);//x
	glVertex2f(-100, 0);
	glVertex2f(100, 0);

	glColor3f(1, 0, 0);//y
	glVertex2f(0, 0);
	glVertex2f(0, 100);

	glColor3f(0, 0, 1);//z
	glVertex3f(0, 0, -100);
	glVertex3f(0, 0, 100);
	glEnd();
}

void DRAW_TRI()
{
	static glm::vec3 pt1(0, 1, 0);
	static glm::vec3 pt2(-0.5, 0, 0);
	static glm::vec3 pt3(0.5, 0, 0);

	glBegin(GL_TRIANGLES);
	glVertex3fv(&pt1[0]); //�A�h���X���w�肷��p�^�[��
	glVertex3f(pt2[0], pt2[1], pt2[2]); //�I�y���[�^���g���p�^�[��
	glVertex3f(pt3.x, pt3.y, pt3.z);//�����o�ϐ����g���p�^�[��
	glEnd();
}