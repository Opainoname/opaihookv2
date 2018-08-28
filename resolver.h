#pragma once
#include "includes.h"
#include "Aimbot.h"
namespace Resolver
{
	static float lastmovinglby[65];
	auto IsNearEqual(float x, float y, float z) ->bool
	{
		return std::abs(x - y) <= std::abs(z);
	};

	auto air_bruteforce(CBaseEntity* entity) ->float
	{
		float yaw;
		switch (g_Aimbot->data[entity->GetIndex()].shoots % 5)
		{
		case 0: yaw = entity->LowerBodyYaw() - 180; break;
		case 1: yaw = entity->LowerBodyYaw() - 90; break;
		case 2: yaw = entity->LowerBodyYaw() + 90; break;
		case 3: yaw = entity->LowerBodyYaw() - 45; break;
		case 4: yaw = entity->LowerBodyYaw() + 45; break;
		}
		return yaw;
	};

	auto stand_bruteforce(CBaseEntity* entity, int shots) -> float
	{
		float yaw;
		switch (shots % 6)
		{
		case 0: yaw = entity->LowerBodyYaw() - 180; break;
		case 1: yaw = entity->LowerBodyYaw() + 180; break;
		case 2: yaw = entity->LowerBodyYaw() - 119; break;
		case 3: yaw = entity->LowerBodyYaw() + 119; break;
		case 4: yaw = entity->LowerBodyYaw() - 90; break;
		case 5: yaw = entity->LowerBodyYaw() + 90; break;
		}
		return yaw;
	};

	auto OverrideYaw() -> bool
	{
		if (!GetAsyncKeyState(Menu.Ragebot.ResolverOverride))
			return false;

		//yes
	};

	bool triggers_981(CBaseEntity* entity)
	{
		int seq_activity[64];

		for (int j = 0; j < 13; j++)
		{
			seq_activity[entity->GetIndex()] = entity->GetSequenceActivity(entity->GetAnimOverlay(j)->m_nSequence);

			if (seq_activity[entity->GetIndex()] == 981 && entity->GetAnimOverlay(j)->m_flWeight == 1)
			{
				return true;
			}
		}

		return false;
	}

	bool is_fakewalk(CBaseEntity* pEntity)
	{
		auto animstate = pEntity->GetBasePlayerAnimState();
		if (triggers_981(pEntity))
		{
			if (pEntity->GetAnimOverlay(12)->m_flWeight > 0)
			{
				if (pEntity->GetAnimOverlay(6)->m_flPlaybackRate < .0001)
				{
					return true;
				}
			}
		}
		return false;
	}

	void Do(CBaseEntity* entity)
	{
		int i = entity->Index();

		static int missed_shots[65];
		if (g_Aimbot->data[entity->Index()].shoots_hit != 0) {
			missed_shots[i] = g_Aimbot->data[entity->Index()].shoots - g_Aimbot->data[entity->Index()].shoots_hit;
		}

		static float new_last_moving_lby[65];

		static float lby_timer[65], last_moving_lby[65];

		static bool is_breaking_lby[65];

		if (!OverrideYaw())
		{
			auto activity = entity->GetSequenceActivity(entity->GetAnimOverlay(3)->m_nSequence);
			if (*entity->GetFlags() & FL_ONGROUND)
			{
				if (entity->IsMoving() && !is_fakewalk(entity))
				{
					lby_timer[i] = entity->GetSimulationTime() + 0.22f;
					last_moving_lby[i] = entity->LowerBodyYaw();
					entity->GetEyeAnglesPtr()->y = entity->LowerBodyYaw();
					is_breaking_lby[i] = false;
				}
				else
				{
					if (entity->GetAnimOverlay(3)->m_flWeight < 0.01f && entity->GetAnimOverlay(3)->m_flCycle < 0.69999f)
					{
						is_breaking_lby[i] = true;
						if (lby_timer[i] <= entity->GetSimulationTime())
							lby_timer[i] = entity->GetSimulationTime() + 1.1f;

						entity->GetEyeAnglesPtr()->y = entity->LowerBodyYaw();
						return;

					}

					if (missed_shots[i] >= 3)
					{
						entity->GetEyeAnglesPtr()->y = stand_bruteforce(entity, missed_shots[i]);
					}
					else
					{
						if (last_moving_lby[i] != new_last_moving_lby[i])
							entity->GetEyeAnglesPtr()->y = entity->LowerBodyYaw() - 180;//угол поменялса ок
						else
							entity->GetEyeAnglesPtr()->y = last_moving_lby[i];
					}
				}
				new_last_moving_lby[i] = last_moving_lby[i];
			}
			else
			{
				is_breaking_lby[i] = false;
				entity->GetEyeAnglesPtr()->y = air_bruteforce(entity);
			}
		}

		if (!Menu.Misc.AntiUT)
		{
			if (entity->GetEyeAnglesPtr()->x < -179.f) entity->GetEyeAnglesPtr()->x += 360.f;
			else if (entity->GetEyeAnglesPtr()->x > 90.0 || entity->GetEyeAnglesPtr()->x < -90.0) entity->GetEyeAnglesPtr()->x = 89.f;
			else if (entity->GetEyeAnglesPtr()->x > 89.0 && entity->GetEyeAnglesPtr()->x < 91.0) entity->GetEyeAnglesPtr()->x -= 90.f;
			else if (entity->GetEyeAnglesPtr()->x > 179.0 && entity->GetEyeAnglesPtr()->x < 181.0) entity->GetEyeAnglesPtr()->x -= 180;
			else if (entity->GetEyeAnglesPtr()->x > -179.0 && entity->GetEyeAnglesPtr()->x < -181.0) entity->GetEyeAnglesPtr()->x += 180;
			else if (fabs(entity->GetEyeAnglesPtr()->x) == 0) entity->GetEyeAnglesPtr()->x = std::copysign(89.0f, entity->GetEyeAnglesPtr()->x);
		}
	}
}