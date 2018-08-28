#include "sdk.h"
#include "Antiaim.h"
#include "global.h"
#include "GameUtils.h"
#include "Math.h"
#include "Aimbot.h"
#include <time.h>

float get_curtime() {
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

bool next_lby_update() 
{
	if (csgo::LocalPlayer) 
	{
		auto net_channel = g_pEngine->GetNetChannel();
		static float next_lby_update_time = 0;
		const float current_time = get_curtime();

		if (csgo::LocalPlayer->GetVelocity().Length2D() > 0.1) {
			next_lby_update_time = current_time + 0.22f;
		}
		else {
			if ((next_lby_update_time < current_time && !net_channel->m_nChokedPackets)) {
				next_lby_update_time = current_time + 1.1f;
				return true;
			}
		}
	}
	return false;
}

namespace binds
{
	static bool left, right, back = true;
	void Do()
	{
		if (GetAsyncKeyState(VK_LEFT))
		{
			left = true;
			right = false;
			back = false;
		}
		else if (GetAsyncKeyState(VK_RIGHT))
		{
			left = false;
			right = true;
			back = false;
		}
		else if (GetAsyncKeyState(VK_DOWN))
		{
			left = false;
			right = false;
			back = true;
		}

		if (left)
			csgo::UserCmd->viewangles.y -= 90;
		else if (right)
			csgo::UserCmd->viewangles.y += 90;
		else if (back)
		{

		}
	}
}

namespace freestanding
{

	mstudiobbox_t* get_hitbox(CBaseEntity* entity, int hitbox_index)
	{
		if (entity->IsDormant() || entity->GetHealth() <= 0)
			return NULL;

		const auto pModel = entity->GetModel();
		if (!pModel)
			return NULL;

		auto pStudioHdr = g_pModelInfo->GetStudioModel(pModel);
		if (!pStudioHdr)
			return NULL;

		auto pSet = pStudioHdr->pHitboxSet(0);
		if (!pSet)
			return NULL;

		if (hitbox_index >= pSet->numhitboxes || hitbox_index < 0)
			return NULL;

		return pSet->pHitbox(hitbox_index);
	}

	Vector get_hitbox_pos(CBaseEntity* entity, int hitbox_id)
	{

		auto hitbox = get_hitbox(entity, hitbox_id);
		if (!hitbox)
			return Vector(0, 0, 0);

		auto bone_matrix = entity->GetBoneMatrix(hitbox->bone);

		Vector bbmin, bbmax;
		Math::VectorTransform(hitbox->bbmin, bone_matrix, bbmin);
		Math::VectorTransform(hitbox->bbmax, bone_matrix, bbmax);

		return (bbmin + bbmax) * 0.5f;
	}

	float fov_player(Vector ViewOffSet, Vector View, CBaseEntity* entity, int hitbox)
	{
		// Anything past 180 degrees is just going to wrap around
		CONST FLOAT MaxDegrees = 180.0f;

		// Get local angles
		Vector Angles = View;

		// Get local view / eye position
		Vector Origin = ViewOffSet;

		// Create and intiialize vectors for calculations below
		Vector Delta(0, 0, 0);
		//Vector Origin(0, 0, 0);
		Vector Forward(0, 0, 0);

		// Convert angles to normalized directional forward vector
		Math::AngleVectors(Angles, &Forward);

		Vector AimPos = get_hitbox_pos(entity, hitbox); //pvs fix disabled

		Math::VectorSubtract(AimPos, Origin, Delta);
		//Delta = AimPos - Origin;

		// Normalize our delta vector
		Math::NormalizeNum(Delta, Delta);

		// Get dot product between delta position and directional forward vectors
		FLOAT DotProduct = Forward.Dot(Delta);

		// Time to calculate the field of view
		return (acos(DotProduct) * (MaxDegrees / M_PI));
	}

	int closest_to_crosshair()
	{
		int index = -1;
		float lowest_fov = INT_MAX;

		CBaseEntity* local_player = csgo::LocalPlayer;

		if (!local_player)
			return -1;

		Vector local_position = local_player->GetOrigin() + local_player->GetViewOffset();

		Vector angles;
		g_pEngine->GetViewAngles(angles);

		for (int i = 1; i <= g_pGlobals->maxClients; i++)
		{
			CBaseEntity *entity = g_pEntitylist->GetClientEntity(i);

			if (!entity || !entity->isAlive() || entity->GetTeamNum() == local_player->GetTeamNum() || entity->IsDormant() || entity == local_player)
				continue;

			float fov = fov_player(local_position, angles, entity, 0);

			if (fov < lowest_fov)
			{
				lowest_fov = fov;
				index = i;
			}
		}

		return index;
	}

	void Do() {
		static float last_real;
		bool no_active = true;
		float bestrotation = 0.f;
		float highestthickness = 0.f;
		Vector besthead;

		auto leyepos = csgo::LocalPlayer->GetOrigin() + csgo::LocalPlayer->GetViewOffset();
		auto headpos = get_hitbox_pos(csgo::LocalPlayer, 0);
		auto origin = csgo::LocalPlayer->GetAbsOrigin();

		auto checkWallThickness = [&](CBaseEntity* pPlayer, Vector newhead) -> float
		{
			Vector endpos1, endpos2;
			Vector eyepos = pPlayer->GetOrigin() + pPlayer->GetViewOffset();

			Ray_t ray;
			ray.Init(newhead, eyepos);

			CTraceFilterSkipTwoEntities filter(pPlayer, csgo::LocalPlayer);

			trace_t trace1, trace2;
			g_pEngineTrace->TraceRay_NEW(ray, MASK_SHOT_BRUSHONLY, &filter, &trace1);

			if (trace1.DidHit())
				endpos1 = trace1.endpos;
			else
				return 0.f;

			ray.Init(eyepos, newhead);
			g_pEngineTrace->TraceRay_NEW(ray, MASK_SHOT_BRUSHONLY, &filter, &trace2);

			if (trace2.DidHit())
				endpos2 = trace2.endpos;

			float add = newhead.Dist(eyepos) - leyepos.Dist(eyepos) + 3.f;
			return endpos1.Dist(endpos2) + add / 3;
		};

		int index = closest_to_crosshair();

		static CBaseEntity* entity;

		if (index != -1)
			entity = g_pEntitylist->GetClientEntity(index);

		float step = (2 * M_PI) / 18.f; // One PI = half a circle ( for stacker cause low iq :sunglasses: ), 28

		float radius = fabs(Vector(headpos - origin).Length2D());

		if (index == -1)
		{
			no_active = true;
		}
		else
		{
			for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
			{
				Vector newhead(radius * cos(rotation) + leyepos.x, radius * sin(rotation) + leyepos.y, leyepos.z);

				float totalthickness = 0.f;

				no_active = false;

				totalthickness += checkWallThickness(entity, newhead);

				if (totalthickness > highestthickness)
				{
					highestthickness = totalthickness;
					bestrotation = rotation;
					besthead = newhead;
				}
			}
		}


		if (next_lby_update())
		{
			csgo::UserCmd->viewangles.y = last_real + Menu.Antiaim.LBYDelta;
		}
		else
		{
			if (!no_active)
				csgo::UserCmd->viewangles.y = RAD2DEG(bestrotation) - 180;
				

			last_real = csgo::UserCmd->viewangles.y;
		}
	}
}

CAntiaim* g_Antiaim = new CAntiaim;

void Accelerate(CBaseEntity *player, Vector &wishdir, float wishspeed, float accel, Vector &outVel)
{
	// See if we are changing direction a bit
	float currentspeed = outVel.Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	float addspeed = wishspeed - currentspeed;

	// If not going to add any speed, done.
	if (addspeed <= 0)
		return;

	// Determine amount of accleration.
	float accelspeed = accel * g_pGlobals->frametime * wishspeed * player->m_surfaceFriction();

	// Cap at addspeed
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// Adjust velocity.
	for (int i = 0; i < 3; i++)
		outVel[i] += accelspeed * wishdir[i];
}

void WalkMove(CBaseEntity *player, Vector &outVel)
{
	Vector forward, right, up, wishvel, wishdir, dest;
	float_t fmove, smove, wishspeed;

	Math::AngleVectors(player->GetEyeAngles(), forward, right, up);  // Determine movement angles

	g_pMoveHelper->SetHost(player);
	fmove = g_pMoveHelper->m_flForwardMove;
	smove = g_pMoveHelper->m_flSideMove;
	g_pMoveHelper->SetHost((CBaseEntity*)nullptr);

	if (forward[2] != 0)
	{
		forward[2] = 0;
		Math::NormalizeVector(forward);
	}

	if (right[2] != 0)
	{
		right[2] = 0;
		Math::NormalizeVector(right);
	}

	for (int i = 0; i < 2; i++)	// Determine x and y parts of velocity
		wishvel[i] = forward[i] * fmove + right[i] * smove;

	wishvel[2] = 0;	// Zero out z part of velocity

	wishdir = wishvel; // Determine maginitude of speed of move
	wishspeed = wishdir.Normalize();

	// Clamp to server defined max speed
	g_pMoveHelper->SetHost(player);
	if ((wishspeed != 0.0f) && (wishspeed > g_pMoveHelper->m_flMaxSpeed))
	{
		VectorMultiply(wishvel, player->m_flMaxspeed() / wishspeed, wishvel);
		wishspeed = player->m_flMaxspeed();
	}
	g_pMoveHelper->SetHost(nullptr);
	// Set pmove velocity
	outVel[2] = 0;
	Accelerate(player, wishdir, wishspeed, g_pCvar->FindVar("sv_accelerate")->GetFloat(), outVel); // Always have to have the biggest dynamic variable searching ever.
	outVel[2] = 0;

	// Add in any base velocity to the current velocity.
	VectorAdd(outVel, player->GetBaseVelocity(), outVel);

	float spd = outVel.Length();

	if (spd < 1.0f)
	{
		outVel.Init();
		// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->GetBaseVelocity(), outVel);
		return;
	}

	g_pMoveHelper->SetHost(player);
	g_pMoveHelper->m_outWishVel += wishdir * wishspeed;
	g_pMoveHelper->SetHost(nullptr);

	// Don't walk up stairs if not on ground.
	if (!(*player->GetFlags() & FL_ONGROUND))
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->GetBaseVelocity(), outVel);
		return;
	}

	// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(outVel, player->GetBaseVelocity(), outVel);
}


void Friction(Vector &outVel)
{
	float speed, newspeed, control;
	float friction;
	float drop;

	speed = outVel.Length();

	if (speed <= 0.1f)
		return;

	drop = 0;

	// apply ground friction
	if (*csgo::LocalPlayer->GetFlags() & FL_ONGROUND)
	{
		friction = g_pCvar->FindVar("sv_friction")->GetFloat() * csgo::LocalPlayer->m_surfaceFriction();

		// Bleed off some speed, but if we have less than the bleed
		// threshold, bleed the threshold amount.
		control = (speed < g_pCvar->FindVar("sv_stopspeed")->GetFloat()) ? g_pCvar->FindVar("sv_stopspeed")->GetFloat() : speed;

		// Add the amount to the drop amount.
		drop += control * friction * g_pGlobals->frametime;
	}

	newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	if (newspeed != speed)
	{
		// Determine proportion of old speed we are using.
		newspeed /= speed;
		// Adjust velocity according to proportion.
		VectorMultiply(outVel, newspeed, outVel);
	}
}

void Fakewalk(CUserCmd *userCMD)
{
	if (GetAsyncKeyState(VK_SHIFT))
	{
		Vector velocity = csgo::vecUnpredictedVel;

		int Iterations = 0;
		for (; Iterations < 15; ++Iterations) {
			if (velocity.Length() < 0.1)
			{
				//g_pCvar->ConsolePrintf("Ticks till stop %d\n", Iterations);
				//Msg("Ticks till stop %d\n", Iterations);
				break;
			}

			Friction(velocity);
			WalkMove(csgo::LocalPlayer, velocity);
		}

		int choked_ticks = csgo::nChokedTicks;

		if (Iterations > 7 - choked_ticks || !choked_ticks)
		{
			float_t speed = velocity.Length();

			QAngle direction;
			Math::VectorAngles(velocity, direction);

			direction.y = userCMD->viewangles.y - direction.y;

			Vector forward;
			Math::AngleVectors(direction, forward);
			Vector negated_direction = forward * -speed;

			userCMD->forwardmove = negated_direction.x;
			userCMD->sidemove = negated_direction.y;
		}

		if (csgo::nChokedTicks < 8)
			csgo::bShouldChoke = true;

	}
}

void CAntiaim::DoAntiAims()
{
	auto Pitch = [](int choose)
	{
		switch (choose)
		{
		case 1:
			csgo::UserCmd->viewangles.x = 89;
			break;
		case 2:
			csgo::UserCmd->viewangles.x = -180;
			break;
		case 3:
			csgo::UserCmd->viewangles.x = -991;
			break;
		case 4:
			csgo::UserCmd->viewangles.x = 991;
			break;
		case 5:
			csgo::UserCmd->viewangles.x = 0;
			break;
		case 6:
			csgo::UserCmd->viewangles.x = 1080;
			break;
		}
	};

	auto Yaw = [](int select)
	{
		switch (select)
		{
		case 1:
			csgo::UserCmd->viewangles.y += 180;
			break;
		case 2:
			csgo::UserCmd->viewangles.y += 180 + Math::RandomFloat(-Menu.Antiaim.JitterRange, Menu.Antiaim.JitterRange);
			break;
		case 3:
		{
			static float t;
			t += 5;
			if (t > 240)
				t = 120;
			csgo::UserCmd->viewangles.y += t;
		}
		break;
		case 4:
		{
			static int y2 = -179;
			int spinBotSpeedFast = Menu.Antiaim.SpinSpeed;

			y2 += spinBotSpeedFast;

			if (y2 >= 179)
				y2 = -179;

			csgo::UserCmd->viewangles.y = y2;
		}
		break;
		case 5:
			csgo::UserCmd->viewangles.y += Math::RandomFloat(-180, 180);
			break;
		}
	};

	if (!csgo::SendPacket)
	{
		switch (Menu.Antiaim.DirectonType)
		{
		case 1://binds
		{	
			if (next_lby_update())
				csgo::UserCmd->viewangles.y += Menu.Antiaim.LBYDelta;
			binds::Do();
		}
		break;
		case 2://freestanding
			freestanding::Do();
			break;
		}
	}

	if (*csgo::LocalPlayer->GetFlags() & FL_ONGROUND)
	{
		if (csgo::LocalPlayer->GetVelocity().Length2D() < 32)
		{
			Pitch(Menu.Antiaim.Stand.pitch);

			if (csgo::SendPacket)
				Yaw(Menu.Antiaim.Stand.fakeyaw);
			else
				Yaw(Menu.Antiaim.Stand.yaw);

		}
		else if (csgo::LocalPlayer->GetVelocity().Length2D() > 32)
		{
			Pitch(Menu.Antiaim.Move.pitch);

			if (csgo::SendPacket)
				Yaw(Menu.Antiaim.Move.fakeyaw);
			else
				Yaw(Menu.Antiaim.Move.yaw);
		}
	}
	else
	{
		Pitch(Menu.Antiaim.Air.pitch);

		if (csgo::SendPacket)
			Yaw(Menu.Antiaim.Air.fakeyaw);
		else
			Yaw(Menu.Antiaim.Air.yaw);
	}
}

void CAntiaim::Run(QAngle org_view)
{
	if (Menu.Antiaim.AntiaimEnable)
	{
		static int iChokedPackets = -1;
		if ((g_Aimbot->fired_in_that_tick && iChokedPackets < 4 && GameUtils::IsAbleToShoot()) && !csgo::ForceRealAA)
		{
			csgo::SendPacket = false;
			iChokedPackets++;
		}
		else
		{
			iChokedPackets = 0;
			CGrenade* pCSGrenade = (CGrenade*)csgo::LocalPlayer->GetWeapon();
			if (csgo::UserCmd->buttons & IN_USE
				|| csgo::LocalPlayer->GetMoveType() == MOVETYPE_LADDER && csgo::LocalPlayer->GetVelocity().Length() > 0
				|| csgo::LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP
				|| *csgo::LocalPlayer->GetFlags() & FL_FROZEN
				|| pCSGrenade && pCSGrenade->GetThrowTime() > 0.f) 
				return;
			
			choke = !choke;

			if (!Menu.Misc.FakelagEnable || (*csgo::LocalPlayer->GetFlags() & FL_ONGROUND && !Menu.Misc.FakelagOnground || *csgo::LocalPlayer->GetFlags() & FL_ONGROUND && csgo::LocalPlayer->GetVelocity().Length() < 3))
				csgo::SendPacket = choke;

			csgo::UserCmd->viewangles = org_view;

			DoAntiAims();

			Fakewalk(csgo::UserCmd);

			if (csgo::bShouldChoke)
				csgo::SendPacket = csgo::bShouldChoke = false;

			if (!csgo::SendPacket)
				csgo::nChokedTicks++;
			else
				csgo::nChokedTicks = 0;
		}
	}
}