#include "sdk.h"
#include "Aimbot.h"
#include "global.h"
#include "Menu.h"
#include "Math.h"
#include "GameUtils.h"
#include "Autowall.h"
#include "BacktrackingHelper.h"
#include "NoSpread.h"

#define TICK_INTERVAL			( g_pGlobals->interval_per_tick )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )

template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}
#define PI_F	((float)(PI)) 
void VectorAngles(const Vector& forward, Vector& up, QAngle& angles)
{
	Vector left = CrossProduct(up, forward);
	left.NormalizeInPlace();

	float forwardDist = forward.Length2D();

	if (forwardDist > 0.001f)
	{
		angles.x = atan2f(-forward.z, forwardDist) * 180 / PI_F;
		angles.y = atan2f(forward.y, forward.x) * 180 / PI_F;

		float upZ = (left.y * forward.x) - (left.x * forward.y);
		angles.z = atan2f(left.z, upZ) * 180 / PI_F;
	}
	else
	{
		angles.x = atan2f(-forward.z, forwardDist) * 180 / PI_F;
		angles.y = atan2f(-left.x, left.y) * 180 / PI_F;
		angles.z = 0;
	}
}


bool hitchance(QAngle angles, CBaseEntity *ent, float chance)
{
	auto weapon = csgo::MainWeapon;

	if (!weapon)
		return false;

	if (Menu.Ragebot.NoSpread)
		return true;

	Vector forward, right, up;
	Vector src = csgo::LocalPlayer->GetEyePosition();
	Math::AngleVectors(angles, forward, right, up);

	int cHits = 0;
	int cNeededHits = static_cast<int>(150.f * (chance / 100.f));

	weapon->UpdateAccuracyPenalty();
	float weap_spread = weapon->GetSpread();
	float weap_inaccuracy = weapon->GetInaccuracy();

	for (int i = 0; i < 150; i++)
	{
		float a = Math::RandomFloat(0.f, 1.f);
		float b = Math::RandomFloat(0.f, 2.f * PI_F);
		float c = Math::RandomFloat(0.f, 1.f);
		float d = Math::RandomFloat(0.f, 2.f * PI_F);

		float inaccuracy = a * weap_inaccuracy;
		float spread = c * weap_spread;

		if (weapon->GetItemDefinitionIndex() == 64)
		{
			a = 1.f - a * a;
			a = 1.f - c * c;
		}

		Vector spreadView((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0), direction;

		direction.x = forward.x + (spreadView.x * right.x) + (spreadView.y * up.x);
		direction.y = forward.y + (spreadView.x * right.y) + (spreadView.y * up.y);
		direction.z = forward.z + (spreadView.x * right.z) + (spreadView.y * up.z);
		direction.Normalized();

		QAngle viewAnglesSpread;
		VectorAngles(direction, up, viewAnglesSpread);
		Math::NormalizeAngle(viewAnglesSpread);

		Vector viewForward;
		Math::AngleVectors(viewAnglesSpread, viewForward);
		viewForward.NormalizeInPlace();

		viewForward = src + (viewForward * weapon->GetCSWpnData()->range);

		trace_t tr;
		Ray_t ray;

		ray.Init(src, viewForward);
		g_pEngineTrace->ClipRayToCBaseEntity(ray, MASK_SHOT | CONTENTS_GRATE, ent, &tr);

		if (tr.m_pEnt == ent)
			++cHits;

		if (static_cast<int>((static_cast<float>(cHits) / 150.f) * 100.f) >= chance)
			return true;

		if ((150 - i + cHits) < cNeededHits)
			return false;
	}
	return false;
}

std::vector<int> GetHitboxesToScan(CBaseEntity* pTarget)
{
	std::vector<int> HitBoxesToScan;
	int HitScanMode = Menu.Ragebot.Hitscan;
	int Aimspot = Menu.Ragebot.AimbotSelection;

	switch (HitScanMode) {
	case 0:
		break;
	case 1:
		HitBoxesToScan.push_back((int)CSGOHitboxID::HEAD);
		HitBoxesToScan.push_back((int)CSGOHitboxID::NECK);
		HitBoxesToScan.push_back((int)CSGOHitboxID::UPPER_CHEST);
		HitBoxesToScan.push_back((int)CSGOHitboxID::PELVIS);
		break;
	case 2:
		HitBoxesToScan.push_back((int)CSGOHitboxID::HEAD);
		HitBoxesToScan.push_back((int)CSGOHitboxID::NECK);
		HitBoxesToScan.push_back((int)CSGOHitboxID::UPPER_CHEST);
		HitBoxesToScan.push_back((int)CSGOHitboxID::PELVIS);
		HitBoxesToScan.push_back((int)CSGOHitboxID::BELLY);

		HitBoxesToScan.push_back((int)CSGOHitboxID::THORAX);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_THIGH);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_THIGH);
	case 3:
		HitBoxesToScan.push_back((int)CSGOHitboxID::HEAD);
		HitBoxesToScan.push_back((int)CSGOHitboxID::NECK);
		HitBoxesToScan.push_back((int)CSGOHitboxID::UPPER_CHEST);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LOWER_CHEST);
		HitBoxesToScan.push_back((int)CSGOHitboxID::PELVIS);
		HitBoxesToScan.push_back((int)CSGOHitboxID::BELLY);

		HitBoxesToScan.push_back((int)CSGOHitboxID::THORAX);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_THIGH);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_THIGH);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_CALF);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_CALF);
		break;
	case 4:
	{
		HitBoxesToScan.push_back((int)CSGOHitboxID::HEAD);
		HitBoxesToScan.push_back((int)CSGOHitboxID::NECK);
		HitBoxesToScan.push_back((int)CSGOHitboxID::UPPER_CHEST);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LOWER_CHEST);
		HitBoxesToScan.push_back((int)CSGOHitboxID::PELVIS);
		HitBoxesToScan.push_back((int)CSGOHitboxID::BELLY);

		HitBoxesToScan.push_back((int)CSGOHitboxID::THORAX);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_THIGH);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_THIGH);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_CALF);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_CALF);

		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_FOOT);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_FOOT);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_HAND);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_HAND);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_UPPER_ARM);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RIGHT_FOREARM);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_UPPER_ARM);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LEFT_FOREARM);
	}
	break;
	}

	return HitBoxesToScan;
}

void SetRecords(CBaseEntity* ent, tick_record record)
{
	ent->InvalidateBoneCache();
	ent->SetAbsOrigin(record.m_vecAbsOrigin);
	ent->SetAngle2(record.m_absangles);
	ent->SetVelocity(record.m_vecVelocity);
	ent->SetFlags(record.m_nFlags);
}

void Restore(CBaseEntity* ent)
{
	ent->SetAbsOrigin(ent->GetAbsOrigin());
	ent->SetAngle2(ent->GetAbsAngles());
	ent->SetVelocity(ent->GetVelocity());
	ent->SetFlags(*ent->GetFlags());
}

Vector RunAimScan(CBaseEntity* pTarget, float &simtime, Vector& origin)
{
	Vector vEyePos = csgo::LocalPlayer->GetEyePosition();
	static float minimum_damage = 1.f;
	if (Menu.Ragebot.Mindamage == 0)
		minimum_damage = pTarget->GetHealth();
	else
		minimum_damage = Menu.Ragebot.Mindamage;

	VMatrix BoneMatrix[128];

	auto index = pTarget->GetIndex();
	if (Menu.Ragebot.PositionAdjustment)
	{
		tick_record record;
		for (int j = g_BacktrackHelper->PlayerRecord[pTarget->Index()].records.size() - 1; j >= 0; j--)
		{
			switch (Menu.Ragebot.TickType)
			{
			case 0:
				record = g_BacktrackHelper->PlayerRecord[index].records.at(0);
				break;
			case 1:
				record = g_BacktrackHelper->PlayerRecord[index].records.at(j);
				break;
			}
		}

		if (g_BacktrackHelper->IsTickValid(record))
		{
			simtime = record.m_flSimulationTime;
			for (auto hitbox : GameUtils::GetMultiplePointsForHitbox(pTarget, Menu.Ragebot.Hitbox, record.boneMatrix))
			{
				SetRecords(pTarget, record);

				if (Menu.Ragebot.Autowall && g_Autowall->PenetrateWall(pTarget, hitbox) && hitbox.IsValid())
					return hitbox;

				Restore(pTarget);

				int hitgroup = -1;

				if (g_pEngineTrace->IsVisible(csgo::LocalPlayer, vEyePos, hitbox, pTarget, hitgroup)) {
					float modified_damage = csgo::MainWeapon->GetCSWpnData()->damage * (float)pow(csgo::MainWeapon->GetCSWpnData()->range_modifier, csgo::MainWeapon->GetCSWpnData()->range * 0.002);

					ScaleDamage(hitgroup, pTarget, csgo::MainWeapon->GetCSWpnData()->armor_ratio, modified_damage);

					if (hitbox.IsValid() && modified_damage >= minimum_damage)
						return hitbox;
				}
			}
			for (auto hitbox_id : GetHitboxesToScan(pTarget))
			{
				auto point = GameUtils::GetMultiplePointsForHitbox(pTarget, hitbox_id, record.boneMatrix);

				for (int i = 0; i < point.size(); i++)
				{
					auto hit_vector = point.at(i);

					if (i == 0)
					{
						SetRecords(pTarget, record);

						if (Menu.Ragebot.Autowall && g_Autowall->PenetrateWall(pTarget, hit_vector) && hit_vector.IsValid())
							return hit_vector;

						Restore(pTarget);
					}

					int hitgroup = -1;

					if (g_pEngineTrace->IsVisible(csgo::LocalPlayer, vEyePos, hit_vector, pTarget, hitgroup)) {
						float modified_damage = csgo::MainWeapon->GetCSWpnData()->damage * (float)pow(csgo::MainWeapon->GetCSWpnData()->range_modifier, csgo::MainWeapon->GetCSWpnData()->range * 0.002);

						ScaleDamage(hitgroup, pTarget, csgo::MainWeapon->GetCSWpnData()->armor_ratio, modified_damage);

						if (hit_vector.IsValid() && modified_damage >= minimum_damage)
							return hit_vector;
					}
				}
			}
		}
	}
	else
	{
		pTarget->SetupBones(BoneMatrix, 128, 0x100, g_pGlobals->curtime);
		simtime = pTarget->GetSimulationTime();

		origin = pTarget->GetOrigin();

		for (auto HitBox : GameUtils::GetMultiplePointsForHitbox(pTarget, Menu.Ragebot.Hitbox, BoneMatrix)) {
			if (g_pEngineTrace->IsVisible(csgo::LocalPlayer, vEyePos, HitBox, pTarget) && !HitBox.IsZero())
				return HitBox;

			if (Menu.Ragebot.Autowall && g_Autowall->PenetrateWall(pTarget, HitBox) && !HitBox.IsZero())
				return HitBox;
		}
		for (auto HitboxID : GetHitboxesToScan(pTarget)) {

			Vector vPoint;

			std::vector<Vector> Points = GameUtils::GetMultiplePointsForHitbox(pTarget, HitboxID, BoneMatrix);
			for (int k = 0; k < Points.size(); k++) {

				vPoint = Points.at(k);
				float damage = 0.f;

				int hitgroup = -1;
				if (g_pEngineTrace->IsVisible(csgo::LocalPlayer, vEyePos, vPoint, pTarget, hitgroup)) {
					if (vPoint != Vector(0, 0, 0))
						return vPoint;
				}

				static float damage_given = 0.f;

				if (Menu.Ragebot.Autowall && g_Autowall->PenetrateWall(pTarget, vPoint)) {
					if (vPoint != Vector(0, 0, 0))
						return vPoint;

				}
			}
		}
	}
	return Vector(0, 0, 0);
}

CAimbot* g_Aimbot = new CAimbot;
void CAimbot::DropTarget()
{
	target_index = -1;
	best_distance = 99999.f;
	aimbotted_in_current_tick = false;
	fired_in_that_tick = false;
	current_aim_position = Vector();
	pTarget = nullptr;
}

float get_curtime1() {
	if (!csgo::LocalPlayer)
		return 0;

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

void AutoZeus()
{
	for (int i = 1; i <= g_pGlobals->maxClients; i++)
	{
		CBaseEntity *entity = g_pEntitylist->GetClientEntity(i);
		if (!entity
			|| entity == csgo::LocalPlayer
			|| entity->IsDormant()
			|| !entity->isAlive()
			|| entity->IsProtected()
			|| !entity->IsPlayer()
			|| entity->GetTeamNum() == csgo::LocalPlayer->GetTeamNum()
			|| !(*entity->GetFlags() & FL_ONGROUND))
			continue;

		Vector traceStart, traceEnd;
		QAngle viewAngles;
		g_pEngine->GetViewAngles(viewAngles);
		QAngle viewAnglesRcs = viewAngles + csgo::LocalPlayer->GetPunchAngle() * 2.0f;

		Math::AngleVectors(viewAnglesRcs, &traceEnd);

		traceStart = csgo::LocalPlayer->GetEyePosition();
		traceEnd = traceStart + (traceEnd * 8192.0f);

		Ray_t ray;
		trace_t Trace;
		ray.Init(traceStart, traceEnd);
		CTraceFilter traceFilter;
		traceFilter.pSkip1 = csgo::LocalPlayer;
		g_pEngineTrace->TraceRay(ray, 0x46004003, &traceFilter, &Trace);

		if (!Trace.m_pEnt)
			return;
		if (!Trace.m_pEnt->IsValidTarget())
			return;

		float playerDistance = csgo::LocalPlayer->GetOrigin().DistTo(entity->GetOrigin());
		if (csgo::MainWeapon->NextPrimaryAttack() < get_curtime1()) {
			if (playerDistance <= 184.f)
				csgo::UserCmd->buttons |= IN_ATTACK;
		}
	}
}

void CAimbot::Run()
{
	if (!Menu.Ragebot.EnableAimbot)
		return;

	CBaseCombatWeapon* pWeapon = csgo::LocalPlayer->GetWeapon();

	if (!pWeapon || pWeapon->Clip1() == 0 || pWeapon->IsMiscWeapon() || !GameUtils::IsAbleToShoot())
		return;

	QAngle view; g_pEngine->GetViewAngles(view);

	if (Menu.Ragebot.AutomaticRevolver)
	{
		if (csgo::MainWeapon->WeaponID() == ItemDefinitionIndex::REVOLVER)
		{
			csgo::UserCmd->buttons |= IN_ATTACK;
			float flPostponeFireReady = csgo::MainWeapon->GetPostponeFireReadyTime();
			if (flPostponeFireReady > 0 && flPostponeFireReady < get_curtime1())
			{
				csgo::UserCmd->buttons &= ~IN_ATTACK;
				if (Menu.Ragebot.NewAutomaticRevolver && flPostponeFireReady + g_pGlobals->interval_per_tick * Menu.Ragebot.NewAutomaticRevolverFactor > get_curtime1())
					csgo::UserCmd->buttons |= IN_ATTACK2;
			}
		}
	}

	for (int i = 1; i < 64; ++i)
	{
		if (i == g_pEngine->GetLocalPlayer())
			continue;
		CBaseEntity* target = g_pEntitylist->GetClientEntity(i);
		if (!target->IsValidTarget())
			continue;

		csgo::Target = target;

		float fov = GameUtils::GetFoV(view, csgo::LocalPlayer->GetEyePosition(), target->GetEyePosition(), false);
		if (fov > 180.f)
			continue;

		float simtime = 0;
		Vector minus_origin = Vector(0, 0, 0);
		Vector aim_position = RunAimScan(target, simtime, minus_origin);
		if (aim_position == Vector(0, 0, 0))
			continue;

		float selection_value = 0;
		selection_value = fov;

		if (best_distance >= selection_value && aim_position != Vector(0, 0, 0))
		{
			best_distance = selection_value;
			target_index = i;
			current_aim_position = aim_position;
			pTarget = target;
			current_aim_simulationtime = simtime;
			current_aim_player_origin = minus_origin;
		}
	}

	if (target_index != -1 && current_aim_position != Vector(0, 0, 0) && pTarget)
	{
		aimbotted_in_current_tick = true;

		QAngle aim = GameUtils::CalculateAngle(csgo::LocalPlayer->GetEyePosition(), current_aim_position);

		aim.y = Math::NormalizeYaw(aim.y);

		csgo::UserCmd->viewangles = aim;

		if (!Menu.Ragebot.SilentAimbot)
			g_pEngine->SetViewAngles(csgo::UserCmd->viewangles);

		if (Menu.Ragebot.AutomaticFire)
		{
			if (Menu.Ragebot.AutomaticScope && pWeapon->IsScopeable() && !csgo::LocalPlayer->IsScoped())
				csgo::UserCmd->buttons |= IN_ATTACK2;
			else
			{
				if (Menu.Ragebot.Minhitchance == 0 || hitchance(csgo::UserCmd->viewangles, pTarget, Menu.Ragebot.Minhitchance))
				{
					csgo::UserCmd->buttons |= IN_ATTACK;
					this->fired_in_that_tick = true;
					data[pTarget->GetIndex()].shoots++;
				}
			}
		}

		if (csgo::UserCmd->buttons & IN_ATTACK)
		{
			this->aimbotted = true;
		}

		if (*csgo::MainWeapon->ItemDefinitionIndex() == WEAPON_TASER)
		{
			if (Menu.Misc.ZeusBot) {
				AutoZeus();
			}
		}

		csgo::UserCmd->tick_count = TIME_TO_TICKS(current_aim_simulationtime) + TIME_TO_TICKS(g_BacktrackHelper->GetLerpTime());// for backtracking
	}
}

void CAimbot::CompensateInaccuracies()
{
	if (csgo::UserCmd->buttons & IN_ATTACK)
	{
		if (Menu.Ragebot.NoRecoil)
		{
			ConVar* recoilscale = g_pCvar->FindVar("weapon_recoil_scale");

			if (recoilscale) {
				QAngle qPunchAngles = csgo::LocalPlayer->GetPunchAngle();
				QAngle qAimAngles = csgo::UserCmd->viewangles;
				qAimAngles -= qPunchAngles * recoilscale->GetFloat();
				csgo::UserCmd->viewangles = qAimAngles;
			}
		}

		if (Menu.Ragebot.NoSpread)
			csgo::UserCmd->viewangles = g_NoSpread->SpreadFactor(csgo::UserCmd->random_seed);
	}
}
