#include "hooks.h"
#include <time.h>
#include "Mmsystem.h"
#include <thread>
#include "global.h"
#include "Menu.h"
#include "controls.h"
#include "ESP.h"
#include "Math.h"
#include "bullet_log.h"
#include "event_log.h"
#include "AW_hitmarker.h"
#include "sounds.h"
#include "Draw.h"
#include "BacktrackingHelper.h"
#include "Aimbot.h"

#pragma comment(lib, "winmm.lib") 

string prefix = "[opaihook] ";

std::vector<impact_info> impacts;
std::vector<hitmarker_info> hitmarkers;

#define EVENT_HOOK( x )
#define TICK_INTERVAL			(g_pGlobals->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

cGameEvent g_Event;

std::vector<cbullet_tracer_info> logs;

std::vector<trace_info> trace_logs;

std::vector <CMessage> Eventlog::messages;


CBaseEntity* get_player(int userid) {
	int i = g_pEngine->GetPlayerForUserID(userid);
	return g_pEntitylist->GetClientEntity(i);
}

float cur() {
	int g_tick = 0;
	CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = csgo::LocalPlayer->GetTickBase();
	}
	else {
		++g_tick;
	}
	g_pLastCmd = csgo::UserCmd;
	float curtime = g_tick * g_pGlobals->interval_per_tick;
	return curtime;
}

int showtime = 3;
namespace Eventlog
{

	void Msg(string str, bool attack)
	{
		Color clr = Color::Green();
		messages.push_back(CMessage(str, cur(), clr, 255));
		g_pEngine->ClientCmd_Unrestricted(("echo " + prefix + str).c_str());
	}
	void Draw()
	{
		for (int i = messages.size() - 1; i >= 0; i--)
		{
			if (messages[i].time + showtime <= g_pGlobals->curtime)
				messages[i].alpha -= 2;

			draw::Textf(70, 20 + (15 * i), Color(255, 255, 255, messages[i].alpha), F::eventlog, (char*)messages[i].str.c_str());
			draw::Textf(10, 20 + (15 * i), Color(messages[i].clr.r(), messages[i].clr.g(), messages[i].clr.b(), messages[i].alpha), F::eventlog, prefix.c_str());

			if (messages[i].alpha <= 0)
				messages.erase(messages.begin() + i);
		}
	}
}


char* HitgroupToName(int hitgroup)
{
	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		return "head";
	case HITGROUP_CHEST:
		return "chest";
	case HITGROUP_STOMACH:
		return "stomach";
	case HITGROUP_LEFTARM:
		return "left arm";
	case HITGROUP_RIGHTARM:
		return "right arm";
	case HITGROUP_LEFTLEG:
		return "left leg";
	case HITGROUP_RIGHTLEG:
		return "right leg";
	default:
		return "body";

	}
}

player_info_t GetInfo(int Index)
{
	player_info_t Info;
	g_pEngine->GetPlayerInfo(Index, &Info);
	return Info;
}

void Hurt(IGameEvent* Event)
{
	int attackerid = Event->GetInt("attacker");
	int entityid = g_pEngine->GetPlayerForUserID(attackerid);
	if (entityid == g_pEngine->GetLocalPlayer())
	{
		int nUserID = Event->GetInt("attacker");
		int nDead = Event->GetInt("userid");
		if (nUserID || nDead)
		{
			player_info_t killed_info = GetInfo(g_pEngine->GetPlayerForUserID(nDead));
			player_info_t killer_info = GetInfo(g_pEngine->GetPlayerForUserID(nUserID));

			std::string _1 = "Hit ";
			std::string _2 = killed_info.m_szPlayerName;
			std::string _3 = " at ";
			std::string _4 = HitgroupToName(Event->GetInt("hitgroup"));
			std::string _5 = " for ";
			std::string _6 = Event->GetString("dmg_health");
			std::string _7 = " damage";
			std::string _8 = " (";
			std::string _9 = Event->GetString("health");
			std::string _10 = " health remaining)";
			string out = _1 + _2 + _3 + _4 + _5 + _6 + _7 + _8 + _9 + _10;

			if (Event->GetInt("health") > 0)
				Eventlog::Msg(out, false);
			else
			{
				std::string _1 = "Killed ";
				std::string _2 = killed_info.m_szPlayerName;
				std::string _3 = " at ";
				std::string _4 = HitgroupToName(Event->GetInt("hitgroup"));
				string out = _1 + _2 + _3 + _4;
				Eventlog::Msg(out, true);
			}
		}

	}
}

void cGameEvent::FireGameEvent(IGameEvent *event)
{
	/*short   m_nUserID		user ID who was hurt
	short	attacker	user ID who attacked
	byte	health		remaining health points
	byte	armor		remaining armor points
	string	weapon		weapon name attacker used, if not the world
	short	dmg_health	damage done to health
	byte	dmg_armor	damage done to armor
	byte	hitgroup	hitgroup that was damaged*/
	const char* szEventName = event->GetName();
	if (!szEventName)
		return;

	if (!strcmp(szEventName, "round_start"))
		csgo::NewRound = true;
	else
		csgo::NewRound = false;

	if (strcmp(szEventName, "player_hurt") == 0)
	{
		Hurt(event);

		auto entity = g_pEntitylist->GetClientEntity(g_pEngine->GetPlayerForUserID(event->GetInt("userid")));
		if (!entity)
			return;

		if (entity->GetTeamNum() == csgo::LocalPlayer->GetTeamNum())
			return;

		auto attacker = get_player(event->GetInt("attacker"));
		if (attacker == csgo::LocalPlayer)
		{
			g_Aimbot->data[entity->Index()].shoots_hit++;
		}
	}

	if (Menu.Visuals.Hitmarker)
	{
		if (strcmp(szEventName, "player_hurt") == 0)
		{
			auto attacker = get_player(event->GetInt("attacker"));
			auto victim = get_player(event->GetInt("userid"));

			if (!attacker || !victim || attacker != csgo::LocalPlayer)
				return;

			Vector enemypos = victim->GetOrigin();
			impact_info best_impact;
			float best_impact_distance = -1;
			float time = g_pGlobals->curtime;

			for (int i = 0; i < impacts.size(); i++)
			{
				auto iter = impacts[i];
				if (time > iter.time + 1.f)
				{
					impacts.erase(impacts.begin() + i);
					continue;
				}
				Vector position = Vector(iter.x, iter.y, iter.z);
				float distance = position.DistTo(enemypos);
				if (distance < best_impact_distance || best_impact_distance == -1)
				{
					best_impact_distance = distance;
					best_impact = iter;
				}
			}
			if (best_impact_distance == -1)
				return;

			hitmarker_info info;
			info.impact = best_impact;
			info.alpha = 255;

			hitmarkers.push_back(info);

			int Attacker = event->GetInt("attacker");

			if (g_pEngine->GetPlayerForUserID(Attacker) == g_pEngine->GetLocalPlayer())
			{

				switch (Menu.Visuals.htSound)
				{
				case 0:
					PlaySoundA(hitmarker_sound, nullptr, SND_MEMORY | SND_ASYNC | SND_NOSTOP);
					break;
				case 1:
					PlaySoundA(hitmarker_sound2, nullptr, SND_MEMORY | SND_ASYNC | SND_NOSTOP);
					break;
				case 2:
					PlaySoundA(hitmarker_sound3, nullptr, SND_MEMORY | SND_ASYNC | SND_NOSTOP);
					break;
				case 3:
					PlaySoundA(water, nullptr, SND_MEMORY | SND_ASYNC | SND_NOSTOP);
					break;
				case 4:
					g_pEngine->ClientCmd_Unrestricted("play buttons\\arena_switch_press_02.wav");
					break;
				case 5:
					PlaySoundA(stapler, nullptr, SND_MEMORY | SND_ASYNC | SND_NOSTOP);
					break;
				case 6:
					PlaySoundA(vk, nullptr, SND_MEMORY | SND_ASYNC | SND_NOSTOP);
					break;
				}
			}
		}
	}

	if (strstr(event->GetName(), "bullet_impact"))
	{
		CBaseEntity* ent = get_player(event->GetInt("userid"));
		if (!ent || ent != csgo::LocalPlayer)
			return;

		impact_info info;
		info.x = event->GetFloat("x");
		info.y = event->GetFloat("y");
		info.z = event->GetFloat("z");

		info.time = g_pGlobals->curtime;

		impacts.push_back(info);
	}

	if (Menu.Visuals.BulletTracers)
	{
		if (Menu.Visuals.bulletType == 0)
		{
			if (strcmp(szEventName, "bullet_impact") == 0)
			{
				auto* index = g_pEntitylist->GetClientEntity(g_pEngine->GetPlayerForUserID(event->GetInt("userid")));

				if (csgo::LocalPlayer)
				{
					Vector position(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));

					if (index)

						trace_logs.push_back(trace_info(index->GetEyePosition(), position, g_pGlobals->curtime, event->GetInt("userid")));
				}
			}
		}
		else if (Menu.Visuals.bulletType == 1)
		{
			if (strstr(event->GetName(), "bullet_impact"))
			{
				//get the user who fired the bullet
				auto index = g_pEngine->GetPlayerForUserID(event->GetInt("userid"));

				//return if the userid is not valid or we werent the entity who was fireing
				if (index != g_pEngine->GetLocalPlayer())
					return;

				//get local player
				auto local = static_cast<CBaseEntity*>(g_pEntitylist->GetClientEntity(index));
				if (!local)
					return;

				//get the bullet impact's position
				Vector position(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));

				Ray_t ray;
				ray.Init(local->GetEyePosition(), position);

				//skip local player
				CTraceFilter filter;
				filter.pSkip1 = local;

				//trace a ray
				trace_t tr;
				g_pEngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

				float Red, Green, Blue;
				Red = Menu.Colors.Bulletracer[0] * 255;
				Green = Menu.Colors.Bulletracer[1] * 255;
				Blue = Menu.Colors.Bulletracer[2] * 255;

				//push info to our vector
				logs.push_back(cbullet_tracer_info(local->GetEyePosition(), position, g_pGlobals->curtime, Color(Red, Green, Blue)));
			}
		}
	}
}


int cGameEvent::GetEventDebugID()
{
	return 42;
}

void cGameEvent::RegisterSelf()
{
	g_pGameEventManager->AddListener(this, "player_connect", false);
	g_pGameEventManager->AddListener(this, "player_hurt", false);
	g_pGameEventManager->AddListener(this, "round_start", false);
	g_pGameEventManager->AddListener(this, "round_end", false);
	g_pGameEventManager->AddListener(this, "player_death", false);
	g_pGameEventManager->AddListener(this, "weapon_fire", false);
	g_pGameEventManager->AddListener(this, "bullet_impact", false);
}

void cGameEvent::Register()
{
	EVENT_HOOK(FireEvent);
}
