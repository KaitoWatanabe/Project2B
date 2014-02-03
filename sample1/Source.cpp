#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#include <GL/freeglut/freeglut.h>

#define WIDTH 500
#define HEIGHT 500

//平行移動用
float y = 150.0f;
bool flag = false;
//緑
GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
//ライトの位置
GLfloat lightpos[] = { 200.0, 150.0, -500.0, 1.0 };

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//視野角,アスペクト比(ウィンドウの幅/高さ),描画する範囲(最も近い距離,最も遠い距離)
	gluPerspective(30.0, (double)WIDTH / (double)HEIGHT, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//視点の設定
	gluLookAt(0.0, 0.0, -1000.0, //カメラの座標
		0.0, 0.0, 0.0, // 注視点の座標
		0.0, 1.0, 0.0); // 画面の上方向を指すベクトル
	//ライトの設定
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	//マテリアルの設定
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
	//平行移動
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
	glutCreateWindow("球を表示して平行移動");
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	Init();
	glutMainLoop();
	return 0;
}