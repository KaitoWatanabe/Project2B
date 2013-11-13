#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#include <Windows.h>
#include <gl\glut.h>

#define WIDTH 320
#define HEIGHT 240

int _x = 50;
int _y = 50;

void Point(int x, int y, float size){
	glPointSize(size);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}
void display(void)
{
	_x += 10;
	_y += 10;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	Point(_x, _y, 10.0);
	glFlush();
}
void Init(){
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
}
void glut_idle(){
	glutPostRedisplay();
	Sleep(1000);
}
int main(int argc, char *argv[])
{
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInit(&argc, argv);
	glutCreateWindow("“_‚ð•`‰æ");
	glutInitDisplayMode(GLUT_RGBA);
	glutDisplayFunc(display);
	glutIdleFunc(glut_idle);
	Init();
	glutMainLoop();
	return 0;
}