#include "hooks.h"
#include "sdk.h"
#include "bullet_log.h"
#include "global.h"


void __fastcall Hooks::g_hkBeginFrame(void* thisptr)
{
	beginFrame->GetOriginalMethod<BeginFrameFn>(9)(thisptr);

	if (Menu.Visuals.BulletTracers && Menu.Visuals.bulletType == 1)
	{
		for (size_t i = 0; i < logs.size(); i++)
		{
			auto current = logs.at(i);
			g_pDebugOverlay->AddLineOverlay(current.src, current.dst, current.color.r(), current.color.g(), current.color.b(), true, -1.f);
			g_pDebugOverlay->AddBoxOverlay(current.dst, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), Menu.Colors.BulletracerBox[0] * 255, Menu.Colors.BulletracerBox[1] * 255, Menu.Colors.BulletracerBox[2] * 255, Menu.Colors.BulletracerBox[3] * 255, -1.f);
			if (fabs(g_pGlobals->curtime - current.time) > 3.f)
				logs.erase(logs.begin() + i);
		}

	}
}