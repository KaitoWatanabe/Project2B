/* *****************************************************************************
 * ファイル名		： fumath.c
 * 機能				： 算術、行列演算
 * ----------------------------------------------------------------------------
 * 作成者　　		： Arakin
 * ***************************************************************************** */
#include <stdio.h>
#include <math.h>

#include "futil.h"

/*
 * float用算術関数
 */
/* x = sin(rad) */
float FUTL_Sin(float rad)
{
	return (float)(sin((double)rad));
}

/* x = cos(rad) */
float FUTL_Cos(float rad)
{
	return (float)(cos((double)rad));
}

/* x = tan(rad) */
float FUTL_Tan(float rad)
{
	return (float)(tan((double)rad));
}

float FUTL_Floor(float x)	/* Add 2003-09-04 */
{
	return (float)(floor((double)x));
}

/* rad = atan(x) */
float FUTL_Atan(float x)
{
	return (float)(atan((double)x));
}

/* rad = atan2(y, x) */
float FUTL_Atan2(float y, float x)
{
	return (float)(atan2((double)y, (double)x));
}

/* abs = |x| */
float FUTL_Fabs(float x)
{
	return (x >= 0.0f) ? x : -x;
}

/* sq = x ^ 0.5 */
float FUTL_Sqrt(float x)
{
	return (float)(sqrt((double)x));
}

/* invsq = 1.0f / (x ^ 0.5) */
float FUTL_InvSqrt(float x)
{
	return 1.0f / (float)(sqrt((double)x));
}

/* p = a ^ b */
float FUTL_Pow(float a, float b)
{
	return (float)(pow((double)a, (double)b));
}


/*
 * 3Dベクトルの正規化（正規化前の長さを返す）
 */
float FUTL_VecNormalize(FVector3D *lpV)
{
	float len2, length, invLength;

	len2 = lpV->x * lpV->x + lpV->y * lpV->y + lpV->z * lpV->z;
	length = FUTL_Sqrt(len2);
	if(length == 0.0f)
	{
		return 0.0f;
	}

	invLength = 1.0f / length;
	lpV->x *= invLength;
	lpV->y *= invLength;
	lpV->z *= invLength;

	return length;
}

/*
 * 2Dベクトルの正規化（正規化前の長さを返す）
 */
float FUTL_Vec2DNormalize(FVector2D *lpV)
{
	float len2, length, invLength;

	len2 = lpV->x * lpV->x + lpV->y * lpV->y;
	length = FUTL_Sqrt(len2);

	if(length==0.0f)
	{
		return 0.0f;
	}

	invLength = 1.0f / length;
	lpV->x *= invLength;
	lpV->y *= invLength;

	return length;
}                                             

/*
 * float用行列演算関数
 */
/*
 * 行列[4x4]と3Dベクトルとの乗算
 */
/* lpS = lpM * pV */
void FUTL_MatVector(
	FVector3D *lpS, 
	FMatrix *lpM, 
	FVector3D *lpV
)
{
	float x, y, z;

	x = lpV->x;
	y = lpV->y;
	z = lpV->z;

	lpS->x = lpM->m00 * x + lpM->m01 * y + lpM->m02 * z + lpM->m03;
	lpS->y = lpM->m10 * x + lpM->m11 * y + lpM->m12 * z + lpM->m13;
	lpS->z = lpM->m20 * x + lpM->m21 * y + lpM->m22 * z + lpM->m23;
}


/*
 * 方向ベクトルの座標変換
 * 第4列(m03, m13, m23, m33)の影響を受けない座標変換
 */
void FUTL_MatDirection(
	FVector3D *lpSDir,			/* 変換された方向ベクトル */
	FMatrix *lpM,				/* マトリックス */
	FVector3D *lpDir			/* 入力方向ベクトル */
)
{
	float x, y, z;

	x = lpDir->x;
	y = lpDir->y;
	z = lpDir->z;

	lpSDir->x = lpM->m00 * x + lpM->m01 * y + lpM->m02 * z;
	lpSDir->y = lpM->m10 * x + lpM->m11 * y + lpM->m12 * z;
	lpSDir->z = lpM->m20 * x + lpM->m21 * y + lpM->m22 * z;
}


/*
 * void FUTL_MatIdentity(FMatrix *lpM)
 * 単位行列のセット
 *
 */
void FUTL_MatIdentity(FMatrix *lpM)
{
	           lpM->m01 = lpM->m02 = lpM->m03 =
	lpM->m10 =            lpM->m12 = lpM->m13 =
	lpM->m20 = lpM->m21 =            lpM->m23 = 
	lpM->m30 = lpM->m31 = lpM->m32 =            0.0f;

	lpM->m00 = lpM->m11 = lpM->m22 = lpM->m33 = 1.0f;
}


/*
 * void FUTL_MatCopy(FMatrix *lpM)
 * 行列のコピー
 */
void FUTL_MatCopy(FMatrix *lpMa, FMatrix *lpMb)
{
	lpMa->m00 = lpMb->m00;
	lpMa->m10 = lpMb->m10;
	lpMa->m20 = lpMb->m20;
	lpMa->m30 = lpMb->m30;

	lpMa->m01 = lpMb->m01;
	lpMa->m11 = lpMb->m11;
	lpMa->m21 = lpMb->m21;
	lpMa->m31 = lpMb->m31;

	lpMa->m02 = lpMb->m02;
	lpMa->m12 = lpMb->m12;
	lpMa->m22 = lpMb->m22;
	lpMa->m32 = lpMb->m32;

	lpMa->m03 = lpMb->m03;
	lpMa->m13 = lpMb->m13;
	lpMa->m23 = lpMb->m23;
	lpMa->m33 = lpMb->m33;
}


/*
 * void FUTL_MatTranslate(FMatrix *m, float x, float y, float z)
 * ワールドでの移動
 * M = T * M
 *
 * | m00 m01 m02 m03 |   | 1   0   0   x |   | m00 m01 m02 m03 |
 * | m10 m11 m12 m13 | = | 0   1   0   y | * | m10 m11 m12 m13 |
 * | m20 m21 m22 m23 |   | 0   0   1   z |   | m20 m21 m22 m23 |
 * | m30 m31 m32 m33 |   | 0   0   0   1 |   | m30 m31 m32 m33 |
 */ 
void FUTL_MatTranslate(FMatrix *m, float x, float y, float z)
{
	float m30, m31, m32, m33;

	m30 = m->m30;
	m31 = m->m31;
	m32 = m->m32;
	m33 = m->m33;

	m->m00 += m30 * x;
	m->m01 += m31 * x;
	m->m02 += m32 * x;
	m->m03 += m33 * x;

	m->m10 += m30 * y;
	m->m11 += m31 * y;
	m->m12 += m32 * y;
	m->m13 += m33 * y;

	m->m20 += m30 * z;
	m->m21 += m31 * z;
	m->m22 += m32 * z;
	m->m23 += m33 * z;
}


/*
 * void FUTL_MatMove(FMatrix *m, float x, float y, float z)
 * ローカルでの移動
 * M = M * T
 *
 * | m00 m01 m02 m03 |   | m00 m01 m02 m03 |   | 1   0   0   x |
 * | m10 m11 m12 m13 | = | m10 m11 m12 m13 | * | 0   1   0   y |
 * | m20 m21 m22 m23 |   | m20 m21 m22 m23 |   | 0   0   1   z |
 * | m30 m31 m32 m33 |   | m30 m31 m32 m33 |   | 0   0   0   1 |
 */ 
void FUTL_MatMove(FMatrix *m, float x, float y, float z)
{
	m->m03 += m->m00 * x + m->m01 * y + m->m02 * z;
	m->m13 += m->m10 * x + m->m11 * y + m->m12 * z;
	m->m23 += m->m20 * x + m->m21 * y + m->m22 * z;
	m->m33 += m->m30 * x + m->m31 * y + m->m32 * z;
}


/*
 * void FUTL_MatRotateX(FMatrix *m, float rad) 
 * X軸回りのワールド回転
 * M = Rx * M
 *
 * | m00 m01 m02 m03 |   | 1   0     0  0 |   | m00 m01 m02 m03 |
 * | m10 m11 m12 m13 | = | 0 cosA -sinA 0 | * | m10 m11 m12 m13 |
 * | m20 m21 m22 m23 |   | 0 sinA  cosA 0 |   | m20 m21 m22 m23 |
 * | m30 m31 m32 m33 |   | 0   0     0  1 |   | m30 m31 m32 m33 |
 *
 */ 
void FUTL_MatRotateX(FMatrix *m, float rad)
{
    float cosA, sinA, tmp1, tmp2;

	if((rad > 2.0f * FUTL_PI) || (rad < -2.0f * FUTL_PI))
	{
		rad = rad - 2.0f * FUTL_PI * (int)(rad / (2.0f * FUTL_PI));
	}

    cosA = FUTL_Cos(rad);
	sinA = FUTL_Sin(rad);

	tmp1 = m->m10;
	tmp2 = m->m20;
	m->m10 = cosA * tmp1 - sinA * tmp2;
	m->m20 = sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m11;
	tmp2 = m->m21;
	m->m11 = cosA * tmp1 - sinA * tmp2;
	m->m21 = sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m12;
	tmp2 = m->m22;
	m->m12 = cosA * tmp1 - sinA * tmp2;
	m->m22 = sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m13;
	tmp2 = m->m23;
	m->m13 = cosA * tmp1 - sinA * tmp2;
	m->m23 = sinA * tmp1 + cosA * tmp2;
}


/*
 * void FUTL_MatRotateY(FMatrix *m, float rad) 
 * Y軸回りのワールド回転
 * M = Ry * M
 *
 * | m00 m01 m02 m03 |   |  cosA 0 sinA 0 |   | m00 m01 m02 m03 |
 * | m10 m11 m12 m13 | = |   0   1   0  0 | * | m10 m11 m12 m13 |
 * | m20 m21 m22 m23 |   | -sinA 0 cosA 0 |   | m20 m21 m22 m23 |
 * | m30 m31 m32 m33 |   |   0   0   0  1 |   | m30 m31 m32 m33 |
 *
 */  
void FUTL_MatRotateY(FMatrix *m, float rad)
{
    float cosA, sinA, tmp1, tmp2;

	if((rad > 2.0f * FUTL_PI) || (rad < -2.0f * FUTL_PI))
	{
		rad = rad - 2.0f * FUTL_PI * (int)(rad / (2.0f * FUTL_PI));
	}

    cosA = FUTL_Cos(rad);
	sinA = FUTL_Sin(rad);

	tmp1 = m->m00;
	tmp2 = m->m20;
	m->m00 =  cosA * tmp1 + sinA * tmp2;
	m->m20 = -sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m01;
	tmp2 = m->m21;
	m->m01 =  cosA * tmp1 + sinA * tmp2;
	m->m21 = -sinA * tmp1 + cosA * tmp2;
	
	tmp1 = m->m02;
	tmp2 = m->m22;
	m->m02 =  cosA * tmp1 + sinA * tmp2;
	m->m22 = -sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m03;
	tmp2 = m->m23;
	m->m03 =  cosA * tmp1 + sinA * tmp2;
	m->m23 = -sinA * tmp1 + cosA * tmp2;
}


/*
 * void FUTL_MatRotateZ(FMatrix *m, float rad) 
 * Z軸回りのワールド回転
 * M = Rz * M
 *
 * | m00 m01 m02 m03 |   | cosA -sinA 0 0 |   | m00 m01 m02 m03 |
 * | m10 m11 m12 m13 | = | sinA  cosA 0 0 | * | m10 m11 m12 m13 |
 * | m20 m21 m22 m23 |   |   0     0  1 0 |   | m20 m21 m22 m23 |
 * | m30 m31 m32 m33 |   |   0     0  0 1 |   | m30 m31 m32 m33 |
 *
 */ 
void FUTL_MatRotateZ(FMatrix *m, float rad)
{
    float cosA, sinA, tmp1, tmp2;

	if((rad > 2.0f * FUTL_PI) || (rad < -2.0f * FUTL_PI))
	{
		rad = rad - 2.0f * FUTL_PI * (int)(rad / (2.0f * FUTL_PI));
	}

    cosA = FUTL_Cos(rad);
	sinA = FUTL_Sin(rad);

	tmp1 = m->m00;
	tmp2 = m->m10;
	m->m00 = cosA * tmp1 - sinA * tmp2;
	m->m10 = sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m01;
	tmp2 = m->m11;
	m->m01 = cosA * tmp1 - sinA * tmp2;
	m->m11 = sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m02;
	tmp2 = m->m12;
	m->m02 = cosA * tmp1 - sinA * tmp2;
	m->m12 = sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m03;
	tmp2 = m->m13;
	m->m03 = cosA * tmp1 - sinA * tmp2;
	m->m13 = sinA * tmp1 + cosA * tmp2;
}


/*
 * void FUTL_MatTurnX(FMatrix *m, float rad) 
 * X軸回りのローカル回転
 * M = M * Rx
 *
 * | m00 m01 m02 m03 |   | m00 m01 m02 m03 |   | 1   0     0  0 |
 * | m10 m11 m12 m13 | = | m10 m11 m12 m13 | * | 0 cosA -sinA 0 |
 * | m20 m21 m22 m23 |   | m20 m21 m22 m23 |   | 0 sinA  cosA 0 |
 * | m30 m31 m32 m33 |   | m30 m31 m32 m33 |   | 0   0     0  1 |
 */ 
void FUTL_MatTurnX(FMatrix *m, float rad)
{
    float cosA, sinA, tmp1, tmp2;

	if((rad > 2.0f * FUTL_PI) || (rad < -2.0f * FUTL_PI))
	{
		rad = rad - 2.0f * FUTL_PI * (int)(rad / (2.0f * FUTL_PI));
	}

    cosA = FUTL_Cos(rad);
	sinA = FUTL_Sin(rad);

	tmp1 = m->m01;
	tmp2 = m->m02;
	m->m01 =  cosA * tmp1 + sinA * tmp2;
	m->m02 = -sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m11;
	tmp2 = m->m12;
	m->m11 =  cosA * tmp1 + sinA * tmp2;
	m->m12 = -sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m21;
	tmp2 = m->m22;
	m->m21 =  cosA * tmp1 + sinA * tmp2;
	m->m22 = -sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m31;
	tmp2 = m->m32;
	m->m31 =  cosA * tmp1 + sinA * tmp2;
	m->m32 = -sinA * tmp1 + cosA * tmp2;
}


/*
 * void FUTL_MatTurnY(FMatrix *m, float rad) 
 * Y軸回りのローカル回転
 * M = M * Ry
 *
 * | m00 m01 m02 m03 |   | m00 m01 m02 m03 |   |  cosA 0 sinA 0 |
 * | m10 m11 m12 m13 | = | m10 m11 m12 m13 | * |   0   1   0  0 |
 * | m20 m21 m22 m23 |   | m20 m21 m22 m23 |   | -sinA 0 cosA 0 |
 * | m30 m31 m32 m33 |   | m30 m31 m32 m33 |   |   0   0   0  1 |
 */ 
void FUTL_MatTurnY(FMatrix *m, float rad)
{
    float cosA, sinA, tmp1, tmp2;

	if((rad > 2.0f * FUTL_PI) || (rad < -2.0f * FUTL_PI))
	{
		rad = rad - 2.0f * FUTL_PI * (int)(rad / (2.0f * FUTL_PI));
	}

    cosA = FUTL_Cos(rad);
	sinA = FUTL_Sin(rad);
	
	tmp1 = m->m00;
	tmp2 = m->m02;
	m->m00 = cosA * tmp1 - sinA * tmp2;
	m->m02 = sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m10;
	tmp2 = m->m12;
	m->m10 = cosA * tmp1 - sinA * tmp2;
	m->m12 = sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m20;
	tmp2 = m->m22;
	m->m20 = cosA * tmp1 - sinA * tmp2;
	m->m22 = sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m30;
	tmp2 = m->m32;
	m->m30 = cosA * tmp1 - sinA * tmp2;
	m->m32 = sinA * tmp1 + cosA * tmp2;
}


/*
 * void FUTL_MatTurnZ(FMatrix *m, float rad) 
 * Z軸回りのローカル回転
 * M = M * Rz
 *
 * | m00 m01 m02 m03 |   | m00 m01 m02 m03 |   | cosA -sinA 0 0 |
 * | m10 m11 m12 m13 | = | m10 m11 m12 m13 | * | sinA  cosA 0 0 |
 * | m20 m21 m22 m23 |   | m20 m21 m22 m23 |   |   0     0  1 0 |
 * | m30 m31 m32 m33 |   | m30 m31 m32 m33 |   |   0     0  0 1 |
 */ 
void FUTL_MatTurnZ(FMatrix *m, float rad)
{
    float cosA, sinA, tmp1, tmp2;

	if((rad > 2.0f * FUTL_PI) || (rad < -2.0f * FUTL_PI))
	{
		rad = rad - 2.0f * FUTL_PI * (int)(rad / (2.0f * FUTL_PI));
	}

    cosA = FUTL_Cos(rad);
	sinA = FUTL_Sin(rad);
	
	tmp1 = m->m00;
	tmp2 = m->m01;
	m->m00 =  cosA * tmp1 + sinA * tmp2;
	m->m01 = -sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m10;
	tmp2 = m->m11;
	m->m10 =  cosA * tmp1 + sinA * tmp2;
	m->m11 = -sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m20;
	tmp2 = m->m21;
	m->m20 =  cosA * tmp1 + sinA * tmp2;
	m->m21 = -sinA * tmp1 + cosA * tmp2;

	tmp1 = m->m30;
	tmp2 = m->m31;
	m->m30 =  cosA * tmp1 + sinA * tmp2;
	m->m31 = -sinA * tmp1 + cosA * tmp2;
}


/*
 * void FUTL_MatScale(FMatrix *m, 
 *					float scalex, float scaley, float scalez) 
 * ワールドの拡大縮小
 * M = S * M
 *
 * | m00 m01 m02 m03 |   |  Sx 0  0  0 |   | m00 m01 m02 m03 |
 * | m10 m11 m12 m13 | = |  0  Sy 0  0 | * | m10 m11 m12 m13 |
 * | m20 m21 m22 m23 |   |  0  0  Sz 0 |   | m20 m21 m22 m23 |
 * | m30 m31 m32 m33 |   |  0  0  0  1 |   | m30 m31 m32 m33 |
 */
void FUTL_MatScale(FMatrix *lpM,
					float scalex, float scaley, float scalez)
{
	lpM->m00 = scalex * lpM->m00;
	lpM->m01 = scalex * lpM->m01;
	lpM->m02 = scalex * lpM->m02;
	lpM->m03 = scalex * lpM->m03;

	lpM->m10 = scaley * lpM->m10;
	lpM->m11 = scaley * lpM->m11;
	lpM->m12 = scaley * lpM->m12;
	lpM->m13 = scaley * lpM->m13;

	lpM->m20 = scalez * lpM->m20;
	lpM->m21 = scalez * lpM->m21;
	lpM->m22 = scalez * lpM->m22;
	lpM->m23 = scalez * lpM->m23;
}


/*
 * void FUTL_MatSize(FMatrix *m, 
 *					float scalex, float scaley, float scalez)
 * ローカルのスケーリング
 * M = M * S
 *
 * | m00 m01 m02 m03 |   | m00 m01 m02 m03 |   |  Sx 0  0  0 |
 * | m10 m11 m12 m13 | = | m10 m11 m12 m13 | * |  0  Sy 0  0 |
 * | m20 m21 m22 m23 |   | m20 m21 m22 m23 |   |  0  0  Sz 0 |
 * | m30 m31 m32 m33 |   | m30 m31 m32 m33 |   |  0  0  0  1 |
 */ 
void FUTL_MatSize(FMatrix *lpM,
			   float scalex, float scaley, float scalez)
{
	lpM->m00 = scalex * lpM->m00;
	lpM->m01 = scaley * lpM->m01;
	lpM->m02 = scalez * lpM->m02;

	lpM->m10 = scalex * lpM->m10;
	lpM->m11 = scaley * lpM->m11;
	lpM->m12 = scalez * lpM->m12;

	lpM->m20 = scalex * lpM->m20;
	lpM->m21 = scaley * lpM->m21;
	lpM->m22 = scalez * lpM->m22;

	lpM->m30 = scalex * lpM->m30;
	lpM->m31 = scaley * lpM->m31;
	lpM->m32 = scalez * lpM->m32;
}



/*
 * クォータニオンの掛け算
 * lpR = lpP * lpQ
 */
void FUTL_QuatMult(FQuat *lpR, FQuat *lpP, FQuat *lpQ)
{
	float pw, px, py, pz;
	float qw, qx, qy, qz;

	pw = lpP->w; px = lpP->x; py = lpP->y; pz = lpP->z;
	qw = lpQ->w; qx = lpQ->x; qy = lpQ->y; qz = lpQ->z;

	lpR->w = pw * qw - px * qx - py * qy - pz * qz;
	lpR->x = pw * qx + px * qw + py * qz - pz * qy;
	lpR->y = pw * qy - px * qz + py * qw + pz * qx;
	lpR->z = pw * qz + px * qy - py * qx + pz * qw;
}


/*
 * クォータニオンを行列に変換
 * lpM <= lpQ
 */
void FUTL_QuatToMatrix(FMatrix *lpM, FQuat *lpQ)
{
	float qw, qx, qy, qz;
	float x2, y2, z2;
	float xy, yz, zx;
	float wx, wy, wz;

	qw = lpQ->w; qx = lpQ->x; qy = lpQ->y; qz = lpQ->z;

	x2 = 2.0f * qx * qx;
	y2 = 2.0f * qy * qy;
	z2 = 2.0f * qz * qz;

	xy = 2.0f * qx * qy;
	yz = 2.0f * qy * qz;
	zx = 2.0f * qz * qx;
	
	wx = 2.0f * qw * qx;
	wy = 2.0f * qw * qy;
	wz = 2.0f * qw * qz;

	lpM->m00 = 1.0f - y2 - z2;
	lpM->m01 = xy - wz;
	lpM->m02 = zx + wy;
	lpM->m03 = 0.0f;

	lpM->m10 = xy + wz;
	lpM->m11 = 1.0f - z2 - x2;
	lpM->m12 = yz - wx;
	lpM->m13 = 0.0f;

	lpM->m20 = zx - wy;
	lpM->m21 = yz + wx;
	lpM->m22 = 1.0f - x2 - y2;
	lpM->m23 = 0.0f;

	lpM->m30 = lpM->m31 = lpM->m32 = 0.0f;
	lpM->m33 = 1.0f;
}


/*
 * 回転クォータニオンの作成
 * lpQ <= 回転軸:(ax, ay, az), 回転角度:rad 
 */
void FUTL_QuatRotation(FQuat *lpQ, float rad, float ax, float ay, float az)
{
	float hrad;
	float s;

	hrad = 0.5f * rad;
	s = FUTL_Sin(hrad);

	lpQ->w = FUTL_Cos(hrad);
	lpQ->x = s * ax;
	lpQ->y = s * ay;
	lpQ->z = s * az;
}


/*
 * 単位クォータニオンの作成
 * lpQ <= (1, 0, 0, 0) 
 */
void FUTL_QuatIdentity(FQuat *lpQ)
{
	lpQ->w = 1.0f;
	lpQ->x = 0.0f;
	lpQ->y = 0.0f;
	lpQ->z = 0.0f;
}


/*
 * クォータニオンのコピー
 */
void FUTL_QuatCopy(FQuat *lpTo, FQuat *lpFrom)
{
	lpTo->w = lpFrom->w;
	lpTo->x = lpFrom->x;
	lpTo->y = lpFrom->y;
	lpTo->z = lpFrom->z;
}
