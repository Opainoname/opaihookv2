#pragma once
#include <string>
#include <vector>

class ClientVariables
{
public:
	struct {
		bool bEnable;
		int iSettingType;
		int iCustomIndex;
		int iBackTrackType[33];
		int iFovType[33];
		int iSmoothType[33];
		float flSmooth[33];
		float flFov[33];
		int iDelay[33];
		float flSwitchTargetDelay[33];
		int iAimType[33];
		bool bUseSpecificAim[33];
		int iSpecificAimType[33];
	} LegitBot;
	struct Ragebot
	{
		bool EnableAimbot = false;
		bool AnimFix = true;
		int AimbotSelection = 0;
		bool AutomaticFire = false;
		bool AutomaticScope = false;
		bool SilentAimbot = false;
		bool NoRecoil = false;
		bool NoSpread = false;
		bool PositionAdjustment = false;
		int Preferbodyaim = 0;
		int Hitbox = 0;
		int Hitscan = 0;
		bool AutowallHitscan = false;
		bool Autowall = false;
		int Multipoint = 0.f;
		float pointscale = 0.f;
		float Fakewalksp = 1.f;
		int Mindamage = 1.f;
		bool Hitchance = false;
		int Minhitchance = 0.f;
		int TickType = 0;
		bool AutomaticResolver;
		int ResolverOverride = 0;
		bool FriendlyFire = false;
		bool Quickstop = false;
		int multimemez;
		int multimemez2;
		bool headaimwalking;
		bool HitScanBones[8];
		bool AutomaticRevolver;
		bool NewAutomaticRevolver;
		int NewAutomaticRevolverFactor;
		bool FakeLatency;
		int fakepingkey;
		float FakeLatencyAmount;

	} Ragebot;

	struct Antiaim
	{
		bool AntiaimEnable = false;
		bool Lines = false;
		bool Indicator = false;
		int AntiAimType;
		struct
		{
			int pitch, yaw, fakeyaw;
		}Stand;

		struct
		{
			int pitch, yaw, fakeyaw;
		}Move;

		struct
		{
			int pitch, yaw, fakeyaw;
		}Air;

		int JitterRange;
		int LBYDelta;
		int SpinSpeed;
		int DirectonType;
		int aatype;
	} Antiaim;

	struct Visuals
	{
		bool EspEnable = false;
		bool EnemyOnly = false;
		bool BoundingBox = false;
		bool Bones = false;
		bool ForceCrosshair = false;
		bool Health = false;
		bool Armor = false;
		bool Scoped = false;
		bool Flags = false;
		bool Fake = false;
		bool Dlight = false;
		bool Name = false;
		bool Weapon = false;
		bool Ammo = false;
		bool AllItems = false;
		bool Rank = false;
		bool Radar = false;
		bool NoFlash = false;
		int thirdperson_dist = 100;
		bool Monitor = false;
		bool Wins = false;
		bool Glow = false;
		bool LGlow = false;
		bool PulseLGlow = false;
		bool LineofSight = false;
		bool SnapLines = false;
		bool GrenadePrediction = false;
		int Crosshair = 0;
		bool SpreadCrosshair = false;
		bool RecoilCrosshair = false;
		bool FartherESP = false;
		bool localESP = false;
		//Cbase/filers
		int DroppedWeapons = 0;
		bool Hostage = false;
		bool ThrownNades = false;
		bool LocalPlayer = false;
		bool BulletTracers = false;
		int bulletType = 0;
		bool Bomb = false;
		bool Spectators = false;
		bool OutOfPOVArrows = false;
		bool DamageIndicators = false;
		bool lbytimer = false;
		//Effects/world
		bool nightmode = false;
		bool blendonscope = false;
		int playeralpha = 255;
		int scopeplayeralpha = 255;
		int enemyalpha = 255;
		int Skybox = 0;
		int FlashbangAlpha = 0;
		bool Nosmoke = false;
		bool Noscope = false;
		bool RemoveParticles = false;
		bool Novisrevoil = false;
		bool Hitmarker = false;
		int hitmarkerSize = 2;
		bool ChamsEnable = false;
		bool ShowBacktrack = false;
		int ChamsStyle = 0;
		bool ChamsL = false;
		bool lagModel = false;
		bool crosssnip = false;
		bool Chamsenemyonly = false;
		bool ChamsPlayer = false;
		bool ChamsPlayerWall = false;
		bool ChamsHands = false;
		bool ChamsHandsWireframe = false;
		bool WeaponWireframe = false;
		bool WeaponChams = false;
		struct
		{
			int resolution = 5;
			int type;
		}Spread;
		int htSound = 0;
	} Visuals;

	struct Misc
	{
		bool AntiUT = true;
		bool WireHand = false;
		bool static_scope = false;
		int PlayerFOV = 0.f;
		int PlayerViewmodel = 0.f;
		int TPangles = 0;
		int TPKey = 0;
		int MenuKey = 0x2d;
		int WalkbotSet = 0x2d;
		int WalkbotDelete = 0x2d;
		int WalkbotStart = 0x2d;
		bool Clantag;
		bool FakeChams;
		bool skins;
		bool AutoJump = false;
		bool AutoStrafe = false;
		bool AutoAccept = false;
		bool Prespeed = false;
		int Retrack = 0.f;
		int PrespeedKey = 0;
		bool FakelagEnable = false;
		bool FakelagOnground = false;
		int FakelagMode = 0;
		int FakelagAmount = 0.f;
		bool FakelagShoot = false;
		int ConfigSelection = 0;
		bool Walkbot = false;
		bool WalkbotBunnyhop = false;
		//int WalkbotSetPoint = 0;
		//int WalkbotDeletePoint = 0;
		//int WalkbotStart = 0;
		bool FakewalkEnable = false;
		int FakewalkKey = 0;
		bool legitbacktrack;
		bool Spectators = true;
		bool KnifeBot = false;
		bool ZeusBot = false;
		char configname[128];
		int circlestrafekey = 0;
	} Misc;

	struct Skins
	{
		bool Enabled;
		int Knife;
		int gloves;
		int KnifeSkin;
		int AK47Skin;
		int M4A1SSkin;
		int M4A4Skin;
		int AUGSkin;
		int FAMASSkin;
		int AWPSkin;
		int SSG08Skin;
		int SCAR20Skin;
		int P90Skin;
		int UMP45Skin;
		int GlockSkin;
		int USPSkin;
		int DeagleSkin;
		int tec9Skin;
		int P2000Skin;
		int P250Skin;
		int CZ75Skin;
		int RevolverSkin;
		int DualSkins;
		int FiveSevenSkin;
	} Skinchanger;

	struct CPlayerlist
	{
		bool bEnabled;
		int iPlayer;
		char* szPlayers[64] = {
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" "
		};
	} Playerlist;

	struct NigColors
	{
		float MenuColor[3] = { 1.f, 1.f, 1.f};
		float Name[3] = { 1.f, 1.f, 1.f};
		float wpn[3] = { 1.f, 1.f, 1.f};
		float f[3] = { 1.f, 1.f, 1.f};


		float BoundingBox[3] = { 1.f, 1.f, 1.f};
		float PlayerChams[3] = { 1.f, 1.f, 1.f};
		float ChamsHistory[3] = { 1.f, 1.f, 1.f};
		float chamsalpha[3] = { 1.f, 1.f, 1.f};
		float PlayerChamsl[3] = { 1.f, 1.f, 1.f};
		float styleshands[3] = { 1.f, 1.f, 1.f};
		float PlayerChamsWall[3] = { 1.f, 1.f, 1.f};
		float Skeletons[3] = { 1.f, 1.f, 1.f};
		float Bulletracer[3] = { 1.f, 1.f, 1.f};
		float BulletracerBox[4] = { 1.f, 1.f, 1.f, 0.7f };

		float WireframeHand[3] = { 1.f, 1.f, 1.f};
		float ChamsHand[3] = { 1.f, 1.f, 1.f};
		float ChamsWeapon[3] = { 1.f, 1.f, 1.f};
		float WireframeWeapon[3] = { 1.f, 1.f, 1.f};
		float Glow[4] = { 1.f, 1.f, 1.f, 0.7f };
		float LGlow[4] = { 1.f, 1.f, 1.f, 0.7f };
		float DroppedWeapon[3] = { 1.f, 1.f, 1.f};
		float Bomb[3] = { 1.f, 1.f, 1.f};
		float PlantedBomb[3] = { 1.f, 1.f, 1.f};
		float Hostage[3] = { 1.f, 1.f, 1.f};
		float GrenadePrediction[3] = { 1.f, 1.f, 1.f};
		float FakeAngleGhost[3] = { 1.f, 1.f, 1.f};
		float SpreadCrosshair[4] = { 1.f, 1.f, 1.f, 0.7f};
		float Snaplines[3] = { 1.f, 1.f, 1.f};
		float DamageIndicator[3] = { 1.f, 1.f, 1.f};
		float lby_timer[3] = { 1.f, 1.f, 1.f};
		float ammo[3] = { 1.f, 1.f, 1.f};
		float Radar[3] = { 1.f, 1.f, 1.f};
		bool Props;
	}Colors;
	struct {
		int tabs;
		bool Opened;
	}Gui;
	bool Save(std::string file_name);
	bool Load(std::string file_name);
	void CreateConfig(std::string name);
	void Delete(std::string name);
	std::vector<std::string> GetConfigs();
};

extern ClientVariables Menu;
