// KeyBoardCommand.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//
#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <direct.h>
#include<time.h>
#include <GL/glut.h>
//#include <gl_screenshot.h>
using namespace std;

double PI = acos(-1.0);

int WindowPositionX = 200;  //��������E�B���h�E�ʒu��X���W
int WindowPositionY = 200;  //��������E�B���h�E�ʒu��Y���W
int WindowWidth = 512;  //��������E�B���h�E�̕�
int WindowHeight = 512;  //��������E�B���h�E�̍���
char WindowTitle[] = "���E�̎n�܂�";  //�E�B���h�E�̃^�C�g��

double ViewPointX = 0.0;
double ViewPointY = -50.0;
double ViewPointZ = 20.0;

bool _Bitmap = true;
int tn = 0;
double t = 0;
double dt = 0.1;
//gl_screenshot gs; //bmp�t�@�C���̏o��

struct {
	double x, y, z;
	double vx, vy, vz;
}p[100];
int pn = 0;
double ax = 0.0, ay = 0.0, az = -2.0;
double vx = 5.0, vy = 10.0, vz = 20.0;
double hanpatu = 0.9;
//----------------------------------------------------
// ���������̒�`
//----------------------------------------------------
struct MaterialStruct {
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat shininess;
};
//jade(�Ő�)
MaterialStruct ms_jade = {
	{ 0.135, 0.2225, 0.1575, 1.0 },
	{ 0.54, 0.89, 0.63, 1.0 },
	{ 0.316228, 0.316228, 0.316228, 1.0 },
	12.8 };
//ruby(���r�[)
MaterialStruct ms_ruby = {
	{ 0.1745, 0.01175, 0.01175, 1.0 },
	{ 0.61424, 0.04136, 0.04136, 1.0 },
	{ 0.727811, 0.626959, 0.626959, 1.0 },
	76.8 };
//----------------------------------------------------
// �F�̒�`�̒�`
//----------------------------------------------------
GLfloat red[] = { 0.8, 0.2, 0.2, 1.0 }; //�ԐF
GLfloat green[] = { 0.2, 0.8, 0.2, 1.0 };//�ΐF
GLfloat blue[] = { 0.2, 0.2, 0.8, 1.0 };//�F
GLfloat yellow[] = { 0.8, 0.8, 0.2, 1.0 };//���F
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };//���F
GLfloat shininess = 30.0;//����̋���
//-----------------------------------------

//----------------------------------------------------
// �֐��v���g�^�C�v�i��ɌĂяo���֐����ƈ����̐錾�j
//----------------------------------------------------
void Initialize(void);
void Display(void);
void Idle();
void Keyboard(unsigned char key, int x, int y);
void Ground(void);  //��n�̕`��

//----------------------------------------------------
// ���C���֐�
//----------------------------------------------------
int main(int argc, char *argv[]){
	srand((unsigned)time(NULL));
	if (_Bitmap) _mkdir("bitmap"); //bmp�t�@�C���ۑ��p�̃t�H���_�̍쐬

	glutInit(&argc, argv);//���̏�����
	glutInitWindowPosition(WindowPositionX, WindowPositionY);//�E�B���h�E�̈ʒu�̎w��
	glutInitWindowSize(WindowWidth, WindowHeight); //�E�B���h�E�T�C�Y�̎w��
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);//�f�B�X�v���C���[�h�̎w��
	glutCreateWindow(WindowTitle);  //�E�B���h�E�̍쐬
	glutDisplayFunc(Display); //�`�掞�ɌĂяo�����֐����w�肷��i�֐����FDisplay�j
	glutKeyboardFunc(Keyboard);//�L�[�{�[�h���͎��ɌĂяo�����֐����w�肷��i�֐����FKeyboard�j
	glutIdleFunc(Idle);       //�v���O�����A�C�h����Ԏ��ɌĂяo�����֐�
	Initialize(); //�����ݒ�̊֐����Ăяo��
	glutMainLoop();
	return 0;
}
//----------------------------------------------------
// �����ݒ�̊֐�
//----------------------------------------------------
void Initialize(void){
	glClearColor(1.0, 1.0, 1.0, 1.0); //�w�i�F
	glEnable(GL_DEPTH_TEST);//�f�v�X�o�b�t�@���g�p�FglutInitDisplayMode() �� GLUT_DEPTH ���w�肷��

	//�����̐ݒ�--------------------------------------
	GLfloat light_position0[] = { -50.0, -50.0, 20.0, 1.0 }; //����0�̍��W
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0); //����0��

}
//----------------------------------------------------
// �`��̊֐�
//----------------------------------------------------
void Display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //�o�b�t�@�̏���

	t = dt * tn;


	//�����ϊ��s��̐ݒ�------------------------------
	glMatrixMode(GL_PROJECTION);//�s�񃂁[�h�̐ݒ�iGL_PROJECTION : �����ϊ��s��̐ݒ�AGL_MODELVIEW�F���f���r���[�ϊ��s��j
	glLoadIdentity();//�s��̏�����
	gluPerspective(30.0, (double)WindowWidth / (double)WindowHeight, 0.1, 1000.0); //�������e�@�̎��̐�gluPerspactive(th, w/h, near, far);

	//���_�̐ݒ�------------------------------
	gluLookAt(
		ViewPointX, ViewPointY, ViewPointZ, // ���_�̈ʒux,y,z;
		0.0, 0.0, ViewPointZ,   // ���E�̒��S�ʒu�̎Q�Ɠ_���Wx,y,z
		0.0, 0.0, 1.0);  //���E�̏�����̃x�N�g��x,y,z
	//----------------------------------------

	//���f���r���[�ϊ��s��̐ݒ�--------------------------
	glMatrixMode(GL_MODELVIEW);//�s�񃂁[�h�̐ݒ�iGL_PROJECTION : �����ϊ��s��̐ݒ�AGL_MODELVIEW�F���f���r���[�ϊ��s��j
	glLoadIdentity();//�s��̏�����
	glViewport(0, 0, WindowWidth, WindowHeight);
	//----------------------------------------------

	//�A�eON-----------------------------
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);//����0�𗘗p
	//-----------------------------------

	for (int i = 1; i <= pn; i++){
		p[i].vx += ax * dt;
		p[i].vy += ay * dt;
		p[i].vz += az * dt;
		p[i].x += p[i].vx * dt;
		p[i].y += p[i].vy * dt;
		p[i].z += p[i].vz * dt;
		if (p[i].z < 4){
			p[i].z = 4.0;
			p[i].vz = -hanpatu * p[i].vz;
		}
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, ms_ruby.ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, ms_ruby.diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, ms_ruby.specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, &ms_ruby.shininess);
		glTranslated(p[i].x, p[i].y, p[i].z);//���s�ړ��l�̐ݒ�
		glutSolidSphere(4.0, 20, 20);//�����F(���a, Z���܂��̕�����, Z���ɉ�����������)
		glPopMatrix();
	}
	//�A�eOFF-----------------------------
	glDisable(GL_LIGHTING);
	//-----------------------------------

	Ground();

	if (_Bitmap){
		ostringstream fname;
		int tt = tn + 10000;
		fname << "bitmap/" << tt << ".bmp";//�o�̓t�@�C����
		string name = fname.str();
	//	gs.screenshot(name.c_str(), 24);
	}
	tn++;

	glutSwapBuffers(); //glutInitDisplayMode(GLUT_DOUBLE)�Ń_�u���o�b�t�@�����O�𗘗p��
}
//----------------------------------------------------
// �A�C�h�����ɌĂяo�����֐�
//----------------------------------------------------
void Idle(){
	glutPostRedisplay(); //glutDisplayFunc()���P����s����
}
//----------------------------------------------------
// �L�[�{�[�h���͎��ɌĂяo�����֐�
//----------------------------------------------------
void Keyboard(unsigned char key, int x, int y){
	switch (key)
	{
	case 'a':
		pn++;
		p[pn].x = 0.0;
		p[pn].y = 0.0;
		p[pn].z = 4.0;
		p[pn].vx = vx * ((double)rand() / (double)RAND_MAX - (double)rand() / (double)RAND_MAX);
		p[pn].vy = vy * ((double)rand() / (double)RAND_MAX);
		p[pn].vz = vz * ((double)rand() / (double)RAND_MAX);

		break;
	case 'q':
		exit(0);
		break;

	default:
		break;
	}
}
//----------------------------------------------------
// ��n�̕`��
//----------------------------------------------------
void Ground(void){
	double ground_max_x = 300.0;
	double ground_max_y = 300.0;
	glColor3d(0.8, 0.8, 0.8);  // ��n�̐F
	glBegin(GL_LINES);
	for (double ly = -ground_max_y; ly <= ground_max_y; ly += 10.0){
		glVertex3d(-ground_max_x, ly, 0);
		glVertex3d(ground_max_x, ly, 0);
	}
	for (double lx = -ground_max_x; lx <= ground_max_x; lx += 10.0){
		glVertex3d(lx, ground_max_y, 0);
		glVertex3d(lx, -ground_max_y, 0);
	}
	glEnd();
}