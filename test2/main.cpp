

#include <gl\glut.h>
#include <gl\GL.h>

#pragma region // --- Constants ---
const double MY_MINDISTANCE = 0.1; // �J��������[0.1, 100.0]�͈̔͂����`�悵�Ȃ�
const double MY_MAXDISTANCE = 100.0;
const double CAMERA_DISTANCE = 30.0; //�J�����ƍ��W�n�̋���
#pragma endregion

void disp(){

#pragma region // --- ���_���� ---
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, 1.0, MY_MINDISTANCE, MY_MAXDISTANCE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//�J�����̍��W, ���_, ���W�n�̌���
	gluLookAt(
		sqrt(CAMERA_DISTANCE / 3.0), sqrt(CAMERA_DISTANCE / 3.0), -sqrt(CAMERA_DISTANCE / 3.0),
		0, 0, 0,
		0, 1.0, 0);

#pragma endregion


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glColor3d(1.0, 0.0, 0.0);
	glutWireTeapot(1.0);

	glFlush();

	glDisable(GL_DEPTH_TEST);

	glutSwapBuffers();
}

void glut_keyboard(unsigned char key, int x, int y){
	if (key == 'q'){
		exit(0);
	}

	glutPostRedisplay();
}

int main(int argc, char** argv){

#pragma region // --- initialize ---
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutCreateWindow("test window");
	glutDisplayFunc(disp);
	glutKeyboardFunc(glut_keyboard);

	glClearColor(0.1, 0.1, 0.1, 0.5); //�w�i�F
#pragma endregion 

	glutMainLoop();

	return 0;

}