#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#pragma comment(lib,"winmm.lib")

#include <windows.h>
#include <GL/freeglut/freeglut.h>
#include <stdio.h>
#include <math.h>

#define WIDTH 500
#define HEIGHT 500

//���s�ړ��p
//float x = 0.0f;
//bool flag = false;

//3�̃x�N�g��
struct Vector3f{
	float x;
	float y;
	float z;
}vec3d;

//���\����
struct Sphere{
	GLfloat Color[4];//�F
	GLfloat Radius;//���a
	Vector3f Pos;//�ʒu
	char* sound_src;
	bool flag = false;
	void Draw();
};
void Sphere::Draw(){
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Color);
	glTranslatef(Pos.x, Pos.y, Pos.z);
	glutSolidSphere(Radius, 16, 16);
}

Sphere Hand, Do, Le, Mi, Fa, So, La, Si;

//���蔻��
bool Calc_Hit(Sphere& a, Sphere& b){
	if (sqrt((a.Pos.x - b.Pos.x)*(a.Pos.x - b.Pos.x) + (a.Pos.y - b.Pos.y)*(a.Pos.y - b.Pos.y) + (a.Pos.z - b.Pos.z)*(a.Pos.z - b.Pos.z)) <= a.Radius + b.Radius){
		return true;
	}
	else{
		return false;
	}
}

//���C�g�̈ʒu
GLfloat lightpos[] = { 200.0, 150.0, -500.0, 1.0 };

void drawSoundSphere(Sphere& s){
	if (Calc_Hit(s, Hand)){
		PlaySound(s.sound_src, NULL, SND_ASYNC);
		s.flag = false;
	}
	glLoadIdentity();
	//���_�̐ݒ�
	gluLookAt(0.0, 0.0, -500.0, //�J�����̍��W
		0.0, 0.0, 0.0, // �����_�̍��W
		0.0, 1.0, 0.0); // ��ʂ̏�������w���x�N�g��

	s.Draw();

}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//����p,�A�X�y�N�g��(�E�B���h�E�̕�/����),�`�悷��͈�(�ł��߂�����,�ł���������)
	gluPerspective(30.0, (double)WIDTH / (double)HEIGHT, 1.0, 2000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//���_�̐ݒ�
	gluLookAt(0.0, 0.0, -500.0, //�J�����̍��W
		0.0, 0.0, 0.0, // �����_�̍��W
		0.0, 1.0, 0.0); // ��ʂ̏�������w���x�N�g��

	

	//���C�g�̐ݒ�
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	Hand.Draw();

	
	
	drawSoundSphere(Do);
	drawSoundSphere(Le);	

	glutSwapBuffers();
}

void timer(int value) {
	if (Le.flag){ 
		Le.Pos.x -= 2.0f;
	}
	else{
		Le.Pos.x += 2.0f;
	}
	if (Le.Pos.x > 130.0f){
		Le.flag = true;
	}
	if (Do.flag){
		Do.Pos.x -= 1.0f;
	}
	else{
		Do.Pos.x += 1.0f;
	}
	if (Do.Pos.x > 130.0f){
		Do.flag = true;
	}
	
	glutTimerFunc(10, timer, 0);
}
void idle(void)
{
	glutPostRedisplay();
}
void Init(){
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);//�u�����h�̗L����
	Do.Color[0] = 0.0f;
	Do.Color[1] = 1.0f;
	Do.Color[2] = 1.0f;
	Do.Color[3] = 0.8f;
	Do.Pos.x = 0.0f;
	Do.Pos.y = 0.0f;
	Do.Pos.z = 0.0f;
	Do.Radius = 10.0f;
	Do.sound_src = "sound/Do.wav";
	Le.Color[0] = 1.0f;
	Le.Color[1] = 0.0f;
	Le.Color[2] = 1.0f;
	Le.Color[3] = 0.8f;
	Le.Pos.x = 130.0f;
	Le.Pos.y = 100.0f;
	Le.Pos.z = 0.0f;
	Le.Radius = 10.0f;
	Le.sound_src = "sound/Le.wav";
	Hand.Color[0] = 1.0f;
	Hand.Color[1] = 1.0f;
	Hand.Color[2] = 0.0f;
	Hand.Color[3] = 0.8f;
	Hand.Pos.x = -130.0f;
	Hand.Pos.y = 100.0f;
	Hand.Pos.z = 0.0f;
	Hand.Radius = 20.0f;
}
int main(int argc, char *argv[])
{
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInit(&argc, argv);
	glutCreateWindow("�o�E���f�B���O�X�t�B�A");
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutTimerFunc(10, timer, 0);
	Init();
	glutMainLoop();
	return 0;
}