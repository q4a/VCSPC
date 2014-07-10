#ifndef __GENERAL
#define __GENERAL

#include "..\common\Date.h"

#define FUNC_CEntity__GetBoundCentre				0x534250

class CSimpleTransform
{
public:
    CVector                         m_translate;
    float                           m_heading;
};

class CRGBA
{
public:
	BYTE r, g, b, a;

	inline CRGBA() {}

	inline CRGBA(const CRGBA& in)
		: r(in.r), g(in.g), b(in.b), a(in.a)
	{}

	inline CRGBA(const CRGBA& in, BYTE alpha)
		: r(in.r), g(in.g), b(in.b), a(alpha)
	{}

	inline CRGBA(BYTE red, BYTE green, BYTE blue, BYTE alpha = 255)
		: r(red), g(green), b(blue), a(alpha)
	{}

	template <typename T>
	friend CRGBA Blend(const CRGBA& One, T OneStrength, const CRGBA& Two, T TwoStrength)
		{	T	TotalStrength = OneStrength + TwoStrength;
			return CRGBA(	((One.r * OneStrength) + (Two.r * TwoStrength))/TotalStrength,
							((One.g * OneStrength) + (Two.g * TwoStrength))/TotalStrength,
							((One.b * OneStrength) + (Two.b * TwoStrength))/TotalStrength,
							((One.a * OneStrength) + (Two.a * TwoStrength))/TotalStrength); }

	template <typename T>
	friend CRGBA Blend(const CRGBA& One, T OneStrength, const CRGBA& Two, T TwoStrength, const CRGBA& Three, T ThreeStrength)
		{	T	TotalStrength = OneStrength + TwoStrength + ThreeStrength;
			return CRGBA(	((One.r * OneStrength) + (Two.r * TwoStrength) + (Three.r * ThreeStrength))/TotalStrength,
							((One.g * OneStrength) + (Two.g * TwoStrength) + (Three.g * ThreeStrength))/TotalStrength,
							((One.b * OneStrength) + (Two.b * TwoStrength) + (Three.b * ThreeStrength))/TotalStrength,
							((One.a * OneStrength) + (Two.a * TwoStrength) + (Three.a * ThreeStrength))/TotalStrength); }

	void	BaseColors__Constructor();
};

class CRect
{
public:
	float x1, y1;
	float x2, y2;

	inline CRect() {}
	inline CRect(float a, float b, float c, float d)
		: x1(a), y1(b), x2(c), y2(d)
	{}
};


class CPlaceable
{
private:
    CSimpleTransform				m_transform;
    CMatrix*						m_pCoords;

public:
	// Line up the VMTs
	virtual ~CPlaceable() {}

	inline CPlaceable() {}

	explicit inline CPlaceable(int dummy)
	{
		// Dummy ctor
		UNREFERENCED_PARAMETER(dummy);
	}

	inline CVector*					GetCoords()
		{ return m_pCoords ? reinterpret_cast<CVector*>(&m_pCoords->matrix.pos) : &m_transform.m_translate; }
	inline CMatrix*					GetMatrix()
		{ return m_pCoords; }
	inline CSimpleTransform&		GetTransform()
		{ return m_transform; }
	inline float					GetHeading()
		{ return m_pCoords ? atan2(-m_pCoords->GetUp()->x, m_pCoords->GetUp()->y) : m_transform.m_heading; }

	inline void						SetCoords(const CVector& pos)
	{	if ( m_pCoords ) { m_pCoords->matrix.pos.x = pos.x; m_pCoords->matrix.pos.y = pos.y; m_pCoords->matrix.pos.z = pos.z; }
		else m_transform.m_translate = pos; }
	inline void						SetHeading(float fHeading)
		{ if ( m_pCoords ) m_pCoords->SetRotateZOnly(fHeading); else m_transform.m_heading = fHeading; }
};

// TODO: May not be the best place to put it?
class NOVMT CEntity	: public CPlaceable
{
public:
    virtual void	Add_CRect();
    virtual void	Add();
    virtual void	Remove();
    virtual void	SetIsStatic(bool);
    virtual void	SetModelIndex(int nModelIndex);
    virtual void	SetModelIndexNoCreate(int nModelIndex);
    virtual void	CreateRwObject();
    virtual void	DeleteRwObject();
    virtual CRect	GetBoundRect();
    virtual void	ProcessControl();
    virtual void	ProcessCollision();
    virtual void	ProcessShift();
    virtual void	TestCollision();
    virtual void	Teleport();
    virtual void	SpecialEntityPreCollisionStuff();
    virtual void	SpecialEntityCalcCollisionSteps();
    virtual void	PreRender();
    virtual void	Render();
    virtual void	SetupLighting();
    virtual void	RemoveLighting();
    virtual void	FlagToDestroyWhenNextProcessed();

//private:
	RwObject*		m_pRwObject;						// 0x18

    /********** BEGIN CFLAGS (0x1C) **************/
    unsigned long	bUsesCollision : 1;				// does entity use collision
    unsigned long	bCollisionProcessed : 1;			// has object been processed by a ProcessEntityCollision function
    unsigned long	bIsStatic : 1;					// is entity static
    unsigned long	bHasContacted : 1;				// has entity processed some contact forces
    unsigned long	bIsStuck : 1;						// is entity stuck
    unsigned long	bIsInSafePosition : 1;			// is entity in a collision free safe position
    unsigned long	bWasPostponed : 1;				// was entity control processing postponed
    unsigned long	bIsVisible : 1;					//is the entity visible

    unsigned long	bIsBIGBuilding : 1;				// Set if this entity is a big building
    unsigned long	bRenderDamaged : 1;				// use damaged LOD models for objects with applicable damage
    unsigned long	bStreamingDontDelete : 1;			// Dont let the streaming remove this
    unsigned long	bRemoveFromWorld : 1;				// remove this entity next time it should be processed
    unsigned long	bHasHitWall : 1;					// has collided with a building (changes subsequent collisions)
    unsigned long	bImBeingRendered : 1;				// don't delete me because I'm being rendered
    unsigned long	bDrawLast :1;						// draw object last
    unsigned long	bDistanceFade :1;					// Fade entity because it is far away

    unsigned long	bDontCastShadowsOn : 1;			// Dont cast shadows on this object
    unsigned long	bOffscreen : 1;					// offscreen flag. This can only be trusted when it is set to true
    unsigned long	bIsStaticWaitingForCollision : 1; // this is used by script created entities - they are static until the collision is loaded below them
    unsigned long	bDontStream : 1;					// tell the streaming not to stream me
    unsigned long	bUnderwater : 1;					// this object is underwater change drawing order
    unsigned long	bHasPreRenderEffects : 1;			// Object has a prerender effects attached to it
    unsigned long	bIsTempBuilding : 1;				// whether or not the building is temporary (i.e. can be created and deleted more than once)
    unsigned long	bDontUpdateHierarchy : 1;			// Don't update the aniamtion hierarchy this frame

    unsigned long	bHasRoadsignText : 1;				// entity is roadsign and has some 2deffect text stuff to be rendered
    unsigned long	bDisplayedSuperLowLOD : 1;
    unsigned long	bIsProcObject : 1;				// set object has been generate by procedural object generator
    unsigned long	bBackfaceCulled : 1;				// has backface culling on
    unsigned long	bLightObject : 1;					// light object with directional lights
    unsigned long	bUnimportantStream : 1;			// set that this object is unimportant, if streaming is having problems
    unsigned long	bTunnel : 1;						// Is this model part of a tunnel
    unsigned long	bTunnelTransition : 1;			// This model should be rendered from within and outside of the tunnel
    /********** END CFLAGS **************/

    WORD			RandomSeed;					// 0x20
    short			m_nModelIndex;				// 0x22
    void*			pReferences;				// 0x24
    void*			m_pLastRenderedLink;		// 0x28
    WORD			m_nScanCode;				// 0x2C
    BYTE			m_iplIndex;					// 0x2E
    BYTE			m_areaCode;					// 0x2F
    CEntity*		m_pLod;					// 0x30
    BYTE			numLodChildren;				// 0x34
    char			numLodChildrenRendered;		// 0x35

    //********* BEGIN CEntityInfo **********//
    BYTE			nType : 3;							// what type is the entity	// 0x36 (2 == Vehicle)
    BYTE			nStatus : 5;						// control status			// 0x36
    //********* END CEntityInfo ************//

public:
	explicit inline CEntity(int dummy)
		: CPlaceable(dummy)
	{
		// Dummy ctor
	}

	inline short&	ModelIndex()
						{ return m_nModelIndex; };
	inline short	GetModelIndex()
					{ return m_nModelIndex; }

	void			UpdateRW();
	void			RegisterReference(CEntity** pAddress);
	void			CleanUpOldReference(CEntity** pAddress);
};

class NOVMT CPhysical : public CEntity
{
private:
    float			pad1; // 56
    int				__pad2; // 60

    unsigned int	b0x01 : 1; // 64
    unsigned int	bApplyGravity : 1;
    unsigned int	bDisableFriction : 1;
    unsigned int	bCollidable : 1;
    unsigned int	b0x10 : 1;
    unsigned int	bDisableMovement : 1;
    unsigned int	b0x40 : 1;
    unsigned int	b0x80 : 1;

    unsigned int	bSubmergedInWater : 1; // 65
    unsigned int	bOnSolidSurface : 1;
    unsigned int	bBroken : 1;
    unsigned int	b0x800 : 1; // ref @ 0x6F5CF0
    unsigned int	b0x1000 : 1;//
    unsigned int	b0x2000 : 1;//
    unsigned int	b0x4000 : 1;//
    unsigned int	b0x8000 : 1;//

    unsigned int	b0x10000 : 1; // 66
    unsigned int	b0x20000 : 1; // ref @ CPhysical__processCollision
    unsigned int	bBulletProof : 1;
    unsigned int	bFireProof : 1;
    unsigned int	bCollisionProof : 1;
    unsigned int	bMeeleProof : 1;
    unsigned int	bInvulnerable : 1;
    unsigned int	bExplosionProof : 1;

    unsigned int	b0x1000000 : 1; // 67
    unsigned int	bAttachedToEntity : 1;
    unsigned int	b0x4000000 : 1;
    unsigned int	bTouchingWater : 1;
    unsigned int	bEnableCollision : 1;
    unsigned int	bDestroyed : 1;
    unsigned int	b0x40000000 : 1;
    unsigned int	b0x80000000 : 1;

    CVector			m_vecLinearVelocity; // 68
    CVector			m_vecAngularVelocity; // 80
    CVector			m_vecCollisionLinearVelocity; // 92
    CVector			m_vecCollisionAngularVelocity; // 104
    CVector			m_vForce;							// 0x74
    CVector			m_vTorque;							// 0x80
	float			fMass;								// 0x8C
    float			fTurnMass;							// 0x90
    float			m_fVelocityFrequency;					// 0x94
    float			m_fAirResistance;						// 0x98
    float			m_fElasticity;						// 0x9C
    float			m_fBuoyancyConstant;					// 0xA0

    CVector			vecCenterOfMass;					// 0xA4
    DWORD			dwUnk1;								// 0xB0
    void*			unkCPtrNodeDoubleLink;				// 0xB4
    BYTE			byUnk: 8;								// 0xB8
    BYTE			byCollisionRecords: 8;					// 0xB9
    BYTE			byUnk2: 8;								// 0xBA (Baracus)
    BYTE			byUnk3: 8;								// 0xBB

    float			pad4[6];								// 0xBC

    float			fDistanceTravelled;					// 0xD4
    float			fDamageImpulseMagnitude;				// 0xD8
    CEntity*		damageEntity;						// 0xDC
    BYTE			pad2[28];								// 0xE0
    CEntity*		pAttachedEntity;					// 0xFC
    CVector			m_vAttachedPosition;				// 0x100
    CVector			m_vAttachedRotation;				// 0x10C
    BYTE			pad3[20];								// 0x118
    float			fLighting;							// 0x12C col lighting? CPhysical::GetLightingFromCol
    float			fLighting_2;							// 0x130 added to col lighting in CPhysical::GetTotalLighting
    BYTE			pad3a[4];								// 0x134

public:
	// Temp
	CPhysical()
	: CEntity(0) {}
};

// TODO: Move it away
class CKeyGen
{
public:
	static unsigned int		GetUppercaseKey(const char* pEntry);
};

// TODO: Move away?
class CGame
{
private:
	static bool&			bMissionPackGame;

public:
	static inline bool		IsMissionPackGame()
		{ return bMissionPackGame; }
};


bool CalcScreenCoors(const CVector& vecIn, CVector* vecOut);
void LoadingScreenLoadingFile(const char* pText);

extern CRGBA*				BaseColors;
extern RwCamera*&			Scene;
extern CCRC32				HashHelper;

static_assert(sizeof(CEntity) == 0x38, "Wrong size: CEntity");
static_assert(sizeof(CPhysical) == 0x138, "Wrong size: CPhysical");

#endif