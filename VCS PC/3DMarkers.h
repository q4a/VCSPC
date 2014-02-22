#ifndef __3DMARKERS
#define __3DMARKERS

#include "General.h"

// VCS colors
#define MARKER_SET_COLOR_R	0xED
#define MARKER_SET_COLOR_G	0x82
#define MARKER_SET_COLOR_B	0xB4

// VC colors
/*#define MARKER_SET_COLOR_R	0xFC
#define MARKER_SET_COLOR_G	0x8A
#define MARKER_SET_COLOR_B	0xF2*/
#define MARKER_SET_COLOR_A	0xE4


class C3DMarker
{
private:
    CMatrix			m_mat;  // local space to world space transform // 0
    RpClump*		m_pRwObject; // 72
    void*			m_pMaterial; // 76
    
    WORD            m_nType; // 80
    bool            m_bIsUsed;  // has this marker been allocated this frame?    // 82
    DWORD           m_nIdentifier; // 84

    DWORD           rwColour; // 88 
    WORD            m_nPulsePeriod; // 92
    short           m_nRotateRate;  // deg per frame (in either direction) // 94
    DWORD           m_nStartTime; // 96
    FLOAT           m_fPulseFraction;  // 100
    FLOAT           m_fStdSize; // 104
    FLOAT           m_fSize; // 108
    FLOAT           m_fBrightness; // 112
    FLOAT           m_fCameraRange; // 116

    CVector     m_normal;           // Normal of the object point at             // 120
    // the following variables remember the last time we read the heigh of the
    // map. Using this we don't have to do this every frame and we can still have moving markers.
    WORD            m_LastMapReadX, m_LastMapReadY; // 132 / 134
    FLOAT           m_LastMapReadResultZ; // 136
    FLOAT           m_roofHeight; // 140
    CVector         m_lastPosition; // 144
    DWORD           m_OnScreenTestTime;     // time last screen check was done // 156

public:
	long double		CalculateRealSize();
};

class C3DMarkers
{
private:
	static float				m_PosZMult;
	static const float			m_MovingMultiplier;

private:
	static C3DMarker*			PlaceMarker(unsigned int nIndex, unsigned short markerID, CVector& vecPos, float fSize, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, unsigned short pulsePeriod, float pulseFraction, short rotateRate, float normalX, float normalY, float normalZ, bool checkZ);

public:
	static inline float*		GetPosZMult()
			{ return &m_PosZMult; };
	static inline const float*	GetMovingMult()
			{ return &m_MovingMultiplier; };

	// Last unused param removed
	static void					PlaceMarkerSet(unsigned int nIndex, unsigned short markerID, CVector& vecPos, float fSize, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, unsigned short pulsePeriod, float pulseFraction);
};

static_assert(sizeof(C3DMarker) == 0xA0, "Wrong size: C3DMarker");

#endif