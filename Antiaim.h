#pragma once

class CAntiaim
{
public:
	bool choke;
	void Run(QAngle org_view);
private:
	void DoAntiAims();



}; extern CAntiaim* g_Antiaim;
