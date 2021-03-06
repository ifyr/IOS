#ifndef c_IOS_REPLAYMANAGER_H
#define c_IOS_REPLAYMANAGER_H

#include "cbase.h"
#include "c_physicsprop.h"

class C_ReplayBall : public C_PhysicsProp
{
public:
	DECLARE_CLASS(C_ReplayBall, C_PhysicsProp);
	DECLARE_CLIENTCLASS();

	C_ReplayBall();
	~C_ReplayBall();
};

class C_ReplayPlayer : public CBaseAnimatingOverlay
{
public:
	DECLARE_CLASS(C_ReplayPlayer, CBaseAnimatingOverlay);
	DECLARE_CLIENTCLASS();

	int m_nTeamNumber;
	int m_nTeamPosNum;
};

class C_ReplayManager : public CBaseEntity
{
public:
	DECLARE_CLASS(C_ReplayManager, CBaseEntity);
	DECLARE_CLIENTCLASS();

	C_ReplayManager();
	~C_ReplayManager();
	bool IsReplaying();
	bool m_bIsReplaying;
	int	m_nReplayRunIndex;
	bool m_bAtMinGoalPos;
};

extern C_ReplayManager *GetReplayManager();

extern C_ReplayBall *GetReplayBall();

#endif