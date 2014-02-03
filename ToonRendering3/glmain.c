/* ************************************************************************
 * �t�@�C����		�F glmain.c
 * �@�\				�F OpenGL & GLUT �T���v��
 *                  �F �g�[���X�̕`��
 * ------------------------------------------------------------------------
 * �쐬�ҁ@�@		�F Arakin
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

/* GLUT�p�̃R�[���o�b�N�֐� */
static void mouse(int button, int state, int x, int y);
static void motion(int x, int y);
static void keyboard(unsigned char key, int x, int y);
static void resize(int w, int h);
static void display(void);
static void idle(void);

static void initState(void);

/* ���f���̈ړ��J�E���g */
static int count = 0;

/* GPU���̕����� */
char gpuStr[256];

/* ���Ԍv���p */
#define STANDARD_OUTPUT		(0)		/* ���Ԃ�W���o�͂ɏo�� */
LARGE_INTEGER startTime, endTime, cycleTime, preCycleTime;
float frameFPS, drawFPS;

char vtxAryStr[] = "VA";
char vtxBufStr[] = "VB";
char *vtxStr;

/* ��ʂ̕��ƍ����̏����l */
int screenWidth = 480;
int screenHeight = 360;

/* ���s���� */
FLight light = {
	/* ���s�����̏ꍇ�AFUTL_SetLight()�֐��ŕ����𐳋K�� */
    0.3f, 0.2f, 0.8f, 0.0f,    /* postition(w=0.0f�͕��s����) */
    1.0f, 1.0f, 1.0f, 1.0f,    /* ambient */
    0.8f, 0.8f, 0.8f, 1.0f,    /* diffuse */
    1.0f, 1.0f, 1.0f, 1.0f,    /* specular */
    0.0f, 0.0f, -1.0f,         /* spotDirection */
    0.0f,                      /* spotExpornent */
    180.0f,                    /* spotCutoff */
    1.0f, 0.0f, 0.0f           /* attenuation */
};

/* ���f���p�N�H�[�^�j�I���Ɖ�]�s�� */
FQuat mdlQtn = {1.0f, 0.0f, 0.0f, 0.0f};
FMatrix mdlRot;

int startX, startY;
FQuat startQtn;

int pmode = 0;			/* �`�惂�[�h�ؑ� */
int move = 1;			/* �J�E���g�A�b�v�l */
int drawVBO = 1;		/* VBO�`�� */
int vsync = 1;			/* ���������҂� */
int drag = 0;

static GLuint shdProg;

/*
 * �O�p�`�̕`��
 */
int main(int argc, char *argv[])
{
	GLenum err;

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutCreateWindow(argv[0]);

	/* GLEW������ */
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

	/* �x���_�[��GPU */
	sprintf_s(gpuStr, sizeof(gpuStr), "%s: OpenGL %s", 
		glGetString(GL_RENDERER), glGetString(GL_VERSION));

	/* �V�F�[�_�v���O�����̓ǂݍ��� */
	if (FUTL_LoadShader("cartoon.vs", "cartoon.fs", &shdProg) < 0)
	{
		return -1;
	}

	/* �g�[���X���f���i�h�[�i�c�j�̍쐬 */
	if (FUTL_MakeTorus("toon.bmp") < 0)
	{
		return -1;
	}

	/* ���f���p��]�s��̏����� */
	FUTL_QuatToMatrix(&mdlRot, &mdlQtn);

	/* GL�X�e�[�g�̏����� */
	initState();

	frameFPS = 0.0f;
	QueryPerformanceCounter(&preCycleTime);

	glutMainLoop();

	return 0;
}

/* �X�e�[�g������ */
static void initState(void)
{
	return;
}


/* �`��̃R�[���o�b�N�֐� */
void display(void)
{
	float ftime;
	int triangles;
	GLint texLoc;
	char str[256];

	QueryPerformanceCounter(&startTime);

	/* ��{�I�ȃX�e�[�g */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	/* ���f���r���[�s��̃Z�b�g�A�b�v */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* ���_ */
	gluLookAt(0.0f, 0.0f, 120.0f, 0.0f, 0.0f, 0.0f, 
		0.0f, 1.0f, 0.0f);

	/* �����ݒ�(���[���h���W�ݒ�) */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	FUTL_SetLight(GL_LIGHT0, &light);

	/* �w�i */
	//glClearColor(0.0f, 0.0f, 1.0f, 1.0f);//origin
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* �|���S���`�惂�[�h */
	if (pmode == 0)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (pmode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	glPushMatrix();

	/* �}�E�X����ɂ�郂�f���̉�] */
	glMultMatrixf((float *)(&mdlRot));

	/* �V�F�[�_�v���O�����̓K�p */
	glUseProgram(shdProg);

	/* �e�N�X�`�����j�b�g0���w�� */
	texLoc = glGetUniformLocation(shdProg, "toontex");
	glUniform1i(texLoc, 0);

	/* �g�[���X�̕`�� */
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

	/* �h���b�O���łȂ��A�ꎞ��~���ł��Ȃ��ꍇ�A�g�[���X�͎��] */
	if (drag == 0 && move != 0)
	{
		count++;
	}

	glPopMatrix();

	/* �g�[���X�̕`�掞�Ԃ��v���i�v���p�����`����܂߂Ȃ��j */
	QueryPerformanceCounter(&endTime);
	ftime = FUTL_SubPCounter(&endTime, &startTime);
	drawFPS = 1.0f / ftime;

	sprintf_s(str, sizeof(str), 
			"%s : Frame=%4.0f[fps]: Draw=%7.0f[fps]: Perf=%.0f[Kt/s]\n", 
			vtxStr, frameFPS, drawFPS, triangles / (1000.0f * ftime));

	/* �������_�̍��W�n�ŕ`�� */
	FUTL_DrawString(gpuStr, 4, (screenHeight - 1)- 15);
	FUTL_DrawString(str, 4, (screenHeight - 1) - 2 * 15);

	glutSwapBuffers();

	/* 1�t���[���̊Ԋu�𑪒� */
	QueryPerformanceCounter(&cycleTime);
	ftime = FUTL_SubPCounter(&cycleTime, &preCycleTime);
	frameFPS = 1.0f / ftime;
	preCycleTime = cycleTime;
}

/* Window���T�C�Y�̃R�[���o�b�N�֐� */
static void resize(
	int width,		/* �E�B���h�E�̕� */
	int height		/* �E�B���h�E�̍��� */
)
{
	float aspect;

	/* ���݂̃E�B���h�E�̕��ƍ��� */
	screenWidth = width;
	screenHeight = height;
	aspect = (float)width / (float)height;

	/* �r���[�|�[�g�̐ݒ� */
	glViewport(0, 0, width, height);

	/* �ˉe�s��̃Z�b�g�A�b�v */
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
 * �}�E�X�ړ����̃R�[���o�b�N�֐�
 * �}�E�X��������Ă��鎞�����A�R�[�������
 */
static void motion(int x, int y)
{
	if (drag != 0)	/* �}�E�X�ŉ�]�� */
	{
		int dx, dy;
		FVector3D axis;
		FQuat dqtn;
		float rot;

		/* �}�E�X�ʒu�͍��㌴�_ */
		dx = x - startX;
		dy = y - startY;

		/* �h���b�O�J�n�ʒu����̕ψʁi���W�A�����Z�j */
		/* �������̓E�B���h�E����������A�c�����̓E�B���h�E����������� */
		/* �������̃}�E�X�ړ���+X����]�A�E�����̃}�E�X�ړ���+Y����] */
		axis.x = 2.0f * FUTL_PI * dy / (float)screenHeight;
		axis.y = 2.0f * FUTL_PI * dx / (float)screenWidth;
		axis.z = 0.0f;

		/* �h���b�O�J�n�ʒu����̉�]�p�x�A�����ɉ�]���𐳋K�� */
		rot = FUTL_VecNormalize(&axis);

		/* �h���b�O�J�n�ʒu����̍��������ɉ�]�N�H�[�^�j�I���𐶐� */
		FUTL_QuatRotation(&dqtn, rot, axis.x, axis.y, axis.z);

		/* ��]�N�H�[�^�j�I�������� */
		FUTL_QuatMult(&mdlQtn, &dqtn, &startQtn);

		/* �N�H�[�^�j�I������}�g���b�N�X�ɕϊ� */
		FUTL_QuatToMatrix(&mdlRot, &mdlQtn);
	}
}


/*
 * �}�E�X�{�^�����쎞�̃R�[���o�b�N�֐�
 * �}�E�X�{�^������������A�������肷�鎞�ɃR�[�������
 */
static void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN && drag == 0)
		{
			/* �h���b�O�ɂ�郂�f���̉�]�J�n */
			drag = 1;

			/* �J�n�ʒu�ƊJ�n���̃N�H�[�^�j�I���̋L�^ */
			startX = x;
			startY = y;
			FUTL_QuatCopy(&startQtn, &mdlQtn);
		}
		else if (state == GLUT_UP && drag == 1)
		{
			/* �h���b�O�ɂ�郂�f���̉�]��~ */
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
		/* �I�� */
		exit(0);
	}
	else if (key == 'p' || key == 'P')
	{
		/* �ꎞ��~ */
		move = 1 - move;
	}
	else if (key == 'd' || key == 'D')
	{
		/* �`�惂�[�h�ؑ� */
		pmode++;
		if (pmode > 2)
			pmode = 0;
	}
	else if (key == 'b' || key == 'B')
	{
		/* �`�惂�[�h�ؑ� */
		drawVBO = 1 - drawVBO;
	}
	else if (key == 's' || key == 'S')
	{
		/* �`�惂�[�h�ؑ� */
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
