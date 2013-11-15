/* *****************************************************************************
 * ファイル名		： torusmodel.c
 * 機能				： トーラス型モデルの作成
 * ----------------------------------------------------------------------------
 * 作成者　　		： Arakin
 * ***************************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#include <gl\freegult\glut.h>
//#include <GL/glut.h>
#include <gl\glew\glew.h>

#include "futil.h"

#define TRS_SECTION		(64)		/* ドーナツ分割パラメータ */
#define TRS_RADIUS		(32.0f)		/* ドーナツの半径(Y軸回り) */
#define TRS_CRS_RADIUS	(16.0f)		/* ドーナツ輪切り部分の半径 */

#define TRS_CRS_NUM_VTX	(TRS_SECTION + 1)					/* 65 */
#define TRS_RND_NUM_VTX	(TRS_SECTION + 1)					/* 65 */
#define TRS_NUM_VTX		(TRS_CRS_NUM_VTX * TRS_RND_NUM_VTX)	/* 65x65 */

#define TRS_CRS_NUM_IDX	(2 * TRS_CRS_NUM_VTX)				/* 130 */

/* idx0, idx1,...., idxn */
#define TRS_NUM_IDX		(TRS_SECTION * TRS_CRS_NUM_IDX)		/* 65 * 130 */

#define TRS_RND_WRAP_NUM	(8)			/* トーラス環のテクスチャ繰り返し数 */
#define TRS_CRS_WRAP_NUM	(4)			/* 輪切り部分のテクスチャ繰り返し数 */

/* 頂点配列 */
static FVector3D torusVtx[TRS_NUM_VTX];			/* 位置 */
static FVector3D torusNrm[TRS_NUM_VTX];			/* 法線 */
static float torusUV[2 * TRS_NUM_VTX];			/* テクスチャ座標 */
static unsigned short torusIdx[TRS_NUM_IDX];	/* インデックス */
static FColorRGBA torusCol[TRS_NUM_VTX];		/* 頂点色 */

static int torusNumTri;							/* ポリゴン数 */
static int torusNumPrim;						/* GLプリミティブ数 */
static int torusNumPrimVtx;						/* プリミティブの頂点数 */

static unsigned int torusTexID;

/* モデルのマテリアル */
static FMaterial material = {
    0.2f, 0.2f, 0.2f, 1.0f,         /* ambient */
    0.8f, 0.8f, 0.8f, 1.0f,         /* diffuse & alpha */
    1.0f, 1.0f, 1.0f, 1.0f,         /* specular */
    0.0f, 0.0f, 0.0f, 1.0f,         /* emission */
    64.0f                           /* shininess */
};

/* 頂点バッファオブジェクトID */
static unsigned int vtxID, nrmID, uvID, idxID, colID;

/*
 * ドーナツモデルの作成
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
		/* テクスチャのロード */
		if (FUTL_LoadTexture(texFile, &torusTexID) < 0)
		{
			return -1;
		}

		/* テクスチャパラメータの設定 */
		glBindTexture(GL_TEXTURE_2D, torusTexID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		/* カートーンでは、クランプ */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		torusTexID = 0;
	}

	/* 頂点配列の作成 */
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

		/* x軸方向に平行移動して、y軸回転 */
		FUTL_MatIdentity(&mat);
		FUTL_MatTranslate(&mat, TRS_RADIUS, 0.0f, 0.0f);
		FUTL_MatRotateY(&mat, radi);

		for(j = 0; j <= TRS_SECTION; j++)
		{
			float rj, radj;
			FVector3D vec, nvec;

			rj = (float)j / (float)(TRS_SECTION);
			radj = (2.0f * FUTL_PI) * rj;

			/* 法線 */
			nvec.x = FUTL_Cos(radj);
			nvec.y = FUTL_Sin(radj);
			nvec.z = 0.0f;

			/* 位置座標 */
			vec.x = TRS_CRS_RADIUS * nvec.x;
			vec.y = TRS_CRS_RADIUS * nvec.y;
			vec.z = 0.0f;

			/* 変換後の位置座標 */
			FUTL_MatVector(lpV, &mat, &vec);

			/* 変換後の法線 */
			FUTL_MatDirection(lpN, &mat, &nvec);

			/* 頂点色 */
			lpC->r = 0.5f * lpN->x + 0.5f;
			lpC->g = 0.5f * lpN->y + 0.5f;
			lpC->b = 0.5f * lpN->z + 0.5f;
			lpC->a = 1.0f;

			/* テクスチャ座標 */
			*lpUV++ = TRS_RND_WRAP_NUM * ri;
			*lpUV++ = TRS_CRS_WRAP_NUM * rj;

			lpV++;
			lpN++;
			lpC++;
		}
	}

	/* 頂点インデックスの作成 */
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

	torusNumTri = (TRS_SECTION * 2) * TRS_SECTION;	/* ポリゴン数 */
	torusNumPrim = TRS_SECTION;						/* GLプリミティブ数 */
	torusNumPrimVtx = TRS_CRS_NUM_IDX;				/* プリミティブの頂点数 */

	/*
	 * 頂点バッファへの登録
	 */
	/* 位置座標の登録 */
	glGenBuffers(1, &vtxID);
	glBindBuffer(GL_ARRAY_BUFFER, vtxID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(torusVtx), torusVtx, GL_STATIC_DRAW);

	/* 法線の登録 */
	glGenBuffers(1, &nrmID);
	glBindBuffer(GL_ARRAY_BUFFER, nrmID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(torusNrm), torusNrm, GL_STATIC_DRAW);
#if 0
	/* 頂点色の登録 */
	glGenBuffers(1, &colID);
	glBindBuffer(GL_ARRAY_BUFFER, colID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(torusCol), torusCol, GL_STATIC_DRAW);

	/* テクスチャ座標の登録 */
	glGenBuffers(1, &uvID);
	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(torusUV), torusUV, GL_STATIC_DRAW);
#endif
	/* インデックスの登録 */
	glGenBuffers(1, &idxID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(torusIdx), torusIdx, GL_STATIC_DRAW);

	return 0;
}


/*
 * モデルの描画
 */
int FUTL_DrawTorusVBO(
	int count
)
{
	int i;
	int offset;
	float rot;

	/* モデリング変換の設定 */
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	/* トーラスの回転 */
	rot = (float)count * 1.0f;
	glRotatef(rot, 0.0f, 1.0f, 0.0f);
	glRotatef(rot, 1.0f, 0.0f, 0.0f);

	/* テクスチャの設定 */
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
	/* マテリアルの設定 */
	FUTL_SetMaterial(&material);
#endif
	/* 位置座標の設定 */
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vtxID);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	/* 法線の設定 */
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, nrmID);
	glNormalPointer(GL_FLOAT, 0, 0);
#if 0
	/* 頂点カラーの設定 */
	glEnableClientState(GL_COLOR_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, colID);
	glColorPointer(4, GL_FLOAT, 0, 0);

	/* テクスチャ座標の設定 */
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
#endif
	/* インデックスの設定 */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxID);

	/* 頂点バッファによる描画 */
	offset = 0;
	for (i = 0; i < torusNumPrim; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, torusNumPrimVtx, 
			GL_UNSIGNED_SHORT, (void *)offset);

		offset += sizeof(unsigned short) * torusNumPrimVtx;
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	/* ポリゴン数の加算 */
	return torusNumTri;
}


/*
 * モデルの描画
 */
int FUTL_DrawTorus(
	int count
)
{
	int i;
	unsigned short *lpIndices;
	float rot;

	/* モデリング変換の設定 */
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	/* トーラスの回転 */
	rot = (float)count * 1.0f;
	glRotatef(rot, 0.0f, 1.0f, 0.0f);
	glRotatef(rot, 1.0f, 0.0f, 0.0f);

	/* テクスチャの設定 */
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
	/* マテリアルの設定 */
	FUTL_SetMaterial(&material);
#endif
	/* VBO無効化 */
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/* 位置座標の設定 */
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, torusVtx);

	/* 法線の設定 */
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 0, torusNrm);
#if 0
	/* 頂点カラーの設定 */
	glDisableClientState(GL_COLOR_ARRAY);

	/* テクスチャ座標の設定 */
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, torusUV);
#endif
	/* 頂点配列による描画 */
	lpIndices = torusIdx;
	for (i = 0; i < torusNumPrim; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, torusNumPrimVtx, 
			GL_UNSIGNED_SHORT, lpIndices);

		lpIndices += torusNumPrimVtx;
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	/* ポリゴン数の加算 */
	return torusNumTri;
}
