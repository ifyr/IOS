#include "cbase.h"
#include "ios_fieldbot.h"
#include "sdk_player.h"

LINK_ENTITY_TO_CLASS(ios_fieldbot, CFieldBot);

void CFieldBot::BotThink()
{
	QAngle angle;

	angle = GetLocalAngles();

	CBaseEntity *pEnt = gEntList.FindEntityByClassnameNearest("football", GetLocalOrigin(), 999999);
	if (!pEnt)
		return;

	Vector plballdir;
	Vector pldir;
	float closestDist = FLT_MAX;
	CSDKPlayer *pClosest = NULL;
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CSDKPlayer *pPlayer = ( CSDKPlayer *) UTIL_PlayerByIndex( i );
		if ( pPlayer &&
			pPlayer->GetTeamNumber() > LAST_SHARED_TEAM &&
			pPlayer->IsAlive() &&
			(pPlayer->m_TeamPos > 1))
		{
			float dist = GetLocalOrigin().DistTo(pPlayer->GetLocalOrigin());
			if (dist < closestDist)
			{
				closestDist = dist;
				pClosest = pPlayer;
				plballdir = pEnt->GetLocalOrigin() - pPlayer->GetLocalOrigin();
				pldir = pPlayer->GetLocalOrigin() - GetLocalOrigin();
				break;
			}
		}
	}

	if (!pClosest)
		return;

	Vector dir = pEnt->GetLocalOrigin() - GetLocalOrigin();
	m_cmd.buttons &= ~IN_ATTACK;
	m_cmd.buttons &= ~IN_ATTACK2;
	m_cmd.forwardmove = 0;
	float pitch = 0;

	if (plballdir.Length2D() > 150)
	{
		//m_cmd.forwardmove = clamp(dir.Length2D() / 2, PLAYER_WALKSPEED, PLAYER_SPRINTSPEED);
		pitch = clamp(plballdir.Length2D() / -50 + 10, -40, 10); //-45;

		if (dir.Length2D() > 50)
		{
			Vector target;
			target = pEnt->GetLocalOrigin() + (plballdir / plballdir.Length()) * 50;
			if (dir.Dot(plballdir) > 0) // < 90�
			{
				VectorAngles(Vector(dir.x, dir.y, 0), angle);
				Vector forward, right, up;
				AngleVectors(angle, &forward, &right, &up);
				target += right * 50;
			}
			dir = target - GetLocalOrigin();

			m_cmd.forwardmove = PLAYER_SPRINTSPEED / 3.5f;
			//m_cmd.buttons &= IN_SPEED;
		}
		else
		{
			dir = plballdir * -1;

			/*if (plballdir.Length2D() < 500)
			{
				m_cmd.buttons |= IN_ATTACK;
			}
			else
			{*/
				m_cmd.buttons |= IN_ATTACK2;
				m_cmd.powershot_strength = 100 * min(1, dir.Length2D() / 1000);
			//}

			m_cmd.forwardmove = PLAYER_RUNSPEED / 3.5f;
			//m_cmd.buttons &= ~IN_SPEED;
		}
	}

	VectorAngles(Vector(dir.x, dir.y, 0), angle);
	angle[PITCH] = pitch;
	m_LastAngles = angle;
	SetLocalAngles( angle );
	m_cmd.viewangles = angle;
}