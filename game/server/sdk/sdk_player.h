//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for SDK Game
//
// $NoKeywords: $
//=============================================================================//

#ifndef SDK_PLAYER_H
#define SDK_PLAYER_H
#pragma once

#include "basemultiplayerplayer.h"
#include "server_class.h"
#include "sdk_playeranimstate.h"
#include "sdk_player_shared.h"
#include "ball.h"
#include "sdk_gamerules.h"
#include "sdk_shareddefs.h"
#include "steam/steam_api.h"

const int MODEL_PLAYER			= 0;
const int MODEL_KEEPER			= 1;
const int MODEL_KEEPER_AND_BALL	= 2;


#define PLAYER_SPEED 280.0f

#define SPRINT_TIME           6.0f     //IOS sprint amount 5.5
#define SPRINT_RECHARGE_TIME  12.0f    //IOS time before sprint re-charges
#define SPRINT_SPEED          90.0f    //IOS sprint increase in speed

#define NUM_PLAYER_FACES 6
#define NUM_BALL_TYPES 6

class CBall;

class CPlayerPersistentData
{
public:
	const	CSteamID *m_SteamID;
	char	m_szSteamID[32];
	char	m_szName[MAX_PLAYER_NAME_LENGTH];
	int		m_nTeam;
	int		m_nNextCardJoin;
	int		m_nRedCards;
	int		m_nYellowCards;
	int		m_nFouls;
	int		m_nFoulsSuffered;
	int		m_nSlidingTackles;
	int		m_nSlidingTacklesCompleted;
	int		m_nGoalsConceded;
	int		m_nShots;
	int		m_nShotsOnGoal;
	int		m_nPassesCompleted;
	int		m_nInterceptions;
	int		m_nOffsides;
	int		m_nGoals;
	int		m_nOwnGoals;
	int		m_nAssists;
	float	m_flPossessionTime;
	int		m_nPossession;
	int		m_nDistanceCovered;
	int		m_nPasses;
	int		m_nFreeKicks;
	int		m_nPenalties;
	int		m_nCorners;
	int		m_nThrowIns;
	int		m_nKeeperSaves;
	int		m_nGoalKicks;
	float	m_flExactDistanceCovered;

//	CPlayerPersistentData(const CSteamID *steamID);
	static void ReallocateAllPlayerData();
	static void ConvertAllPlayerDataToJson();
	static void AllocateData(CSDKPlayer *pPl);
	static CUtlVector<CPlayerPersistentData *> m_PlayerPersistentData;
	void ResetData();
};

// Function table for each player state.
class CSDKPlayerStateInfo
{
public:
	SDKPlayerState m_iPlayerState;
	const char *m_pStateName;
	
	void (CSDKPlayer::*pfnEnterState)();	// Init and deinit the state.
	void (CSDKPlayer::*pfnLeaveState)();
	void (CSDKPlayer::*pfnPreThink)();	// Do a PreThink() in this state.
};

//=============================================================================
// >> SDK Game player
//=============================================================================
class CSDKPlayer : public CBaseMultiplayerPlayer
{
public:
	DECLARE_CLASS( CSDKPlayer, CBaseMultiplayerPlayer );
	DECLARE_SERVERCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CSDKPlayer();
	~CSDKPlayer();

	static CSDKPlayer *CreatePlayer( const char *className, edict_t *ed );
	static CSDKPlayer* Instance( int iEnt );

	// This passes the event to the client's and server's CPlayerAnimState.
	void DoServerAnimationEvent(PlayerAnimEvent_t event);
	void DoAnimationEvent(PlayerAnimEvent_t event);

	virtual void FlashlightTurnOn( void ) {};
	virtual void FlashlightTurnOff( void ) {};
	virtual int FlashlightIsOn( void ) { return 0; };

	virtual void PreThink();
	virtual void PostThink();
	virtual void Spawn();
	virtual void InitialSpawn();

	virtual void GiveDefaultItems() {};

	// Animstate handles this.
	void SetAnimation( PLAYER_ANIM playerAnim ); //ios {return; }

	virtual void Precache();
	virtual int			OnTakeDamage( const CTakeDamageInfo &inputInfo ) { return 0; };
	virtual int			OnTakeDamage_Alive( const CTakeDamageInfo &info ) { return 0; };
	virtual void Event_Killed( const CTakeDamageInfo &info ) {};
	virtual void TraceAttack( const CTakeDamageInfo &inputInfo, const Vector &vecDir, trace_t *ptr ) {};
	virtual void LeaveVehicle( const Vector &vecExitPoint, const QAngle &vecExitAngles ) {};
	
	virtual void	CreateViewModel( int viewmodelindex = 0 ) {};

	virtual void	CheatImpulseCommands( int iImpulse );
	
	virtual int		SpawnArmorValue( void ) const { return m_iSpawnArmorValue; }
	virtual void	SetSpawnArmorValue( int i ) { m_iSpawnArmorValue = i; }

	CNetworkQAngle( m_angEyeAngles );	// Copied from EyeAngles() so we can send it to the client.
	CNetworkVar( int, m_iShotsFired );	// number of shots fired recently

	// Tracks our ragdoll entity.
	CNetworkHandle( CBaseEntity, m_hRagdoll );	// networked entity handle 
#if defined ( SDK_USE_PLAYERCLASSES )
	int GetPlayerClassAsString( char *pDest, int iDestSize );
	void SetClassMenuOpen( bool bIsOpen );
	bool IsClassMenuOpen( void );
#endif
	void PhysObjectSleep();
	void PhysObjectWake();

	virtual void ChangeTeam( int iTeamNum );

	// Player avoidance
	virtual	bool		ShouldCollide( int collisionGroup, int contentsMask ) const;
	void SDKPushawayThink(void);

// In shared code.
public:
	void FireBullet( 
		Vector vecSrc, 
		const QAngle &shootAngles, 
		float vecSpread, 
		int iDamage, 
		int iBulletType,
		CBaseEntity *pevAttacker,
		bool bDoEffects,
		float x,
		float y ) {};

	CNetworkVarEmbedded( CSDKPlayerShared, m_Shared );
	virtual void			PlayerDeathThink( void ) {};
	virtual bool		ClientCommand( const CCommand &args );

	void IncreaseShotsFired() { m_iShotsFired++; if (m_iShotsFired > 16) m_iShotsFired = 16; }
	void DecreaseShotsFired() { m_iShotsFired--; if (m_iShotsFired < 0) m_iShotsFired = 0; }
	void ClearShotsFired() { m_iShotsFired = 0; }
	int GetShotsFired() { return m_iShotsFired; }

#if defined ( SDK_USE_SPRINTING )
	void SetSprinting( bool bIsSprinting );
#endif // SDK_USE_SPRINTING
	// Returns true if the player is allowed to attack.
	bool CanAttack( void ) { return false; };

	virtual int GetPlayerStance();

	// Called whenever this player fires a shot.
	void NoteWeaponFired() {};
	virtual bool WantsLagCompensationOnEntity( const CBasePlayer *pPlayer, const CUserCmd *pCmd, const CBitVec<MAX_EDICTS> *pEntityTransmitBits ) const;

// ------------------------------------------------------------------------------------------------ //
// Player state management.
// ------------------------------------------------------------------------------------------------ //
public:

	CPlayerPersistentData *m_pData;

	void State_Transition( SDKPlayerState newState );
	SDKPlayerState State_Get() const;				// Get the current state.

	virtual bool	ModeWantsSpectatorGUI( int iMode ) { return ( iMode != OBS_MODE_DEATHCAM && iMode != OBS_MODE_FREEZECAM ); }

	virtual const Vector	GetPlayerMins( void ) const; // uses local player
	virtual const Vector	GetPlayerMaxs( void ) const; // uses local player

	virtual bool		CanSpeak(bool isTeamOnly);
	virtual void		SetPlayerName(const char *name);

private:
	//ios bool SelectSpawnSpot( const char *pEntClassName, CBaseEntity* &pSpot );

	void State_Enter( SDKPlayerState newState );	// Initialize the new state.
	void State_Leave();								// Cleanup the previous state.
	void State_PreThink();							// Update the current state.

	void State_PICKINGTEAM_Enter();
	void State_PICKINGCLASS_Enter();

private:

	void State_ACTIVE_Enter();
	void State_ACTIVE_PreThink();

	void State_OBSERVER_MODE_Enter();
	void State_OBSERVER_MODE_Leave();
	void State_OBSERVER_MODE_PreThink();

	// Find the state info for the specified state.
	static CSDKPlayerStateInfo* State_LookupInfo( SDKPlayerState state );

	// This tells us which state the player is currently in (joining, observer, dying, etc).
	// Each state has a well-defined set of parameters that go with it (ie: observer is movetype_noclip, non-solid,
	// invisible, etc).
	CNetworkVar( SDKPlayerState, m_iPlayerState );
	CNetworkVar(float, m_flStateEnterTime);

	CSDKPlayerStateInfo *m_pCurStateInfo;			// This can be NULL if no state info is defined for m_iPlayerState.
	bool HandleCommand_JoinTeam( int iTeam );

#if defined ( SDK_USE_PLAYERCLASSES )
	bool HandleCommand_JoinClass( int iClass );
	void ShowClassSelectMenu();
	bool m_bIsClassMenuOpen;
#endif

#if defined ( SDK_USE_PRONE )
	void InitProne( void );
#endif // SDK_USE_PRONE

#if defined ( SDK_USE_SPRINTING )
	void InitSprinting( void );
	bool IsSprinting( void );
	bool CanSprint( void );
#endif // SDK_USE_SPRINTING

	void InitSpeeds( void ); //Tony; called EVERY spawn on server and client after class has been chosen (if any!)

	// from CBasePlayer
	void			SetupVisibility( CBaseEntity *pViewEntity, unsigned char *pvs, int pvssize );

	virtual void	SharedSpawn();

private:
	// Last usercmd we shot a bullet on.
	int m_iLastWeaponFireUsercmd;

	virtual void Weapon_Equip( CBaseCombatWeapon *pWeapon ) {};		//Tony; override so diethink can be cleared
	virtual void ThrowActiveWeapon( void ) {};
	virtual void SDKThrowWeapon( CWeaponSDKBase *pWeapon, const Vector &vecForward, const QAngle &vecAngles, float flDiameter  ) {};
	virtual void SDKThrowWeaponDir( CWeaponSDKBase *pWeapon, const Vector &vecForward, Vector *pVecThrowDir ) {};
	// When the player joins, it cycles their view between trigger_camera entities.
	// This is the current camera, and the time that we'll switch to the next one.
	EHANDLE m_pIntroCamera;
	float m_fIntroCamTime;

	void CreateRagdollEntity() {};
	void DestroyRagdoll( void ) {};


	CSDKPlayerAnimState *m_PlayerAnimState;

	CNetworkVar( bool, m_bSpawnInterpCounter );

	int m_iSpawnArmorValue;
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_ArmorValue );
public:
#if defined ( SDK_USE_PRONE )
	bool m_bUnProneToDuck;		//Tony; GAMEMOVEMENT USED VARIABLE
#endif // SDK_USE_PRONE

public:
	//virtual void		SetAnimation( PLAYER_ANIM playerAnim );
	void				PlayerUse ( void ) {};				//IOS
	CBaseEntity			*FindUseEntity(void) { return NULL; };
	//bool				ClientCommand(const char *pcmd);
	//CBaseEntity			*EntSelectSpawnPoint(void);

	void				ChoosePlayerSkin(void);
	void				ChooseKeeperSkin(void);
	bool				IsTeamPosFree(int team, int posIndex, bool ignoreBots, CSDKPlayer **pPlayerOnPos);

	int					m_nTeamPosIndex;
	int					m_nTeamPosNum;
	int					m_nPreferredTeamPosNum;
	int					m_nPreferredSkin;
	
	float				m_flNextShot;

	inline CPlayerPersistentData *GetData() { return m_pData; }
	inline void					 SetData(CPlayerPersistentData *data) { m_pData = data; }

	int					GetRedCards(void) { return GetData()->m_nRedCards; }
	void				SetRedCards(int amount) { GetData()->m_nRedCards = amount; }

	int					GetYellowCards(void) { return GetData()->m_nYellowCards; }
	void				SetYellowCards(int amount) { GetData()->m_nYellowCards = amount; }

	int					GetFouls(void) { return GetData()->m_nFouls; }
	void				SetFouls(int amount) { GetData()->m_nFouls = amount; }

	int					GetFoulsSuffered(void) { return GetData()->m_nFoulsSuffered; }
	void				SetFoulsSuffered(int amount) { GetData()->m_nFoulsSuffered = amount; }

	int					GetSlidingTackles(void) { return GetData()->m_nSlidingTackles; }
	void				SetSlidingTackles(int amount) { GetData()->m_nSlidingTackles = amount; }

	int					GetSlidingTacklesCompleted(void) { return GetData()->m_nSlidingTacklesCompleted; }
	void				SetSlidingTacklesCompleted(int amount) { GetData()->m_nSlidingTacklesCompleted = amount; }

	int					GetGoalsConceded(void) { return GetData()->m_nGoalsConceded; }
	void				SetGoalsConceded(int amount) { GetData()->m_nGoalsConceded = amount; }

	int					GetShots(void) { return GetData()->m_nShots; }
	void				SetShots(int amount) { GetData()->m_nShots = amount; }

	int					GetShotsOnGoal(void) { return GetData()->m_nShotsOnGoal; }
	void				SetShotsOnGoal(int amount) { GetData()->m_nShotsOnGoal = amount; }

	int					GetPassesCompleted(void) { return GetData()->m_nPassesCompleted; }
	void				SetPassesCompleted(int amount) { GetData()->m_nPassesCompleted = amount; }

	int					GetInterceptions(void) { return GetData()->m_nInterceptions; }
	void				SetInterceptions(int amount) { GetData()->m_nInterceptions = amount; }

	int					GetOffsides(void) { return GetData()->m_nOffsides; }
	void				SetOffsides(int amount) { GetData()->m_nOffsides = amount; }

	int					GetGoals(void) { return GetData()->m_nGoals; }
	void				SetGoals(int amount) { GetData()->m_nGoals = amount; }

	int					GetOwnGoals(void) { return GetData()->m_nOwnGoals; }
	void				SetOwnGoals(int amount) { GetData()->m_nOwnGoals = amount; }

	int					GetAssists(void) { return GetData()->m_nAssists; }
	void				SetAssists(int amount) { GetData()->m_nAssists = amount; }

	float				GetPossessionTime(void) { return GetData()->m_flPossessionTime; }
	void				SetPossessionTime(float time) { GetData()->m_flPossessionTime = time; }

	int					GetPossession(void) { return GetData()->m_nPossession; }
	void				SetPossession(int amount) { GetData()->m_nPossession = amount; }

	float				GetExactDistanceCovered(void) { return GetData()->m_flExactDistanceCovered; }
	void				SetExactDistanceCovered(float amount) { GetData()->m_flExactDistanceCovered = amount; }

	int					GetDistanceCovered(void) { return GetData()->m_nDistanceCovered; }
	void				SetDistanceCovered(int amount) { GetData()->m_nDistanceCovered = amount; }

	int					GetPasses(void) { return GetData()->m_nPasses; }
	void				SetPasses(int amount) { GetData()->m_nPasses = amount; }

	int					GetFreeKicks(void) { return GetData()->m_nFreeKicks; }
	void				SetFreeKicks(int amount) { GetData()->m_nFreeKicks = amount; }

	int					GetPenalties(void) { return GetData()->m_nPenalties; }
	void				SetPenalties(int amount) { GetData()->m_nPenalties = amount; }

	int					GetCorners(void) { return GetData()->m_nCorners; }
	void				SetCorners(int amount) { GetData()->m_nCorners = amount; }

	int					GetThrowIns(void) { return GetData()->m_nThrowIns; }
	void				SetThrowIns(int amount) { GetData()->m_nThrowIns = amount; }

	int					GetKeeperSaves(void) { return GetData()->m_nKeeperSaves; }
	void				SetKeeperSaves(int amount) { GetData()->m_nKeeperSaves = amount; }

	int					GetGoalKicks(void) { return GetData()->m_nGoalKicks; }
	void				SetGoalKicks(int amount) { GetData()->m_nGoalKicks = amount; }

	int					GetNextCardJoin(void) { return GetData()->m_nNextCardJoin; }
	void				SetNextCardJoin(int seconds) { GetData()->m_nNextCardJoin = seconds; }

	const char			*GetLastKnownName() { return GetData()->m_szName; }
	void				SetLastKnownName(const char *name) { Q_strncpy(GetData()->m_szName, name, MAX_PLAYER_NAME_LENGTH); }

	int					GetLastKnownTeam() { return GetData()->m_nTeam; }
	void				SetLastKnownTeam(int team) { GetData()->m_nTeam = team; }

	float				GetNextJoin() { return m_flNextJoin; }
	void				SetNextJoin(float time) { m_flNextJoin = time; }

	int					GetTeamPosNum(void);

	int					GetTeamPosType(void);

	int					GetTeamPosIndex(void) { return m_nTeamPosIndex; }

	int					GetTeamToJoin(void) { return m_nTeamToJoin; }

	int					GetTeamPosIndexToJoin(void) { return m_nTeamPosIndexToJoin; }

	void				SetPreferredTeamPosNum(int num) { m_nPreferredTeamPosNum = clamp(num, 0, 11); }
	int					FindUnfilledTeamPosNum();

	int					GetPreferredSkin() { return m_nPreferredSkin; }
	void				SetPreferredSkin(int num);

	void				FindSafePos(Vector &startPos);

	void				ResetFlags();

	char				*GetClubName() { return m_szClubName; }
	void				SetClubName(const char *name) { Q_strncpy(m_szClubName, name, sizeof(m_szClubName)); m_bClubNameChanged = true; } 

	int					GetCountryName() { return m_nCountryName; }
	void				SetCountryName(int name) { m_nCountryName = name; } 
	bool				IsLegacySideCurl() { return m_bLegacySideCurl; } 
	void				SetLegacySideCurl(bool enable) { m_bLegacySideCurl = enable; } 

	bool				ShotButtonsPressed();

	virtual bool		ShotButtonsReleased();
	virtual void		SetShotButtonsReleased(bool released);

	int					m_nBaseSkin;					//keeper skin before ball offset added

	void				LookAtBall(void);

	float				m_flRemoteControlledStartTime;

	virtual void		VPhysicsCollision( int index, gamevcollisionevent_t *pEvent );

	//int					m_BallInPenaltyBox;	 //-1 =	not	in box,	0,1	= teams	box

	char				m_szClubName[MAX_CLUBNAME_LENGTH];
	bool				m_bClubNameChanged;

	int					m_nCountryName;

	bool				m_bLegacySideCurl;

	Vector				EyeDirection2D();
	Vector				EyeDirection3D();

	const Vector		GetVisualLocalOrigin();

	void				SetOffside(bool isOffside);
	void				SetOffsidePos(Vector pos);
	bool				IsOffside();
	Vector				GetOffsidePos();
	void				SetOffsideLastOppPlayerPos(Vector pos);
	Vector				GetOffsideLastOppPlayerPos();
	void				SetOffsideBallPos(Vector pos);
	Vector				GetOffsideBallPos();
	
	void				SetPosInsideShield(const Vector &pos, bool holdAtTargetPos);
	void				SetPosOutsideShield();
	void				SetPosOutsideBall(const Vector &playerPos);
	void				GetTargetPos(const Vector &pos, Vector &targetPos);
	void				ActivateRemoteControlling(const Vector &targetPos);

	Vector				GetSpawnPos(bool findSafePos);

	bool				IsAway() { return m_bIsAway; }

	CNetworkVector(m_vTargetPos);
	CNetworkVar(bool, m_bIsAtTargetPos);
	CNetworkVar(bool, m_bHoldAtTargetPos);
	CNetworkVar(float, m_flNextClientSettingsChangeTime);

	CNetworkVar(int, m_nInPenBoxOfTeam);

	static bool			IsOnField(CSDKPlayer *pPl);

	void				CheckShotCharging();
	void				ResetShotCharging();

	int					m_nTeamToJoin;
	int					m_nTeamPosIndexToJoin;

	int					m_ePenaltyState;
	void				SetPlayerBall(CBall *pPlayerBall) { m_pPlayerBall = pPlayerBall; }
	CBall				*GetPlayerBall() { return m_pPlayerBall; }
	bool				ChangeTeamPos(int team, int posIndex, bool setJoinDelay);

	void				CheckBallShield(const Vector &oldPos, Vector &newPos, const Vector &oldVel, Vector &newVel, const QAngle &oldAng, QAngle &newAng);
	void				MoveToTargetPos(Vector &pos, Vector &vel, QAngle &ang);

	bool				IsNormalshooting(bool noShotOnCancel = false);
	bool				IsPowershooting(bool noShotOnCancel = false);
	bool				IsChargedshooting();
	bool				IsAutoPassing();
	bool				IsShooting();
	CSDKPlayer			*FindClosestPlayerToSelf(bool teammatesOnly, bool forwardOnly = false, float maxYawAngle = 360);

	CHandle<CBall>		m_pHoldingBall;

	Vector				m_vPreReplayPos;
	QAngle				m_aPreReplayAngles;

	int					m_nMotmChoiceIds[2];

	float				m_flLastShotOnGoal;

protected:

	bool				m_bIsAway;
	float				m_flLastMoveTime;
	bool				m_bIsOffside;
	Vector				m_vOffsidePos;
	CHandle<CBall>		m_pPlayerBall;
	Vector				m_vOffsideLastOppPlayerPos;
	Vector				m_vOffsideBallPos;

	bool				m_bShotButtonsReleased;

	float				m_flNextJoin;
};


inline CSDKPlayer *ToSDKPlayer( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return NULL;

#ifdef _DEBUG
	Assert( dynamic_cast<CSDKPlayer*>( pEntity ) != 0 );
#endif
	return static_cast< CSDKPlayer* >( pEntity );
}

inline SDKPlayerState CSDKPlayer::State_Get() const
{
	return m_iPlayerState;
}

#endif	// SDK_PLAYER_H
