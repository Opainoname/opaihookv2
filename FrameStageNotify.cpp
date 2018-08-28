#include "hooks.h"
#include "global.h"
#include "Menu.h"
#include "Animfix.h"
#include "BacktrackingHelper.h"
#include "xor.h"
#include <intrin.h>
#include "SpoofedConvar.h"
#include "Math.h"
#include "GameUtils.h"
#include "Items.h"
#include "Aimbot.h"
#include "resolver.h"
#include "Misc.h"

std::vector<const char*> smoke_materials = {
	"particle/beam_smoke_01",
	"particle/particle_smokegrenade",
	"particle/particle_smokegrenade1",
	"particle/particle_smokegrenade2",
	"particle/particle_smokegrenade3",
	"particle/particle_smokegrenade_sc",
	"particle/smoke1/smoke1",
	"particle/smoke1/smoke1_ash",
	"particle/smoke1/smoke1_nearcull",
	"particle/smoke1/smoke1_nearcull2",
	"particle/smoke1/smoke1_snow",
	"particle/smokesprites_0001",
	"particle/smokestack",
	"particle/vistasmokev1/vistasmokev1",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
	"particle/vistasmokev1/vistasmokev1_fire",
	"particle/vistasmokev1/vistasmokev1_nearcull",
	"particle/vistasmokev1/vistasmokev1_nearcull_fog",
	"particle/vistasmokev1/vistasmokev1_nearcull_nodepth",
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev4_emods_nocull",
	"particle/vistasmokev1/vistasmokev4_nearcull",
	"particle/vistasmokev1/vistasmokev4_nocull"
};

void DrawBeam(Vector src, Vector end, Color color)
{
	BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMTESLA;
	beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
	beamInfo.m_nModelIndex = -1;
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = 3.0f;
	beamInfo.m_flWidth = 2.0f;
	beamInfo.m_flEndWidth = 2.0f;
	beamInfo.m_flFadeLength = 0.0f;
	beamInfo.m_flAmplitude = 2.0f;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.2f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_flRed = color.r();
	beamInfo.m_flGreen = color.g();
	beamInfo.m_flBlue = color.b();
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = end;
	Beam_t* myBeam = g_pViewRenderBeams->CreateBeamPoints(beamInfo);
	if (myBeam)
		g_pViewRenderBeams->DrawBeam(myBeam);
}

void __stdcall Hooks::FrameStageNotify(ClientFrameStage_t curStage)
{

/*	if (curStage == FRAME_RENDER_START)
	{
		if (Menu.Ragebot.AnimFix)
		{
			for (int i = 1; i < g_pGlobals->maxClients; i++)
			{
				CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);
				if (pEntity != nullptr)
				{
					CBasePlayerAnimState* Animationss = pEntity->GetBasePlayerAnimState(); // DONT ASK ABOUT IT LEL
					if (Animationss->m_iLastClientSideAnimationUpdateFramecount)
						Animationss->m_iLastClientSideAnimationUpdateFramecount--;
					if (Animationss->m_flLastClientSideAnimationUpdateTime)
						Animationss->m_flLastClientSideAnimationUpdateTime -= g_pGlobals->interval_per_tick;
				}
			}
		}
	}
	*/
	static std::string old_Skyname = "";
	static bool OldNightmode;
	static int OldSky;
	if (!csgo::LocalPlayer || !g_pEngine->IsConnected() || !g_pEngine->IsInGame())
	{
		clientVMT->GetOriginalMethod<FrameStageNotifyFn>(37)(curStage);
		old_Skyname = "";
		OldNightmode = false;
		OldSky = 0;
		return;
	}
	static ConVar*r_DrawSpecificStaticProp;
	if (OldNightmode != Menu.Visuals.nightmode)
	{
		if (!r_DrawSpecificStaticProp)
			r_DrawSpecificStaticProp = g_pCvar->FindVar("r_DrawSpecificStaticProp");
		r_DrawSpecificStaticProp->SetValue(0);
		for (MaterialHandle_t i = g_pMaterialSystem->FirstMaterial(); i != g_pMaterialSystem->InvalidMaterial(); i = g_pMaterialSystem->NextMaterial(i))
		{
			IMaterial* pMaterial = g_pMaterialSystem->GetMaterial(i);
			if (!pMaterial)
				continue;
			if (strstr(pMaterial->GetTextureGroupName(), "World") || strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
			{
				if (Menu.Visuals.nightmode)
					if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
						pMaterial->ColorModulate(0.3f, 0.3f, 0.3f);
					else
						pMaterial->ColorModulate(0.05f, 0.05f, 0.05f);
				else
					pMaterial->ColorModulate(1.0f, 1.0f, 1.0f);
			}
		}
		OldNightmode = Menu.Visuals.nightmode;
	}
	if (OldSky != Menu.Visuals.Skybox)
	{
		auto LoadNamedSky = reinterpret_cast<void(__fastcall*)(const char*)>(FindPatternIDA("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));
		if (old_Skyname == "")
		{
			//old_Skyname = g_pCvar->FindVar("sv_skyname")->GetName();
		}
		int type = Menu.Visuals.Skybox;
		/*if(type == 0)
		LoadNamedSky(old_Skyname.c_str());
		else */if (type == 1)
			LoadNamedSky("cs_baggage_skybox_");
		else if (type == 2)
			LoadNamedSky("cs_tibet");
		else if (type == 3)
			LoadNamedSky("italy");
		else if (type == 4)
			LoadNamedSky("jungle");
		else if (type == 5)
			LoadNamedSky("office");
		else if (type == 6)
			LoadNamedSky("sky_cs15_daylight02_hdr");
		else if (type == 7)
			LoadNamedSky("sky_csgo_night02");
		else if (type == 8)
			LoadNamedSky("vertigo");
		OldSky = Menu.Visuals.Skybox;
	}
	static Vector oldViewPunch;
	static Vector oldAimPunch;
	Vector* view_punch = csgo::LocalPlayer->GetViewPunchPtr();
	Vector* aim_punch = csgo::LocalPlayer->GetPunchAnglePtr();
	if (curStage == FRAME_RENDER_START && csgo::LocalPlayer->isAlive())
	{
		static bool enabledtp = false, check = false;

		if (GetAsyncKeyState(Menu.Misc.TPKey) && csgo::LocalPlayer->isAlive())
		{
			if (!check)
				enabledtp = !enabledtp;
			check = true;
		}
		else
			check = false;
		CBasePlayerAnimState* Animations = csgo::LocalPlayer->GetBasePlayerAnimState();
		if (enabledtp)
		{
			if (Menu.Misc.TPangles == 0)
			{
				*reinterpret_cast<QAngle*>(reinterpret_cast<DWORD>(csgo::LocalPlayer + 0x31C0 + 0x8)) = csgo::FakeAngle;
			}
			else if (Menu.Misc.TPangles == 1)
			{
				*reinterpret_cast<QAngle*>(reinterpret_cast<DWORD>(csgo::LocalPlayer + 0x31C0 + 0x8)) = csgo::RealAngle, csgo::PitchAngle = 89;
			}
			else if (Menu.Misc.TPangles == 2)
			{
				*reinterpret_cast<QAngle*>(reinterpret_cast<DWORD>(csgo::LocalPlayer + 0x31C0 + 0x8)) = QAngle(csgo::UserCmd->viewangles.x, csgo::LocalPlayer->LowerBodyYaw(), csgo::UserCmd->viewangles.z);
			}
		}
		if (view_punch && aim_punch && Menu.Visuals.Novisrevoil)
		{
			oldViewPunch = *view_punch;
			oldAimPunch = *aim_punch;
			view_punch->Init();
			aim_punch->Init();
		}
		if (enabledtp && csgo::LocalPlayer->isAlive())
		{
			*(bool*)((DWORD)g_pInput + 0xA5) = true;
			*(float*)((DWORD)g_pInput + 0xA8 + 0x8) = Menu.Visuals.thirdperson_dist; // Thirdperson Distance
		}
		else
		{
			*(bool*)((DWORD)g_pInput + 0xA5) = false;
			*(float*)((DWORD)g_pInput + 0xA8 + 0x8);
		}
	}
	if (curStage == FRAME_NET_UPDATE_START)
	{
		if (Menu.Visuals.BulletTracers && Menu.Visuals.bulletType == 0)
		{
			float Red, Green, Blue;
			Red = Menu.Colors.Bulletracer[0] * 255;
			Green = Menu.Colors.Bulletracer[1] * 255;
			Blue = Menu.Colors.Bulletracer[2] * 255;
			for (unsigned int i = 0; i < trace_logs.size(); i++) {
				auto *shooter = g_pEntitylist->GetClientEntity(g_pEngine->GetPlayerForUserID(trace_logs[i].userid));
				if (!shooter) 
					return;
				DrawBeam(trace_logs[i].start, trace_logs[i].position, Color(Red, Green, Blue, 255));
				trace_logs.erase(trace_logs.begin() + i);
			}
		}
		for (auto material_name : smoke_materials) {
			IMaterial* mat = g_pMaterialSystem->FindMaterial(material_name, TEXTURE_GROUP_OTHER);
			mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Menu.Visuals.Nosmoke ? true : false);
		}
		if (Menu.Visuals.Nosmoke) {
			static int* smokecount = *(int**)(FindPatternIDA(client_dll, "8B 1D ? ? ? ? 56 33 F6 57 85 DB") + 0x2);
			*smokecount = 0;
		}
		if (Menu.Visuals.NoFlash) {
			IMaterial* Flash = g_pMaterialSystem->FindMaterial("effects\\flashbang", "ClientEffect textures");
			IMaterial* FlashWhite = g_pMaterialSystem->FindMaterial("effects\\flashbang_white", "ClientEffect textures");
			Flash->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			FlashWhite->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		}
		
	}
	if (curStage == FRAME_RENDER_START)
	{
		for (int i = 1; i < g_pGlobals->maxClients; i++)
		{
			if (i == g_pEngine->GetLocalPlayer())
				continue;
			CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);
			if (pEntity)
			{
				if (pEntity->GetHealth() > 0 && !pEntity->IsDormant())
				{
					*(int*)((uintptr_t)pEntity + 0xA30) = g_pGlobals->framecount; //we'll skip occlusion checks now
					*(int*)((uintptr_t)pEntity + 0xA28) = 0;//clear occlusion flags
				}
			}
		}
	}
	/*if (curStage == FRAME_RENDER_START)
	{
		if (Menu.Ragebot.AnimFix)
		{
			if (csgo::LocalPlayer && csgo::LocalPlayer->isAlive()) {
				animation_fix::get().local();
			}
		}
	}*/

	if (curStage == FRAME_RENDER_START)
	{
		if (csgo::LocalPlayer && csgo::LocalPlayer->isAlive())
		{
			for (int i = 1; i < g_pGlobals->maxClients; i++)
			{
				CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);
				if (pEntity)
				{
					if (pEntity->GetHealth() > 0)
					{
						if (i != g_pEngine->GetLocalPlayer())
						{
							VarMapping_t* map = pEntity->GetVarMap();
							if (!map) return;
							for (int i = 0; i < map->m_nInterpolatedEntries; i++)
							{
								VarMapEntry_t *e = &map->m_Entries[i];
								e->m_bNeedsToInterpolate = false;
							}

						}
					}
				}
			}
		}
	}

	if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		if (csgo::LocalPlayer && csgo::LocalPlayer->isAlive())
		{
			if (Menu.Ragebot.AutomaticResolver)
			{
				for (int i = 0; i < g_pGlobals->maxClients; i++) {
					auto pEntity = g_pEntitylist->GetClientEntity(i);
					if (pEntity == nullptr || !pEntity->IsValidTarget())
						continue;
					Resolver::Do(pEntity);
				}
			}
		}
		/*if (Menu.Ragebot.AnimFix)
			animation_fix::get().post_update_start();*/
	}

	if (curStage == FRAME_RENDER_START)
	{
		for (int i = 1; i < g_pGlobals->maxClients; i++)
		{
			CBaseEntity* pEntity = g_pEntitylist->GetClientEntity(i);
			if (pEntity)
			{
				if (pEntity->GetHealth() > 0 && !pEntity->IsDormant())
				{
					g_BacktrackHelper->UpdateBacktrackRecords(pEntity);
				}
			}
		}
	}

	clientVMT->GetOriginalMethod<FrameStageNotifyFn>(37)(curStage);
	if (curStage == FRAME_RENDER_START && csgo::LocalPlayer && csgo::LocalPlayer->GetHealth() > 0)
	{
		*(Vector*)((DWORD)csgo::LocalPlayer + 0x31C8) = csgo::FakeAngle;

		if (Menu.Visuals.Novisrevoil)
		{
			*aim_punch = oldAimPunch;
			*view_punch = oldViewPunch;
		}
	}
	/*if (curStage == FRAME_RENDER_START)
		animation_fix::get().post_render_start();*/
}


