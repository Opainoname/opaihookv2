#pragma once
#include "FixMove.h"
class CMisc
{
public:
	void RankReveal();
	void Bunnyhop();
	void WalkBotCM(Vector & oldang);
	void AutoStrafe();
	void FixCmd();
	void FixMovement();
	void HandleClantag();
	bool FakeLag();
	void DoCircle();
	std::vector<Vector> path;
	float m_circle_yaw = 0;
}; extern CMisc* g_Misc;

