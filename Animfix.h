#pragma once
#include "includes.h"
#include "singleton.h"
/*class animation_fix : public Singleton<animation_fix>
{
public:
	//ÝÒÎ ÍÅÏÐÀÂÈËÜÍÛÉ ÀÍÈÌÔÈÊÑ, ÎÍ ÄÅËÀÅÒÑß ÂÎÎÁÙÅ ÏÎ ÄÐÓÃÎÌÓ ÍÀÕÓÉ!!!


	/*void post_update_start()
	{
		if (g_pEngine->IsConnected() && g_pEngine->IsInGame())
		{
			if (csgo::LocalPlayer && csgo::LocalPlayer->isAlive())
			{
				for (auto i = 0; i < g_pGlobals->maxClients; i++)
				{
					CBaseEntity* pEnt = g_pEntitylist->GetClientEntity(i);

					if (pEnt == nullptr || !(pEnt->IsValidTarget()))
						continue;

					clientanimating_t *animating = nullptr;
					int animflags;

					//Make sure game is allowed to client side animate. Probably unnecessary
					for (unsigned int i = 0; i < ClientSideAnimationList->count; i++)
					{
						clientanimating_t *tanimating = (clientanimating_t*)ClientSideAnimationList->Retrieve(i, sizeof(clientanimating_t));
						CBaseEntity *pAnimEntity = (CBaseEntity*)tanimating->pAnimating;
						if (pAnimEntity == pEnt)
						{
							animating = tanimating;
							animflags = tanimating->flags;
							tanimating->flags |= FCLIENTANIM_SEQUENCE_CYCLE;
							break;
						}
					}

					auto animstate = pEnt->GetBasePlayerAnimState();
					if (animstate)
					{
						if (animstate->m_flLastClientSideAnimationUpdateTime == g_pGlobals->curtime)
							animstate->m_flLastClientSideAnimationUpdateTime -= g_pGlobals->interval_per_tick;
						if (animstate->m_iLastClientSideAnimationUpdateFramecount == g_pGlobals->framecount)
							animstate->m_iLastClientSideAnimationUpdateFramecount--;
					}

					AnimationLayer backup_layers[15];
					std::memcpy(backup_layers, pEnt->GetAnimOverlays(), (sizeof(AnimationLayer) * pEnt->GetNumAnimOverlays()));

					//Update animations/poses
					pEnt->UpdateClientSideAnimation();

					//Restore anim flags
					if (animating)
						animating->flags = animflags;

					std::memcpy(pEnt->GetAnimOverlays(), backup_layers, (sizeof(AnimationLayer) * pEnt->GetNumAnimOverlays()));
					SkipAnimation(pEnt);
				}
			}
		}
	}
	void post_render_start()
	{
		if (g_pEngine->IsConnected() && g_pEngine->IsInGame())
		{
			if (csgo::LocalPlayer && csgo::LocalPlayer->isAlive())
			{
				for (unsigned int i = 0; i < ClientSideAnimationList->count; i++)
				{
					clientanimating_t *animating = (clientanimating_t*)ClientSideAnimationList->Retrieve(i, sizeof(clientanimating_t));
					CBaseEntity *Entity = (CBaseEntity*)animating->pAnimating;

					if (Entity->IsPlayer() && Entity != csgo::LocalPlayer && !Entity->IsDormant() && Entity->isAlive())
					{
						unsigned int flags = animating->flags;
						ClientSideAnimationFlags[Entity->Index()] = flags;
						HadClientAnimSequenceCycle[Entity->Index()] = (flags & FCLIENTANIM_SEQUENCE_CYCLE);
						if (HadClientAnimSequenceCycle[Entity->Index()])
						{
							animating->flags |= 1;
						}
					}
				}
			}
		}
	}
	void pre_render_start()
	{
		if (g_pEngine->IsConnected() && g_pEngine->IsInGame())
		{
			if (csgo::LocalPlayer && csgo::LocalPlayer->isAlive())
			{
				for (unsigned int i = 0; i < ClientSideAnimationList->count; i++)
				{
					clientanimating_t *animating = (clientanimating_t*)ClientSideAnimationList->Retrieve(i, sizeof(clientanimating_t));
					CBaseEntity *Entity = (CBaseEntity*)animating->pAnimating;
					if (Entity->IsPlayer() == 35 && Entity != csgo::LocalPlayer && !Entity->IsDormant() && Entity->isAlive())
					{
						unsigned int flags = animating->flags;
						ClientSideAnimationFlags[Entity->Index()] = flags;
						HadClientAnimSequenceCycle[Entity->Index()] = (flags & FCLIENTANIM_SEQUENCE_CYCLE);
						if (HadClientAnimSequenceCycle[Entity->Index()])
						{
							animating->flags &= ~1;
						}
					}
				}
			}
		}
	}
	void local() //ãîâíî ñ þö, êîòîðîå ñïàñ÷åííî ñ gucci
	{
		csgo::LocalPlayer->GetClientSideAnimation() = true;

		auto old_curtime = g_pGlobals->curtime;
		auto old_frametime = g_pGlobals->frametime;
		auto old_ragpos = csgo::LocalPlayer->get_ragdoll_pos();
		g_pGlobals->curtime = csgo::LocalPlayer->GetSimulationTime();
		g_pGlobals->frametime = g_pGlobals->interval_per_tick;
		auto player_animation_state = reinterpret_cast<DWORD*>(csgo::LocalPlayer + 0x3894);
		auto player_model_time = reinterpret_cast<int*>(player_animation_state + 112);
		if (player_animation_state != nullptr && player_model_time != nullptr)
			if (*player_model_time == g_pGlobals->framecount)
				*player_model_time = g_pGlobals->framecount - 1;

		csgo::LocalPlayer->get_ragdoll_pos() = old_ragpos;
		csgo::LocalPlayer->UpdateClientSideAnimation();

		g_pGlobals->curtime = old_curtime;
		g_pGlobals->frametime = old_frametime;

		csgo::LocalPlayer->SetAngle2(Vector(0.f, csgo::LocalPlayer->GetBasePlayerAnimState()->m_flGoalFeetYaw, 0.f));//if u not doin dis it f*cks up the model lol

		csgo::LocalPlayer->GetClientSideAnimation() = false;
	}
private:
	bool HadClientAnimSequenceCycle[65];
	int ClientSideAnimationFlags[65];

	void SkipAnimation(CBaseEntity *player)
	{
		static ConVar *sv_pvsskipanimation = g_pCvar->FindVar("sv_pvsskipanimation");

		int32_t backup_sv_pvsskipanimation = sv_pvsskipanimation->GetInt();
		sv_pvsskipanimation->SetValue(0);

		*(int32_t*)((uintptr_t)player + 0xA30) = 0;
		*(int32_t*)((uintptr_t)player + 0x269C) = 0;

		int32_t backup_effects = *(int32_t*)((uintptr_t)player + 0xEC);
		*(int32_t*)((uintptr_t)player + 0xEC) |= 8;

		player->SetupBones(NULL, -1, 0x7FF00, g_pGlobals->curtime);

		*(int32_t*)((uintptr_t)player + 0xEC) = backup_effects;
		sv_pvsskipanimation->SetValue(backup_sv_pvsskipanimation);
	};

	class CBaseAnimating
	{
	public:

	};
	struct clientanimating_t
	{
		CBaseAnimating *pAnimating;
		unsigned int	flags;
		clientanimating_t(CBaseAnimating *_pAnim, unsigned int _flags) : pAnimating(_pAnim), flags(_flags) {}
	};

	class CUtlVectorSimple
	{
	public:
		unsigned memory;
		char pad[8];
		unsigned int count;
		inline void* Retrieve(int index, unsigned sizeofdata)
		{
			return (void*)((*(unsigned*)this) + (sizeofdata * index));
		}
	};

	clientanimating_t *animating = nullptr;
	int animflags;

	const unsigned int FCLIENTANIM_SEQUENCE_CYCLE = 0x00000001;

	CUtlVectorSimple *ClientSideAnimationList = (CUtlVectorSimple*)*(DWORD*)(FindPatternIDA(client_dll, "A1 ? ? ? ? F6 44 F0 04 01 74 0B") + 1);
};*/