#include "hooks.h"
#include "Menu.h"
#include "global.h"
#include "MaterialHelper.h"
#include "xor.h"
#include "BacktrackingHelper.h"
#include "Chams.h"
#include "Math.h"
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

IMaterial* CoveredLit;
IMaterial* OpenLit;
IMaterial* CoveredFlat;
IMaterial* OpenFlat;
IMaterial* backtrack;
IMaterial* materialMetall = nullptr;
IMaterial* materialMetallnZ = nullptr;

void Chams::CreateMaterialChams()
{
	CoveredLit = g_MaterialHelper->CreateMaterial(true);
	OpenLit = g_MaterialHelper->CreateMaterial(false);
	CoveredFlat = g_MaterialHelper->CreateMaterial(true, false);
	backtrack = g_MaterialHelper->CreateMaterial(true, false);
	OpenFlat = g_MaterialHelper->CreateMaterial(false, false);

	std::ofstream("csgo\\materials\\simple_ignorez_reflective.vmt") << R"#("VertexLitGeneric"
{

  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       "env_cubemap"
  "$normalmapalphaenvmapmask"  "1"
  "$envmapcontrast"             "1"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

	std::ofstream("csgo\\materials\\simple_regular_reflective.vmt") << R"#("VertexLitGeneric"
{

  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       "env_cubemap"
  "$normalmapalphaenvmapmask"  "1"
  "$envmapcontrast"             "1"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

	materialMetall = g_pMaterialSystem->FindMaterial("simple_ignorez_reflective", TEXTURE_GROUP_MODEL);
	materialMetallnZ = g_pMaterialSystem->FindMaterial("simple_regular_reflective", TEXTURE_GROUP_MODEL);
}

Chams::~Chams()
{
	std::remove("csgo\\materials\\simple_ignorez_reflective.vmt");
	std::remove("csgo\\materials\\simple_regular_reflective.vmt");
}

void __fastcall Hooks::scene_end(void* thisptr, void* edx) {

	static auto scene_end_o = renderviewVMT->GetOriginalMethod< decltype(&scene_end) >(9);
	scene_end_o(thisptr, edx);
	static bool init = false;
	if (!init)
	{
		Chams::get().CreateMaterialChams();
		init = true;
	}

	if (g_pEngine->IsConnected() && g_pEngine->IsInGame())
	{
		auto m_local = csgo::LocalPlayer;
		for (auto i = 0; i < g_GlowObjManager->size; i++)
		{
			auto glow_object = &g_GlowObjManager->m_GlowObjectDefinitions[i];

			CBaseEntity *m_entity = glow_object->m_pEntity;

			if (!glow_object->m_pEntity || glow_object->IsUnused())
				continue;

			if (m_entity->IsPlayer())
			{
				if (Menu.Visuals.Glow && m_entity->GetTeamNum() != m_local->GetTeamNum())
				{
					float Red = Menu.Colors.Glow[0];
					float Green = Menu.Colors.Glow[1];
					float Blue = Menu.Colors.Glow[2];
					float A = Menu.Colors.Glow[3];

					glow_object->m_vGlowColor = Vector(Red, Green, Blue);
					glow_object->m_flGlowAlpha = A;

					glow_object->m_bRenderWhenOccluded = true;
					glow_object->m_bRenderWhenUnoccluded = false;
				}

				if (m_entity == m_local && Menu.Visuals.LGlow)
				{
					float Red = Menu.Colors.LGlow[0];
					float Green = Menu.Colors.LGlow[1];
					float Blue = Menu.Colors.LGlow[2];
					float A = Menu.Colors.LGlow[3];

					if (Menu.Visuals.PulseLGlow) {
						glow_object->m_bPulsatingChams = true;
					}
					glow_object->m_vGlowColor = Vector(Red, Green, Blue);
					glow_object->m_flGlowAlpha = A;

					glow_object->m_bRenderWhenOccluded = true;
					glow_object->m_bRenderWhenUnoccluded = false;
				}
			
			}
		}
	}

	if (Menu.Visuals.ChamsEnable)
	{
		static IMaterial *covered;
		static IMaterial *open;

		switch (Menu.Visuals.ChamsStyle)
		{
		case 0:
			covered = CoveredLit;
			open = OpenLit;
			break;
		case 1:
			covered = CoveredFlat;
			open = OpenFlat;
			break;
		case 2:
			covered = materialMetall;
			open = materialMetallnZ;
			break;

		}

		auto b_IsThirdPerson = *reinterpret_cast<bool*>(reinterpret_cast<DWORD>(g_pInput) + 0xA5);

		for (int i = 1; i < g_pEngine->GetMaxClients(); ++i) {
			CBaseEntity* ent = (CBaseEntity*)g_pEntitylist->GetClientEntity(i);

			if (ent == csgo::LocalPlayer && csgo::LocalPlayer != nullptr)
			{
				if (csgo::LocalPlayer->isAlive() && !csgo::LocalPlayer->IsScoped())
				{
					if (b_IsThirdPerson && Menu.Visuals.ChamsL)
					{
						g_pRenderView->SetColorModulation(Menu.Colors.PlayerChamsl);

						g_pModelRender->ForcedMaterialOverride(open);
						csgo::LocalPlayer->draw_model(0x1, 255);
						g_pModelRender->ForcedMaterialOverride(nullptr);
					}
				}
			}

			if (ent->IsValidRenderable() && Menu.Visuals.ChamsPlayer)
			{
				if (Menu.Visuals.ChamsPlayerWall)
				{
					g_pRenderView->SetColorModulation(Menu.Colors.PlayerChamsWall);
					g_pModelRender->ForcedMaterialOverride(covered);
					ent->draw_model(0x1, 255);
					g_pModelRender->ForcedMaterialOverride(nullptr);
				}
				g_pRenderView->SetColorModulation(Menu.Colors.PlayerChams);
				g_pModelRender->ForcedMaterialOverride(open);
				ent->draw_model(0x1, 255);
				g_pModelRender->ForcedMaterialOverride(nullptr);
			}

			if (ent->IsValidRenderable() && Menu.Visuals.ShowBacktrack)
			{
				Vector orig = ent->GetAbsOrigin();

				if (g_BacktrackHelper->PlayerRecord[i].records.size() > 0)
				{
					tick_record record = g_BacktrackHelper->PlayerRecord[i].records.at(0);

					if (g_BacktrackHelper->IsTickValid(record) && orig != record.m_vecAbsOrigin)
					{
						g_pRenderView->SetColorModulation(Menu.Colors.ChamsHistory);

						ent->SetAbsOrigin(record.m_vecAbsOrigin);

						g_pModelRender->ForcedMaterialOverride(covered);
						ent->draw_model(0x1, 200);
						g_pModelRender->ForcedMaterialOverride(nullptr);

						ent->SetAbsOrigin(orig);
					}
				}
			}
		}
	}
}

void __fastcall Hooks::DrawModelExecute(void* ecx, void* edx, void* * ctx, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
{

	if (!csgo::LocalPlayer)
	{
		modelrenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(ecx, ctx, state, pInfo, pCustomBoneToWorld);
		return;
	}

	auto ent = g_pEntitylist->GetClientEntity(pInfo.entity_index);

	const char* ModelName = g_pModelInfo->GetModelName((model_t*)pInfo.pModel);



	if (ent == csgo::LocalPlayer)
	{

		if (csgo::LocalPlayer->IsScoped())
		{
			if (Menu.Visuals.blendonscope)
			{
				g_pRenderView->SetBlend(0.5f);
			}
			else
				g_pRenderView->SetBlend(Menu.Visuals.playeralpha / 255.f);
		}
		else
			g_pRenderView->SetBlend(Menu.Visuals.playeralpha / 255.f);
	}


	const auto mdl = pInfo.pModel;

	if (Menu.Colors.Props) {
		if (strstr(ModelName, "models/props")) {
			g_pRenderView->SetBlend(0.5f);
		}
	}

	if (Menu.Misc.WireHand) {
		if (strstr(ModelName, "arms")) {
			IMaterial* mat = g_MaterialHelper->CreateMaterial(true, false, true);
			mat->ColorModulate(Menu.Colors.styleshands[0] * 255, Menu.Colors.styleshands[1] * 255, Menu.Colors.styleshands[2] * 255);
			g_pModelRender->ForcedMaterialOverride(mat);
		}
	}

	IMaterial *xblur_mat = g_pMaterialSystem->FindMaterial("dev/blurfilterx_nohdr", TEXTURE_GROUP_OTHER, true);
	IMaterial *yblur_mat = g_pMaterialSystem->FindMaterial("dev/blurfiltery_nohdr", TEXTURE_GROUP_OTHER, true);
	IMaterial *scope = g_pMaterialSystem->FindMaterial("dev/scope_bluroverlay", TEXTURE_GROUP_OTHER, true);
	xblur_mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
	yblur_mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
	scope->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

	modelrenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(ecx, ctx, state, pInfo, pCustomBoneToWorld);
	g_pModelRender->ForcedMaterialOverride(NULL);
}
