/* *****************************************************************************
 * �t�@�C����		�F torusmodel.c
 * �@�\				�F �g�[���X�^���f���̍쐬
 * ----------------------------------------------------------------------------
 * �쐬�ҁ@�@		�F Arakin
 * ***************************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#include <gl\freegult\glut.h>
//#include <GL/glut.h>
#include <gl\glew\glew.h>

#include "futil.h"

#define TRS_SECTION		(64)		/* �h�[�i�c�����p�����[�^ */
#define TRS_RADIUS		(32.0f)		/* �h�[�i�c�̔��a(Y�����) */
#define TRS_CRS_RADIUS	(16.0f)		/* �h�[�i�c�֐؂蕔���̔��a */

#define TRS_CRS_NUM_VTX	(TRS_SECTION + 1)					/* 65 */
#define TRS_RND_NUM_VTX	(TRS_SECTION + 1)					/* 65 */
#define TRS_NUM_VTX		(TRS_CRS_NUM_VTX * TRS_RND_NUM_VTX)	/* 65x65 */

#define TRS_CRS_NUM_IDX	(2 * TRS_CRS_NUM_VTX)				/* 130 */

/* idx0, idx1,...., idxn */
#define TRS_NUM_IDX		(TRS_SECTION * TRS_CRS_NUM_IDX)		/* 65 * 130 */

#define TRS_RND_WRAP_NUM	(8)			/* �g�[���X�̃e�N�X�`���J��Ԃ��� */
#define TRS_CRS_WRAP_NUM	(4)			/* �֐؂蕔���̃e�N�X�`���J��Ԃ��� */

/* ���_�z�� */
static FVector3D torusVtx[TRS_NUM_VTX];			/* �ʒu */
static FVector3D torusNrm[TRS_NUM_VTX];			/* �@�� */
static float torusUV[2 * TRS_NUM_VTX];			/* �e�N�X�`�����W */
static unsigned short torusIdx[TRS_NUM_IDX];	/* �C���f�b�N�X */
static FColorRGBA torusCol[TRS_NUM_VTX];		/* ���_�F */

static int torusNumTri;							/* �|���S���� */
static int torusNumPrim;						/* GL�v���~�e�B�u�� */
static int torusNumPrimVtx;						/* �v���~�e�B�u�̒��_�� */

static unsigned int torusTexID;

/* ���f���̃}�e���A�� */
static FMaterial material = {
    0.2f, 0.2f, 0.2f, 1.0f,         /* ambient */
    0.8f, 0.8f, 0.8f, 1.0f,         /* diffuse & alpha */
    1.0f, 1.0f, 1.0f, 1.0f,         /* specular */
    0.0f, 0.0f, 0.0f, 1.0f,         /* emission */
    64.0f                           /* shininess */
};

/* ���_�o�b�t�@�I�u�W�F�N�gID */
static unsigned int vtxID, nrmID, uvID, idxID, colID;

/*
 * �h�[�i�c���f���̍쐬
 */
int FUTL_MakeTorus(
	char *texFile
)
{
	int i, j;
	FVector3D *lpV, *lpN;
	float *lpUV;
	FColorRGBA *lpC;
	int num;
	unsigned short *lpIndices;

	if (texFile != NULL)
	{
		/* �e�N�X�`���̃��[�h */
		if (FUTL_LoadTexture(texFile, &torusTexID) < 0)
		{
			return -1;
		}

		/* �e�N�X�`���p�����[�^�̐ݒ� */
		glBindTexture(GL_TEXTURE_2D, torusTexID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		/* �J�[�g�[���ł́A�N�����v */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		torusTexID = 0;
	}

	/* ���_�z��̍쐬 */
	lpV = &(torusVtx[0]);
	lpN = &(torusNrm[0]);
	lpUV = &(torusUV[0]);
	lpC = &(torusCol[0]);
	for(i = 0; i <= TRS_SECTION; i++)
	{
		float ri, radi;
		FMatrix mat;

		ri = (float)i / (float)(TRS_SECTION);
		radi = (2.0f * FUTL_PI) * ri;

		/* x�������ɕ��s�ړ����āAy����] */
		FUTL_MatIdentity(&mat);
		FUTL_MatTranslate(&mat, TRS_RADIUS, 0.0f, 0.0f);
		FUTL_MatRotateY(&mat, radi);

		for(j = 0; j <= TRS_SECTION; j++)
		{
			float rj, radj;
			FVector3D vec, nvec;

			rj = (float)j / (float)(TRS_SECTION);
			radj = (2.0f * FUTL_PI) * rj;

			/* �@�� */
			nvec.x = FUTL_Cos(radj);
			nvec.y = FUTL_Sin(radj);
			nvec.z = 0.0f;

			/* �ʒu���W */
			vec.x = TRS_CRS_RADIUS * nvec.x;
			vec.y = TRS_CRS_RADIUS * nvec.y;
			vec.z = 0.0f;

			/* �ϊ���̈ʒu���W */
			FUTL_MatVector(lpV, &mat, &vec);

			/* �ϊ���̖@�� */
			FUTL_MatDirection(lpN, &mat, &nvec);

			/* ���_�F */
			lpC->r = 0.5f * lpN->x + 0.5f;
			lpC->g = 0.5f * lpN->y + 0.5f;
			lpC->b = 0.5f * lpN->z + 0.5f;
			lpC->a = 1.0f;

			/* �e�N�X�`�����W */
			*lpUV++ = TRS_RND_WRAP_NUM * ri;
			*lpUV++ = TRS_CRS_WRAP_NUM * rj;

			lpV++;
			lpN++;
			lpC++;
		}
	}

	/* ���_�C���f�b�N�X�̍쐬 */
	num = 0;
	lpIndices = &(torusIdx[0]);
	for(i = 0; i < TRS_SECTION; i++)
	{
		for(j = 0; j <= TRS_SECTION; j++)
		{
			*lpIndices++ = num;
			*lpIndices++ = num + TRS_CRS_NUM_VTX;
			num++;
		}
	}

	torusNumTri = (TRS_SECTION * 2) * TRS_SECTION;	/* �|���S���� */
	torusNumPrim = TRS_SECTION;						/* GL�v���~�e�B�u�� */
	torusNumPrimVtx = TRS_CRS_NUM_IDX;				/* �v���~�e�B�u�̒��_�� */

	/*
	 * ���_�o�b�t�@�ւ̓o�^
	 */
	/* �ʒu���W�̓o�^ */
	glGenBuffers(1, &vtxID);
	glBindBuffer(GL_ARRAY_BUFFER, vtxID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(torusVtx), torusVtx, GL_STATIC_DRAW);

	/* �@���̓o�^ */
	glGenBuffers(1, &nrmID);
	glBindBuffer(GL_ARRAY_BUFFER, nrmID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(torusNrm), torusNrm, GL_STATIC_DRAW);
#if 0
	/* ���_�F�̓o�^ */
	glGenBuffers(1, &colID);
	glBindBuffer(GL_ARRAY_BUFFER, colID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(torusCol), torusCol, GL_STATIC_DRAW);

	/* �e�N�X�`�����W�̓o�^ */
	glGenBuffers(1, &uvID);
	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(torusUV), torusUV, GL_STATIC_DRAW);
#endif
	/* �C���f�b�N�X�̓o�^ */
	glGenBuffers(1, &idxID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(torusIdx), torusIdx, GL_STATIC_DRAW);

	return 0;
}


/*
 * ���f���̕`��
 */
int FUTL_DrawTorusVBO(
	int count
)
{
	int i;
	int offset;
	float rot;

	/* ���f�����O�ϊ��̐ݒ� */
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	/* �g�[���X�̉�] */
	rot = (float)count * 1.0f;
	glRotatef(rot, 0.0f, 1.0f, 0.0f);
	glRotatef(rot, 1.0f, 0.0f, 0.0f);

	/* �e�N�X�`���̐ݒ� */
	if (torusTexID != 0)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, torusTexID);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}
#if 0
	/* �}�e���A���̐ݒ� */
	FUTL_SetMaterial(&material);
#endif
	/* �ʒu���W�̐ݒ� */
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vtxID);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	/* �@���̐ݒ� */
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, nrmID);
	glNormalPointer(GL_FLOAT, 0, 0);
#if 0
	/* ���_�J���[�̐ݒ� */
	glEnableClientState(GL_COLOR_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, colID);
	glColorPointer(4, GL_FLOAT, 0, 0);

	/* �e�N�X�`�����W�̐ݒ� */
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
#endif
	/* �C���f�b�N�X�̐ݒ� */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxID);

	/* ���_�o�b�t�@�ɂ��`�� */
	offset = 0;
	for (i = 0; i < torusNumPrim; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, torusNumPrimVtx, 
			GL_UNSIGNED_SHORT, (void *)offset);

		offset += sizeof(unsigned short) * torusNumPrimVtx;
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	/* �|���S�����̉��Z */
	return torusNumTri;
}


/*
 * ���f���̕`��
 */
int FUTL_DrawTorus(
	int count
)
{
	int i;
	unsigned short *lpIndices;
	float rot;

	/* ���f�����O�ϊ��̐ݒ� */
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	/* �g�[���X�̉�] */
	rot = (float)count * 1.0f;
	glRotatef(rot, 0.0f, 1.0f, 0.0f);
	glRotatef(rot, 1.0f, 0.0f, 0.0f);

	/* �e�N�X�`���̐ݒ� */
	if (torusTexID != 0)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, torusTexID);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}
#if 0
	/* �}�e���A���̐ݒ� */
	FUTL_SetMaterial(&material);
#endif
	/* VBO������ */
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/* �ʒu���W�̐ݒ� */
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, torusVtx);

	/* �@���̐ݒ� */
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 0, torusNrm);
#if 0
	/* ���_�J���[�̐ݒ� */
	glDisableClientState(GL_COLOR_ARRAY);

	/* �e�N�X�`�����W�̐ݒ� */
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, torusUV);
#endif
	/* ���_�z��ɂ��`�� */
	lpIndices = torusIdx;
	for (i = 0; i < torusNumPrim; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, torusNumPrimVtx, 
			GL_UNSIGNED_SHORT, lpIndices);

		lpIndices += torusNumPrimVtx;
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	/* �|���S�����̉��Z */
	return torusNumTri;
}
