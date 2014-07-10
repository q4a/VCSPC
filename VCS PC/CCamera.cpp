#include "StdAfx.h"
#include "CCamera.h"

#include "Sprite.h"

bool CCamera::bDontTouchFOVInWidescreen;

WRAPPER void CamShakeNoPos(CCamera* pCamera, float fStrength) { WRAPARG(pCamera); WRAPARG(fStrength); EAXJMP(0x50A970); }

WRAPPER int CCamera::GetFadeStage() { EAXJMP(0x50AE20); }
WRAPPER int CCamera::GetLookDirection() { EAXJMP(0x50AE90); }
WRAPPER bool CCamera::IsPositionVisible(const CVector& vecPos, float fRadius) { WRAPARG(vecPos); WRAPARG(fRadius); EAXJMP(0x420D40); }

void CCamera::DrawBordersForWideScreen()
{
	if ( bDontTouchFOVInWidescreen )
		return;

	CRect		ScreenRect;

	ScreenRect.x1 = -1000.0f;
	ScreenRect.y1 = -1000.0f;
	ScreenRect.x2 = -1000.0f;
	ScreenRect.y2 = -1000.0f;

	GetScreenRect(ScreenRect);

	/* May be unused?
	if ( m_BlurType == 0 || m_BlurType == 2 )
		m_imotionBlurAddAlpha = 80; */

	// Letterbox
	if ( ScreenRect.y1 > 0.0 && ScreenRect.y2 > 0.0 )
	{		
		CSprite2d::DrawRect(CRect(-5.0f, -5.0f, RsGlobal.MaximumWidth + 5.0f, ScreenRect.y1), CRGBA(0, 0, 0, 255));
		CSprite2d::DrawRect(CRect(-5.0f, ScreenRect.y2, RsGlobal.MaximumWidth + 5.0f, RsGlobal.MaximumHeight + 5.0f), CRGBA(0, 0, 0, 255));
	}
	// Pillarbox
	else if ( ScreenRect.x1 > 0.0 && ScreenRect.x2 > 0.0 )
	{		
		CSprite2d::DrawRect(CRect(-5.0f, -5.0f, ScreenRect.x1, RsGlobal.MaximumHeight + 5.0f), CRGBA(0, 0, 0, 255));
		CSprite2d::DrawRect(CRect(ScreenRect.x2, -5.0f, RsGlobal.MaximumWidth + 5.0f, RsGlobal.MaximumHeight + 5.0f), CRGBA(0, 0, 0, 255));
	}
}

void CCamera::GetScreenRect(CRect& rect)
{
	float			fScreenRatio = ScreenAspectRatio;
	float			dScreenHeightWeWannaCut = ((-9.0f/16.0f) * fScreenRatio + 1.0f);
	float			dBorderProportionsFix = ((-144643.0f/50000.0f) * fScreenRatio * fScreenRatio) + ((807321.0f/100000.0f) * fScreenRatio) - (551143.0f/100000.0f);

	if ( dBorderProportionsFix < 0.0 )
		dBorderProportionsFix = 0.0;

	if ( dScreenHeightWeWannaCut > 0.0 )
	{
		// Letterbox
		rect.y1 = (RsGlobal.MaximumHeight / 2) * (dScreenHeightWeWannaCut - dBorderProportionsFix);
		rect.y2 = RsGlobal.MaximumHeight - ((RsGlobal.MaximumHeight / 2) * (dScreenHeightWeWannaCut + dBorderProportionsFix));
	}
	else
	{
		// Pillarbox
		dScreenHeightWeWannaCut = -dScreenHeightWeWannaCut;

		rect.x1 = (RsGlobal.MaximumWidth / 4) * dScreenHeightWeWannaCut;
		rect.x2 = RsGlobal.MaximumWidth - (RsGlobal.MaximumWidth / 4) * dScreenHeightWeWannaCut;
	}
}