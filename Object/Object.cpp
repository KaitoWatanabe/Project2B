#include <GL/glut.h>

//------------ プロトタイプ宣言 -------------------//
void LIGHT_INIT();
void display();
void reshape(int w, int h);
void SetMaterialGold();

//------------- OpenGLの初期設定 --------------//
void GLUT_INIT()
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Golden Ball");
}

void GLUT_CALL_FUNC()
{
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
}


void MY_INIT()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);

	LIGHT_INIT();
	glEnable(GL_NORMALIZE);//法線の有効化
	glEnable(GL_DEPTH_TEST);//ZバッファON
}



//光源の設定
void LIGHT_INIT()
{
	glEnable(GL_LIGHTING);//光源の有効化
	glEnable(GL_LIGHT0);
}


//----------- メイン関数 --------------//
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	GLUT_INIT();
	GLUT_CALL_FUNC();
	MY_INIT();
	glutMainLoop();

	return 0;
}

//------------ ここから各種コールバック ---------------//
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //視点の設定

	static float Light0Pos[] = { 1, 1, 1, 0 }; //光源の位置
	glLightfv(GL_LIGHT0, GL_POSITION, Light0Pos);//位置だけ設定(あとはデフォルト)
	SetMaterialGold(); //材質
	glutSolidSphere(0.5, 60, 60);//球の描画

	glutSwapBuffers();
}


void reshape(int w, int h)
{
	glViewport(0, 0, w, h);  //ビューポートの設定

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0); //視野の設定

	glMatrixMode(GL_MODELVIEW);
}




//---------- ここから各種関数 ------------//
void SetMaterialGold()
{
	static GLfloat gold_amb[] = { 0.24725, 0.1995, 0.0745, 1.0 };
	static GLfloat gold_diff[] = { 0.75164, 0.60648, 0.22648, 1.0 };
	static GLfloat gold_spe[] = { 0.628281, 0.555802, 0.366065, 1.0 };
	static GLfloat gold_shin = 51.2;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, gold_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, gold_diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, gold_spe);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, gold_shin);

}