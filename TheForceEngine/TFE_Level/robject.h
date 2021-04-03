#pragma once
//////////////////////////////////////////////////////////////////////
// Wall
// Dark Forces Derived Renderer - Wall functions
//////////////////////////////////////////////////////////////////////
#include <TFE_System/types.h>
#include <TFE_Asset/spriteAsset_Jedi.h>
#include "core_math.h"

using namespace TFE_CoreMath;

struct JediModel;

struct RSector;
	
enum ObjectType
{
	OBJ_TYPE_NONE   = 0,
	OBJ_TYPE_SPRITE = 1,
	OBJ_TYPE_3D     = 2,
	OBJ_TYPE_FRAME  = 3,
};

enum ObjectFlags
{
	OBJ_FLAG_RENDERABLE = (1 << 2),		// The object is renderable, such as a 3D object, sprite or frame.
	OBJ_FLAG_FULLBRIGHT = (1 << 3),		// The object should be rendered fullbright (no shading).
};

enum EntityTypeFlags
{
	ETFLAG_CAN_WAKE    = FLAG_BIT(6),	// An inactive object or animation waiting to be "woken up" - such as Vues waiting to play.
	ETFLAG_CAN_DISABLE = FLAG_BIT(10),	// An object that can be enabled or disabled by INF.
	ETFLAG_SMART_OBJ   = FLAG_BIT(11),	// An object that can manipulate the level, such as opening doors.
	ETFLAG_PLAYER      = FLAG_BIT(31),	// This is the player object.

	ETFLAG_NONE = 0,
};

#define SPRITE_SCALE_FIXED FIXED(10)

struct SecObject
{
	SecObject* self;
	ObjectType type;
	u32 entityFlags;    // see EntityTypeFlags above.

	// Position
	vec3_fixed posWS;
	vec3_fixed posVS;

	// World Size
	fixed16_16 worldWidth;
	fixed16_16 worldHeight;

	// 3x3 transformation matrix.
	fixed16_16 transform[9];

	// Rendering data.
	union
	{
		JediModel* model;
		Wax*   wax;
		WaxFrame* fme;
		void* ptr;
	};
	s32 frame;
	s32 anim;
	RSector* sector;

	// See ObjectFlags above.
	u32 flags;

	// Orientation.
	angle14_16 pitch;
	angle14_16 yaw;
	angle14_16 roll;
	// index in containing sector object list.
	s16 index;
};

namespace TFE_Level
{
	SecObject* allocateObject();

	// 3D objects
	void obj3d_setData(SecObject* obj, JediModel* pod);
	void obj3d_computeTransform(SecObject* obj);

	// Sprites
	void sprite_setData(SecObject* obj, JediWax* data);

	// Frames
	void frame_setData(SecObject* obj, WaxFrame* data);
}