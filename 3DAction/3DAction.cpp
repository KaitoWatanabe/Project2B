// 3DAction.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
// �R�c�A�N�V������{

#include "DxLib.h"
#include <math.h>

// �֐��v���g�^�C�v�錾 ---------------------------------------------------------------------------

void Input_Process(void);			// ���͏���

void Player_Initialize(void);		// �v���C���[�̏�����
void Player_Terminate(void);			// �v���C���[�̌�n��
void Player_Process(void);			// �v���C���[�̏���
void Player_Move(VECTOR MoveVector);		// �v���C���[�̈ړ�����
void Player_AngleProcess(void);		// �v���C���[�̌�����ς��鏈��
void Player_PlayAnim(int PlayAnim);		// �v���C���[�ɐV���ȃA�j���[�V�������Đ�����
void Player_AnimProcess(void);		// �v���C���[�̃A�j���[�V��������
void Player_ShadowRender(void);		// �v���C���[�̉e��`��

void Stage_Initialize(void);			// �X�e�[�W�̏���������
void Stage_Terminate(void);			// �X�e�[�W�̌�n������

void Camera_Initialize(void);		// �J�����̏���������
void Camera_Process(void);			// �J�����̏���

void Render_Process(void);			// �`�揈��



// �Œ�l��` -------------------------------------------------------------------------------------

// �v���C���[�֌W�̒�`
#define PLAYER_PLAY_ANIM_SPEED			250.0f		// �A�j���[�V�������x
#define PLAYER_MOVE_SPEED			30.0f		// �ړ����x
#define PLAYER_ANIM_BLEND_SPEED			0.1f		// �A�j���[�V�����̃u�����h���ω����x
#define PLAYER_ANGLE_SPEED			0.2f		// �p�x�ω����x
#define PLAYER_JUMP_POWER			100.0f		// �W�����v��
#define PLAYER_FALL_UP_POWER			20.0f		// ���𓥂݊O�������̃W�����v��
#define PLAYER_GRAVITY				3.0f		// �d��
#define PLAYER_MAX_HITCOLL			2048		// ��������R���W�����|���S���̍ő吔
#define PLAYER_ENUM_DEFAULT_SIZE		800.0f		// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_HIT_WIDTH			200.0f		// �����蔻��J�v�Z���̔��a
#define PLAYER_HIT_HEIGHT			700.0f		// �����蔻��J�v�Z���̍���
#define PLAYER_HIT_TRYNUM			16		// �ǉ����o�������̍ő厎�s��
#define PLAYER_HIT_SLIDE_LENGTH			5.0f		// ��x�̕ǉ����o�������ŃX���C�h�����鋗��
#define PLAYER_SHADOW_SIZE			200.0f		// �e�̑傫��
#define PLAYER_SHADOW_HEIGHT			700.0f		// �e�������鍂��

// �J�����֌W�̒�`
#define CAMERA_ANGLE_SPEED			0.05f		// ���񑬓x
#define CAMERA_PLAYER_TARGET_HEIGHT		400.0f		// �v���C���[���W����ǂꂾ�������ʒu�𒍎��_�Ƃ��邩
#define CAMERA_PLAYER_LENGTH			1600.0f		// �v���C���[�Ƃ̋���
#define CAMERA_COLLISION_SIZE			50.0f		// �J�����̓����蔻��T�C�Y



// �\���̒�` -------------------------------------------------------------------------------------

// ���͏��\����
struct PADINPUT
{
	int		NowInput;				// ���݂̓���
	int		EdgeInput;				// ���݂̃t���[���ŉ����ꂽ�{�^���̂݃r�b�g�������Ă�����͒l
};

// �v���C���[���\����
struct PLAYER
{
	VECTOR		Position;				// ���W
	VECTOR		TargetMoveDirection;			// ���f���������ׂ������̃x�N�g��
	float		Angle;					// ���f���������Ă�������̊p�x
	float		JumpPower;				// �x�������̑��x
	int		ModelHandle;				// ���f���n���h��
	int		ShadowHandle;				// �e�摜�n���h��
	int		State;					// ���( 0:�����~�܂�  1:����  2:�W�����v )

	int		PlayAnim1;				// �Đ����Ă���A�j���[�V�����P�̃A�^�b�`�ԍ�( -1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ� )
	float		AnimPlayCount1;			// �Đ����Ă���A�j���[�V�����P�̍Đ�����
	int		PlayAnim2;				// �Đ����Ă���A�j���[�V�����Q�̃A�^�b�`�ԍ�( -1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ� )
	float		AnimPlayCount2;			// �Đ����Ă���A�j���[�V�����Q�̍Đ�����
	float		AnimBlendRate;				// �Đ����Ă���A�j���[�V�����P�ƂQ�̃u�����h��
};

// �X�e�[�W���\����
struct STAGE
{
	int		ModelHandle;				// ���f���n���h��
};

// �J�������\����
struct CAMERA
{
	float		AngleH;				// �����p�x
	float		AngleV;				// �����p�x
	VECTOR		Eye;					// �J�������W
	VECTOR		Target;				// �����_���W
};



// ���̐錾 ---------------------------------------------------------------------------------------

PADINPUT inp;		// ���͏��̎��̐錾
PLAYER pl;		// �v���C���[���̎��̐錾
STAGE stg;		// �X�e�[�W���̎��̐錾
CAMERA cam;		// �J�������̎��̐錾




// �֐����� ---------------------------------------------------------------------------------------

// ���͏���
void Input_Process(void)
{
	int Old;

	// �ЂƂO�̃t���[���̓��͂�ϐ��ɂƂ��Ă���
	Old = inp.NowInput;

	// ���݂̓��͏�Ԃ��擾
	inp.NowInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);

	// ���̃t���[���ŐV���ɉ����ꂽ�{�^���̃r�b�g���������Ă���l�� EdgeInput �ɑ������
	inp.EdgeInput = inp.NowInput & ~Old;
}

// �v���C���[�̏�����
void Player_Initialize(void)
{
	// �������W�͌��_
	pl.Position = VGet(0.0f, 0.0f, 0.0f);

	// ��]�l�͂O
	pl.Angle = 0.0f;

	// �W�����v�͂͏�����Ԃł͂O
	pl.JumpPower = 0.0f;

	// ���f���̓ǂݍ���
	pl.ModelHandle = MV1LoadModel("DxChara.x");

	// �e�`��p�̉摜�̓ǂݍ���
	pl.ShadowHandle = LoadGraph("Shadow.tga");

	// ������Ԃł́u�����~��v���
	pl.State = 0;

	// ������ԂŃv���C���[�������ׂ������͂w������
	pl.TargetMoveDirection = VGet(1.0f, 0.0f, 0.0f);

	// �A�j���[�V�����̃u�����h����������
	pl.AnimBlendRate = 1.0f;

	// ������Ԃł̓A�j���[�V�����̓A�^�b�`����Ă��Ȃ��ɂ���
	pl.PlayAnim1 = -1;
	pl.PlayAnim2 = -1;

	// ���������Ă���A�j���[�V�������Đ�
	Player_PlayAnim(4);
}

// �v���C���[�̌�n��
void Player_Terminate(void)
{
	// ���f���̍폜
	MV1DeleteModel(pl.ModelHandle);

	// �e�p�摜�̍폜
	DeleteGraph(pl.ShadowHandle);
}

// �v���C���[�̏���
void Player_Process(void)
{
	VECTOR UpMoveVec;		// �����{�^���u���v����͂������Ƃ��̃v���C���[�̈ړ������x�N�g��
	VECTOR LeftMoveVec;		// �����{�^���u���v����͂������Ƃ��̃v���C���[�̈ړ������x�N�g��
	VECTOR MoveVec;		// ���̃t���[���̈ړ��x�N�g��
	int MoveFlag;			// �ړ��������ǂ����̃t���O( 1:�ړ�����  0:�ړ����Ă��Ȃ� )

	// ���[�g�t���[���̂y�������̈ړ��p�����[�^�𖳌��ɂ���
	{
		MATRIX LocalMatrix;

		// ���[�U�[�s�����������
		MV1ResetFrameUserLocalMatrix(pl.ModelHandle, 2);

		// ���݂̃��[�g�t���[���̍s����擾����
		LocalMatrix = MV1GetFrameLocalMatrix(pl.ModelHandle, 2);

		// �y�������̕��s�ړ������𖳌��ɂ���
		LocalMatrix.m[3][2] = 0.0f;

		// ���[�U�[�s��Ƃ��ĕ��s�ړ������𖳌��ɂ����s������[�g�t���[���ɃZ�b�g����
		MV1SetFrameUserLocalMatrix(pl.ModelHandle, 2, LocalMatrix);
	}

	// �v���C���[�̈ړ������̃x�N�g�����Z�o
	{
		// �����{�^���u���v���������Ƃ��̃v���C���[�̈ړ��x�N�g���̓J�����̎�����������x�����𔲂�������
		UpMoveVec = VSub(cam.Target, cam.Eye);
		UpMoveVec.y = 0.0f;

		// �����{�^���u���v���������Ƃ��̃v���C���[�̈ړ��x�N�g���͏���������Ƃ��̕����x�N�g���Ƃx���̃v���X�����̃x�N�g���ɐ����ȕ���
		LeftMoveVec = VCross(UpMoveVec, VGet(0.0f, 1.0f, 0.0f));

		// ��̃x�N�g���𐳋K��( �x�N�g���̒������P�D�O�ɂ��邱�� )
		UpMoveVec = VNorm(UpMoveVec);
		LeftMoveVec = VNorm(LeftMoveVec);
	}

	// ���̃t���[���ł̈ړ��x�N�g����������
	MoveVec = VGet(0.0f, 0.0f, 0.0f);

	// �ړ��������ǂ����̃t���O��������Ԃł́u�ړ����Ă��Ȃ��v��\���O�ɂ���
	MoveFlag = 0;

	// �p�b�h�̂R�{�^���ƍ��V�t�g���ǂ����������Ă��Ȃ�������v���C���[�̈ړ�����
	if (CheckHitKey(KEY_INPUT_LSHIFT) == 0 && (inp.NowInput & PAD_INPUT_C) == 0)
	{
		// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă���������猩�č������Ɉړ�����
		if (inp.NowInput & PAD_INPUT_LEFT)
		{
			// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g�������Z����
			MoveVec = VAdd(MoveVec, LeftMoveVec);

			// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
			MoveFlag = 1;
		}
		else
			// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă���������猩�ĉE�����Ɉړ�����
		if (inp.NowInput & PAD_INPUT_RIGHT)
		{
			// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g���𔽓]�������̂����Z����
			MoveVec = VAdd(MoveVec, VScale(LeftMoveVec, -1.0f));

			// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
			MoveFlag = 1;
		}

		// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă�������Ɉړ�����
		if (inp.NowInput & PAD_INPUT_UP)
		{
			// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g�������Z����
			MoveVec = VAdd(MoveVec, UpMoveVec);

			// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
			MoveFlag = 1;
		}
		else
			// �����{�^���u���v�����͂��ꂽ��J�����̕����Ɉړ�����
		if (inp.NowInput & PAD_INPUT_DOWN)
		{
			// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g���𔽓]�������̂����Z����
			MoveVec = VAdd(MoveVec, VScale(UpMoveVec, -1.0f));

			// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
			MoveFlag = 1;
		}

		// �v���C���[�̏�Ԃ��u�W�����v�v�ł͂Ȃ��A���{�^���P��������Ă�����W�����v����
		if (pl.State != 2 && (inp.EdgeInput & PAD_INPUT_A))
		{
			// ��Ԃ��u�W�����v�v�ɂ���
			pl.State = 2;

			// �x�������̑��x���Z�b�g
			pl.JumpPower = PLAYER_JUMP_POWER;

			// �W�����v�A�j���[�V�����̍Đ�
			Player_PlayAnim(2);
		}
	}

	// �ړ��{�^���������ꂽ���ǂ����ŏ����𕪊�
	if (MoveFlag)
	{
		// �ړ��x�N�g���𐳋K���������̂��v���C���[�������ׂ������Ƃ��ĕۑ�
		pl.TargetMoveDirection = VNorm(MoveVec);

		// �v���C���[�������ׂ������x�N�g�����v���C���[�̃X�s�[�h�{�������̂��ړ��x�N�g���Ƃ���
		MoveVec = VScale(pl.TargetMoveDirection, PLAYER_MOVE_SPEED);

		// �������܂Łu�����~�܂�v��Ԃ�������
		if (pl.State == 0)
		{
			// ����A�j���[�V�������Đ�����
			Player_PlayAnim(1);

			// ��Ԃ��u����v�ɂ���
			pl.State = 1;
		}
	}
	else
	{
		// ���̃t���[���ňړ����Ă��Ȃ��āA����Ԃ��u����v��������
		if (pl.State == 1)
		{
			// �����~��A�j���[�V�������Đ�����
			Player_PlayAnim(4);

			// ��Ԃ��u�����~��v�ɂ���
			pl.State = 0;
		}
	}

	// ��Ԃ��u�W�����v�v�̏ꍇ��
	if (pl.State == 2)
	{
		// �x�������̑��x���d�͕����Z����
		pl.JumpPower -= PLAYER_GRAVITY;

		// �����������Ă��Ċ��Đ�����Ă���A�j���[�V�������㏸���p�̂��̂������ꍇ��
		if (pl.JumpPower < 0.0f && MV1GetAttachAnim(pl.ModelHandle, pl.PlayAnim1) == 2)
		{
			// �������悤�̃A�j���[�V�������Đ�����
			Player_PlayAnim(3);
		}

		// �ړ��x�N�g���̂x�������x�������̑��x�ɂ���
		MoveVec.y = pl.JumpPower;
	}

	// �v���C���[�̈ړ������Ƀ��f���̕������߂Â���
	Player_AngleProcess();

	// �ړ��x�N�g�������ɃR���W�������l�����v���C���[���ړ�
	Player_Move(MoveVec);

	// �A�j���[�V��������
	Player_AnimProcess();
}

// �v���C���[�̈ړ�����
void Player_Move(VECTOR MoveVector)
{
	int i, j, k;						// �ėp�J�E���^�ϐ�
	int MoveFlag;						// ���������Ɉړ��������ǂ����̃t���O( �O:�ړ����Ă��Ȃ�  �P:�ړ����� )
	int HitFlag;						// �|���S���ɓ����������ǂ������L�����Ă����̂Ɏg���ϐ�( �O:�������Ă��Ȃ�  �P:�������� )
	MV1_COLL_RESULT_POLY_DIM HitDim;			// �v���C���[�̎��͂ɂ���|���S�������o�������ʂ��������铖���蔻�茋�ʍ\����
	int KabeNum;						// �ǃ|���S���Ɣ��f���ꂽ�|���S���̐�
	int YukaNum;						// ���|���S���Ɣ��f���ꂽ�|���S���̐�
	MV1_COLL_RESULT_POLY *Kabe[PLAYER_MAX_HITCOLL];	// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY *Yuka[PLAYER_MAX_HITCOLL];	// ���|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă������߂̃|�C���^�z��
	MV1_COLL_RESULT_POLY *Poly;				// �|���S���̍\���̂ɃA�N�Z�X���邽�߂Ɏg�p����|�C���^( �g��Ȃ��Ă��ς܂����܂����v���O�����������Ȃ�̂ŁE�E�E )
	HITRESULT_LINE LineRes;				// �����ƃ|���S���Ƃ̓����蔻��̌��ʂ�������\����
	VECTOR OldPos;						// �ړ��O�̍��W	
	VECTOR NowPos;						// �ړ���̍��W

	// �ړ��O�̍��W��ۑ�
	OldPos = pl.Position;

	// �ړ���̍��W���Z�o
	NowPos = VAdd(pl.Position, MoveVector);

	// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����
	// ( ���o����͈͈͂ړ��������l������ )
	HitDim = MV1CollCheck_Sphere(stg.ModelHandle, -1, pl.Position, PLAYER_ENUM_DEFAULT_SIZE + VSize(MoveVector));

	// x����y�������� 0.01f �ȏ�ړ������ꍇ�́u�ړ������v�t���O���P�ɂ���
	if (fabs(MoveVector.x) > 0.01f || fabs(MoveVector.z) > 0.01f)
	{
		MoveFlag = 1;
	}
	else
	{
		MoveFlag = 0;
	}

	// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
	{
		// �ǃ|���S���Ə��|���S���̐�������������
		KabeNum = 0;
		YukaNum = 0;

		// ���o���ꂽ�|���S���̐������J��Ԃ�
		for (i = 0; i < HitDim.HitNum; i++)
		{
			// �w�y���ʂɐ������ǂ����̓|���S���̖@���̂x�������O�Ɍ���Ȃ��߂����ǂ����Ŕ��f����
			if (HitDim.Dim[i].Normal.y < 0.000001f && HitDim.Dim[i].Normal.y > -0.000001f)
			{
				// �ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{�P�D�O����荂���|���S���̂ݓ����蔻����s��
				if (HitDim.Dim[i].Position[0].y > pl.Position.y + 1.0f ||
					HitDim.Dim[i].Position[1].y > pl.Position.y + 1.0f ||
					HitDim.Dim[i].Position[2].y > pl.Position.y + 1.0f)
				{
					// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
					if (KabeNum < PLAYER_MAX_HITCOLL)
					{
						// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
						Kabe[KabeNum] = &HitDim.Dim[i];

						// �ǃ|���S���̐������Z����
						KabeNum++;
					}
				}
			}
			else
			{
				// �|���S���̐����񋓂ł�����E���ɒB���Ă��Ȃ�������|���S����z��ɒǉ�
				if (YukaNum < PLAYER_MAX_HITCOLL)
				{
					// �|���S���̍\���̂̃A�h���X�����|���S���|�C���^�z��ɕۑ�����
					Yuka[YukaNum] = &HitDim.Dim[i];

					// ���|���S���̐������Z����
					YukaNum++;
				}
			}
		}
	}

	// �ǃ|���S���Ƃ̓����蔻�菈��
	if (KabeNum != 0)
	{
		// �ǂɓ����������ǂ����̃t���O�͏�����Ԃł́u�������Ă��Ȃ��v�ɂ��Ă���
		HitFlag = 0;

		// �ړ��������ǂ����ŏ����𕪊�
		if (MoveFlag == 1)
		{
			// �ǃ|���S���̐������J��Ԃ�
			for (i = 0; i < KabeNum; i++)
			{
				// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				Poly = Kabe[i];

				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
				if (HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), PLAYER_HIT_WIDTH, Poly->Position[0], Poly->Position[1], Poly->Position[2]) == FALSE) continue;

				// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
				HitFlag = 1;

				// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
				{
					VECTOR SlideVec;	// �v���C���[���X���C�h������x�N�g��

					// �i�s�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o
					SlideVec = VCross(MoveVector, Poly->Normal);

					// �Z�o�����x�N�g���ƕǃ|���S���̖@���x�N�g���ɐ����ȃx�N�g�����Z�o�A���ꂪ
					// ���̈ړ���������Ǖ����̈ړ������𔲂����x�N�g��
					SlideVec = VCross(Poly->Normal, SlideVec);

					// ������ړ��O�̍��W�ɑ��������̂�V���ȍ��W�Ƃ���
					NowPos = VAdd(OldPos, SlideVec);
				}

				// �V���Ȉړ����W�ŕǃ|���S���Ɠ������Ă��Ȃ����ǂ����𔻒肷��
				for (j = 0; j < KabeNum; j++)
				{
					// j�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					Poly = Kabe[j];

					// �������Ă����烋�[�v���甲����
					if (HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), PLAYER_HIT_WIDTH, Poly->Position[0], Poly->Position[1], Poly->Position[2]) == TRUE) break;
				}

				// j �� KabeNum �������ꍇ�͂ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂�
				// �ǂɓ��������t���O��|������Ń��[�v���甲����
				if (j == KabeNum)
				{
					HitFlag = 0;
					break;
				}
			}
		}
		else
		{
			// �ړ����Ă��Ȃ��ꍇ�̏���

			// �ǃ|���S���̐������J��Ԃ�
			for (i = 0; i < KabeNum; i++)
			{
				// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
				Poly = Kabe[i];

				// �|���S���ɓ������Ă����瓖�������t���O�𗧂Ă���Ń��[�v���甲����
				if (HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), PLAYER_HIT_WIDTH, Poly->Position[0], Poly->Position[1], Poly->Position[2]) == TRUE)
				{
					HitFlag = 1;
					break;
				}
			}
		}

		// �ǂɓ������Ă�����ǂ��牟���o���������s��
		if (HitFlag == 1)
		{
			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
			for (k = 0; k < PLAYER_HIT_TRYNUM; k++)
			{
				// �ǃ|���S���̐������J��Ԃ�
				for (i = 0; i < KabeNum; i++)
				{
					// i�Ԗڂ̕ǃ|���S���̃A�h���X��ǃ|���S���|�C���^�z�񂩂�擾
					Poly = Kabe[i];

					// �v���C���[�Ɠ������Ă��邩�𔻒�
					if (HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), PLAYER_HIT_WIDTH, Poly->Position[0], Poly->Position[1], Poly->Position[2]) == FALSE) continue;

					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
					NowPos = VAdd(NowPos, VScale(Poly->Normal, PLAYER_HIT_SLIDE_LENGTH));

					// �ړ�������ŕǃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
					for (j = 0; j < KabeNum; j++)
					{
						// �������Ă����烋�[�v�𔲂���
						Poly = Kabe[j];
						if (HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), PLAYER_HIT_WIDTH, Poly->Position[0], Poly->Position[1], Poly->Position[2]) == TRUE) break;
					}

					// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
					if (j == KabeNum) break;
				}

				// i �� KabeNum �ł͂Ȃ��ꍇ�͑S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
				if (i != KabeNum) break;
			}
		}
	}

	// ���|���S���Ƃ̓����蔻��
	if (YukaNum != 0)
	{
		// �W�����v�����㏸���̏ꍇ�͏����𕪊�
		if (pl.State == 2 && pl.JumpPower > 0.0f)
		{
			float MinY;

			// �V��ɓ����Ԃ��鏈�����s��

			// ��ԒႢ�V��ɂԂ���ׂ̔���p�ϐ���������
			MinY = 0.0f;

			// �����������ǂ����̃t���O�𓖂����Ă��Ȃ����Ӗ�����O�ɂ��Ă���
			HitFlag = 0;

			// ���|���S���̐������J��Ԃ�
			for (i = 0; i < YukaNum; i++)
			{
				// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				Poly = Yuka[i];

				// ���悩�瓪�̍����܂ł̊ԂŃ|���S���ƐڐG���Ă��邩�ǂ����𔻒�
				LineRes = HitCheck_Line_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), Poly->Position[0], Poly->Position[1], Poly->Position[2]);

				// �ڐG���Ă��Ȃ������牽�����Ȃ�
				if (LineRes.HitFlag == FALSE) continue;

				// ���Ƀ|���S���ɓ������Ă��āA�����܂Ō��o�����V��|���S����荂���ꍇ�͉������Ȃ�
				if (HitFlag == 1 && MinY < LineRes.Position.y) continue;

				// �|���S���ɓ��������t���O�𗧂Ă�
				HitFlag = 1;

				// �ڐG�����x���W��ۑ�����
				MinY = LineRes.Position.y;
			}

			// �ڐG�����|���S�������������ǂ����ŏ����𕪊�
			if (HitFlag == 1)
			{
				// �ڐG�����ꍇ�̓v���C���[�̂x���W��ڐG���W�����ɍX�V
				NowPos.y = MinY - PLAYER_HIT_HEIGHT;

				// �x�������̑��x�͔��]
				pl.JumpPower = -pl.JumpPower;
			}
		}
		else
		{
			float MaxY;

			// ���~�����W�����v���ł͂Ȃ��ꍇ�̏���

			// ���|���S���ɓ����������ǂ����̃t���O��|���Ă���
			HitFlag = 0;

			// ��ԍ������|���S���ɂԂ���ׂ̔���p�ϐ���������
			MaxY = 0.0f;

			// ���|���S���̐������J��Ԃ�
			for (i = 0; i < YukaNum; i++)
			{
				// i�Ԗڂ̏��|���S���̃A�h���X�����|���S���|�C���^�z�񂩂�擾
				Poly = Yuka[i];

				// �W�����v�����ǂ����ŏ����𕪊�
				if (pl.State == 2)
				{
					// �W�����v���̏ꍇ�͓��̐悩�瑫���菭���Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�
					LineRes = HitCheck_Line_Triangle(VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), VAdd(NowPos, VGet(0.0f, -1.0f, 0.0f)), Poly->Position[0], Poly->Position[1], Poly->Position[2]);
				}
				else
				{
					// �����Ă���ꍇ�͓��̐悩�炻�������Ⴂ�ʒu�̊Ԃœ������Ă��邩�𔻒�( �X�΂ŗ�����ԂɈڍs���Ă��܂�Ȃ��� )
					LineRes = HitCheck_Line_Triangle(VAdd(NowPos, VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)), VAdd(NowPos, VGet(0.0f, -40.0f, 0.0f)), Poly->Position[0], Poly->Position[1], Poly->Position[2]);
				}

				// �������Ă��Ȃ������牽�����Ȃ�
				if (LineRes.HitFlag == FALSE) continue;

				// ���ɓ��������|���S��������A�����܂Ō��o�������|���S�����Ⴂ�ꍇ�͉������Ȃ�
				if (HitFlag == 1 && MaxY > LineRes.Position.y) continue;

				// �|���S���ɓ��������t���O�𗧂Ă�
				HitFlag = 1;

				// �ڐG�����x���W��ۑ�����
				MaxY = LineRes.Position.y;
			}

			// ���|���S���ɓ����������ǂ����ŏ����𕪊�
			if (HitFlag == 1)
			{
				// ���������ꍇ

				// �ڐG�����|���S���ň�ԍ����x���W���v���C���[�̂x���W�ɂ���
				NowPos.y = MaxY;

				// �x�������̈ړ����x�͂O��
				pl.JumpPower = 0.0f;

				// �����W�����v���������ꍇ�͒��n��Ԃɂ���
				if (pl.State == 2)
				{
					// �ړ����Ă������ǂ����Œ��n��̏�ԂƍĐ�����A�j���[�V�����𕪊򂷂�
					if (MoveFlag)
					{
						// �ړ����Ă���ꍇ�͑����Ԃ�
						Player_PlayAnim(1);
						pl.State = 1;
					}
					else
					{
						// �ړ����Ă��Ȃ��ꍇ�͗����~���Ԃ�
						Player_PlayAnim(4);
						pl.State = 0;
					}

					// ���n���̓A�j���[�V�����̃u�����h�͍s��Ȃ�
					pl.AnimBlendRate = 1.0f;
				}
			}
			else
			{
				// ���R���W�����ɓ������Ă��Ȃ��Ċ��W�����v��Ԃł͂Ȃ������ꍇ��
				if (pl.State != 2)
				{
					// �W�����v���ɂ���
					pl.State = 2;

					// ������Ƃ����W�����v����
					pl.JumpPower = PLAYER_FALL_UP_POWER;

					// �A�j���[�V�����͗������̂��̂ɂ���
					Player_PlayAnim(3);
				}
			}
		}
	}

	// �V�������W��ۑ�����
	pl.Position = NowPos;

	// �v���C���[�̃��f���̍��W���X�V����
	MV1SetPosition(pl.ModelHandle, pl.Position);

	// ���o�����v���C���[�̎��͂̃|���S�������J������
	MV1CollResultPolyDimTerminate(HitDim);
}

// �v���C���[�̌�����ς��鏈��
void Player_AngleProcess(void)
{
	float TargetAngle;			// �ڕW�p�x
	float SaAngle;				// �ڕW�p�x�ƌ��݂̊p�x�Ƃ̍�

	// �ڕW�̕����x�N�g������p�x�l���Z�o����
	TargetAngle = atan2(pl.TargetMoveDirection.x, pl.TargetMoveDirection.z);

	// �ڕW�̊p�x�ƌ��݂̊p�x�Ƃ̍�������o��
	{
		// �ŏ��͒P���Ɉ����Z
		SaAngle = TargetAngle - pl.Angle;

		// ����������炠������̍����P�W�O�x�ȏ�ɂȂ邱�Ƃ͖����̂�
		// ���̒l���P�W�O�x�ȏ�ɂȂ��Ă�����C������
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

	// �p�x�̍����O�ɋ߂Â���
	if (SaAngle > 0.0f)
	{
		// �����v���X�̏ꍇ�͈���
		SaAngle -= PLAYER_ANGLE_SPEED;
		if (SaAngle < 0.0f)
		{
			SaAngle = 0.0f;
		}
	}
	else
	{
		// �����}�C�i�X�̏ꍇ�͑���
		SaAngle += PLAYER_ANGLE_SPEED;
		if (SaAngle > 0.0f)
		{
			SaAngle = 0.0f;
		}
	}

	// ���f���̊p�x���X�V
	pl.Angle = TargetAngle - SaAngle;
	MV1SetRotationXYZ(pl.ModelHandle, VGet(0.0f, pl.Angle + DX_PI_F, 0.0f));
}

// �v���C���[�ɐV���ȃA�j���[�V�������Đ�����
void Player_PlayAnim(int PlayAnim)
{
	// �Đ����̃��[�V�����Q���L����������f�^�b�`����
	if (pl.PlayAnim2 != -1)
	{
		MV1DetachAnim(pl.ModelHandle, pl.PlayAnim2);
		pl.PlayAnim2 = -1;
	}

	// ���܂ōĐ����̃��[�V�����P���������̂̏����Q�Ɉړ�����
	pl.PlayAnim2 = pl.PlayAnim1;
	pl.AnimPlayCount2 = pl.AnimPlayCount1;

	// �V���Ɏw��̃��[�V���������f���ɃA�^�b�`���āA�A�^�b�`�ԍ���ۑ�����
	pl.PlayAnim1 = MV1AttachAnim(pl.ModelHandle, PlayAnim);
	pl.AnimPlayCount1 = 0.0f;

	// �u�����h���͍Đ����̃��[�V�����Q���L���ł͂Ȃ��ꍇ�͂P�D�O��( �Đ����̃��[�V�����P���P�O�O���̏�� )�ɂ���
	pl.AnimBlendRate = pl.PlayAnim2 == -1 ? 1.0f : 0.0f;
}

// �v���C���[�̃A�j���[�V��������
void Player_AnimProcess(void)
{
	float AnimTotalTime;		// �Đ����Ă���A�j���[�V�����̑�����

	// �u�����h�����P�ȉ��̏ꍇ�͂P�ɋ߂Â���
	if (pl.AnimBlendRate < 1.0f)
	{
		pl.AnimBlendRate += PLAYER_ANIM_BLEND_SPEED;
		if (pl.AnimBlendRate > 1.0f)
		{
			pl.AnimBlendRate = 1.0f;
		}
	}

	// �Đ����Ă���A�j���[�V�����P�̏���
	if (pl.PlayAnim1 != -1)
	{
		// �A�j���[�V�����̑����Ԃ��擾
		AnimTotalTime = MV1GetAttachAnimTotalTime(pl.ModelHandle, pl.PlayAnim1);

		// �Đ����Ԃ�i�߂�
		pl.AnimPlayCount1 += PLAYER_PLAY_ANIM_SPEED;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (pl.AnimPlayCount1 >= AnimTotalTime)
		{
			pl.AnimPlayCount1 = fmod(pl.AnimPlayCount1, AnimTotalTime);
		}

		// �ύX�����Đ����Ԃ����f���ɔ��f������
		MV1SetAttachAnimTime(pl.ModelHandle, pl.PlayAnim1, pl.AnimPlayCount1);

		// �A�j���[�V�����P�̃��f���ɑ΂��锽�f�����Z�b�g
		MV1SetAttachAnimBlendRate(pl.ModelHandle, pl.PlayAnim1, pl.AnimBlendRate);
	}

	// �Đ����Ă���A�j���[�V�����Q�̏���
	if (pl.PlayAnim2 != -1)
	{
		// �A�j���[�V�����̑����Ԃ��擾
		AnimTotalTime = MV1GetAttachAnimTotalTime(pl.ModelHandle, pl.PlayAnim2);

		// �Đ����Ԃ�i�߂�
		pl.AnimPlayCount2 += PLAYER_PLAY_ANIM_SPEED;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (pl.AnimPlayCount2 > AnimTotalTime)
		{
			pl.AnimPlayCount2 = fmod(pl.AnimPlayCount2, AnimTotalTime);
		}

		// �ύX�����Đ����Ԃ����f���ɔ��f������
		MV1SetAttachAnimTime(pl.ModelHandle, pl.PlayAnim2, pl.AnimPlayCount2);

		// �A�j���[�V�����Q�̃��f���ɑ΂��锽�f�����Z�b�g
		MV1SetAttachAnimBlendRate(pl.ModelHandle, pl.PlayAnim2, 1.0f - pl.AnimBlendRate);
	}
}

// �v���C���[�̉e��`��
void Player_ShadowRender(void)
{
	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY *HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;

	// ���C�e�B���O�𖳌��ɂ���
	SetUseLighting(FALSE);

	// �y�o�b�t�@��L���ɂ���
	SetUseZBuffer3D(TRUE);

	// �e�N�X�`���A�h���X���[�h�� CLAMP �ɂ���( �e�N�X�`���̒[����͒[�̃h�b�g�����X���� )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// �v���C���[�̒����ɑ��݂���n�ʂ̃|���S�����擾
	HitResDim = MV1CollCheck_Capsule(stg.ModelHandle, -1, pl.Position, VAdd(pl.Position, VGet(0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f)), PLAYER_SHADOW_SIZE);

	// ���_�f�[�^�ŕω��������������Z�b�g
	Vertex[0].dif = GetColorU8(255, 255, 255, 255);
	Vertex[0].spc = GetColorU8(0, 0, 0, 0);
	Vertex[0].su = 0.0f;
	Vertex[0].sv = 0.0f;
	Vertex[1] = Vertex[0];
	Vertex[2] = Vertex[0];

	// ���̒����ɑ��݂���|���S���̐������J��Ԃ�
	HitRes = HitResDim.Dim;
	for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
	{
		// �|���S���̍��W�͒n�ʃ|���S���̍��W
		Vertex[0].pos = HitRes->Position[0];
		Vertex[1].pos = HitRes->Position[1];
		Vertex[2].pos = HitRes->Position[2];

		// ������Ǝ����グ�ďd�Ȃ�Ȃ��悤�ɂ���
		SlideVec = VScale(HitRes->Normal, 0.5f);
		Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
		Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
		Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

		// �|���S���̕s�����x��ݒ肷��
		Vertex[0].dif.a = 0;
		Vertex[1].dif.a = 0;
		Vertex[2].dif.a = 0;
		if (HitRes->Position[0].y > pl.Position.y - PLAYER_SHADOW_HEIGHT)
			Vertex[0].dif.a = 128 * (1.0f - fabs(HitRes->Position[0].y - pl.Position.y) / PLAYER_SHADOW_HEIGHT);

		if (HitRes->Position[1].y > pl.Position.y - PLAYER_SHADOW_HEIGHT)
			Vertex[1].dif.a = 128 * (1.0f - fabs(HitRes->Position[1].y - pl.Position.y) / PLAYER_SHADOW_HEIGHT);

		if (HitRes->Position[2].y > pl.Position.y - PLAYER_SHADOW_HEIGHT)
			Vertex[2].dif.a = 128 * (1.0f - fabs(HitRes->Position[2].y - pl.Position.y) / PLAYER_SHADOW_HEIGHT);

		// �t�u�l�͒n�ʃ|���S���ƃv���C���[�̑��΍��W���犄��o��
		Vertex[0].u = (HitRes->Position[0].x - pl.Position.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[0].v = (HitRes->Position[0].z - pl.Position.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[1].u = (HitRes->Position[1].x - pl.Position.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[1].v = (HitRes->Position[1].z - pl.Position.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[2].u = (HitRes->Position[2].x - pl.Position.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[2].v = (HitRes->Position[2].z - pl.Position.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

		// �e�|���S����`��
		DrawPolygon3D(Vertex, 1, pl.ShadowHandle, TRUE);
	}

	// ���o�����n�ʃ|���S�����̌�n��
	MV1CollResultPolyDimTerminate(HitResDim);

	// ���C�e�B���O��L���ɂ���
	SetUseLighting(TRUE);

	// �y�o�b�t�@�𖳌��ɂ���
	SetUseZBuffer3D(FALSE);
}

// �X�e�[�W�̏���������
void Stage_Initialize(void)
{
	// �X�e�[�W���f���̓ǂݍ���
	stg.ModelHandle = MV1LoadModel("ColTestStage.mqo");

	// ���f���S�̂̃R���W�������̃Z�b�g�A�b�v
	MV1SetupCollInfo(stg.ModelHandle, -1);
}

// �X�e�[�W�̌�n������
void Stage_Terminate(void)
{
	// �X�e�[�W���f���̌�n��
	MV1DeleteModel(stg.ModelHandle);
}

// �J�����̏���������
void Camera_Initialize(void)
{
	// �J�����̏��������p�x�͂P�W�O�x
	cam.AngleH = DX_PI_F;

	// �����p�x�͂O�x
	cam.AngleV = 0.0f;
}

// �J�����̏���
void Camera_Process(void)
{
	// �p�b�h�̂R�{�^�����A�V�t�g�L�[��������Ă���ꍇ�̂݊p�x�ύX������s��
	if (CheckHitKey(KEY_INPUT_LSHIFT) || (inp.NowInput & PAD_INPUT_C))
	{
		// �u���v�{�^����������Ă����琅���p�x���}�C�i�X����
		if (inp.NowInput & PAD_INPUT_LEFT)
		{
			cam.AngleH -= CAMERA_ANGLE_SPEED;

			// �|�P�W�O�x�ȉ��ɂȂ�����p�x�l���傫���Ȃ肷���Ȃ��悤�ɂR�U�O�x�𑫂�
			if (cam.AngleH < -DX_PI_F)
			{
				cam.AngleH += DX_TWO_PI_F;
			}
		}

		// �u���v�{�^����������Ă����琅���p�x���v���X����
		if (inp.NowInput & PAD_INPUT_RIGHT)
		{
			cam.AngleH += CAMERA_ANGLE_SPEED;

			// �P�W�O�x�ȏ�ɂȂ�����p�x�l���傫���Ȃ肷���Ȃ��悤�ɂR�U�O�x������
			if (cam.AngleH > DX_PI_F)
			{
				cam.AngleH -= DX_TWO_PI_F;
			}
		}

		// �u���v�{�^����������Ă����琂���p�x���}�C�i�X����
		if (inp.NowInput & PAD_INPUT_UP)
		{
			cam.AngleV -= CAMERA_ANGLE_SPEED;

			// ������p�x�ȉ��ɂ͂Ȃ�Ȃ��悤�ɂ���
			if (cam.AngleV < -DX_PI_F / 2.0f + 0.6f)
			{
				cam.AngleV = -DX_PI_F / 2.0f + 0.6f;
			}
		}

		// �u���v�{�^����������Ă����琂���p�x���v���X����
		if (inp.NowInput & PAD_INPUT_DOWN)
		{
			cam.AngleV += CAMERA_ANGLE_SPEED;

			// ������p�x�ȏ�ɂ͂Ȃ�Ȃ��悤�ɂ���
			if (cam.AngleV > DX_PI_F / 2.0f - 0.6f)
			{
				cam.AngleV = DX_PI_F / 2.0f - 0.6f;
			}
		}
	}

	// �J�����̒����_�̓v���C���[���W����K��l���������W
	cam.Target = VAdd(pl.Position, VGet(0.0f, CAMERA_PLAYER_TARGET_HEIGHT, 0.0f));

	// �J�����̍��W�����肷��
	{
		MATRIX RotZ, RotY;
		float Camera_Player_Length;
		MV1_COLL_RESULT_POLY_DIM HRes;
		int HitNum;

		// ���������̉�]�͂x����]
		RotY = MGetRotY(cam.AngleH);

		// ���������̉�]�͂y����] )
		RotZ = MGetRotZ(cam.AngleV);

		// �J��������v���C���[�܂ł̏����������Z�b�g
		Camera_Player_Length = CAMERA_PLAYER_LENGTH;

		// �J�����̍��W���Z�o
		// �w���ɃJ�����ƃv���C���[�Ƃ̋����������L�т��x�N�g����
		// ����������]( �y����] )���������Ɛ���������]( �x����] )���čX��
		// �����_�̍��W�𑫂������̂��J�����̍��W
		cam.Eye = VAdd(VTransform(VTransform(VGet(-Camera_Player_Length, 0.0f, 0.0f), RotZ), RotY), cam.Target);

		// �����_����J�����̍��W�܂ł̊ԂɃX�e�[�W�̃|���S�������邩���ׂ�
		HRes = MV1CollCheck_Capsule(stg.ModelHandle, -1, cam.Target, cam.Eye, CAMERA_COLLISION_SIZE);
		HitNum = HRes.HitNum;
		MV1CollResultPolyDimTerminate(HRes);
		if (HitNum != 0)
		{
			float NotHitLength;
			float HitLength;
			float TestLength;
			VECTOR TestPosition;

			// �������疳���ʒu�܂Ńv���C���[�ɋ߂Â�

			// �|���S���ɓ�����Ȃ��������Z�b�g
			NotHitLength = 0.0f;

			// �|���S���ɓ����鋗�����Z�b�g
			HitLength = Camera_Player_Length;
			do
			{
				// �����邩�ǂ����e�X�g���鋗�����Z�b�g( ������Ȃ������Ɠ����鋗���̒��� )
				TestLength = NotHitLength + (HitLength - NotHitLength) / 2.0f;

				// �e�X�g�p�̃J�������W���Z�o
				TestPosition = VAdd(VTransform(VTransform(VGet(-TestLength, 0.0f, 0.0f), RotZ), RotY), cam.Target);

				// �V�������W�ŕǂɓ����邩�e�X�g
				HRes = MV1CollCheck_Capsule(stg.ModelHandle, -1, cam.Target, TestPosition, CAMERA_COLLISION_SIZE);
				HitNum = HRes.HitNum;
				MV1CollResultPolyDimTerminate(HRes);
				if (HitNum != 0)
				{
					// ���������瓖���鋗���� TestLength �ɕύX����
					HitLength = TestLength;
				}
				else
				{
					// ������Ȃ������瓖����Ȃ������� TestLength �ɕύX����
					NotHitLength = TestLength;
				}

				// HitLength �� NoHitLength ���\���ɋ߂Â��Ă��Ȃ������烋�[�v
			} while (HitLength - NotHitLength > 0.1f);

			// �J�����̍��W���Z�b�g
			cam.Eye = TestPosition;
		}
	}

	// �J�����̏������C�u�����̃J�����ɔ��f������
	SetCameraPositionAndTarget_UpVecY(cam.Eye, cam.Target);
}

// �`�揈��
void Render_Process(void)
{
	// �X�e�[�W���f���̕`��
	MV1DrawModel(stg.ModelHandle);

	// �v���C���[���f���̕`��
	MV1DrawModel(pl.ModelHandle);

	// �v���C���[�̉e�̕`��
	Player_ShadowRender();
}

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// �E�C���h�E���[�h�ŋN��
	ChangeWindowMode(TRUE);

	// ���C�u�����̏�����
	if (DxLib_Init() < 0)
		return -1;

	// �v���C���[�̏�����
	Player_Initialize();

	// �X�e�[�W�̏�����
	Stage_Initialize();

	// �J�����̏�����
	Camera_Initialize();

	// �`���𗠉�ʂɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	// �d�r�b�L�[��������邩�A�E�C���h�E��������܂Ń��[�v
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		// ��ʂ��N���A
		ClearDrawScreen();

		// ���͏���
		Input_Process();

		// �v���C���[�̏���
		Player_Process();

		// �J�����̏���
		Camera_Process();

		// �`�揈��
		Render_Process();

		// ����ʂ̓��e��\��ʂɔ��f
		ScreenFlip();
	}

	// �v���C���[�̌�n��
	Player_Terminate();

	// �X�e�[�W�̌�n��
	Stage_Terminate();

	// ���C�u�����̌�n��
	DxLib_End();

	// �\�t�g�I��
	return 0;
};

int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}
