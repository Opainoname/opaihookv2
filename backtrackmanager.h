#pragma once
#include "Singleton.h"

struct RecordTick_t
{
	int iIndex = -1;
	float flSimulationTime = -1.f;
	Vector vecOrigin = Vector();
	QAngle angEyeAngle = QAngle();
	QAngle angAbsAngle = QAngle();
	float flPoseParameter[24];
	VMatrix boneMatrix[128];
	Vector vecVelocity = Vector();
	int iFlags = -1;
	float flLowerBodyYaw = -1.f;
	int iLayerCount = -1;
	AnimationLayer cAnimationLayer[15];
	RecordTick_t(CBaseEntity* pEntity);
	RecordTick_t() {};
	Vector vecMin = Vector(), vecMax = Vector();
};

class CBackTrackManager : public Singleton<CBackTrackManager>
{
private:
	std::vector<RecordTick_t> RecordedTicks[64];
	int GetEstimatedServerTickCount(float latency);
	float GetLerpTime();
public:
	bool IsVisibleTick(RecordTick_t tick, bool smokecheck, bool bodycheck);
	void SetRecord(CBaseEntity* pEntity, RecordTick_t tick);
	void UpdateTicks();
	bool IsValidTick(RecordTick_t tick);
	std::vector<RecordTick_t> GetRecords(CBaseEntity* pEntity) { return RecordedTicks[pEntity->GetIndex()]; }
	RecordTick_t GetLastValidRecord(CBaseEntity* pEntity);
	std::vector<RecordTick_t> GetLastValidRecords();
};