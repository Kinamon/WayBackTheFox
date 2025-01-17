#include "CNormalEnemyBase.h"

/******************************************
*		通常敵の基底クラス.
*********/
CNormalEnemyBase::CNormalEnemyBase()
	: m_HitNum			(0)
	, m_HitMoveDirection(0)
	, m_HitFlag			(false)
	, m_ButtonNum		(0)
	, m_enInputDecision	(nullptr)
	, m_pCEnemyFlyEffect(nullptr)
{
	//飛ぶエフェクトクラスのインスタンス.
	m_pCEnemyFlyEffect = new CEnemyFlyEffect();

	//判定の動的確保.
	m_enInputDecision = new enInput_Decision[DECISION_MAX]();
	//初期化処理.
	for (int command = 0; command < DECISION_MAX; command++) {
		m_enInputDecision[command] = enInput_Decision::Max;
	}
}

CNormalEnemyBase::~CNormalEnemyBase()
{
	//解放処理.
	SAFE_DELETE_ARRAY(m_enInputDecision);
	SAFE_DELETE(m_pCEnemyFlyEffect);
}

//==================================.
//		飛ぶ判定処理関数.
//==================================.
void CNormalEnemyBase::JudgeFly()
{
	m_enInputDecision[STANDERD_USE_COMMAND] = m_pCCommand_Base->GetInputDeision();

	//Great,Goodの時がついたとき.
	if (m_enInputDecision[STANDERD_USE_COMMAND] == enInput_Decision::Great || 
		m_enInputDecision[STANDERD_USE_COMMAND] == enInput_Decision::Good) {
		m_HitNum++;
		m_HitFlag = true;
		//当たった時のSE.
		m_pCSEPlayManager->SetSEPlayFlag(CSoundResource::enSoundSE::EnemyFly, true);
		//〜〜〜〜〜〜〜〜〜〜〜.
		//エフェクト.
		//〜〜〜〜〜〜〜〜〜〜〜.
		//表示フラグ.
		m_pCEffectBase->SetEffectStart(true);
	}
}

//==================================.
//		飛んでいく移動処理関数.
//==================================.
void CNormalEnemyBase::MoveFly()
{
	//飛ぶ角度調整.
	m_vPos.x += FLY_SPEED.x * m_HitMoveDirection;
	m_vPos.y += FLY_SPEED.y;


	//飛んでいるときの傾き.	
	switch (m_HitNum % 2) {
		//左に飛ぶキャラ.
	case static_cast<int>(enFlyDirection::Left):
		if (m_vRot.z < LEFT_JUMP_ROT) {
			m_HitMoveDirection = LEFT_DIRECTION;
			m_vRot.z += ROT_SPEED;
		}
		break;
		//右に飛ぶキャラ.
	case static_cast<int>(enFlyDirection::Right):
		if (m_vRot.z > RIGHT_JUMP_ROT) {
			m_HitMoveDirection = RIGHT_DIRECTION;
			m_vRot.z -= ROT_SPEED;
		}
		break;
	}

	//〜〜〜〜〜〜〜〜〜〜〜.
	//エフェクト.
	//〜〜〜〜〜〜〜〜〜〜〜.
	//位置設定.
	m_pCEnemyFlyEffect->SetCenterPos(m_vPos);
	//更新処理関数.
	m_pCEnemyFlyEffect->Update();
}

