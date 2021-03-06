#include "cbase.h"
#include "c_ball.h"
#include "c_sdk_player.h"
#include "fx_line.h"
#include "sdk_gamerules.h"
#include "c_team.h"

LINK_ENTITY_TO_CLASS(football, C_Ball);

IMPLEMENT_CLIENTCLASS_DT( C_Ball, DT_Ball, CBall )
	RecvPropInt( RECVINFO( m_iPhysicsMode ) ),
	RecvPropFloat( RECVINFO( m_fMass ) ),
	RecvPropEHandle(RECVINFO(m_pCreator)),
	RecvPropEHandle(RECVINFO(m_pCurrentPlayer)),
	RecvPropInt(RECVINFO(m_nCurrentTeam)),
	RecvPropBool(RECVINFO(m_bIsPlayerBall)),
	RecvPropInt(RECVINFO(m_eBallState)),
END_RECV_TABLE()

C_Ball *g_pBall = NULL;

C_Ball *GetBall()
{
	return g_pBall;
}

C_Ball::C_Ball()
{
	m_pCreator = NULL;
	m_bIsPlayerBall = false;
}

C_Ball::~C_Ball()
{
	if (!m_bIsPlayerBall)
		g_pBall = NULL;
}

void C_Ball::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged( updateType );

	if (!g_pBall && !m_bIsPlayerBall)
		g_pBall = this;

	return;
}

bool C_Ball::ShouldInterpolate()
{
	if (this == GetBall())
		return true;
	else
		return BaseClass::ShouldInterpolate();
}