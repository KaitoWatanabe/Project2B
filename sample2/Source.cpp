//#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#pragma comment(lib,"winmm.lib")

#include <iostream>
#include <sstream>

#include <windows.h>
#include <GL/freeglut/freeglut.h>
#include <stdio.h>
#include <math.h>

#include <NuiApi.h>

#define WIDTH 500
#define HEIGHT 500


#define ERROR_CHECK( ret )  \
if ( ret != S_OK ) {    \
std::stringstream ss;	\
ss << "failed " #ret " " << std::hex << ret << std::endl;			\
throw std::runtime_error( ss.str().c_str() );			\
}

const NUI_IMAGE_RESOLUTION CAMERA_RESOLUTION = NUI_IMAGE_RESOLUTION_640x480;

class KinectSample
{
private:

INuiSensor* kinect;
HANDLE imageStreamHandle;
HANDLE depthStreamHandle;
HANDLE streamEvent;

DWORD width;
DWORD height;

public:
KinectSample()
{
}

~KinectSample()
{
// �I������
if ( kinect != 0 ) {
kinect->NuiShutdown();
kinect->Release();
}
}
int kinectX[20];
int kinectY[20];

void initialize()
{
createInstance();

// Kinect�̐ݒ������������
ERROR_CHECK( kinect->NuiInitialize( NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON ) );

// RGB�J����������������
ERROR_CHECK( kinect->NuiImageStreamOpen( NUI_IMAGE_TYPE_COLOR, CAMERA_RESOLUTION,
0, 2, 0, &imageStreamHandle ) );

// �����J����������������
ERROR_CHECK( kinect->NuiImageStreamOpen( NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, CAMERA_RESOLUTION,
0, 2, 0, &depthStreamHandle ) );

// Near���[�h
//ERROR_CHECK( kinect->NuiImageStreamSetImageFrameFlags(
//  depthStreamHandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE ) );

// �X�P���g��������������
ERROR_CHECK( kinect->NuiSkeletonTrackingEnable( 0, NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT ) );

// �t���[���X�V�C�x���g�̃n���h�����쐬����
streamEvent = ::CreateEvent( 0, TRUE, FALSE, 0 );
ERROR_CHECK( kinect->NuiSetFrameEndEvent( streamEvent, 0 ) );

// �w�肵���𑜓x�́A��ʃT�C�Y���擾����
::NuiImageResolutionToSize(CAMERA_RESOLUTION, width, height );
}

void run()
{
getSkeleton();
}

private:

void createInstance()
{
// �ڑ�����Ă���Kinect�̐����擾����
int count = 0;
ERROR_CHECK( ::NuiGetSensorCount( &count ) );
if ( count == 0 ) {
throw std::runtime_error( "Kinect ��ڑ����Ă�������" );
}

// �ŏ���Kinect�̃C���X�^���X���쐬����
ERROR_CHECK( ::NuiCreateSensorByIndex( 0, &kinect ) );

// Kinect�̏�Ԃ��擾����
HRESULT status = kinect->NuiStatus();
if ( status != S_OK ) {
throw std::runtime_error( "Kinect �����p�\�ł͂���܂���" );
}
}



void getSkeleton(  )
{
// �X�P���g���̃t���[�����擾����
NUI_SKELETON_FRAME skeletonFrame = { 0 };
kinect->NuiSkeletonGetNextFrame( 0, &skeletonFrame );
//ERROR_CHECK( kinect->NuiSkeletonGetNextFrame( 0, &skeletonFrame ) );
for ( int i = 0; i < NUI_SKELETON_COUNT; ++i ) {
NUI_SKELETON_DATA& skeletonData = skeletonFrame.SkeletonData[i];
if ( skeletonData.eTrackingState == NUI_SKELETON_TRACKED ) {
for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; ++j){
if (skeletonData.eSkeletonPositionTrackingState[j] != NUI_SKELETON_POSITION_NOT_TRACKED) {
getJoint(skeletonData.SkeletonPositions[j], j);
}
}

}

}
}

void getJoint( Vector4 position, int part )
{
FLOAT depthX = 0, depthY = 0;
::NuiTransformSkeletonToDepthImage( position, &depthX, &depthY, CAMERA_RESOLUTION );

LONG colorX = 0;
LONG colorY = 0;

kinect->NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(
CAMERA_RESOLUTION, CAMERA_RESOLUTION,
0, (LONG)depthX , (LONG)depthY, 0, &colorX, &colorY );

kinectX[part] = (int)colorX;
kinectY[part] = (int)colorY;

}

};

KinectSample kinect;


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
	bool flag = true;
	void Draw();
};
void Sphere::Draw(){
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Color);
	glTranslatef(Pos.x, Pos.y, Pos.z);
	glutSolidSphere(Radius, 16, 16);
}

Sphere Hand, Do, So, La, Do1, So1, La1, So2;

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
	kinect.run();

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


	//Hand.Pos.x = 500 - kinect.kinectX[NUI_SKELETON_POSITION_HAND_LEFT];
	Hand.Pos.y = 130.0 - float(kinect.kinectY[NUI_SKELETON_POSITION_HAND_LEFT]) * (260.0/480.0);
	std::cout << Hand.Pos.y << std::endl;

	Hand.Draw();

	
	
	drawSoundSphere(Do);
	drawSoundSphere(Do1);
	drawSoundSphere(So);
	drawSoundSphere(So1);
	drawSoundSphere(La);
	drawSoundSphere(La1);
	drawSoundSphere(So2);

	glutSwapBuffers();
}

void timer(int value) {
	
	if (Do.flag){
		Do.Pos.x -= 1.0f;
	}
	else{
		Do.Pos.x += 1.0f;
	}
	if (Do1.flag){
		Do1.Pos.x -= 1.0f;
	}
	else{
		Do1.Pos.x += 1.0f;
	}
	if (So.flag){
		So.Pos.x -= 1.0f;
	}
	else{
		So.Pos.x += 1.0f;
	}
	if (So1.flag){
		So1.Pos.x -= 1.0f;
	}
	else{
		So1.Pos.x += 1.0f;
	}
	if (La.flag){
		La.Pos.x -= 1.0f;
	}
	else{
		La.Pos.x += 1.0f;
	}
	if (La1.flag){
		La1.Pos.x -= 1.0f;
	}
	else{
		La1.Pos.x += 1.0f;
	}
	if (So2.flag){
		So2.Pos.x -= 1.0f;
	}
	else{
		So2.Pos.x += 1.0f;
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
	Do.Color[0] = 1.0f;
	Do.Color[1] = 0.0f;
	Do.Color[2] = 0.0f;
	Do.Color[3] = 0.8f;
	Do.Pos.x = 130.0f;
	Do.Pos.y = 0.0f;
	Do.Pos.z = 0.0f;
	Do.Radius = 10.0f;
	Do.sound_src = "sound/Do.wav";
	Do1 = Do;
	Do1.Pos.x += 100.0f;
	Do1.Pos.y = 30.0f;
	So = Do1;
	So.Color[0] = 0.0f;
	So.Color[1] = 1.0f;
	So.Pos.x += 100.0f;
	So.Pos.y = -50.0f;
	So.sound_src = "sound/So.wav";
	So1 = So;
	So1.Pos.x += 100.0f;
	So1.Pos.y = -10.0f;
	La = So1;
	La.Color[1] = 0.0f;
	La.Color[2] = 1.0f;
	La.Pos.x += 100.0f;
	La.Pos.y = 100.0f;
	La.sound_src = "sound/La.wav";
	La1 = La;
	La1.Pos.x += 100.0f;
	La1.Pos.y = 0.0f;
	So2 = La1;
	So2.Color[1] = 1.0f;
	So2.Color[2] = 0.0f;
	So2.Pos.x += 100.0f;
	So2.sound_src = "sound/So.wav";


	Hand.Color[0] = 1.0f;
	Hand.Color[1] = 1.0f;
	Hand.Color[2] = 0.0f;
	Hand.Color[3] = 0.8f;
	Hand.Pos.x = -130.0f;
	Hand.Pos.y = 0.0f;
	Hand.Pos.z = 0.0f;
	Hand.Radius = 20.0f;
}
int main(int argc, char *argv[])
{	
	kinect.initialize();
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