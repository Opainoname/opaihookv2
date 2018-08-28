#pragma once
#include "../sdk.h"

struct Weapon_Info
{
	char pad00[0xC8];
	int m_WeaponType; // 0xC8
	char padCC[0x20];
	int m_Damage; // 0xEC
	float m_ArmorRatio; // 0xF0
	char padF4[0x4];
	float m_Penetration; // 0xF8
	char padFC[0x8];
	float m_Range; // 0x104
	float m_RangeModifier; // 0x108
	char pad10C[0x10];
	bool m_HasSilencer; // 0x11C
};
enum WEAPONCLASS
{
	WEPCLASS_INVALID,
	WEPCLASS_RIFLE,
	WEPCLASS_PISTOL,
	WEPCLASS_SHOTGUN,
	WEPCLASS_SNIPER,
	WEPCLASS_SMG,
	WEPCLASS_MACHINEGUN,
	WEPCLASS_KNIFE,
};
enum ItemDefinitionIndex : int {
	DEAGLE = 1,
	ELITE = 2,
	FIVESEVEN = 3,
	GLOCK = 4,
	AK47 = 7,
	AUG = 8,
	AWP = 9,
	FAMAS = 10,
	G3SG1 = 11,
	GALILAR = 13,
	M249 = 14,
	M4A1 = 16,
	MAC10 = 17,
	P90 = 19,
	MP5 = 23,
	UMP45 = 24,
	XM1014 = 25,
	BIZON = 26,
	MAG7 = 27,
	NEGEV = 28,
	SAWEDOFF = 29,
	TEC9 = 30,
	TASER = 31,
	HKP2000 = 32,
	MP7 = 33,
	MP9 = 34,
	NOVA = 35,
	P250 = 36,
	SCAR20 = 38,
	SG556 = 39,
	SSG08 = 40,
	KNIFE = 42,
	FLASHBANG = 43,
	HEGRENADE = 44,
	SMOKEGRENADE = 45,
	MOLOTOV = 46,
	DECOY = 47,
	INCGRENADE = 48,
	C4 = 49,
	KNIFE_T = 59,
	M4A1_SILENCER = 60,
	USP_SILENCER = 61,
	CZ75A = 63,
	REVOLVER = 64,
	KNIFE_BAYONET = 500,
	KNIFE_FLIP = 505,
	KNIFE_GUT = 506,
	KNIFE_KARAMBIT = 507,
	KNIFE_M9_BAYONET = 508,
	KNIFE_TACTICAL = 509,
	KNIFE_FALCHION = 512,
	KNIFE_BUTTERFLY = 515,
	KNIFE_PUSH = 516,
	KNIFE_BOWIE = 514,
	KNIFE_NAVAJA = 520,
	KNIFE_STILETTO = 522,
	KNIFE_URSUS = 519,
	KNIFE_TALON = 523,
};
struct CHudTexture
{
	char	szShortName[64];	//0x0000
	char	szTextureFile[64];	//0x0040
	bool	bRenderUsingFont;	//0x0080
	bool	bPrecached;			//0x0081
	char	cCharacterInFont;	//0x0082
	BYTE	pad_0x0083;			//0x0083
	int		hFont;				//0x0084
	int		iTextureId;			//0x0088
	float	afTexCoords[4];		//0x008C
	int		iPosX[4];			//0x009C
}; //Size=0x00AC

   /*class CSWeaponInfo
   {
   public:

   virtual ~CSWeaponInfo() {};

   char*		consoleName;				// 0x0004
   char		pad_0008[12];				// 0x0008
   int			iMaxClip1;					// 0x0014
   int			iMaxClip2;					// 0x0018
   int			iDefaultClip1;				// 0x001C
   int			iDefaultClip2;				// 0x0020
   char		pad_0024[8];				// 0x0024
   char*		szWorldModel;				// 0x002C
   char*		szViewModel;				// 0x0030
   char*		szDroppedModel;				// 0x0034
   char		pad_0038[4];				// 0x0038
   char*		N0000023E;					// 0x003C
   char		pad_0040[56];				// 0x0040
   char*		szEmptySound;				// 0x0078
   char		pad_007C[4];				// 0x007C
   char*		szBulletType;				// 0x0080
   char		pad_0084[4];				// 0x0084
   char*		szHudName;					// 0x0088
   char*		szWeaponName;				// 0x008C
   char		pad_0090[56];				// 0x0090
   int 		WeaponType;					// 0x00C8
   int			iWeaponPrice;				// 0x00CC
   int			iKillAward;					// 0x00D0
   char*		szAnimationPrefix;			// 0x00D4
   float		flCycleTime;				// 0x00D8
   float		flCycleTimeAlt;				// 0x00DC
   float		flTimeToIdle;				// 0x00E0
   float		flIdleInterval;				// 0x00E4
   bool		bFullAuto;					// 0x00E8
   char		pad_0x00E5[3];			// 0x00E9
   int			iDamage;					// 0x00EC
   float		flArmorRatio;				// 0x00F0
   int			iBullets;					// 0x00F4
   float		flPenetration;				// 0x00F8
   float		flFlinchVelocityModifierLarge;	// 0x00FC
   float		flFlinchVelocityModifierSmall;	// 0x0100
   float		flRange;					// 0x0104
   float		flRangeModifier;			// 0x0108
   float		flThrowVelocity;			// 0x010C
   char		pad_0x010C[12];			// 0x0110
   bool		bHasSilencer;				// 0x011C
   char		pad_0x0119[3];			// 0x011D
   char*		pSilencerModel;				// 0x0120
   int			iCrosshairMinDistance;		// 0x0124
   int			iCrosshairDeltaDistance;	// 0x0128 - iTeam?
   float		flMaxPlayerSpeed;			// 0x012C
   float		flMaxPlayerSpeedAlt;		// 0x0130
   float		flSpread;					// 0x0134
   float		flSpreadAlt;				// 0x0138
   float		flInaccuracyCrouch;			// 0x013C
   float		flInaccuracyCrouchAlt;		// 0x0140
   float		flInaccuracyStand;			// 0x0144
   float		flInaccuracyStandAlt;		// 0x0148
   float		flInaccuracyJumpInitial;	// 0x014C
   float		flInaccuracyJump;			// 0x0150
   float		flInaccuracyJumpAlt;		// 0x0154
   float		flInaccuracyLand;			// 0x0158
   float		flInaccuracyLandAlt;		// 0x015C
   float		flInaccuracyLadder;			// 0x0160
   float		flInaccuracyLadderAlt;		// 0x0164
   float		flInaccuracyFire;			// 0x0168
   float		flInaccuracyFireAlt;		// 0x016C
   float		flInaccuracyMove;			// 0x0170
   float		flInaccuracyMoveAlt;		// 0x0174
   float		flInaccuracyReload;			// 0x0178
   int			iRecoilSeed;				// 0x017C
   float		flRecoilAngle;				// 0x0180
   float		flRecoilAngleAlt;			// 0x0184
   float		flRecoilAngleVariance;		// 0x0188
   float		flRecoilAngleVarianceAlt;	// 0x018C
   float		flRecoilMagnitude;			// 0x0190
   float		flRecoilMagnitudeAlt;		// 0x0194
   float		flRecoilMagnitudeVariance;	// 0x0198
   float		flRecoilMagnitudeVarianceAlt;	// 0x019C
   float		flRecoveryTimeCrouch;		// 0x01A0
   float		flRecoveryTimeStand;		// 0x01A4
   float		flRecoveryTimeCrouchFinal;	// 0x01A8
   float		flRecoveryTimeStandFinal;	// 0x01AC
   int			iRecoveryTransitionStartBullet;	// 0x01B0
   int			iRecoveryTransitionEndBullet;	// 0x01B4
   bool		bUnzoomAfterShot;			// 0x01B8
   bool		bHideViewModelZoomed;		// 0x01B9
   char		pad_0x01B5[2];			// 0x01BA
   char		iZoomLevels[3];			// 0x01BC
   int			iZoomFOV[2];				// 0x01C0
   float		fZoomTime[3];				// 0x01C4
   char*		szWeaponClass;				// 0x01D4
   float		flAddonScale;				// 0x01D8
   char		pad_0x01DC[4];			// 0x01DC
   char*		szEjectBrassEffect;			// 0x01E0
   char*		szTracerEffect;				// 0x01E4
   int			iTracerFrequency;			// 0x01E8
   int			iTracerFrequencyAlt;		// 0x01EC
   char*		szMuzzleFlashEffect_1stPerson;	// 0x01F0
   char		pad_0x01F4[4];				// 0x01F4
   char*		szMuzzleFlashEffect_3rdPerson;	// 0x01F8
   char		pad_0x01FC[4];			// 0x01FC
   char*		szMuzzleSmokeEffect;		// 0x0200
   float		flHeatPerShot;				// 0x0204
   char*		szZoomInSound;				// 0x0208
   char*		szZoomOutSound;				// 0x020C
   float		flInaccuracyPitchShift;		// 0x0210
   float		flInaccuracySoundThreshold;	// 0x0214
   float		flBotAudibleRange;			// 0x0218
   char		pad_0x0218[8];			// 0x0220
   char*		pWrongTeamMsg;				// 0x0224
   bool		bHasBurstMode;				// 0x0228
   char		pad_0x0225[3];			// 0x0229
   bool		bIsRevolver;				// 0x022C
   bool		bCannotShootUnderwater;		// 0x0230
   };*/

class CSWeaponInfo
{
public:
	char _0x0000[20];
	__int32 max_clip;			//0x0014 
	char _0x0018[12];
	__int32 max_reserved_ammo;	//0x0024 
	char _0x0028[96];
	char* hud_name;				//0x0088 
	char* weapon_name;			//0x008C 
	char _0x0090[60];
	
	__int32 type;				//0x00CC 
	__int32 price;				//0x00D0 
	__int32 reward;				//0x00D4 
	char _0x00D8[20];
	BYTE full_auto;				//0x00EC 
	char _0x00ED[3];
	__int32 damage;				//0x00F0 
	float armor_ratio;			//0x00F4 
	__int32 bullets;			//0x00F8 
	float penetration;			//0x00FC 
	char _0x0100[8];
	float range;				//0x0108 
	float range_modifier;		//0x010C 
	char _0x0110[16];
	BYTE silencer;				//0x0120 
	char _0x0121[15];
	float max_speed;			//0x0130 
	float max_speed_alt;		//0x0134 
	char _0x0138[76];
	__int32 recoil_seed;		//0x0184 
	char _0x0188[32];
};

class CBomb
{
public:
	float GetC4BlowTime()
	{
		return *reinterpret_cast<float*>((DWORD)this + offys.m_flC4Blow);
	}
	HANDLE GetOwnerEntity()
	{
		return *reinterpret_cast<HANDLE*>((DWORD)this + offys.m_hOwnerEntity);
	}
	bool IsDefused()
	{
		return *reinterpret_cast<bool*>((DWORD)this + offys.m_bBombDefused);
	}
};
class CGrenade
{
public:
	bool IsPinPulled()
	{
		return *reinterpret_cast<bool*>((DWORD)this + offys.m_bPinPulled);
	}
	float GetThrowTime()
	{
		return *reinterpret_cast<float*>((DWORD)this + offys.m_fThrowTime);
	}
};
class CBaseCombatWeapon
{
	template<class T>
	T GetFieldValue(int offset) {
		return *(T*)((DWORD)this + offset);
	}
	template<class T>
	T* GetFieldPointer(int offset) {
		return (T*)((DWORD)this + offset);
	}
public:

	void SetPattern(int skin, int quality, int seed, int stattrak, const char* name)
	{

		*(int*)((DWORD)this + offys.m_nFallbackPaintKit) = skin;
		*(int*)((DWORD)this + offys.m_iEntityQuality) = quality;
		*(int*)((DWORD)this + offys.m_nFallbackSeed) = seed;
		*(int*)((DWORD)this + offys.m_nFallbackStatTrak) = stattrak;
		*(float*)((DWORD)this + offys.m_flFallbackWear) = 0.0001f;

		if (name != "") {
			char* a = (char*)((DWORD)this + offys.m_szCustomName);
			sprintf_s(a, 32, "%s", name);
		}

		*(int*)((DWORD)this + offys.m_iItemIDHigh) = -1;
	}

	//CBaseViewModel* GetViewModel()
	//{
	//	if (!this)
	//		return nullptr;
	//static std::uintptr_t offset = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_hViewModel[0]"));
	//return (CBaseViewModel*)I::ClientCBaseEntityList->GetClientEntityFromHandle(GetFieldValue<CHandle<CBaseViewModel>>(offset));
	//}

	HANDLE m_hWeaponWorldModel()
	{
		return *(HANDLE*)((uintptr_t)this + offys.m_hWeaponWorldModel);
	}

	int* OwnerXuidLow()
	{
		return (int*)((uintptr_t)this + offys.m_OriginalOwnerXuidLow);
	}
	int* OwnerXuidHigh()
	{
		return (int*)((uintptr_t)this + offys.m_OriginalOwnerXuidHigh);
	}

	bool IsValid()
	{
		WeaponType t = get_wpn_type();

		if (t != WeaponType::Knife && t != WeaponType::Grenade && t != WeaponType::Bomb)
		{
			if (Clip1() > 0)
				return true;
		}
		return false;
	}
	enum WeaponType : byte
	{
		Shotgun,
		Pistol,
		Automatic,
		Sniper,
		Grenade,
		Knife,
		Bomb
	};

	WeaponType get_wpn_type()
	{
		auto WeaponID = this->GetItemDefinitionIndex();

		if (WeaponID == WEAPON_XM1014 || WeaponID == WEAPON_NOVA || WeaponID == WEAPON_SAWEDOFF || WeaponID == WEAPON_MAG7)
			return WeaponType::Shotgun;

		if (WeaponID == WEAPON_P2000
			|| WeaponID == WEAPON_USPS
			|| WeaponID == WEAPON_GLOCK
			|| WeaponID == WEAPON_DUALS
			|| WeaponID == WEAPON_P250
			|| WeaponID == WEAPON_CZ75
			|| WeaponID == WEAPON_P2000
			|| WeaponID == WEAPON_FIVE7
			|| WeaponID == WEAPON_TEC9
			|| WeaponID == WEAPON_DEAGLE
			|| WeaponID == WEAPON_REVOLVER)
			return WeaponType::Pistol;

		if (WeaponID == WEAPON_M249
			|| WeaponID == WEAPON_NEGEV
			|| WeaponID == WEAPON_MAC10
			|| WeaponID == WEAPON_MP9
			|| WeaponID == WEAPON_MP7
			|| WeaponID == WEAPON_UMP45
			|| WeaponID == WEAPON_P90
			|| WeaponID == WEAPON_BIZON
			|| WeaponID == WEAPON_FAMAS
			|| WeaponID == WEAPON_GALIL
			|| WeaponID == WEAPON_M4A1
			|| WeaponID == WEAPON_M4A1S
			|| WeaponID == WEAPON_AUG
			|| WeaponID == WEAPON_SG553
			|| WeaponID == WEAPON_AK47)
			return WeaponType::Automatic;

		if (WeaponID == WEAPON_AWP
			|| WeaponID == WEAPON_SCAR20
			|| WeaponID == WEAPON_G3SG1)
			return WeaponType::Sniper;

		if (WeaponID == WEAPON_FLASH
			|| WeaponID == WEAPON_HE
			|| WeaponID == WEAPON_DECOY
			|| WeaponID == WEAPON_SMOKE
			|| WeaponID == WEAPON_MOLOTOV
			|| WeaponID == WEAPON_INC)
			return WeaponType::Grenade;

		if (WeaponID == WEAPON_KNIFE_SHADOWDAGGERS
			|| WeaponID == WEAPON_KNIFE_T
			|| WeaponID == WEAPON_KNIFE_CT
			|| WeaponID == WEAPON_KNIFE_BAYONET
			|| WeaponID == WEAPON_KNIFE_FLIP
			|| WeaponID == WEAPON_KNIFE_GUT
			|| WeaponID == WEAPON_KNIFE_KARAMBIT
			|| WeaponID == WEAPON_KNIFE_M9BAYONET
			|| WeaponID == WEAPON_KNIFE_HUNTSMAN
			|| WeaponID == WEAPON_KNIFE_FALCHION
			|| WeaponID == WEAPON_KNIFE_BUTTERFLY ||
			WeaponID == KNIFE_NAVAJA ||
			WeaponID == KNIFE_STILETTO ||
			WeaponID == KNIFE_URSUS ||
			WeaponID == KNIFE_TALON)
			return WeaponType::Knife;

		if (WeaponID == WEAPON_C4)
			return WeaponType::Bomb;
	}

	int* GetEntityQuality() {
		// DT_BaseAttributableItem -> m_AttributeManager -> m_Item -> m_iEntityQuality
		return (int*)((DWORD)this + 0x2D70 + 0x40 + 0x1DC);
	}

	float* FallbackWear()
	{
		return (float*)((uintptr_t)this + offys.m_flFallbackWear);
	}

	inline int* GetFallbackPaintKit() {
		// DT_BaseAttributableItem -> m_nFallbackPaintKit: 0x3140
		return (int*)((DWORD)this + offys.m_nFallbackPaintKit);
	}

	int* ItemIDHigh()
	{
		return (int*)((uintptr_t)this + 0x1F0);
	}

	int* ViewModelIndex()
	{
		return (int*)((uintptr_t)this + offys.m_iViewModelIndex);
	}

	int* WorldModelIndex()
	{
		return (int*)((uintptr_t)this + offys.m_iWorldModelIndex);
	}

	int* ModelIndex()
	{
		return (int*)((uintptr_t)this + offys.m_nModeIndex);
	}

	short* ItemDefinitionIndex()
	{
		return (short*)((uintptr_t)this + 0x1D8);
	}

	model_t* CBaseCombatWeapon::GetModel()
	{
		return *(model_t**)((DWORD)this + 0x6C);
	}
	Vector GetOrigin()
	{
		return *reinterpret_cast<Vector*>((DWORD)this + 0x0134);
	}
	HANDLE GetOwnerCBaseEntity()
	{
		return *reinterpret_cast<HANDLE*>((DWORD)this + 0x0148);
	}

	float NextPrimaryAttack()
	{
		return *(float*)((DWORD)this + 0x3208);
	}

	float NextSecondaryAttack()
	{
		return *reinterpret_cast<float*>((DWORD)this + 0x31CC);
	}
	float GetAccuracyPenalty()
	{
		return *reinterpret_cast<float*>((DWORD)this + offys.m_fAccuracyPenalty);
	}
	int Clip1()
	{
		return *reinterpret_cast<int*>((DWORD)this + offys.m_iClip1);
	}

	char* GetGunText()
	{
		int WeaponId = this->WeaponID();
		switch (WeaponId)
		{
		case KNIFE:
		case 500:
		case 505:
		case 506:
		case 507:
		case 508:
		case 509:
		case 512:
		case 514:
		case 515:
		case 516:
		case 520:
		case 522:
		case 519:
		case 523:
			return "KNIFE";
		case DEAGLE:
			return "DEAGLE";
		case ELITE:
			return "BERRETS";
		case FIVESEVEN:
			return "FIVESEVEN";
		case GLOCK:
			return "GLOCK";
		case HKP2000:
			return "P2000";
		case P250:
			return "P250";
		case USP_SILENCER:
			return "USP";
		case TEC9:
			return "TEC9";
		case CZ75A:
			return "CZ75A";
		case REVOLVER:
			return "REVOLVER";
		case MAC10:
			return "MAC10";
		case UMP45:
			return "UMP45";
		case BIZON:
			return "BIZON";
		case MP7:
			return "MP7";
		case MP9:
			return "MP9";
		case P90:
			return "P90";
		case GALILAR:
			return "GALILAR";
		case FAMAS:
			return "FAMAS";
		case M4A1_SILENCER:
			return "M4A1-S";
		case M4A1:
			return "M4A1";
		case AUG:
			return "AUG";
		case SG556:
			return "SSG553";
		case AK47:
			return "AK47";
		case G3SG1:
			return "G3SG1";
		case SCAR20:
			return "SCAR20";
		case AWP:
			return "AWP";
		case SSG08:
			return "SSG08";
		case XM1014:
			return "XM1014";
		case SAWEDOFF:
			return "SAWED";
		case MAG7:
			return "MAG7";
		case NOVA:
			return "NOVA";
		case NEGEV:
			return "NEGEV";
		case M249:
			return "M249";
		case TASER:
			return "TASER";
		case FLASHBANG:
			return "FLASHBANG";
		case HEGRENADE:
			return "GRENADE";
		case SMOKEGRENADE:
			return "SMOKE";
		case MOLOTOV:
			return "MOLOTOV";
		case DECOY:
			return "DECOY";
		case INCGRENADE:
			return "MOLOTOV";
		case C4:
			return "C4";
		default:
			return " ";
		}
	}

	int GetMaxAmmoReserve(void)
	{
		return *(int*)((DWORD)this + offys.m_iPrimaryReserveAmmoCount);
	}
	int GetLoadedAmmo()
	{
		return *(int*)((DWORD)this + offys.m_iClip1);
	}
	int WeaponID()
	{
		return GetItemDefinitionIndex();
	}
	short GetItemDefinitionIndex(void)
	{
		if (!this) return 0;
		return *(short*)((DWORD)this + offys.m_iItemDefinitionIndex);
	}
	float GetPostponeFireReadyTime()
	{
		return *reinterpret_cast<float*>((DWORD)this + offys.m_flPostponeFireReadyTime);
	}
	int GetZoomLevel()
	{
		return *reinterpret_cast<int*>((DWORD)this + 0x3330);
	}
	const char* GetWeaponName()
	{
		typedef const char*(__thiscall* GetWeaponNameFn)(void*);
		GetWeaponNameFn Name = (GetWeaponNameFn)((*(PDWORD_PTR*)this)[378]);
		return Name(this);
	}
	std::string GetName(bool Ammo)
	{
		const char* name = GetWeaponName();
		std::string Name = name;
		std::string NName;
		NName = Name.substr(7, Name.length() - 7);


		if (Ammo && !this->IsMiscWeapon())
		{
			char buffer[32]; sprintf_s(buffer, " [%i]", Clip1());
			NName.append(buffer);
			return NName;
		}
		return NName;
	}
	char* GetGunIcon()
	{
		int WeaponId = this->WeaponID();
		switch (WeaponId)
		{
		case KNIFE:
		case 500:
		case 505:
		case 506:
		case 507:
		case 508:
		case 509:
		case 512:
		case 514:
		case 515:
		case 516:
		case 520:
		case 522:
		case 519:
		case 523:
			return "]";
		case DEAGLE:
			return "A";
		case ELITE:
			return "B";
		case FIVESEVEN:
			return "C";
		case GLOCK:
			return "D";
		case HKP2000:
			return "E";
		case P250:
			return "F";
		case USP_SILENCER:
			return "G";
		case TEC9:
			return "H";
		case CZ75A:
			return "I";
		case REVOLVER:
			return "J";
		case MAC10:
			return "K";
		case UMP45:
			return "L";
		case BIZON:
			return "M";
		case MP7:
			return "N";
		case MP9:
			return "O";
		case P90:
			return "P";
		case GALILAR:
			return "Q";
		case FAMAS:
			return "R";
		case M4A1_SILENCER:
			return "S";
		case M4A1:
			return "T";
		case AUG:
			return "U";
		case SG556:
			return "V";
		case AK47:
			return "W";
		case G3SG1:
			return "X";
		case SCAR20:
			return "Y";
		case AWP:
			return "Z";
		case SSG08:
			return "a";
		case XM1014:
			return "b";
		case SAWEDOFF:
			return "c";
		case MAG7:
			return "d";
		case NOVA:
			return "e";
		case NEGEV:
			return "f";
		case M249:
			return "g";
		case TASER:
			return "h";
		case FLASHBANG:
			return "i";
		case HEGRENADE:
			return "j";
		case SMOKEGRENADE:
			return "k";
		case MOLOTOV:
			return "l";
		case DECOY:
			return "m";
		case INCGRENADE:
			return "n";
		case C4:
			return "o";
		default:
			return " ";
		}
	}
	int GetWeaponType()
	{
		if (!this) return WEPCLASS_INVALID;
		auto id = this->WeaponID();
		switch (id)
		{
		case DEAGLE:
		case ELITE:
		case FIVESEVEN:
		case HKP2000:
		case USP_SILENCER:
		case CZ75A:
		case TEC9:
		case REVOLVER:
		case GLOCK:
		case P250:
			return WEPCLASS_PISTOL;
			break;
		case AK47:
		case M4A1:
		case M4A1_SILENCER:
		case GALILAR:
		case AUG:
		case FAMAS:
		case SG556:
		case MP5:
			return WEPCLASS_RIFLE;
			break;
		case P90:
		case BIZON:
		case MP7:
		case MP9:
		case MAC10:
		case UMP45:
			return WEPCLASS_SMG;
			break;
		case AWP:
		case G3SG1:
		case SCAR20:
		case SSG08:
			return WEPCLASS_SNIPER;
			break;
		case NEGEV:
		case M249:
			return WEPCLASS_MACHINEGUN;
			break;
		case MAG7:
		case SAWEDOFF:
		case NOVA:
		case XM1014:
			return WEPCLASS_SHOTGUN;
			break;
		case KNIFE:
		case KNIFE_BAYONET:
		case KNIFE_BUTTERFLY:
		case KNIFE_FALCHION:
		case KNIFE_FLIP:
		case KNIFE_GUT:
		case KNIFE_KARAMBIT:
		case KNIFE_TACTICAL:
		case KNIFE_M9_BAYONET:
		case KNIFE_PUSH:
		case KNIFE_BOWIE:
		case KNIFE_NAVAJA:
		case KNIFE_STILETTO:
		case KNIFE_URSUS:
		case KNIFE_TALON:
		case KNIFE_T:
			return WEPCLASS_KNIFE;
			break;

		default:
			return WEPCLASS_INVALID;
		}
	}
	//float GetInaccuracy()
	//{
	//	if (!this)
	//		return 0;

	//	typedef float(__thiscall* GetInaccuracyFn)(void*);
	//	GetInaccuracyFn Inaccuracy = (GetInaccuracyFn)((*(PDWORD_PTR*)this)[468]); //459
	//	return Inaccuracy(this);
	//}
	float GetFloatRecoilIndex()
	{
		if (!this)
			return -1.f;
		return *reinterpret_cast<float*>((DWORD)this + 0x32D0);
	}


	float GetSpread()
	{
		typedef float(__thiscall* OriginalFn)(void*);
		return CallVFunction<OriginalFn>(this, 436)(this);
	}
	float GetCone()
	{
		if (!this)
			return -1.f;
		typedef float(__thiscall* OriginalFn)(void*);
		return CallVFunction<OriginalFn>(this, 468)(this);

	}

	//float GetInaccuracy()
	//{
	//	if (this != nullptr)
	//	{
	//		typedef float(__thiscall* GetInaccuracyFn)(void*);
	//		GetInaccuracyFn Inaccuracy = (GetInaccuracyFn)((*(PDWORD_PTR*)this)[468]); //459
	//		return Inaccuracy(this);
	//	}
	//}

	float GetInaccuracy()
	{
		typedef float(__thiscall* GetInaccuracyFn)(void*);
		return CallVFunction<GetInaccuracyFn>(this, 467)(this);

	}

	void UpdateAccuracyPenalty()
	{
		typedef void(__thiscall* UpdateAccuracyPenaltyFn)(void*);
		CallVFunction<UpdateAccuracyPenaltyFn>(this, 468)(this);
	}

	int GetWeaponNum()
	{
		int defindex = WeaponID();
		switch (defindex)
		{
		case WEAPON_GLOCK:
			return 0;
		case WEAPON_CZ75:
			return 1;
		case WEAPON_P250:
			return 2;
		case WEAPON_FIVE7:
			return 3;
		case WEAPON_DEAGLE:
			return 4;
		case WEAPON_DUALS:
			return 5;
		case WEAPON_TEC9:
			return 6;
		case WEAPON_P2000:
			return 7;
		case WEAPON_USPS:
			return 8;
		case WEAPON_REVOLVER:
			return 9;
		case WEAPON_MAC10:
			return 10;
		case WEAPON_MP9:
			return 11;
		case WEAPON_MP7:
			return 12;
		case WEAPON_UMP45:
			return 13;
		case WEAPON_BIZON:
			return 14;
		case WEAPON_P90:
			return 15;
		case WEAPON_GALIL:
			return 16;
		case WEAPON_FAMAS:
			return 17;
		case WEAPON_AK47:
			return 18;
		case WEAPON_M4A1:
			return 19;
		case WEAPON_M4A1S:
			return 20;
		case WEAPON_SG553:
			return 21;
		case WEAPON_AUG:
			return 22;
		case WEAPON_SCOUT:
			return 23;
		case WEAPON_AWP:
			return 24;
		case WEAPON_G3SG1:
			return 25;
		case WEAPON_SCAR20:
			return 26;
		case WEAPON_NOVA:
			return 27;
		case WEAPON_XM1014:
			return 28;
		case WEAPON_SAWEDOFF:
			return 29;
		case WEAPON_MAG7:
			return 30;
		case WEAPON_M249:
			return 31;
		case WEAPON_NEGEV:
			return 32;
		default:
			return -1;
		}
		return -1;
	}

	bool IsRifle()
	{
		int iWeaponID = WeaponID();
	}
	bool IsScopeable()
	{
		int iWeaponID = WeaponID();
		return (iWeaponID == 38 || iWeaponID == 11 || iWeaponID == 9 || iWeaponID == 40 || iWeaponID == 8 || iWeaponID == SG556);
	}
	bool IsSniper()
	{
		int iWeaponID = WeaponID();
		return (iWeaponID == SSG08 || iWeaponID == AWP || iWeaponID == SCAR20 || iWeaponID == G3SG1);
	}
	bool IsPistol()
	{
		int iWeaponID = WeaponID();
		return (iWeaponID == GLOCK || iWeaponID == HKP2000
			|| iWeaponID == P250 || iWeaponID == DEAGLE
			|| iWeaponID == ELITE || iWeaponID == TEC9 || iWeaponID == USP_SILENCER
			|| iWeaponID == FIVESEVEN);
	}
	bool IsMiscWeapon()
	{
		int iWeaponID = WeaponID();
		return (iWeaponID == KNIFE
			|| iWeaponID == C4
			|| iWeaponID == HEGRENADE || iWeaponID == DECOY
			|| iWeaponID == FLASHBANG || iWeaponID == MOLOTOV
			|| iWeaponID == SMOKEGRENADE || iWeaponID == INCGRENADE || iWeaponID == KNIFE_T
			|| iWeaponID == 500 || iWeaponID == 505 || iWeaponID == 506
			|| iWeaponID == 507 || iWeaponID == 508 || iWeaponID == 509
			|| iWeaponID == 515 || iWeaponID == KNIFE_NAVAJA ||
			iWeaponID == KNIFE_STILETTO ||
			iWeaponID == KNIFE_URSUS ||
			iWeaponID == KNIFE_TALON);
	}
	bool IsGun()
	{
		int id = this->GetItemDefinitionIndex();

		if (id == KNIFE || id == HEGRENADE || id == WEAPON_DECOY || id == WEAPON_INC || id == WEAPON_MOLOTOV || id == WEAPON_C4 || id == TASER || id == FLASHBANG || id == SMOKEGRENADE || id == KNIFE)
			return false;
		else
			return true;
	}
	bool IsGrenade()
	{

		int iWeaponID = WeaponID();
		return (iWeaponID == HEGRENADE || iWeaponID == DECOY
			|| iWeaponID == FLASHBANG || iWeaponID == MOLOTOV
			|| iWeaponID == SMOKEGRENADE || iWeaponID == INCGRENADE);
	}
	bool IsKnife()
	{
		int iWeaponID = WeaponID();
		return (iWeaponID == 42 || iWeaponID == 59 || iWeaponID == 41
			|| iWeaponID == 500 || iWeaponID == 505 || iWeaponID == 506
			|| iWeaponID == 507 || iWeaponID == 508 || iWeaponID == 509
			|| iWeaponID == 515 || iWeaponID == KNIFE_NAVAJA ||
			iWeaponID == KNIFE_STILETTO ||
			iWeaponID == KNIFE_URSUS ||
			iWeaponID == KNIFE_TALON);
	}
	float GetPenetration()
	{
		if (!this)
			return -1.f;
		return *reinterpret_cast<float*>((DWORD)this + 0x7C4);
	}
	float GetDamage()
	{
		if (!this)
			return -1.f;
		return *reinterpret_cast<float*>((DWORD)this + 0x7C8);
	}
	float GetRange()
	{
		if (!this)
			return -1.f;
		return *reinterpret_cast<float*>((DWORD)this + 0x7CC);
	}
	float GetRangeModifier()
	{
		if (!this)
			return -1.f;
		return *reinterpret_cast<float*>((DWORD)this + 0x7D0);
	}
	float GetArmorRatio()
	{
		if (!this)
			return -1.f;
		return *reinterpret_cast<float*>((DWORD)this + 0x7AC);
	}
	CSWeaponInfo* GetCSWpnData();
	bool IsReloadingVisually();
};

class CBaseViewModel
{
public:
	CBaseViewModel(void);
	~CBaseViewModel(void);


	bool IsViewable(void) { return false; }

	virtual void					UpdateOnRemove(void);

	// Weapon client handling
	virtual void			SendViewModelMatchingSequence(int sequence);
	virtual void			SetWeaponModel(const char *pszModelname, CBaseCombatWeapon *weapon);

	void SendViewModelMatchingSequenceManual(int sequence)
	{
		typedef void(__thiscall* OriginalFn)(void*, int);
		return CallVFunction<OriginalFn>(this, 241)(this, sequence);
	}

};
enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,	// indicates it was created +and+ entered the pvs
								//	DATA_UPDATE_ENTERED_PVS,
								DATA_UPDATE_DATATABLE_CHANGED,
								//	DATA_UPDATE_LEFT_PVS,
								//	DATA_UPDATE_DESTROYED,		// FIXME: Could enable this, but it's a little worrying
								// since it changes a bunch of existing code
};
class C_BasePlayer
{
public:


	C_BasePlayer();
	virtual			~C_BasePlayer();
	virtual void	Spawn(void);
	virtual void	SharedSpawn(); // Shared between client and server.
	void*		Classify(void) { return 0; }
	// IClientCBaseEntity overrides.
	virtual void	OnPreDataChanged(DataUpdateType_t updateType);
	virtual void	OnDataChanged(DataUpdateType_t updateType);
	virtual void	PreDataUpdate(DataUpdateType_t updateType);
	virtual void	PostDataUpdate(DataUpdateType_t updateType);
	virtual void	ReceiveMessage(int classID, void* &msg);
	virtual void	OnRestore();
	virtual void	MakeTracer(const Vector &vecTracerSrc, const void* &tr, int iTracerType);
	virtual void	GetToolRecordingState(KeyValues *msg);
	void	SetAnimationExtension(const char *pExtension);
	CBaseViewModel			*GetViewModel(int viewmodelindex = 0);
};

class GlowObjectDefinition_t
{
public:
	GlowObjectDefinition_t() { memset(this, 0, sizeof(*this)); }

	class CBaseEntity* m_pEntity;    //0x0000
	Vector m_vGlowColor;           //0x0004
	float   m_flAlpha;                 //0x0010
	uint8_t pad_0014[4];               //0x0014
	float   m_flSomeFloat;             //0x0018
	uint8_t pad_001C[4];               //0x001C
	float   m_flAnotherFloat;          //0x0020
	bool    m_bRenderWhenOccluded;     //0x0024
	bool    m_bRenderWhenUnoccluded;   //0x0025
	bool    m_bFullBloomRender;        //0x0026
	uint8_t pad_0027[5];               //0x0027
	int32_t m_nGlowStyle;              //0x002C
	int32_t m_nSplitScreenSlot;        //0x0030
	int32_t m_nNextFreeSlot;           //0x0034

	bool IsUnused() const { return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE; }

	static const int END_OF_FREE_LIST = -1;
	static const int ENTRY_IN_USE = -2;
}; //Size: 0x0038 (56)

class CGlowObjectManager
{
public:

	class GlowObjectDefinition_t
	{
	public:

		class CBaseEntity *m_pEntity;
		union
		{
			Vector m_vGlowColor;           //0x0004
			struct
			{
				float   m_flRed;           //0x0004
				float   m_flGreen;         //0x0008
				float   m_flBlue;          //0x000C
			};
		};
		float m_flGlowAlpha;
		uint8_t pad_0014[4];
		float m_flSomeFloatThingy;
		uint8_t pad_001C[4];
		float m_flAnotherFloat;
		bool m_bRenderWhenOccluded;
		bool m_bRenderWhenUnoccluded;
		bool m_bFullBloomRender;
		uint8_t pad_0027[5];
		int32_t m_bPulsatingChams;
		int32_t m_nSplitScreenSlot;
		int32_t m_nNextFreeSlot;

		bool IsUnused() const { return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE; }

		static const int END_OF_FREE_LIST = -1;
		static const int ENTRY_IN_USE = -2;
	};

	GlowObjectDefinition_t *m_GlowObjectDefinitions;
	int max_size;
	int pad;
	int size;
	GlowObjectDefinition_t *m_Unknown;
	int	currentObjects;
};