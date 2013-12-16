// 3DAction.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
// ３Ｄアクション基本

#include "DxLib.h"
#include <math.h>

// 関数プロトタイプ宣言 ---------------------------------------------------------------------------

void Input_Process(void);			// 入力処理

void Player_Initialize(void);		// プレイヤーの初期化
void Player_Terminate(void);			// プレイヤーの後始末
void Player_Process(void);			// プレイヤーの処理
void Player_Move(VECTOR MoveVector);		// プレイヤーの移動処理
void Player_AngleProcess(void);		// プレイヤーの向きを変える処理
void Player_PlayAnim(int PlayAnim);		// プレイヤーに新たなアニメーションを再生する
void Player_AnimProcess(void);		// プレイヤーのアニメーション処理
void Player_ShadowRender(void);		// プレイヤーの影を描画

void Stage_Initialize(void);			// ステージの初期化処理
void Stage_Terminate(void);			// ステージの後始末処理

void Camera_Initialize(void);		// カメラの初期化処理
void Camera_Process(void);			// カメラの処理

void Render_Process(void);			// 描画処理



// 固定値定義 -------------------------------------------------------------------------------------

// プレイヤー関係の定義
#define PLAYER_PLAY_ANIM_SPEED			250.0f		// アニメーション速度
#define PLAYER_MOVE_SPEED			30.0f		// 移動速度
#define PLAYER_ANIM_BLEND_SPEED			0.1f		// アニメーションのブレンド率変化速度
#define PLAYER_ANGLE_SPEED			0.2f		// 角度変化速度
#define PLAYER_JUMP_POWER			100.0f		// ジャンプ力
#define PLAYER_FALL_UP_POWER			20.0f		// 足を踏み外した時のジャンプ力
#define PLAYER_GRAVITY				3.0f		// 重力
#define PLAYER_MAX_HITCOLL			2048		// 処理するコリジョンポリゴンの最大数
#define PLAYER_ENUM_DEFAULT_SIZE		800.0f		// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_HIT_WIDTH			200.0f		// 当たり判定カプセルの半径
#define PLAYER_HIT_HEIGHT			700.0f		// 当たり判定カプセルの高さ
#define PLAYER_HIT_TRYNUM			16		// 壁押し出し処理の最大試行回数
#define PLAYER_HIT_SLIDE_LENGTH			5.0f		// 一度の壁押し出し処理でスライドさせる距離
#define PLAYER_SHADOW_SIZE			200.0f		// 影の大きさ
#define PLAYER_SHADOW_HEIGHT			700.0f		// 影が落ちる高さ

// カメラ関係の定義
#define CAMERA_ANGLE_SPEED			0.05f		// 旋回速度
#define CAMERA_PLAYER_TARGET_HEIGHT		400.0f		// プレイヤー座標からどれだけ高い位置を注視点とするか
#define CAMERA_PLAYER_LENGTH			1600.0f		// プレイヤーとの距離
#define CAMERA_COLLISION_SIZE			50.0f		// カメラの当たり判定サイズ



// 構造体定義 -------------------------------------------------------------------------------------

// 入力情報構造体
struct PADINPUT
{
	int		NowInput;				// 現在の入力
	int		EdgeInput;				// 現在のフレームで押されたボタンのみビットが立っている入力値
};

// プレイヤー情報構造体
struct PLAYER
{
	VECTOR		Position;				// 座標
	VECTOR		TargetMoveDirection;			// モデルが向くべき方向のベクトル
	float		Angle;					// モデルが向いている方向の角度
	float		JumpPower;				// Ｙ軸方向の速度
	int		ModelHandle;				// モデルハンドル
	int		ShadowHandle;				// 影画像ハンドル
	int		State;					// 状態( 0:立ち止まり  1:走り  2:ジャンプ )

	int		PlayAnim1;				// 再生しているアニメーション１のアタッチ番号( -1:何もアニメーションがアタッチされていない )
	float		AnimPlayCount1;			// 再生しているアニメーション１の再生時間
	int		PlayAnim2;				// 再生しているアニメーション２のアタッチ番号( -1:何もアニメーションがアタッチされていない )
	float		AnimPlayCount2;			// 再生しているアニメーション２の再生時間
	float		AnimBlendRate;				// 再生しているアニメーション１と２のブレンド率
};

// ステージ情報構造体
struct STAGE
{
	int		ModelHandle;				// モデルハンドル
};

// カメラ情報構造体
struct CAMERA
{
	float		AngleH;				// 水平角度
	float		AngleV;				// 垂直角度
	VECTOR		Eye;					// カメラ座標
	VECTOR		Target;				// 注視点座標
};



// 実体宣言 ---------------------------------------------------------------------------------------

PADINPUT inp;		// 入力情報の実体宣言
PLAYER pl;		// プレイヤー情報の実体宣言
STAGE stg;		// ステージ情報の実体宣言
CAMERA cam;		// カメラ情報の実体宣言




// 関数実体 ---------------------------------------------------------------------------------------

// 入力処理
void Input_Process(void)
{
	int Old;

	// ひとつ前のフレームの入力を変数にとっておく
	Old = inp.NowInput;

	// 現在の入力状態を取得
	inp.NowInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);

	// 今のフレームで新たに押されたボタンのビットだけ立っている値を EdgeInput に代入する
	inp.EdgeInput = inp.NowInput & ~Old;
}

// プレイヤーの初期化
void Player_Initialize(void)
{
	// 初期座標は原点
	pl.Position = VGet(0.0f, 0.0f, 0.0f);

	// 回転値は０
	pl.Angle = 0.0f;

	// ジャンプ力は初期状態では０
	pl.JumpPower = 0.0f;

	// モデルの読み込み
	pl.ModelHandle = MV1LoadModel("DxChara.x");

	// 影描画用の画像の読み込み
	pl.ShadowHandle = LoadGraph("Shadow.tga");

	// 初期状態では「立ち止り」状態
	pl.State = 0;

	// 初期状態でプレイヤーが向くべき方向はＸ軸方向
	pl.TargetMoveDirection = VGet(1.0f, 0.0f, 0.0f);

	// アニメーションのブレンド率を初期化
	pl.AnimBlendRate = 1.0f;

	// 初期状態ではアニメーションはアタッチされていないにする
	pl.PlayAnim1 = -1;
	pl.PlayAnim2 = -1;

	// ただ立っているアニメーションを再生
	Player_PlayAnim(4);
}

// プレイヤーの後始末
void Player_Terminate(void)
{
	// モデルの削除
	MV1DeleteModel(pl.ModelHandle);

	// 影用画像の削除
	DeleteGraph(pl.ShadowHandle);
}

// プレイヤーの処理
void Player_Process(void)
{
	VECTOR UpMoveVec;		// 方向ボタン「↑」を入力をしたときのプレイヤーの移動方向ベクトル
	VECTOR LeftMoveVec;		// 方向ボタン「←」を入力をしたときのプレイヤーの移動方向ベクトル
	VECTOR MoveVec;		// このフレームの移動ベクトル
	int MoveFlag;			// 移動したかどうかのフラグ( 1:移動した  0:移動していない )

	// ルートフレームのＺ軸方向の移動パラメータを無効にする
	{
		MATRIX LocalMatrix;

		// ユーザー行列を解除する
		MV1ResetFrameUserLocalMatrix(pl.ModelHandle, 2);

		// 現在のルートフレームの行列を取得する
		LocalMatrix = MV1GetFrameLocalMatrix(pl.ModelHandle, 2);

		// Ｚ軸方向の平行移動成分を無効にする
		LocalMatrix.m[3][2] = 0.0f;

		// ユーザー行列として平行移動成分を無効にした行列をルートフレームにセットする
		MV1SetFrameUserLocalMatrix(pl.ModelHandle, 2, LocalMatrix);
	}

	// プレイヤーの移動方向のベクトルを算出
	{
		// 方向ボタン「↑」を押したときのプレイヤーの移動ベクトルはカメラの視線方向からＹ成分を抜いたもの
		UpMoveVec = VSub(cam.Target, cam.Eye);
		UpMoveVec.y = 0.0f;

		// 方向ボタン「←」を押したときのプレイヤーの移動ベクトルは上を押したときの方向ベクトルとＹ軸のプラス方向のベクトルに垂直な方向
		LeftMoveVec = VCross(UpMoveVec, VGet(0.0f, 1.0f, 0.0f));

		// 二つのベクトルを正規化( ベクトルの長さを１．０にすること )
		UpMoveVec = VNorm(UpMoveVec);
		LeftMoveVec = VNorm(LeftMoveVec);
	}

	// このフレームでの移動ベクトルを初期化
	MoveVec = VGet(0.0f, 0.0f, 0.0f);

	// 移動したかどうかのフラグを初期状態では「移動していない」を表す０にする
	MoveFlag = 0;

	// パッドの３ボタンと左シフトがどちらも押されていなかったらプレイヤーの移動処理
	if (CheckHitKey(KEY_INPUT_LSHIFT) == 0 && (inp.NowInput & PAD_INPUT_C) == 0)
	{
		// 方向ボタン「←」が入力されたらカメラの見ている方向から見て左方向に移動する
		if (inp.NowInput & PAD_INPUT_LEFT)
		{
			// 移動ベクトルに「←」が入力された時の移動ベクトルを加算する
			MoveVec = VAdd(MoveVec, LeftMoveVec);

			// 移動したかどうかのフラグを「移動した」にする
			MoveFlag = 1;
		}
		else
			// 方向ボタン「→」が入力されたらカメラの見ている方向から見て右方向に移動する
		if (inp.NowInput & PAD_INPUT_RIGHT)
		{
			// 移動ベクトルに「←」が入力された時の移動ベクトルを反転したものを加算する
			MoveVec = VAdd(MoveVec, VScale(LeftMoveVec, -1.0f));

			// 移動したかどうかのフラグを「移動した」にする
			MoveFlag = 1;
		}

		// 方向ボタン「↑」が入力されたらカメラの見ている方向に移動する
		if (inp.NowInput & PAD_INPUT_UP)
		{
			// 移動ベクトルに「↑」が入力された時の移動ベクトルを加算する
			MoveVec = VAdd(MoveVec, UpMoveVec);

			// 移動したかどうかのフラグを「移動した」にする
			MoveFlag = 1;
		}
		else
			// 方向ボタン「↓」が入力されたらカメラの方向に移動する
		if (inp.NowInput & PAD_INPUT_DOWN)
		{
			// 移動ベクトルに「↑」が入力された時の移動ベクトルを反転したものを加算する
			MoveVec = VAdd(MoveVec, VScale(UpMoveVec, -1.0f));

			// 移動したかどうかのフラグを「移動した」にする
			MoveFlag = 1;
		}

		// プレイヤーの状態が「ジャンプ」ではなく、且つボタン１が押されていたらジャンプする
		if (pl.State != 2 && (inp.EdgeInput & PAD_INPUT_A))
		{
			// 状態を「ジャンプ」にする
			pl.State = 2;

			// Ｙ軸方向の速度をセット
			pl.JumpPower = PLAYER_JUMP_POWER;

			// ジャンプアニメーションの再生
			Player_PlayAnim(2);
		}
	}

	// 移動ボタンが押されたかどうかで処理を分岐
	if (MoveFlag)
	{
		// 移動ベクトルを正規化したものをプレイヤーが向くべき方向として保存
		pl.TargetMoveDirection = VNorm(MoveVec);

		// プレイヤーが向くべき方向ベクトルをプレイヤーのスピード倍したものを移動ベクトルとする
		MoveVec = VScale(pl.TargetMoveDirection, PLAYER_MOVE_SPEED);

		// もし今まで「立ち止まり」状態だったら
		if (pl.State == 0)
		{
			// 走りアニメーションを再生する
			Player_PlayAnim(1);

			// 状態を「走り」にする
			pl.State = 1;
		}
	}
	else
	{
		// このフレームで移動していなくて、且つ状態が「走り」だったら
		if (pl.State == 1)
		{
			// 立ち止りアニメーションを再生する
			Player_PlayAnim(4);

			// 状態を「立ち止り」にする
			pl.State = 0;
		}
	}

	// 状態が「ジャンプ」の場合は
	if (pl.State == 2)
	{
		// Ｙ軸方向の速度を重力分減算する
		pl.JumpPower -= PLAYER_GRAVITY;

		// もし落下していて且つ再生されているアニメーションが上昇中用のものだった場合は
		if (pl.JumpPower < 0.0f && MV1GetAttachAnim(pl.ModelHandle, pl.PlayAnim1) == 2)
		{
			// 落下中ようのアニメーションを再生する
			Player_PlayAnim(3);
		}

		// 移動ベクトルのＹ成分をＹ軸方向の速度にする
		MoveVec.y = pl.JumpPower;
	}

	// プレイヤーの移動方向にモデルの方向を近づける
	Player_AngleProcess();

	// 移動ベクトルを元にコリジョンを考慮しつつプレイヤーを移動
	Player_Move(MoveVec);

	// アニメーション処理
	Player_AnimProcess();
}

// プレイヤーの移動処理
void Player_Move(VECTOR MoveVector)
{
	int i, j, k;						// 汎用カウンタ変数
	int MoveFlag;						// 水平方向に移動したかどうかのフラグ( ０:移動していない  １:移動した )
	int HitFlag;						// ポリゴンに当たったかどうかを記憶しておくのに使う変数( ０:当たっていない  １:当たった )
	MV1_COLL_RESULT_POLY_DIM HitDim;			// プレイヤーの周囲にあるポリゴンを検出した結果が代入される当たり判定結果構造体
	int KabeNum;						// 壁ポリゴンと判断されたポリゴンの数
	int YukaNum;						// 床ポリゴンと判断されたポリゴンの数
	MV1_COLL_RESULT_POLY *Kabe[PLAYER_MAX_HITCOLL];	// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY *Yuka[PLAYER_MAX_HITCOLL];	// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY *Poly;				// ポリゴンの構造体にアクセスするために使用するポインタ( 使わなくても済ませられますがプログラムが長くなるので・・・ )
	HITRESULT_LINE LineRes;				// 線分とポリゴンとの当たり判定の結果を代入する構造体
	VECTOR OldPos;						// 移動前の座標	
	VECTOR NowPos;						// 移動後の座標

	// 移動前の座標を保存
	OldPos = pl.Position;

	// 移動後の座標を算出
	NowPos = VAdd(pl.Position, MoveVector);

	// プレイヤーの周囲にあるステージポリゴンを取得する
	// ( 検出する範囲は移動距離も考慮する )
	HitDim = MV1CollCheck_Sphere(stg.ModelHandle, -1, pl.Position, PLAYER_ENUM_DEFAULT_SIZE + VSize(MoveVector));

	// x軸かy軸方向に 0.01f 以上移動した場合は「移動した」フラグを１にする
	if (fabs(MoveVector.x) > 0.01f || fabs(MoveVector.z) > 0.01f)
	{
		MoveFlag = 1;
	}
	else
	{
		MoveFlag = 0;
	}

	// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
	{
		// 壁ポリゴンと床ポリゴンの数を初期化する
		KabeNum = 0;
		YukaNum = 0;

		// 検出されたポリゴンの数だけ繰り返し
		for (i = 0; i < HitDim.HitNum; i++)
		{
			// ＸＺ平面に垂直かどうかはポリゴンの法線のＹ成分が０に限りなく近いかどうかで判断する
			if (HitDim.Dim[i].Normal.y < 0.000001f && HitDim.Dim[i].Normal.y > -0.000001f)
			{
				// 壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋１．０ｆより高いポリゴンのみ当たり判定を行う
				if (HitDim.Dim[i].Position[0].y > pl.Position.y + 1.0f ||
					HitDim.Dim[i].Position[1].y > pl.Position.y + 1.0f ||
					HitDim.Dim[i].Position[2].y > pl.Position.y + 1.0f)
				{
					// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
					if (KabeNum < PLAYER_MAX_HITCOLL)
					{
						// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
						Kabe[KabeNum] = &HitDim.Dim[i];

						// 壁ポリゴンの数を加算する
						KabeNum++;
					}
				}
			}
			else
			{
				// ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
				if (YukaNum < PLAYER_MAX_HITCOLL)
				{
					// ポリゴンの構造体のアドレスを床ポリゴンポインタ配列に保存する
					Yuka[YukaNum] = &HitDim.Dim[i];

					// 床ポリゴンの数を加算する
					YukaNum++;
				}
			}
		}
	}

	// 壁ポリゴンとの当たり判定処理
	if (KabeNum != 0)
	{
		// 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
		HitFlag = 0;

		// 移動したかどうかで処理を分岐
		if (MoveFlag == 1)
		{
			// 壁ポリゴンの数だけ繰り返し
			for (i = 0; i < KabeNum; i++)
			{
				// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				Poly = Kabe[i];

				// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
				if (HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), PLAYER_HIT_WIDTH, Poly->Position[0], Poly->Position[1], Poly->Position[2]) == FALSE) continue;

				// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
				HitFlag = 1;

				// 壁に当たったら壁に遮られない移動成分分だけ移動する
				{
					VECTOR SlideVec;	// プレイヤーをスライドさせるベクトル

					// 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
					SlideVec = VCross(MoveVector, Poly->Normal);

					// 算出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出、これが
					// 元の移動成分から壁方向の移動成分を抜いたベクトル
					SlideVec = VCross(Poly->Normal, SlideVec);

					// それを移動前の座標に足したものを新たな座標とする
					NowPos = VAdd(OldPos, SlideVec);
				}

				// 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
				for (j = 0; j < KabeNum; j++)
				{
					// j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					Poly = Kabe[j];

					// 当たっていたらループから抜ける
					if (HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), PLAYER_HIT_WIDTH, Poly->Position[0], Poly->Position[1], Poly->Position[2]) == TRUE) break;
				}

				// j が KabeNum だった場合はどのポリゴンとも当たらなかったということなので
				// 壁に当たったフラグを倒した上でループから抜ける
				if (j == KabeNum)
				{
					HitFlag = 0;
					break;
				}
			}
		}
		else
		{
			// 移動していない場合の処理

			// 壁ポリゴンの数だけ繰り返し
			for (i = 0; i < KabeNum; i++)
			{
				// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				Poly = Kabe[i];

				// ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
				if (HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), PLAYER_HIT_WIDTH, Poly->Position[0], Poly->Position[1], Poly->Position[2]) == TRUE)
				{
					HitFlag = 1;
					break;
				}
			}
		}

		// 壁に当たっていたら壁から押し出す処理を行う
		if (HitFlag == 1)
		{
			// 壁からの押し出し処理を試みる最大数だけ繰り返し
			for (k = 0; k < PLAYER_HIT_TRYNUM; k++)
			{
				// 壁ポリゴンの数だけ繰り返し
				for (i = 0; i < KabeNum; i++)
				{
					// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
					Poly = Kabe[i];

					// プレイヤーと当たっているかを判定
					if (HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), PLAYER_HIT_WIDTH, Poly->Position[0], Poly->Position[1], Poly->Position[2]) == FALSE) continue;

					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
					NowPos = VAdd(NowPos, VScale(Poly->Normal, PLAYER_HIT_SLIDE_LENGTH));

					// 移動した上で壁ポリゴンと接触しているかどうかを判定
					for (j = 0; j < KabeNum; j++)
					{
						// 当たっていたらループを抜ける
						Poly = Kabe[j];
						if (HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), PLAYER_HIT_WIDTH, Poly->Position[0], Poly->Position[1], Poly->Position[2]) == TRUE) break;
					}

					// 全てのポリゴンと当たっていなかったらここでループ終了
					if (j == KabeNum) break;
				}

				// i が KabeNum ではない場合は全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
				if (i != KabeNum) break;
			}
		}
	}

	// 床ポリゴンとの当たり判定
	if (YukaNum != 0)
	{
		// ジャンプ中且つ上昇中の場合は処理を分岐
		if (pl.State == 2 && pl.JumpPower > 0.0f)
		{
			float MinY;

			// 天井に頭をぶつける処理を行う

			// 一番低い天井にぶつける為の判定用変数を初期化
			MinY = 0.0f;

			// 当たったかどうかのフラグを当たっていないを意味する０にしておく
			HitFlag = 0;

			// 床ポリゴンの数だけ繰り返し
			for (i = 0; i < YukaNum; i++)
			{
				// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				Poly = Yuka[i];

				// 足先から頭の高さまでの間でポリゴンと接触しているかどうかを判定
				LineRes = HitCheck_Line_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), Poly->Position[0], Poly->Position[1], Poly->Position[2]);

				// 接触していなかったら何もしない
				if (LineRes.HitFlag == FALSE) continue;

				// 既にポリゴンに当たっていて、且つ今まで検出した天井ポリゴンより高い場合は何もしない
				if (HitFlag == 1 && MinY < LineRes.Position.y) continue;

				// ポリゴンに当たったフラグを立てる
				HitFlag = 1;

				// 接触したＹ座標を保存する
				MinY = LineRes.Position.y;
			}

			// 接触したポリゴンがあったかどうかで処理を分岐
			if (HitFlag == 1)
			{
				// 接触した場合はプレイヤーのＹ座標を接触座標を元に更新
				NowPos.y = MinY - PLAYER_HIT_HEIGHT;

				// Ｙ軸方向の速度は反転
				pl.JumpPower = -pl.JumpPower;
			}
		}
		else
		{
			float MaxY;

			// 下降中かジャンプ中ではない場合の処理

			// 床ポリゴンに当たったかどうかのフラグを倒しておく
			HitFlag = 0;

			// 一番高い床ポリゴンにぶつける為の判定用変数を初期化
			MaxY = 0.0f;

			// 床ポリゴンの数だけ繰り返し
			for (i = 0; i < YukaNum; i++)
			{
				// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
				Poly = Yuka[i];

				// ジャンプ中かどうかで処理を分岐
				if (pl.State == 2)
				{
					// ジャンプ中の場合は頭の先から足先より少し低い位置の間で当たっているかを判定
					LineRes = HitCheck_Line_Triangle(VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), VAdd(NowPos, VGet(0.0f, -1.0f, 0.0f)), Poly->Position[0], Poly->Position[1], Poly->Position[2]);
				}
				else
				{
					// 走っている場合は頭の先からそこそこ低い位置の間で当たっているかを判定( 傾斜で落下状態に移行してしまわない為 )
					LineRes = HitCheck_Line_Triangle(VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), VAdd(NowPos, VGet(0.0f, -40.0f, 0.0f)), Poly->Position[0], Poly->Position[1], Poly->Position[2]);
				}

				// 当たっていなかったら何もしない
				if (LineRes.HitFlag == FALSE) continue;

				// 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
				if (HitFlag == 1 && MaxY > LineRes.Position.y) continue;

				// ポリゴンに当たったフラグを立てる
				HitFlag = 1;

				// 接触したＹ座標を保存する
				MaxY = LineRes.Position.y;
			}

			// 床ポリゴンに当たったかどうかで処理を分岐
			if (HitFlag == 1)
			{
				// 当たった場合

				// 接触したポリゴンで一番高いＹ座標をプレイヤーのＹ座標にする
				NowPos.y = MaxY;

				// Ｙ軸方向の移動速度は０に
				pl.JumpPower = 0.0f;

				// もしジャンプ中だった場合は着地状態にする
				if (pl.State == 2)
				{
					// 移動していたかどうかで着地後の状態と再生するアニメーションを分岐する
					if (MoveFlag)
					{
						// 移動している場合は走り状態に
						Player_PlayAnim(1);
						pl.State = 1;
					}
					else
					{
						// 移動していない場合は立ち止り状態に
						Player_PlayAnim(4);
						pl.State = 0;
					}

					// 着地時はアニメーションのブレンドは行わない
					pl.AnimBlendRate = 1.0f;
				}
			}
			else
			{
				// 床コリジョンに当たっていなくて且つジャンプ状態ではなかった場合は
				if (pl.State != 2)
				{
					// ジャンプ中にする
					pl.State = 2;

					// ちょっとだけジャンプする
					pl.JumpPower = PLAYER_FALL_UP_POWER;

					// アニメーションは落下中のものにする
					Player_PlayAnim(3);
				}
			}
		}
	}

	// 新しい座標を保存する
	pl.Position = NowPos;

	// プレイヤーのモデルの座標を更新する
	MV1SetPosition(pl.ModelHandle, pl.Position);

	// 検出したプレイヤーの周囲のポリゴン情報を開放する
	MV1CollResultPolyDimTerminate(HitDim);
}

// プレイヤーの向きを変える処理
void Player_AngleProcess(void)
{
	float TargetAngle;			// 目標角度
	float SaAngle;				// 目標角度と現在の角度との差

	// 目標の方向ベクトルから角度値を算出する
	TargetAngle = atan2(pl.TargetMoveDirection.x, pl.TargetMoveDirection.z);

	// 目標の角度と現在の角度との差を割り出す
	{
		// 最初は単純に引き算
		SaAngle = TargetAngle - pl.Angle;

		// ある方向からある方向の差が１８０度以上になることは無いので
		// 差の値が１８０度以上になっていたら修正する
		if (SaAngle < -DX_PI_F)
		{
			SaAngle += DX_TWO_PI_F;
		}
		else
		if (SaAngle > DX_PI_F)
		{
			SaAngle -= DX_TWO_PI_F;
		}
	}

	// 角度の差が０に近づける
	if (SaAngle > 0.0f)
	{
		// 差がプラスの場合は引く
		SaAngle -= PLAYER_ANGLE_SPEED;
		if (SaAngle < 0.0f)
		{
			SaAngle = 0.0f;
		}
	}
	else
	{
		// 差がマイナスの場合は足す
		SaAngle += PLAYER_ANGLE_SPEED;
		if (SaAngle > 0.0f)
		{
			SaAngle = 0.0f;
		}
	}

	// モデルの角度を更新
	pl.Angle = TargetAngle - SaAngle;
	MV1SetRotationXYZ(pl.ModelHandle, VGet(0.0f, pl.Angle + DX_PI_F, 0.0f));
}

// プレイヤーに新たなアニメーションを再生する
void Player_PlayAnim(int PlayAnim)
{
	// 再生中のモーション２が有効だったらデタッチする
	if (pl.PlayAnim2 != -1)
	{
		MV1DetachAnim(pl.ModelHandle, pl.PlayAnim2);
		pl.PlayAnim2 = -1;
	}

	// 今まで再生中のモーション１だったものの情報を２に移動する
	pl.PlayAnim2 = pl.PlayAnim1;
	pl.AnimPlayCount2 = pl.AnimPlayCount1;

	// 新たに指定のモーションをモデルにアタッチして、アタッチ番号を保存する
	pl.PlayAnim1 = MV1AttachAnim(pl.ModelHandle, PlayAnim);
	pl.AnimPlayCount1 = 0.0f;

	// ブレンド率は再生中のモーション２が有効ではない場合は１．０ｆ( 再生中のモーション１が１００％の状態 )にする
	pl.AnimBlendRate = pl.PlayAnim2 == -1 ? 1.0f : 0.0f;
}

// プレイヤーのアニメーション処理
void Player_AnimProcess(void)
{
	float AnimTotalTime;		// 再生しているアニメーションの総時間

	// ブレンド率が１以下の場合は１に近づける
	if (pl.AnimBlendRate < 1.0f)
	{
		pl.AnimBlendRate += PLAYER_ANIM_BLEND_SPEED;
		if (pl.AnimBlendRate > 1.0f)
		{
			pl.AnimBlendRate = 1.0f;
		}
	}

	// 再生しているアニメーション１の処理
	if (pl.PlayAnim1 != -1)
	{
		// アニメーションの総時間を取得
		AnimTotalTime = MV1GetAttachAnimTotalTime(pl.ModelHandle, pl.PlayAnim1);

		// 再生時間を進める
		pl.AnimPlayCount1 += PLAYER_PLAY_ANIM_SPEED;

		// 再生時間が総時間に到達していたら再生時間をループさせる
		if (pl.AnimPlayCount1 >= AnimTotalTime)
		{
			pl.AnimPlayCount1 = fmod(pl.AnimPlayCount1, AnimTotalTime);
		}

		// 変更した再生時間をモデルに反映させる
		MV1SetAttachAnimTime(pl.ModelHandle, pl.PlayAnim1, pl.AnimPlayCount1);

		// アニメーション１のモデルに対する反映率をセット
		MV1SetAttachAnimBlendRate(pl.ModelHandle, pl.PlayAnim1, pl.AnimBlendRate);
	}

	// 再生しているアニメーション２の処理
	if (pl.PlayAnim2 != -1)
	{
		// アニメーションの総時間を取得
		AnimTotalTime = MV1GetAttachAnimTotalTime(pl.ModelHandle, pl.PlayAnim2);

		// 再生時間を進める
		pl.AnimPlayCount2 += PLAYER_PLAY_ANIM_SPEED;

		// 再生時間が総時間に到達していたら再生時間をループさせる
		if (pl.AnimPlayCount2 > AnimTotalTime)
		{
			pl.AnimPlayCount2 = fmod(pl.AnimPlayCount2, AnimTotalTime);
		}

		// 変更した再生時間をモデルに反映させる
		MV1SetAttachAnimTime(pl.ModelHandle, pl.PlayAnim2, pl.AnimPlayCount2);

		// アニメーション２のモデルに対する反映率をセット
		MV1SetAttachAnimBlendRate(pl.ModelHandle, pl.PlayAnim2, 1.0f - pl.AnimBlendRate);
	}
}

// プレイヤーの影を描画
void Player_ShadowRender(void)
{
	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY *HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;

	// ライティングを無効にする
	SetUseLighting(FALSE);

	// Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	// テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// プレイヤーの直下に存在する地面のポリゴンを取得
	HitResDim = MV1CollCheck_Capsule(stg.ModelHandle, -1, pl.Position, VAdd(pl.Position, VGet(0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f)), PLAYER_SHADOW_SIZE);

	// 頂点データで変化が無い部分をセット
	Vertex[0].dif = GetColorU8(255, 255, 255, 255);
	Vertex[0].spc = GetColorU8(0, 0, 0, 0);
	Vertex[0].su = 0.0f;
	Vertex[0].sv = 0.0f;
	Vertex[1] = Vertex[0];
	Vertex[2] = Vertex[0];

	// 球の直下に存在するポリゴンの数だけ繰り返し
	HitRes = HitResDim.Dim;
	for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
	{
		// ポリゴンの座標は地面ポリゴンの座標
		Vertex[0].pos = HitRes->Position[0];
		Vertex[1].pos = HitRes->Position[1];
		Vertex[2].pos = HitRes->Position[2];

		// ちょっと持ち上げて重ならないようにする
		SlideVec = VScale(HitRes->Normal, 0.5f);
		Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
		Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
		Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

		// ポリゴンの不透明度を設定する
		Vertex[0].dif.a = 0;
		Vertex[1].dif.a = 0;
		Vertex[2].dif.a = 0;
		if (HitRes->Position[0].y > pl.Position.y - PLAYER_SHADOW_HEIGHT)
			Vertex[0].dif.a = 128 * (1.0f - fabs(HitRes->Position[0].y - pl.Position.y) / PLAYER_SHADOW_HEIGHT);

		if (HitRes->Position[1].y > pl.Position.y - PLAYER_SHADOW_HEIGHT)
			Vertex[1].dif.a = 128 * (1.0f - fabs(HitRes->Position[1].y - pl.Position.y) / PLAYER_SHADOW_HEIGHT);

		if (HitRes->Position[2].y > pl.Position.y - PLAYER_SHADOW_HEIGHT)
			Vertex[2].dif.a = 128 * (1.0f - fabs(HitRes->Position[2].y - pl.Position.y) / PLAYER_SHADOW_HEIGHT);

		// ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
		Vertex[0].u = (HitRes->Position[0].x - pl.Position.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[0].v = (HitRes->Position[0].z - pl.Position.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[1].u = (HitRes->Position[1].x - pl.Position.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[1].v = (HitRes->Position[1].z - pl.Position.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[2].u = (HitRes->Position[2].x - pl.Position.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[2].v = (HitRes->Position[2].z - pl.Position.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

		// 影ポリゴンを描画
		DrawPolygon3D(Vertex, 1, pl.ShadowHandle, TRUE);
	}

	// 検出した地面ポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(HitResDim);

	// ライティングを有効にする
	SetUseLighting(TRUE);

	// Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);
}

// ステージの初期化処理
void Stage_Initialize(void)
{
	// ステージモデルの読み込み
	stg.ModelHandle = MV1LoadModel("ColTestStage.mqo");

	// モデル全体のコリジョン情報のセットアップ
	MV1SetupCollInfo(stg.ModelHandle, -1);
}

// ステージの後始末処理
void Stage_Terminate(void)
{
	// ステージモデルの後始末
	MV1DeleteModel(stg.ModelHandle);
}

// カメラの初期化処理
void Camera_Initialize(void)
{
	// カメラの初期水平角度は１８０度
	cam.AngleH = DX_PI_F;

	// 垂直角度は０度
	cam.AngleV = 0.0f;
}

// カメラの処理
void Camera_Process(void)
{
	// パッドの３ボタンか、シフトキーが押されている場合のみ角度変更操作を行う
	if (CheckHitKey(KEY_INPUT_LSHIFT) || (inp.NowInput & PAD_INPUT_C))
	{
		// 「←」ボタンが押されていたら水平角度をマイナスする
		if (inp.NowInput & PAD_INPUT_LEFT)
		{
			cam.AngleH -= CAMERA_ANGLE_SPEED;

			// −１８０度以下になったら角度値が大きくなりすぎないように３６０度を足す
			if (cam.AngleH < -DX_PI_F)
			{
				cam.AngleH += DX_TWO_PI_F;
			}
		}

		// 「→」ボタンが押されていたら水平角度をプラスする
		if (inp.NowInput & PAD_INPUT_RIGHT)
		{
			cam.AngleH += CAMERA_ANGLE_SPEED;

			// １８０度以上になったら角度値が大きくなりすぎないように３６０度を引く
			if (cam.AngleH > DX_PI_F)
			{
				cam.AngleH -= DX_TWO_PI_F;
			}
		}

		// 「↑」ボタンが押されていたら垂直角度をマイナスする
		if (inp.NowInput & PAD_INPUT_UP)
		{
			cam.AngleV -= CAMERA_ANGLE_SPEED;

			// ある一定角度以下にはならないようにする
			if (cam.AngleV < -DX_PI_F / 2.0f + 0.6f)
			{
				cam.AngleV = -DX_PI_F / 2.0f + 0.6f;
			}
		}

		// 「↓」ボタンが押されていたら垂直角度をプラスする
		if (inp.NowInput & PAD_INPUT_DOWN)
		{
			cam.AngleV += CAMERA_ANGLE_SPEED;

			// ある一定角度以上にはならないようにする
			if (cam.AngleV > DX_PI_F / 2.0f - 0.6f)
			{
				cam.AngleV = DX_PI_F / 2.0f - 0.6f;
			}
		}
	}

	// カメラの注視点はプレイヤー座標から規定値分高い座標
	cam.Target = VAdd(pl.Position, VGet(0.0f, CAMERA_PLAYER_TARGET_HEIGHT, 0.0f));

	// カメラの座標を決定する
	{
		MATRIX RotZ, RotY;
		float Camera_Player_Length;
		MV1_COLL_RESULT_POLY_DIM HRes;
		int HitNum;

		// 水平方向の回転はＹ軸回転
		RotY = MGetRotY(cam.AngleH);

		// 垂直方向の回転はＺ軸回転 )
		RotZ = MGetRotZ(cam.AngleV);

		// カメラからプレイヤーまでの初期距離をセット
		Camera_Player_Length = CAMERA_PLAYER_LENGTH;

		// カメラの座標を算出
		// Ｘ軸にカメラとプレイヤーとの距離分だけ伸びたベクトルを
		// 垂直方向回転( Ｚ軸回転 )させたあと水平方向回転( Ｙ軸回転 )して更に
		// 注視点の座標を足したものがカメラの座標
		cam.Eye = VAdd(VTransform(VTransform(VGet(-Camera_Player_Length, 0.0f, 0.0f), RotZ), RotY), cam.Target);

		// 注視点からカメラの座標までの間にステージのポリゴンがあるか調べる
		HRes = MV1CollCheck_Capsule(stg.ModelHandle, -1, cam.Target, cam.Eye, CAMERA_COLLISION_SIZE);
		HitNum = HRes.HitNum;
		MV1CollResultPolyDimTerminate(HRes);
		if (HitNum != 0)
		{
			float NotHitLength;
			float HitLength;
			float TestLength;
			VECTOR TestPosition;

			// あったら無い位置までプレイヤーに近づく

			// ポリゴンに当たらない距離をセット
			NotHitLength = 0.0f;

			// ポリゴンに当たる距離をセット
			HitLength = Camera_Player_Length;
			do
			{
				// 当たるかどうかテストする距離をセット( 当たらない距離と当たる距離の中間 )
				TestLength = NotHitLength + (HitLength - NotHitLength) / 2.0f;

				// テスト用のカメラ座標を算出
				TestPosition = VAdd(VTransform(VTransform(VGet(-TestLength, 0.0f, 0.0f), RotZ), RotY), cam.Target);

				// 新しい座標で壁に当たるかテスト
				HRes = MV1CollCheck_Capsule(stg.ModelHandle, -1, cam.Target, TestPosition, CAMERA_COLLISION_SIZE);
				HitNum = HRes.HitNum;
				MV1CollResultPolyDimTerminate(HRes);
				if (HitNum != 0)
				{
					// 当たったら当たる距離を TestLength に変更する
					HitLength = TestLength;
				}
				else
				{
					// 当たらなかったら当たらない距離を TestLength に変更する
					NotHitLength = TestLength;
				}

				// HitLength と NoHitLength が十分に近づいていなかったらループ
			} while (HitLength - NotHitLength > 0.1f);

			// カメラの座標をセット
			cam.Eye = TestPosition;
		}
	}

	// カメラの情報をライブラリのカメラに反映させる
	SetCameraPositionAndTarget_UpVecY(cam.Eye, cam.Target);
}

// 描画処理
void Render_Process(void)
{
	// ステージモデルの描画
	MV1DrawModel(stg.ModelHandle);

	// プレイヤーモデルの描画
	MV1DrawModel(pl.ModelHandle);

	// プレイヤーの影の描画
	Player_ShadowRender();
}

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// ウインドウモードで起動
	ChangeWindowMode(TRUE);

	// ライブラリの初期化
	if (DxLib_Init() < 0)
		return -1;

	// プレイヤーの初期化
	Player_Initialize();

	// ステージの初期化
	Stage_Initialize();

	// カメラの初期化
	Camera_Initialize();

	// 描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

	// ＥＳＣキーが押されるか、ウインドウが閉じられるまでループ
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		// 画面をクリア
		ClearDrawScreen();

		// 入力処理
		Input_Process();

		// プレイヤーの処理
		Player_Process();

		// カメラの処理
		Camera_Process();

		// 描画処理
		Render_Process();

		// 裏画面の内容を表画面に反映
		ScreenFlip();
	}

	// プレイヤーの後始末
	Player_Terminate();

	// ステージの後始末
	Stage_Terminate();

	// ライブラリの後始末
	DxLib_End();

	// ソフト終了
	return 0;
};

int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

