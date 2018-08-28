#include "sdk.h"
#include "Misc.h"
#include "global.h"
#include <chrono>
#include "GameUtils.h"
#include "Math.h"
#include "xor.h"
#include "Draw.h"
#include "Aimbot.h"
#include <chrono>
CMisc* g_Misc = new CMisc;
//using RevealAllFn = void(*)(int); Double V fix: Can't use this syntax with my VS version!
typedef void(*RevealAllFn)(int);
RevealAllFn fnReveal;
void CMisc::RankReveal()
{
	if (!Menu.Visuals.Rank)
		return;

	if(!fnReveal)
		fnReveal = (RevealAllFn)Utilities::Memory::FindPattern(XorStr(client_dll), (PBYTE)XorStr("\x55\x8B\xEC\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00"), XorStr("xxxxx????x????"));

	int iBuffer[1];

	if (csgo::UserCmd->buttons & IN_SCORE)
		fnReveal(iBuffer[1]);
}

void CMisc::Bunnyhop()
{
	if ((*csgo::LocalPlayer->GetFlags() & FL_ONGROUND) && csgo::walkbotenabled)
		csgo::UserCmd->buttons = IN_JUMP;

	if (Menu.Misc.AutoJump/* && Cvar->FindVar("sv_enablebunnyhopping")->GetValue() != 1*/)
	{
		static auto bJumped = false;
		static auto bFake = false;
		if (!bJumped && bFake)
		{
			bFake = false;
			csgo::UserCmd->buttons |= IN_JUMP;
		}
		else if (csgo::UserCmd->buttons & IN_JUMP)
		{
			if (*csgo::LocalPlayer->GetFlags() & FL_ONGROUND)
			{
				bJumped = true;
				bFake = true;
			}
			else
			{
				csgo::UserCmd->buttons &= ~IN_JUMP;
				bJumped = false;
			}
		}
		else
		{
			bJumped = false;
			bFake = false;
		}
	}
}

float RightMovement;
bool IsActive;
float StrafeAngle;



void CMisc::WalkBotCM(Vector& oldang)
{
	static bool active = false;
	static bool firstrun = true;
	bool walkbotting = false;
	float wbdistance = 0;

	if (GetAsyncKeyState(Menu.Misc.WalkbotStart) & 0x1)
		active = !active;

	Vector localpos = csgo::LocalPlayer->GetAbsOrigin();

	if (GetAsyncKeyState(Menu.Misc.WalkbotSet) & 0x1)
	{
		csgo::walkpoints.push_back(localpos);
		csgo::wbpoints++;
	}
	else if (GetAsyncKeyState(Menu.Misc.WalkbotDelete) & 0x1)
	{
		if (csgo::walkpoints.size() > 0)
			csgo::walkpoints.pop_back();

		if (csgo::wbpoints > -1)
			csgo::wbpoints--;
	}

	if (csgo::NewRound)
		firstrun = true;

	if (!active)
	{
		csgo::wbcurpoint = 0;
		firstrun = true;
		csgo::walkbotenabled = false;
		return;
	}

	csgo::walkbotenabled = true;
	walkbotting = true;

	if (csgo::wbcurpoint > csgo::wbpoints)
		csgo::wbcurpoint = 0;

	if (csgo::wbpoints == -1)
		return;

	Vector point = csgo::walkpoints[csgo::wbcurpoint];
	wbdistance = fabs(Vector(localpos - point).Length2D());

	if (wbdistance < 25.f)
		csgo::wbcurpoint++;

	if (csgo::wbcurpoint > csgo::wbpoints)
		csgo::wbcurpoint = 0;

	if (csgo::wbpoints == -1)
		return;

	point = csgo::walkpoints[csgo::wbcurpoint];
	wbdistance = fabs(Vector(localpos - point).Length2D());

	if (csgo::wbcurpoint == 0 && firstrun == true)
	{
		float lowdist = wbdistance;

		for (int i = 0; i < csgo::wbpoints; i++)
		{
			Vector pt = csgo::walkpoints[i];
			float dist = fabs(Vector(localpos - pt).Length2D());

			if (dist < lowdist)
			{
				lowdist = dist;
				csgo::wbcurpoint = i;
				point = csgo::walkpoints[csgo::wbcurpoint];
				wbdistance = dist;
			}
		}

		firstrun = false;
	}

	static Vector lastang;

	Vector curang = GameUtils::CalculateAngle(csgo::LocalPlayer->GetEyePosition(), point);
	curang.x = 0.f;

	Math::NormalizeVector(curang);
	Math::ClampAngles(curang);
	lastang = curang;

	csgo::StrafeAngle = curang;
}

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

void CMisc::AutoStrafe()
{
	if (!Menu.Misc.AutoStrafe)
		return;

	bool bKeysPressed = true;
	if (GetAsyncKeyState(0x41) || GetAsyncKeyState(0x57) || GetAsyncKeyState(0x53) || GetAsyncKeyState(0x44)) bKeysPressed = false;

	if ((GetAsyncKeyState(VK_SPACE) && !(*csgo::LocalPlayer->GetFlags() & FL_ONGROUND)) && bKeysPressed)
	{
		csgo::UserCmd->forwardmove = (1550.f * 5) / csgo::LocalPlayer->GetVelocity().Length2D();
		csgo::UserCmd->sidemove = (csgo::UserCmd->command_number % 2) == 0 ? -450.f : 450.f;
		if (csgo::UserCmd->forwardmove > 450.f)
			csgo::UserCmd->forwardmove = 450.f;
	}
}

typedef void(__fastcall* ClanTagFn)(const char*, const char*);
ClanTagFn dw_ClanTag;
void SetClanTag(const char* tag, const char* name)
{
	//static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30", "xxxxxxxxx????xxxxxx")); // 0x9AF10
	if (!dw_ClanTag)
		dw_ClanTag = reinterpret_cast<ClanTagFn>(FindPatternIDA("engine.dll", "53 56 57 8B DA 8B F9 FF 15"));

	if (dw_ClanTag)
		dw_ClanTag(tag, name);
}

void CMisc::HandleClantag()
{
	
}


void CMisc::FixCmd()
{
	if (Menu.Misc.AntiUT)
	{
		csgo::UserCmd->viewangles.y = Math::NormalizeYaw(csgo::UserCmd->viewangles.y);
		Math::ClampAngles(csgo::UserCmd->viewangles);

		if (csgo::UserCmd->forwardmove > 450)
			csgo::UserCmd->forwardmove = 450;
		if (csgo::UserCmd->forwardmove < -450)
			csgo::UserCmd->forwardmove = -450;

		if (csgo::UserCmd->sidemove > 450)
			csgo::UserCmd->sidemove = 450;
		if (csgo::UserCmd->sidemove < -450)
			csgo::UserCmd->sidemove = -450;
	}
}



int LagCompBreak() {
	
	Vector velocity = csgo::LocalPlayer->GetVelocity();
	velocity.z = 0;
	float speed = velocity.Length();
	if (speed > 0.f) {
		auto distance_per_tick = speed *
			g_pGlobals->interval_per_tick;
		int choked_ticks = std::ceilf(65.f / distance_per_tick);
		return std::min<int>(choked_ticks, Menu.Misc.FakelagAmount);
	}
	return 1;
}

int lagticks = 0;
int lagticksMax = 15;
bool CMisc::FakeLag()
{
	static int ticks = 0;
	const int max_choke = 15;
	const int min_choke = 1;
	static int tick_choke = 1;
	bool m = true;
	if (*csgo::LocalPlayer->GetFlags() & FL_ONGROUND)
	{
		if (!Menu.Misc.FakelagOnground)
		{
			csgo::SendPacket = true;
			return false;
		}

		if (csgo::LocalPlayer->GetVelocity().Length() < 0.1f)
		{
			csgo::SendPacket = true;
			return false;
		}
	}

	if (csgo::UserCmd->buttons & IN_ATTACK)
	{
		return false;
	}

	switch (Menu.Misc.FakelagMode)
	{
	case 0:
		csgo::chokedticks = Menu.Misc.FakelagAmount;
		break;

	case 1:
		csgo::chokedticks = std::min<int>(static_cast<int>(std::ceilf(64 / (csgo::LocalPlayer->GetVelocity().Length() * g_pGlobals->interval_per_tick))), Menu.Misc.FakelagAmount);
		break;

	case 2:
		csgo::chokedticks = LagCompBreak();
		break;
	}
	if (ticks >= csgo::chokedticks)
	{
		ticks = 0;
		csgo::SendPacket = true;
	}
	else
	{
		csgo::SendPacket = false;
		ticks++;
	}
}

float VerifyRotation(float ideal_rotation)
{
	static constexpr float ray_length = 1000.f;
	static constexpr float minimum_distance = 50.f;
	static constexpr float rotation_step = 5.f;

	auto local_player = csgo::LocalPlayer;
	if (!local_player)
		return ideal_rotation;

	auto collideable = local_player->GetCollision();
	if (!collideable)
		return ideal_rotation;

	auto TraceRayBoundingBox = [](Vector start, Vector end, Vector min, Vector max, float& fraction) -> void
	{
		Vector starts[8];
		Vector ends[8];

		starts[0] = start + Vector(min.x, min.y, min.z); /// min
		starts[1] = start + Vector(max.x, min.y, min.z);
		starts[2] = start + Vector(max.x, max.y, min.z);
		starts[3] = start + Vector(max.x, max.y, max.z); /// max
		starts[4] = start + Vector(min.x, max.y, max.z);
		starts[5] = start + Vector(min.x, min.y, max.z);
		starts[6] = start + Vector(max.x, min.y, max.z);
		starts[7] = start + Vector(min.x, max.y, min.z);

		ends[0] = end + Vector(min.x, min.y, min.z); /// min
		ends[1] = end + Vector(max.x, min.y, min.z);
		ends[2] = end + Vector(max.x, max.y, min.z);
		ends[3] = end + Vector(max.x, max.y, max.z); /// max
		ends[4] = end + Vector(min.x, max.y, max.z);
		ends[5] = end + Vector(min.x, min.y, max.z);
		ends[6] = end + Vector(max.x, min.y, max.z);
		ends[7] = end + Vector(min.x, max.y, min.z);

		float fractions[8];
		for (int i = 0; i < 8; i++)
		{
			CTraceWorldOnly filter;
			trace_t trace;
			Ray_t ray;
			ray.Init(starts[i], ends[i]);

			g_pEngineTrace->TraceRay_NEW(ray, MASK_ALL, &filter, &trace);
			fractions[i] = trace.fraction;
		}

		fraction = 1.f;
		for (const float& frac : fractions)
		{
			if (frac < fraction)
				fraction = frac;
		}
	};

	Vector bbmin, bbmax;
	bbmin = collideable->VecMins();
	bbmax = collideable->VecMaxs();

	const Vector velocity_angle = GameUtils::CalculateAngle(Vector(0, 0, 0), local_player->GetVelocity());
	float verified_roation = ideal_rotation;

	for (float i = ideal_rotation; ((ideal_rotation > 0) ? (i < 180.f) : (i > -180.f)); ((ideal_rotation > 0) ? (i += rotation_step) : (i -= rotation_step)))
	{
		Vector direction;
		float fraction_1a, fraction_1b;
		float fraction_2a, fraction_2b;

		Math::AngleVectors(Vector(0.f, velocity_angle.y + i, 0), &direction);
		TraceRayBoundingBox(local_player->GetOrigin(), local_player->GetOrigin() + (direction * ray_length), bbmin, bbmax, fraction_1a);
		Math::AngleVectors(Vector(0.f, velocity_angle.y + i + rotation_step, 0), &direction);
		TraceRayBoundingBox(local_player->GetOrigin(), local_player->GetOrigin() + (direction * ray_length), bbmin, bbmax, fraction_1b);

		Math::AngleVectors(Vector(0.f, velocity_angle.y - i, 0), &direction);
		TraceRayBoundingBox(local_player->GetOrigin(), local_player->GetOrigin() + (direction * ray_length), bbmin, bbmax, fraction_2a);
		Math::AngleVectors(Vector(0.f, velocity_angle.y - (i + rotation_step), 0), &direction);
		TraceRayBoundingBox(local_player->GetOrigin(), local_player->GetOrigin() + (direction * ray_length), bbmin, bbmax, fraction_2b);

		if ((fraction_1a * ray_length > minimum_distance) && (fraction_1b * ray_length > minimum_distance))
		{
			verified_roation = i;
			break;
		}
		else if ((fraction_2a * ray_length > minimum_distance) && (fraction_2b * ray_length > minimum_distance))
		{
			verified_roation = -i;
			break;
		}
	}

	return verified_roation;
}

template <class T>
constexpr const T& Clamp(const T& v, const T& lo, const T& hi)
{
	return (v >= lo && v <= hi) ? v : (v < lo ? lo : hi);
}

static inline float GetIdealRotation(float speed)
{
	return Clamp<float>(RAD2DEG(std::atan2(15.f, speed)), 0.f, 45.f) * g_pGlobals->interval_per_tick;
} 

void RotateMovement (float yaw)
{
	QAngle viewangles;
	g_pEngine->GetViewAngles(viewangles);

	float rotation = DEG2RAD(viewangles.y - yaw);

	float cos_rot = cos(rotation);
	float sin_rot = sin(rotation);

	float new_forwardmove = (cos_rot * csgo::UserCmd->forwardmove) - (sin_rot * csgo::UserCmd->sidemove);
	float new_sidemove = (sin_rot * csgo::UserCmd->forwardmove) + (cos_rot * csgo::UserCmd->sidemove);

	csgo::UserCmd->forwardmove = new_forwardmove;
	csgo::UserCmd->sidemove = new_sidemove;
}


void CMisc::DoCircle()
{
	if (!GetAsyncKeyState(Menu.Misc.circlestrafekey))
		return;

	auto local_player = csgo::LocalPlayer;
	if (!local_player || local_player->GetHealth() <= 0 || *local_player->GetFlags() & FL_ONGROUND)
		return;

	static bool clock_wise = false;

	const float velocity_yaw = GameUtils::CalculateAngle(Vector(0, 0, 0), local_player->GetVelocity()).y;
	const float ideal_rotation_amount = GetIdealRotation(local_player->GetVelocity().Length2D()) * (clock_wise ? -1.f : 1.f);

	float rotation_amount = VerifyRotation(ideal_rotation_amount);

	csgo::UserCmd->forwardmove = 0.f;
	csgo::UserCmd->sidemove = (rotation_amount > 0) ? -450.f : 450.f;
	RotateMovement(velocity_yaw + rotation_amount);
}


void CMisc::FixMovement()
{
	auto cmd = csgo::UserCmd;
	Vector real_viewangles;
	g_pEngine->GetViewAngles(real_viewangles);

	Vector vecMove(cmd->forwardmove, cmd->sidemove, cmd->upmove);
	float speed = sqrt(vecMove.x * vecMove.x + vecMove.y * vecMove.y);

	Vector angMove;
	Math::VectorAngles(vecMove, angMove);

	float yaw = DEG2RAD(cmd->viewangles.y - real_viewangles.y + angMove.y);

	cmd->forwardmove = cos(yaw) * speed;
	cmd->sidemove = sin(yaw) * speed;

	cmd->buttons &= ~IN_RIGHT;
	cmd->buttons &= ~IN_MOVERIGHT;
	cmd->buttons &= ~IN_LEFT;
	cmd->buttons &= ~IN_MOVELEFT;
	cmd->buttons &= ~IN_FORWARD;
	cmd->buttons &= ~IN_BACK;

	if (cmd->forwardmove > 0.f)
		cmd->buttons |= IN_FORWARD;
	else if (cmd->forwardmove < 0.f)
		cmd->buttons |= IN_BACK;

	if (cmd->sidemove > 0.f)
	{
		cmd->buttons |= IN_RIGHT;
		cmd->buttons |= IN_MOVERIGHT;
	}
	else if (cmd->sidemove < 0.f)
	{
		cmd->buttons |= IN_LEFT;
		cmd->buttons |= IN_MOVELEFT;
	}
}


void LoadPathing()
{

	char path[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(NULL), path, 255);
	for (int i = strlen(path); i > 0; i--)
	{
		if (path[i] == '\\')
		{
			path[i + 1] = 0;
			break;
		}
	}

	char size[8];
	char vecstr[64];
	char itostr[8];
	CBaseEntity *pLocal = csgo::LocalPlayer;
	if (!pLocal)
		return;
	if (g_Misc->path.size() > 1)
	{

		g_Misc->path.erase(g_Misc->path.begin(), g_Misc->path.end());
		sprintf_s(path, "\\%s_%s_%d.cfg", "pathes", "test"/*g_pEngine->GetLevelName()*/, pLocal->GetTeamNum());
		GetPrivateProfileStringA("Points", "Size", "0", size, 8, path);
		int numPoints = atoi(size);
		for (int i = 0; i < numPoints - 1; i++)
		{
			char vecstr[64];
			char itostr[8];
			sprintf_s(itostr, "%d", i);
			GetPrivateProfileStringA("Pathing", itostr, "0.0 0.0 0.0", vecstr, 64, path);
			std::string PosStr = vecstr;
			string buffer;
			stringstream ss(PosStr);
			vector<string> floats;
			while (ss >> buffer)
				floats.push_back(buffer);

			g_Misc->path.push_back(Vector(stof(floats[0]), stof(floats[1]), stof(floats[2])));
		}
	}
}

void SavePathing()
{

	char path[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(NULL), path, 255);
	for (int i = strlen(path); i > 0; i--)
	{
		if (path[i] == '\\')
		{
			path[i + 1] = 0;
			break;
		}
	}
	char size[8];
	char vecstr[64];
	char itostr[8];

	if (g_Misc->path.size() > 1)
	{
		sprintf_s(path, "\\%s_%s_%d.cfg", "pathes", "test"/*g_pEngine->GetLevelName()*/, csgo::LocalPlayer->GetTeamNum());
		printf("Path %s\n", path);
		sprintf_s(size, "%d", g_Misc->path.size() + 1);
		WritePrivateProfileStringA("Points", "Size", size, path);
		for (int i = 0; i < g_Misc->path.size(); i++)
		{
			sprintf_s(itostr, "%d", i);
			sprintf_s(vecstr, "%f %f %f", g_Misc->path.at(i).x, g_Misc->path.at(i).y, g_Misc->path.at(i).z);
			WritePrivateProfileStringA("Pathing", itostr, vecstr, path);
		}
	}
}
bool MarksIsVisible(CBaseEntity* local, Vector& vTo) 
{
	Ray_t ray;
	trace_t trace;
	CTraceFilterNoPlayer filter;
	filter.pSkip1 = local;

	ray.Init(local->GetEyePosition(), vTo);
	g_pEngineTrace->TraceRay(ray, 0x4600400B, &filter, &trace);
	return (trace.fraction > 0.99f);
}