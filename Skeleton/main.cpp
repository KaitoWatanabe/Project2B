#include <iostream>
#include <sstream>

// NuiApi.hの前にWindows.hをインクルードする
#include <Windows.h>
#include <NuiApi.h>
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

#define WIDTH 640
#define HEIGHT 480

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
	std::cout << part << std::endl;
	kinectX[part] = (int)colorX;
	kinectY[part] = (int)colorY;
	
  }
 
};

KinectSample kinect;

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
		Point(kinect.kinectX[i], kinect.kinectY[i], 20.0);
	}
	
	glFlush();
}
void Init(){
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
}
void glut_idle(){
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{

  try {
	  kinect.initialize();
	  glutInitWindowPosition(100, 100);
	  glutInitWindowSize(WIDTH, HEIGHT);
	  glutInit(&argc, argv);
	  glutCreateWindow("点を描画");
	  glutInitDisplayMode(GLUT_RGBA);
	  glutDisplayFunc(display);
	  glutIdleFunc(glut_idle);
	  Init();
	  glutMainLoop();
	  return 0;
	  
  }
  catch ( std::exception& ex ) {
    std::cout << ex.what() << std::endl;
  }
}



