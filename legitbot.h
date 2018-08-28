#pragma once
#include "Singleton.h"
struct AimPath_t
{
	QAngle angAimAngle = QAngle();
	bool bNoShot = false, bSilentHit = false, bForceShot = false;
};
class CLegitBot : public Singleton<CLegitBot>
{
public:
	
	void Run();
private:
	bool SettingProxy();
	QAngle Smooth(QAngle delta);
	float Get2DFov(Vector pos); //W2S length it's P
	float GetAngFov(Vector pos); //CalcAng length it's bad idea
	float Get3DFov(Vector pos); //Mad idea
	bool ItPossibleTarget(CBaseEntity* target, Vector movement); //Based on the similarity of triangles
	bool Aimbot();
	void CreateAimPath(RecordTick_t target);
	std::vector<AimPath_t> AimPath;
	RecordTick_t NearestTick;
	RecordTick_t NearestEntity;
	bool bAimboting = false;
	QAngle UserMoveDetect;
	float ReTargetTime;
	int LastTargetIndex;
	struct AimData_t
	{
		CBaseEntity* pEntity = nullptr;
		float flStandTime = 0;
		float flVisibleTime = 0;
	} AimData[64];
	struct Statictic_t
	{
		QAngle angOldEyeAngle = QAngle();
		float flAverageMovementSpeed = 0;
		int iRecordCount = 0;
	} Statictic;
	struct {
		int iBackTrackType = 2; // 0 - No BackTrack | 1 - Without aimbot | 2 - Aimbot in nearest tick
		int iFovType = 0; // 0 - Circle | 1 - Cone
		int iSmoothType = 0; //0 - Adaptive | 1 - Factor
		float flSmooth = .5; //ms
		float flFov = 90;
		bool bRecordStatictic = true;
		int iDelay = 0; // 0 - Without Delay | 1 - AutoDelay(Press) | 2 - AutoDelay(Click)
		float flSwitchTargetDelay = .2; //ms
		int iAimType = 0; //0 - Linear | 1 - Curve
		bool bUseSpecificAim = false;
		int iSpecificAimType = 0; //0 - Flick | 1 - Tapping
	} Setting;
};


