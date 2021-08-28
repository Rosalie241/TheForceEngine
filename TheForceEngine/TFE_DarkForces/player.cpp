#include "player.h"
#include "agent.h"
#include "hud.h"
#include "pickup.h"
#include "weapon.h"
#include <TFE_Jedi/Level/level.h>

namespace TFE_DarkForces
{
	PlayerInfo s_playerInfo = { 0 };
	GoalItems s_goalItems   = { 0 };
	fixed16_16 s_energy = 2 * ONE_16;
	s32 s_lifeCount;

	JBool s_invincibility = JFALSE;
	JBool s_wearingCleats = JFALSE;
	JBool s_wearingGasmask = JFALSE;
	JBool s_nightvisionActive = JFALSE;
	JBool s_headlampActive = JFALSE;
	JBool s_superCharge   = JFALSE;
	JBool s_superChargeHud= JFALSE;
	u32*  s_playerInvSaved = nullptr;
	JBool s_goals[16] = { 0 };

	SecObject* s_playerObject = nullptr;
	SecObject* s_playerEye = nullptr;
	vec3_fixed s_eyePos = { 0 };	// s_camX, s_camY, s_camZ in the DOS code.
	angle14_32 s_pitch = 0, s_yaw = 0, s_roll = 0;
	Tick s_playerTick;
	Tick s_prevPlayerTick;
	Tick s_nextShieldDmgTick;
	Task* s_playerTask = nullptr;

	JBool s_itemUnknown1;	// 0x282428
	JBool s_itemUnknown2;	// 0x28242c

	SoundSourceID s_landSplashSound;
	SoundSourceID s_landSolidSound;
	SoundSourceID s_gasDamageSoundSource;
	SoundSourceID s_maskSoundSource1;
	SoundSourceID s_maskSoundSource2;
	SoundSourceID s_invCountdownSound;
	SoundSourceID s_jumpSoundSource;
	SoundSourceID s_nightVisionActiveSoundSource;
	SoundSourceID s_nightVisionDeactiveSoundSource;
	SoundSourceID s_playerDeathSoundSource;
	SoundSourceID s_crushSoundSource;
	SoundSourceID s_playerHealthHitSoundSource;
	SoundSourceID s_kyleScreamSoundSource;
	SoundSourceID s_playerShieldHitSoundSource;

	// Player Controller
	static fixed16_16 s_externalYawSpd;
	static fixed16_16 s_playerPitch;
	static fixed16_16 s_playerRoll;
	static fixed16_16 s_forwardSpd;
	static fixed16_16 s_strafeSpd;
	static fixed16_16 s_maxMoveDist;
	static fixed16_16 s_playerStopAccel;
	static fixed16_16 s_minEyeDistFromFloor;
	static fixed16_16 s_gravityAccel;
	static fixed16_16 s_postLandVel;
	static fixed16_16 s_playerVelX;
	static fixed16_16 s_playerUpVel;
	static fixed16_16 s_playerUpVel2;
	static fixed16_16 s_playerVelZ;
	static fixed16_16 s_externalVelX;
	static fixed16_16 s_externalVelZ;
	static fixed16_16 s_playerCrouchSpd;
	static fixed16_16 s_playerSpeed;
	// Speed Modifiers
	static s32 s_playerRun = 0;
	static s32 s_jumpScale = 0;
	static s32 s_playerSlow = 0;
	static s32 s_waterSpeed = 0;
	// Misc
	static s32 s_weaponLight = 0;
	static s32 s_levelAtten = 0;
	static s32 s_headwaveVerticalOffset;
	static Safe* s_curSafe = nullptr;
	// Actions
	static JBool s_playerUse = JFALSE;
	static JBool s_playerActionUse = JFALSE;
	static JBool s_weaponFiring = JFALSE;
	static JBool s_weaponFiringSec = JFALSE;
	static JBool s_playerPrimaryFire = JFALSE;
	static JBool s_playerSecFire = JFALSE;
	static JBool s_playerJumping = JFALSE;
	// Currently playing sound effects.
	static SoundEffectID s_crushSoundId = 0;
	static SoundEffectID s_kyleScreamSoundId = 0;
	// Position and orientation.
	static vec3_fixed s_playerPos;
	static fixed16_16 s_playerObjHeight;
	static angle14_32 s_playerObjPitch;
	static angle14_32 s_playerObjYaw;
	static RSector* s_playerObjSector;
		   
	void playerControlTaskFunc(s32 id);

	void player_init()
	{
		s_landSplashSound                = sound_Load("swim-in.voc");
		s_landSolidSound                 = sound_Load("land-1.voc");
		s_gasDamageSoundSource           = sound_Load("choke.voc");
		s_maskSoundSource1               = sound_Load("mask1.voc");
		s_maskSoundSource2               = sound_Load("mask2.voc");
		s_invCountdownSound              = sound_Load("quarter.voc");
		s_jumpSoundSource                = sound_Load("jump-1.voc");
		s_nightVisionActiveSoundSource   = sound_Load("goggles1.voc");
		s_nightVisionDeactiveSoundSource = sound_Load("goggles2.voc");
		s_playerDeathSoundSource         = sound_Load("kyledie1.voc");
		s_crushSoundSource               = sound_Load("crush.voc");
		s_playerHealthHitSoundSource     = sound_Load("health1.voc");
		s_kyleScreamSoundSource          = sound_Load("fall.voc");
		s_playerShieldHitSoundSource     = sound_Load("shield1.voc");

		s_playerInfo.ammoEnergy  = pickup_addToValue(0, 100, 999);
		s_playerInfo.ammoPower   = pickup_addToValue(0, 100, 999);
		s_playerInfo.ammoPlasma  = pickup_addToValue(0, 100, 999);
		s_playerInfo.shields     = pickup_addToValue(0, 100, 200);
		s_playerInfo.health      = pickup_addToValue(0, 100, 100);
		s_playerInfo.healthFract = 0;
		s_energy = FIXED(2);
	}
		
	void player_readInfo(u8* inv, s32* ammo)
	{
		// Read the inventory.
		s_playerInfo.itemPistol    = inv[0];
		s_playerInfo.itemRifle     = inv[1];
		s_itemUnknown1             = inv[2];
		s_playerInfo.itemAutogun   = inv[3];
		s_playerInfo.itemMortar    = inv[4];
		s_playerInfo.itemFusion    = inv[5];
		s_itemUnknown2             = inv[6];
		s_playerInfo.itemConcussion= inv[7];
		s_playerInfo.itemCannon    = inv[8];
		s_playerInfo.itemRedKey    = inv[9];
		s_playerInfo.itemYellowKey = inv[10];
		s_playerInfo.itemBlueKey   = inv[11];
		s_playerInfo.itemGoggles   = inv[12];
		s_playerInfo.itemCleats    = inv[13];
		s_playerInfo.itemMask      = inv[14];
		s_playerInfo.itemPlans     = inv[15];
		s_playerInfo.itemPhrik     = inv[16];
		s_playerInfo.itemNava      = inv[17];
		s_playerInfo.itemDatatape  = inv[18];
		s_playerInfo.itemUnkown    = inv[19];
		s_playerInfo.itemDtWeapon  = inv[20];
		s_playerInfo.itemCode1 = inv[21];
		s_playerInfo.itemCode2 = inv[22];
		s_playerInfo.itemCode3 = inv[23];
		s_playerInfo.itemCode4 = inv[24];
		s_playerInfo.itemCode5 = inv[25];
		s_playerInfo.itemCode6 = inv[26];
		s_playerInfo.itemCode7 = inv[27];
		s_playerInfo.itemCode8 = inv[28];
		s_playerInfo.itemCode9 = inv[29];

		// Handle current weapon.
		s32 curWeapon = inv[30];
		weapon_setNext(curWeapon);
		s_playerInfo.maxWeapon = max(curWeapon, s_playerInfo.maxWeapon);

		// Life Count.
		s_lifeCount = inv[31];

		// Read Ammo, shields, health and energy.
		s_playerInfo.ammoEnergy    = ammo[0];
		s_playerInfo.ammoPower     = ammo[1];
		s_playerInfo.ammoPlasma    = ammo[2];
		s_playerInfo.ammoDetonator = ammo[3];
		s_playerInfo.ammoShell     = ammo[4];
		s_playerInfo.ammoMine      = ammo[5];
		s_playerInfo.ammoMissile   = ammo[6];
		s_playerInfo.shields = ammo[7];
		s_playerInfo.health  = ammo[8];
		s_energy = ammo[9];
	}
		
	void player_createController()
	{
		s_playerTask = pushTask(playerControlTaskFunc);
		task_setNextTick(s_playerTask, TASK_SLEEP);

		// Clear out inventory items that the player shouldn't start a level with, such as objectives and keys
		s_playerInfo.itemPlans    = JFALSE;
		s_playerInfo.itemPhrik    = JFALSE;
		s_playerInfo.itemNava     = JFALSE;
		s_playerInfo.itemUnkown   = JFALSE;
		s_playerInfo.itemDtWeapon = JFALSE;
		s_playerInfo.itemCode1    = JFALSE;
		s_playerInfo.itemCode2    = JFALSE;
		s_playerInfo.itemCode3    = JFALSE;
		s_playerInfo.itemCode4    = JFALSE;
		s_playerInfo.itemCode5    = JFALSE;
		s_playerInfo.itemCode6    = JFALSE;
		s_playerInfo.itemCode7    = JFALSE;
		s_playerInfo.itemCode8    = JFALSE;
		s_playerInfo.itemCode9    = JFALSE;
		s_playerInfo.itemRedKey   = JFALSE;
		s_playerInfo.itemYellowKey= JFALSE;
		s_playerInfo.itemBlueKey  = JFALSE;

		// Clear out any lingering damage or flash effects.
		s_healthDamageFx = 0;
		s_shieldDamageFx = 0;
		s_flashEffect = 0;

		s_pickupFlags = 0;
		s_externalYawSpd = 0;
		s_playerPitch = 0;
		s_playerRoll = 0;
		s_forwardSpd = 0;
		s_strafeSpd = 0;

		// Set constants.
		s_maxMoveDist         = FIXED(32);	// 32.00
		s_playerStopAccel     = -540672;	// -8.25
		s_minEyeDistFromFloor = FIXED(2);	//  2.00
		s_gravityAccel        = FIXED(150);	// 150.0

		// Initialize values.
		s_postLandVel = 0;
		s_playerRun = 0;
		s_jumpScale = 0;
		s_playerSlow = 0;
		s_waterSpeed = 0;

		s_playerVelX = 0;
		s_playerUpVel = 0;
		s_playerUpVel2 = 0;
		s_playerVelZ = 0;
		s_externalVelX = 0;
		s_externalVelZ = 0;
		s_playerCrouchSpd = 0;
		s_playerSpeed = 0;

		s_weaponLight = 0;
		s_levelAtten = 0;
		s_prevPlayerTick = 0;
		s_headwaveVerticalOffset = 0;

		s_playerUse = JFALSE;
		s_playerActionUse = JFALSE;
		s_weaponFiring = JFALSE;
		s_weaponFiringSec = JFALSE;
		s_playerPrimaryFire = JFALSE;
		s_playerSecFire = JFALSE;
		s_playerJumping = JFALSE;

		s_crushSoundId = 0;
		s_kyleScreamSoundId = 0;

		s_curSafe = nullptr;
		// The player will always start a level with at least 3 lives.
		s_lifeCount = max(3, s_lifeCount);

		s_playerInvSaved = nullptr;
		s_invincibilityTask = nullptr;
		s_gasmaskTask = nullptr;
		s_gasSectorTask = nullptr;
		s_nextShieldDmgTick = 0;
		s_invincibility = 0;

		// The player will always start a level with at least 100 shields, though if they have more it carries over.
		s_playerInfo.shields = max(100, s_playerInfo.shields);
		// The player starts a new level with full health and energy.
		s_playerInfo.health = 100;
		s_playerInfo.healthFract = 0;
		s_energy = FIXED(2);

		s_wearingGasmask    = JFALSE;
		s_wearingCleats     = JFALSE;
		s_nightvisionActive = JFALSE;
		s_headlampActive    = JFALSE;

		// Handle level-specific hacks.
		const char* levelName = agent_getLevelName();
		if (!strcasecmp(levelName, "japship"))
		{
			// TODO
		}
		else if (!strcasecmp(levelName, "gromas"))
		{
			// Adjust the lighting on gromas in order to accenuate the fog effect.
			s_levelAtten = 30;
		}
	}
		
	void playerControlTaskFunc(s32 id)
	{
		task_begin;
		// s_curSafe = getSafeFromSector(s_playerObject->sector);
		s_playerPos.x = s_playerObject->posWS.x;
		s_playerPos.y = s_playerObject->posWS.y;
		s_playerPos.z = s_playerObject->posWS.z;
		s_playerObjHeight = s_playerObject->worldHeight;
		s_playerObjPitch = s_playerObject->pitch;
		s_playerObjYaw = s_playerObject->yaw;
		s_playerObjSector = s_playerObject->sector;
		s_prevPlayerTick = s_curTick;

		while (id != -1)
		{
			if (id == 0)
			{
				// handlePlayerMoveControls();
				// handlePlayerPhysics();
				// handlePlayerActions();
				// handlePlayerScreenFx();
				// TODO
			}
			else if (id == 22)
			{
				// TODO
			}
			else if (id == 23)
			{
				// TODO
			}

			s_prevPlayerTick = s_playerTick;
			task_yield(TASK_NO_DELAY);
		}
		task_end;
	}
}  // TFE_DarkForces