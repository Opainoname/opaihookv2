#pragma once
#include "sdk.h"
#include "global.h"
#include "vmt.h"

using do_post_screen_space_effects_t = bool(__thiscall*)(void*, CViewSetup*);

bool _fastcall hkDoPostScreenSpaceEffects(void* ecx, void* edx, CViewSetup* pSetup)
{
	static auto ofunc = g_pClientModeHook->GetMethod<do_post_screen_space_effects_t>(44);
	return ofunc(ecx, pSetup);
}