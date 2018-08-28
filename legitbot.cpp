#include "sdk.h"
#include "backtrackmanager.h"
#include "legitbot.h"
#include "global.h"
#include "GameUtils.h"
#include "Math.h"

bool CLegitBot::SettingProxy()
{
	if (!csgo::MainWeapon)
		return false;
	if (csgo::MainWeapon->IsMiscWeapon())
		return false;
	if (csgo::MainWeapon->GetWeaponType() == WEPCLASS_KNIFE || csgo::MainWeapon->GetWeaponType() == WEPCLASS_INVALID)
		return false;
	int WeaponID = csgo::MainWeapon->GetWeaponNum();

	if (WeaponID < 0)
		return false;

	Setting.flFov = Menu.LegitBot.flFov[WeaponID];
	Setting.bUseSpecificAim = Menu.LegitBot.bUseSpecificAim[WeaponID];
	Setting.flSwitchTargetDelay = Menu.LegitBot.flSwitchTargetDelay[WeaponID];
	Setting.iAimType = Menu.LegitBot.iAimType[WeaponID];
	Setting.iBackTrackType = Menu.LegitBot.iBackTrackType[WeaponID];
	Setting.iDelay = Menu.LegitBot.iDelay[WeaponID];
	Setting.iFovType = Menu.LegitBot.iFovType[WeaponID];
	Setting.iSmoothType = Menu.LegitBot.iSmoothType[WeaponID];
	Setting.flSmooth = Menu.LegitBot.flSmooth[WeaponID];
	Setting.iSpecificAimType = Menu.LegitBot.iSpecificAimType[WeaponID];
	return true;
}

QAngle CLegitBot::Smooth(QAngle delta)
{
	if (delta.IsZero())
		return QAngle();
	if (delta.Length() < Statictic.flAverageMovementSpeed)
		return delta;
	return delta *  (1 / (delta.Length() / Statictic.flAverageMovementSpeed));
}

void CLegitBot::Run()
{
	if (!csgo::LocalPlayer)
		return;
	CBaseCombatWeapon* pWeapon = csgo::LocalPlayer->GetWeapon();
	if (!pWeapon || pWeapon->IsMiscWeapon())
		return;
	float flNearestTick = FLT_MAX;
	float flNearestEntity = FLT_MAX;
	if (!SettingProxy())
		return;
	for (int i = 0; i <= g_pGlobals->maxClients; i++)
	{
		CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);
		if (!pEntity || !pEntity->isAlive() || !pEntity->IsPlayer() || !pEntity->IsEnemy())
			continue;

		if (pEntity->GetVelocity().Length() < .1f)
			AimData[i].flStandTime += g_pGlobals->interval_per_tick;
		else
			AimData[i].flStandTime = 0;

		if (Setting.iBackTrackType > 0)
		{
			auto Ticks = CBackTrackManager::get().GetRecords(pEntity);

			if (Ticks.empty())
				continue;

			for (auto Tick : Ticks)
			{
				if (AimData[i].flStandTime > .2f)
					continue;

				if (!CBackTrackManager::get().IsValidTick(Tick))
					continue;

				if (!CBackTrackManager::get().IsVisibleTick(Tick, false, true))
					continue;

				Vector HeadPos = pEntity->GetBonePos(8, Tick.boneMatrix);
				float Fov = Get2DFov(HeadPos);

				if (Fov > 0 && Fov < flNearestTick)
				{
					flNearestTick = Fov;
					NearestTick = Tick;
				}
			}
		}
		RecordTick_t CurentTick(pEntity);
		if (CBackTrackManager::get().IsVisibleTick(CurentTick, false, true))
		{
			Vector HeadPos = pEntity->GetBonePos(8);
			float Fov = Get2DFov(HeadPos);

			if (Fov > 0 && Fov < flNearestEntity)
			{
				flNearestEntity = Fov;
				NearestEntity = CurentTick;
			}
		}
	}

	if (Setting.bRecordStatictic)
	{
		QAngle MyAngle;
		g_pEngine->GetViewAngles(MyAngle);
		if (Statictic.angOldEyeAngle != QAngle())
		{
			QAngle Movement = Statictic.angOldEyeAngle - MyAngle;
			Movement.Normalized();
			float flMovement = Movement.Length();
			if (flMovement > 1.2f)
			{
				if (Statictic.iRecordCount > 50000) //дальше уже изменения особо не увидешь, возможно есть смысл сделать ещё меньше
					Statictic.iRecordCount = 50000;
				float flAllMovement = Statictic.flAverageMovementSpeed * Statictic.iRecordCount++;
				flAllMovement += flMovement;
				Statictic.flAverageMovementSpeed = flAllMovement / Statictic.iRecordCount;
			}
		}
		Statictic.angOldEyeAngle = MyAngle;
	}
	bool ValidEnt = CBackTrackManager::get().IsValidTick(NearestEntity);
	bool ValidTick = CBackTrackManager::get().IsValidTick(NearestTick);
	RecordTick_t Target;
	if (Setting.iBackTrackType < 2 && ValidEnt)
		Target = NearestEntity;
	if (Setting.iBackTrackType == 2 && ValidTick)
		Target = NearestTick;
	else if (Setting.iBackTrackType == 2 && ValidEnt)
		Target = NearestEntity;
	if (Aimbot() && CBackTrackManager::get().IsValidTick(Target) && CBackTrackManager::get().IsVisibleTick(Target, true, true) && csgo::UserCmd->buttons & IN_ATTACK && (ReTargetTime + Setting.flSwitchTargetDelay < g_pGlobals->curtime || Target.iIndex == LastTargetIndex))
	{
		CreateAimPath(Target);
	}
	if (csgo::UserCmd->buttons & IN_ATTACK && ValidTick && Setting.iBackTrackType > 0)
	{
		csgo::UserCmd->tick_count = TIME_TO_TICKS(NearestTick.flSimulationTime);
	}
	g_pEngine->GetViewAngles(UserMoveDetect);
}

float CLegitBot::Get2DFov(Vector pos)
{
	Vector vecScreen;
	int iWidth, iHeight;
	QAngle punchAngle = csgo::LocalPlayer->GetPunchAngle() * 2;
	g_pEngine->GetScreenSize(iWidth, iHeight);
	int x = iWidth / 2;
	int y = iHeight / 2;
	int dy = iHeight / 90;
	int dx = iWidth / 90;
	x -= (dx * punchAngle.y);
	y += (dy * punchAngle.x);
	if (GameUtils::WorldToScreen(pos, vecScreen))
		return (Vector(x - vecScreen.x, y - vecScreen.y, 0).Length2D()) / (iHeight / 2) * 90;
	return -1.f;
}

float CLegitBot::GetAngFov(Vector pos)
{
	QAngle angDelta = GameUtils::CalculateAngle(csgo::LocalPlayer->GetEyePosition(), pos) - UserMoveDetect /*- csgo::LocalPlayer->GetPunchAngle() * 2*/;
	return Vector(Math::NormalizeYaw(angDelta.x), Math::NormalizeYaw(angDelta.y), 0).Length2D();
}

float CLegitBot::Get3DFov(Vector pos) 
{
	float flDist = (csgo::LocalPlayer->GetEyePosition() - pos).Length();
	Vector forward, endtrace;

	Math::AngleVectors(csgo::LocalPlayer->GetEyeAngles(), &forward);
	endtrace = csgo::LocalPlayer->GetEyePosition() + (forward * flDist);

	return (pos - endtrace).Length();
}

bool CLegitBot::Aimbot()
{
	if (AimPath.empty())
		return true;
	QAngle MyAngle;
	g_pEngine->GetViewAngles(MyAngle);
	if ((MyAngle - UserMoveDetect).Length() > 0.3) 
	{
		AimPath.clear();
		return true;
	}
	if (AimPath[0].bNoShot/* && csgo::UserCmd->buttons & IN_ATTACK*/) //Perfect way for delay
		csgo::UserCmd->buttons &= ~IN_ATTACK;
	if (AimPath[0].bSilentHit && !AimPath[0].bNoShot)
	{
		csgo::UserCmd->viewangles.y = AimPath[AimPath.size() - 1].angAimAngle.y - csgo::LocalPlayer->GetPunchAngle().y * 2;
		csgo::SendPacket = false;
		if (!csgo::UserCmd->buttons & IN_ATTACK)
			csgo::UserCmd->buttons |= IN_ATTACK;
	}
	if (AimPath[0].bForceShot)
		csgo::UserCmd->buttons |= IN_ATTACK;
	g_pEngine->SetViewAngles(AimPath[0].angAimAngle - csgo::LocalPlayer->GetPunchAngle() * 2/* - Smooth(csgo::LocalPlayer->GetPunchAngle() * 2)*/);
	AimPath.erase(AimPath.begin());
	if (AimPath.empty())
		ReTargetTime = g_pGlobals->curtime;
	return false;
}

void CLegitBot::CreateAimPath(RecordTick_t target)
{
	auto VectorTransform_Wrapper = [](const Vector& in1, const VMatrix &in2, Vector &out)
	{
		auto VectorTransform = [](const float *in1, const VMatrix& in2, float *out)
		{
			auto DotProducts = [](const float *v1, const float *v2)
			{
				return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
			};
			out[0] = DotProducts(in1, in2[0]) + in2[0][3];
			out[1] = DotProducts(in1, in2[1]) + in2[1][3];
			out[2] = DotProducts(in1, in2[2]) + in2[2][3];
		};
		VectorTransform(&in1.x, in2, &out.x);
	};
	CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(target.iIndex);
	if (!pEntity || !pEntity->isAlive() || !pEntity->IsPlayer())
		return;
	studiohdr_t* pStudioModel = g_pModelInfo->GetStudioModel(pEntity->GetModel());
	if (!pStudioModel)
		return;
	mstudiohitboxset_t* set = pStudioModel->pHitboxSet(0);
	if (!set)
		return;
	float Fov = FLT_MAX;
	Vector Target;
	for (int i = 0; i < set->numhitboxes; i++) {
		mstudiobbox_t *hitbox = set->pHitbox(i);
		if (!hitbox)
			continue;

		Vector max;
		Vector min;

		VectorTransform_Wrapper(hitbox->bbmax, target.boneMatrix[hitbox->bone], max);
		VectorTransform_Wrapper(hitbox->bbmin, target.boneMatrix[hitbox->bone], min);

		auto center = (min + max) * 0.5f;
		float fov = Get2DFov(center);
		if (fov < Fov)
		{
			Fov = fov;
			Target = center;
		}
	}
	if (Fov > Setting.flFov) //FovCheck
		return;
	if (!Target.IsValid() || Target.IsZero())
		return;
	QAngle AimAngle = Math::NormalizeAngle2(GameUtils::CalculateAngle(csgo::LocalPlayer->GetEyePosition(), Target));
	QAngle MyAngle;
	g_pEngine->GetViewAngles(MyAngle);
	MyAngle += csgo::LocalPlayer->GetPunchAngle() * 2;
	if (Math::NormalizeAngle2(AimAngle - MyAngle).Length2D() < 0.7)
		return;
	int AimSteps = 1;
	if (Setting.iSmoothType == 0)
	{
		AimSteps = AimAngle.Length2D() / Statictic.flAverageMovementSpeed;
		if (AimAngle.Length2D() < Statictic.flAverageMovementSpeed)
			AimSteps = 1;
	}
	else if (Setting.iSmoothType == 1)
	{
		AimSteps = Setting.flSmooth > g_pGlobals->interval_per_tick ? Setting.flSmooth / g_pGlobals->interval_per_tick : 1;
	}
	
	for (int i = 1; i <= AimSteps; i++)
	{
		AimPath_t Path;
		Path.angAimAngle = Math::NormalizeAngle2(MyAngle + (Math::NormalizeAngle2(AimAngle - MyAngle) / AimSteps * i));
		Path.bNoShot = Setting.iDelay > 0;
		Path.bSilentHit = false;
		Path.bForceShot = false;
		AimPath.push_back(Path);
	}
	if (Setting.iDelay == 2)
	{
		AimPath_t Path;
		Path.angAimAngle = Math::NormalizeAngle2(MyAngle + (Math::NormalizeAngle2(AimAngle - MyAngle)));
		Path.bNoShot = false;
		Path.bSilentHit = false;
		Path.bForceShot = true;
		AimPath.push_back(Path);
		AimPath.push_back(Path);
	}
	LastTargetIndex = target.iIndex;
	csgo::UserCmd->buttons &= ~IN_ATTACK;
}

