#include "CStartCount.h"

CStartCount::CStartCount()
	: m_pCStartCount(nullptr)
	, m_pCStart(nullptr)
	, m_vPattarn(0.0f, 0.0f)
	, m_Frame(0)
	, m_Second(0)
	, m_bDispFlag(false)
{
	m_vPos = START_POSITION;
	m_Second = 4;
}

CStartCount::~CStartCount()
{
	Release();
}

//=====================================
//	更新処理関数.
//=====================================
void CStartCount::UpDate()
{
	m_Frame++;
	if (m_Frame % 60 == 0)
	{
		m_Second--;
	}
	if (m_Second == 0)
	{
		m_bDispFlag = false;
		m_Frame = 0;
	}
}

//=====================================
//	描画処理関数.
//=====================================
void CStartCount::Render()
{
	if (m_bDispFlag == true) {
		m_pCStartCount = m_pCResourceManager->GetSpriteUI(CResourceSpriteUI::enSpriteUI::TimerNum);

		m_vPattarn = D3DXVECTOR2(static_cast<float>((m_Second - 1) % IMAGE_WIDTH_MAX), static_cast<float>((m_Second - 1) / IMAGE_HIGH_MAX));
		m_pCStartCount->SetPattern(m_vPattarn);

		m_pCStartCount->SetPosition(POSITION);
		if (m_Second > 1) {
			m_pCStartCount->Render();
		}

		m_pCStart = m_pCResourceManager->GetSpriteUI(CResourceSpriteUI::enSpriteUI::Start_Char);

		m_pCStart->SetPosition(m_vPos);
		if (m_Second == 1) {
			m_pCStart->Render();
		}
	}
}

//=====================================
//	解放処理関数.
//=====================================
void CStartCount::Release()
{
}

//=====================================
//	動き処理関数.
//=====================================
void CStartCount::Move()
{
}

