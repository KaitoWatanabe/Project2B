#include <iostream>
#include <sstream>

// NuiApi.hの前にWindows.hをインクルードする
#include <Windows.h>
#include <NuiApi.h>

#include "GLMetaseq.h"
#include <gl/freeglut/glut.h>




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
    // 終了処理
    if ( kinect != 0 ) {
      kinect->NuiShutdown();
      kinect->Release();
    }
  }
  int kinectX[20];
  int kinectY[20];
  float kinectZ[20];

  void initialize()
  {
    createInstance();

    // Kinectの設定を初期化する
    ERROR_CHECK( kinect->NuiInitialize( NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON ) );

    // RGBカメラを初期化する
    ERROR_CHECK( kinect->NuiImageStreamOpen( NUI_IMAGE_TYPE_COLOR, CAMERA_RESOLUTION,
      0, 2, 0, &imageStreamHandle ) );

    // 距離カメラを初期化する
    ERROR_CHECK( kinect->NuiImageStreamOpen( NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, CAMERA_RESOLUTION,
      0, 2, 0, &depthStreamHandle ) );

    // Nearモード
    //ERROR_CHECK( kinect->NuiImageStreamSetImageFrameFlags(
    //  depthStreamHandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE ) );

    // スケルトンを初期化する
    ERROR_CHECK( kinect->NuiSkeletonTrackingEnable( 0, NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT ) );

    // フレーム更新イベントのハンドルを作成する
    streamEvent = ::CreateEvent( 0, TRUE, FALSE, 0 );
    ERROR_CHECK( kinect->NuiSetFrameEndEvent( streamEvent, 0 ) );

    // 指定した解像度の、画面サイズを取得する
    ::NuiImageResolutionToSize(CAMERA_RESOLUTION, width, height );
  }

  void run()
  {
      getSkeleton();
  }

private:

  void createInstance()
  {
    // 接続されているKinectの数を取得する
    int count = 0;
    ERROR_CHECK( ::NuiGetSensorCount( &count ) );
    if ( count == 0 ) {
      throw std::runtime_error( "Kinect を接続してください" );
    }

    // 最初のKinectのインスタンスを作成する
    ERROR_CHECK( ::NuiCreateSensorByIndex( 0, &kinect ) );

    // Kinectの状態を取得する
    HRESULT status = kinect->NuiStatus();
    if ( status != S_OK ) {
      throw std::runtime_error( "Kinect が利用可能ではありません" );
    }
  }



  void getSkeleton(  )
  {
    // スケルトンのフレームを取得する
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
	kinectZ[part] = position.z;
	
  }
 
};
#define WIDTH 640
#define HEIGHT 480

KinectSample kinect;
GLfloat lightpos[] = { 200.0, 150.0, 500.0, 1.0 };
MQO_MODEL model;

void Point(int x, int y, float size){
	glPointSize(size);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}
void display(void)
{
	kinect.run();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	for (int i = 0; i < 20; ++i){
		Point(kinect.kinectX[i]-WIDTH*0.45, (kinect.kinectY[i]-HEIGHT*0.45)*(-1), 20.0);
	}
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//視野角,アスペクト比(ウィンドウの幅/高さ),描画する範囲(最も近い距離,最も遠い距離)
	gluPerspective(30.0, (double)WIDTH / (double)HEIGHT, 1.0, 10000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//視点の設定
	gluLookAt(kinect.kinectX[NUI_SKELETON_POSITION_HAND_LEFT] - WIDTH / 2, 
		(kinect.kinectY[NUI_SKELETON_POSITION_HAND_LEFT] - HEIGHT / 2)*(-1), 
		500.0*kinect.kinectZ[NUI_SKELETON_POSITION_HAND_LEFT], //カメラの座標
		0.0, 100.0, 0.0, // 注視点の座標
		0.0, 1.0, 0.0); // 画面の上方向を指すベクトル

	std::cout << kinect.kinectX[NUI_SKELETON_POSITION_HAND_LEFT] << std::endl;
	//ライトの設定
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	mqoCallModel(model);

	glutSwapBuffers();
}
void Init(){
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	mqoInit();
	model = mqoCreateModel("mikumikoto.mqo", 2.0);
}
void idle(void)
{
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{

  try {
	  kinect.initialize();
	  glutInitWindowPosition(100, 100);
	  glutInitWindowSize(WIDTH, HEIGHT);
	  glutInit(&argc, argv);
	  glutCreateWindow(".MQOを読み込んで表示");
	  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	  glutDisplayFunc(display);
	  glutIdleFunc(idle);
	  Init();
	  glutMainLoop();
	  mqoDeleteModel(model);
	  mqoCleanup();
	  return 0;
	  
  }
  catch ( std::exception& ex ) {
    std::cout << ex.what() << std::endl;
  }
}



