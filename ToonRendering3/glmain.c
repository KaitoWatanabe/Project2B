/* ************************************************************************
 * ファイル名		： glmain.c
 * 機能				： OpenGL & GLUT サンプル
 *                  ： トーラスの描画
 * ------------------------------------------------------------------------
 * 作成者　　		： Arakin
 * ************************************************************************ */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#include <gl/glew/glew.h>
#include <gl/glew/wglew.h>
#include <gl/freeglut/glut.h>

#include "futil.h"

/* GLUT用のコールバック関数 */
static void mouse(int button, int state, int x, int y);
static void motion(int x, int y);
static void keyboard(unsigned char key, int x, int y);
static void resize(int w, int h);
static void display(void);
static void idle(void);

static void initState(void);

/* モデルの移動カウント */
static int count = 0;

/* GPU情報の文字列 */
char gpuStr[256];

/* 時間計測用 */
#define STANDARD_OUTPUT		(0)		/* 時間を標準出力に出力 */
LARGE_INTEGER startTime, endTime, cycleTime, preCycleTime;
float frameFPS, drawFPS;

char vtxAryStr[] = "VA";
char vtxBufStr[] = "VB";
char *vtxStr;

/* 画面の幅と高さの初期値 */
int screenWidth = 480;
int screenHeight = 360;

/* 平行光源 */
FLight light = {
	/* 平行光源の場合、FUTL_SetLight()関数で方向を正規化 */
    0.3f, 0.2f, 0.8f, 0.0f,    /* postition(w=0.0fは平行光源) */
    1.0f, 1.0f, 1.0f, 1.0f,    /* ambient */
    0.8f, 0.8f, 0.8f, 1.0f,    /* diffuse */
    1.0f, 1.0f, 1.0f, 1.0f,    /* specular */
    0.0f, 0.0f, -1.0f,         /* spotDirection */
    0.0f,                      /* spotExpornent */
    180.0f,                    /* spotCutoff */
    1.0f, 0.0f, 0.0f           /* attenuation */
};

/* モデル用クォータニオンと回転行列 */
FQuat mdlQtn = {1.0f, 0.0f, 0.0f, 0.0f};
FMatrix mdlRot;

int startX, startY;
FQuat startQtn;

int pmode = 0;			/* 描画モード切替 */
int move = 1;			/* カウントアップ値 */
int drawVBO = 1;		/* VBO描画 */
int vsync = 1;			/* 垂直同期待ち */
int drag = 0;

static GLuint shdProg;

/*
 * 三角形の描画
 */
int main(int argc, char *argv[])
{
	GLenum err;

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutCreateWindow(argv[0]);

	/* GLEW初期化 */
	err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(motion);
	glutReshapeFunc(resize);
	glutDisplayFunc(display);
    glutIdleFunc(idle);

	/* ベンダーとGPU */
	sprintf_s(gpuStr, sizeof(gpuStr), "%s: OpenGL %s", 
		glGetString(GL_RENDERER), glGetString(GL_VERSION));

	/* シェーダプログラムの読み込み */
	if (FUTL_LoadShader("cartoon.vs", "cartoon.fs", &shdProg) < 0)
	{
		return -1;
	}

	/* トーラスモデル（ドーナツ）の作成 */
	if (FUTL_MakeTorus("toon.bmp") < 0)
	{
		return -1;
	}

	/* モデル用回転行列の初期化 */
	FUTL_QuatToMatrix(&mdlRot, &mdlQtn);

	/* GLステートの初期化 */
	initState();

	frameFPS = 0.0f;
	QueryPerformanceCounter(&preCycleTime);

	glutMainLoop();

	return 0;
}

/* ステート初期化 */
static void initState(void)
{
	return;
}


/* 描画のコールバック関数 */
void display(void)
{
	float ftime;
	int triangles;
	GLint texLoc;
	char str[256];

	QueryPerformanceCounter(&startTime);

	/* 基本的なステート */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	/* モデルビュー行列のセットアップ */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* 視点 */
	gluLookAt(0.0f, 0.0f, 120.0f, 0.0f, 0.0f, 0.0f, 
		0.0f, 1.0f, 0.0f);

	/* 光源設定(ワールド座標設定) */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	FUTL_SetLight(GL_LIGHT0, &light);

	/* 背景 */
	//glClearColor(0.0f, 0.0f, 1.0f, 1.0f);//origin
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* ポリゴン描画モード */
	if (pmode == 0)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (pmode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	glPushMatrix();

	/* マウス操作によるモデルの回転 */
	glMultMatrixf((float *)(&mdlRot));

	/* シェーダプログラムの適用 */
	glUseProgram(shdProg);

	/* テクスチャユニット0を指定 */
	texLoc = glGetUniformLocation(shdProg, "toontex");
	glUniform1i(texLoc, 0);

	/* トーラスの描画 */
	if (drawVBO)
	{
		triangles = FUTL_DrawTorusVBO(count);
		vtxStr = vtxBufStr;
	}
	else
	{
		triangles = FUTL_DrawTorus(count);
		vtxStr = vtxAryStr;
	}

	/* ドラッグ中でなく、一時停止中でもない場合、トーラスは自転 */
	if (drag == 0 && move != 0)
	{
		count++;
	}

	glPopMatrix();

	/* トーラスの描画時間を計測（計測用文字描画を含めない） */
	QueryPerformanceCounter(&endTime);
	ftime = FUTL_SubPCounter(&endTime, &startTime);
	drawFPS = 1.0f / ftime;

	sprintf_s(str, sizeof(str), 
			"%s : Frame=%4.0f[fps]: Draw=%7.0f[fps]: Perf=%.0f[Kt/s]\n", 
			vtxStr, frameFPS, drawFPS, triangles / (1000.0f * ftime));

	/* 左下原点の座標系で描画 */
	FUTL_DrawString(gpuStr, 4, (screenHeight - 1)- 15);
	FUTL_DrawString(str, 4, (screenHeight - 1) - 2 * 15);

	glutSwapBuffers();

	/* 1フレームの間隔を測定 */
	QueryPerformanceCounter(&cycleTime);
	ftime = FUTL_SubPCounter(&cycleTime, &preCycleTime);
	frameFPS = 1.0f / ftime;
	preCycleTime = cycleTime;
}

/* Windowリサイズのコールバック関数 */
static void resize(
	int width,		/* ウィンドウの幅 */
	int height		/* ウィンドウの高さ */
)
{
	float aspect;

	/* 現在のウィンドウの幅と高さ */
	screenWidth = width;
	screenHeight = height;
	aspect = (float)width / (float)height;

	/* ビューポートの設定 */
	glViewport(0, 0, width, height);

	/* 射影行列のセットアップ */
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	glFrustum(
		-0.6f * aspect,		/* left */
		 0.6f * aspect,		/* right */
		-0.6f,				/* bottom */
		 0.6f,				/* top */
		 1.0f, 10000.0f		/* near, far */
		);
}


static void idle(void)
{
	glutPostRedisplay();
}

/*
 * マウス移動時のコールバック関数
 * マウスが押されている時だけ、コールされる
 */
static void motion(int x, int y)
{
	if (drag != 0)	/* マウスで回転中 */
	{
		int dx, dy;
		FVector3D axis;
		FQuat dqtn;
		float rot;

		/* マウス位置は左上原点 */
		dx = x - startX;
		dy = y - startY;

		/* ドラッグ開始位置からの変位（ラジアン換算） */
		/* 横方向はウィンドウ幅が一周分、縦方向はウィンドウ高さが一周分 */
		/* 下方向のマウス移動は+X軸回転、右方向のマウス移動は+Y軸回転 */
		axis.x = 2.0f * FUTL_PI * dy / (float)screenHeight;
		axis.y = 2.0f * FUTL_PI * dx / (float)screenWidth;
		axis.z = 0.0f;

		/* ドラッグ開始位置からの回転角度、同時に回転軸を正規化 */
		rot = FUTL_VecNormalize(&axis);

		/* ドラッグ開始位置からの差分を元に回転クォータニオンを生成 */
		FUTL_QuatRotation(&dqtn, rot, axis.x, axis.y, axis.z);

		/* 回転クォータニオンを合成 */
		FUTL_QuatMult(&mdlQtn, &dqtn, &startQtn);

		/* クォータニオンからマトリックスに変換 */
		FUTL_QuatToMatrix(&mdlRot, &mdlQtn);
	}
}


/*
 * マウスボタン操作時のコールバック関数
 * マウスボタンを押したり、離したりする時にコールされる
 */
static void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN && drag == 0)
		{
			/* ドラッグによるモデルの回転開始 */
			drag = 1;

			/* 開始位置と開始時のクォータニオンの記録 */
			startX = x;
			startY = y;
			FUTL_QuatCopy(&startQtn, &mdlQtn);
		}
		else if (state == GLUT_UP && drag == 1)
		{
			/* ドラッグによるモデルの回転停止 */
			drag = 0;
		}
		break;

	default:
		break;
	}
}


static void keyboard(unsigned char key, int x, int y)
{
	if (key == 'q' || key == 'Q')
	{
		/* 終了 */
		exit(0);
	}
	else if (key == 'p' || key == 'P')
	{
		/* 一時停止 */
		move = 1 - move;
	}
	else if (key == 'd' || key == 'D')
	{
		/* 描画モード切替 */
		pmode++;
		if (pmode > 2)
			pmode = 0;
	}
	else if (key == 'b' || key == 'B')
	{
		/* 描画モード切替 */
		drawVBO = 1 - drawVBO;
	}
	else if (key == 's' || key == 'S')
	{
		/* 描画モード切替 */
		if (glewGetExtension("WGL_EXT_swap_control"))
		{
			vsync = 1 - vsync;
			if (vsync == 0)
				wglSwapIntervalEXT(0);
			else
				wglSwapIntervalEXT(1);
		}
	}
}
