// NPR3DModel.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

//#include "GLMetaseq.h"
#include <gl/freeglut/freeglut.h>
#include <cmath>
#define WIDTH 640
#define HEIGHT 480

using namespace std;
//Chapter.7 Geometric Models and Their Exploitation in NPR

void algorithm_7_1(){//classify edges based on the angle between adjacent faces in the model M
	/*for ( polygon : p) {// Model M �ŏ���
	compute the normal vector np for p
	if (np.dot(V) > 0) {
	mark p as visible;
	}else{
	mark p as invisible;
	}
	}

	Mdash := all polygons marked visible in M;
	for (edge : e) {//Model Mdash �ŏ���
	if (e belongs to only one polygon) {
	mark e as being a contour edge;
	}else{
	alphaE := angle between adjacent faces to e;

	if (alphaE > alpha_MAX) {//alpha_MAX�̓O���[�o���ϐ�
	mark e as sharp edge;
	}else if(alphaE == 0){
	mark e as  triangulation edge;//�O�p�`�̃��b�V��GL_TRIANGLES�Ƃ���e ��o�^
	}
	}
	}
	draw model according to visualization goals;*/
};

void algorithm_7_2(){//creating the haloed line effect
	/*
	I: = all intersections of the given line segments;

	*/
	/*
	object�^�͏�Œ�`����I
	for each (object var in collection_to_loop)
	{
	�� := angle between intersecting edges;
	trim edge further from the viewer by (t/sin(��)) on both sides of i;
	}
	*/
}

void algorithm_7_3(){//Computing intersections between a geometric model and a plane
	/*
	render model using a conventional shader and take a snapshot;
	extract the pixels on the  boundary(I contour);
	generate I = I contour - I cutcontour;
	convert I to line segments;
	*/
}

void algorithm_7_4(){//Computing the skeleton of a plygon mesh
	/*
	while (there are faces in the model M){
	sort all edges by length;
	object�^��edge�^�A�ϐ�e(from shortest to longest) do
	for each (edge var in collection_to_loop)
	{
	if (either start or end vertex are marked){
	mark other vertex;
	}else{
	collapse edge e;
	mark the new vertex;
	}
	}
	move all edges without vaild faces attached to it to the skelton S;
	}
	*/
}

void algorithm_7_5(){
	/*Creating a point coverage of a surface.The algorithm computes m coordinate
	pairs in parameter space that are sorted in the set P.
	*/
	/*
	P = NULL;
	n = number of points to cover the surface;
	calculate Rmax for the given surface;
	int i = 0;
	do
	{
	(u, v) = randomly generated coordinates in parameter space;
	double r = random number between zero and one;//0�`1�̗���
	if ((R(u, v)/Rmax) > r)
	{
	P = P ��{ (u, v) };//���͘a�W��
	i++;
	}
	} while (i < m);
	*/
}

/*
int main(int argc, char *argv[])
{

	try {
		kinect.initialize();
		glutInitWindowPosition(100, 100);
		glutInitWindowSize(WIDTH, HEIGHT);
		glutInit(&argc, argv);
		glutCreateWindow(".MQO��ǂݍ���ŕ\��");
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

		glutDisplayFunc(display);
		glutIdleFunc(idle);
		Init();
		glutMainLoop();
		mqoDeleteModel(model);
		mqoCleanup();
		return 0;

	}
	catch (exception& ex) {
		cout << ex.what() << endl;
	}
}
*/

