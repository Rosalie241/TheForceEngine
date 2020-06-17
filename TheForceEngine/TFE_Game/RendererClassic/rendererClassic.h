#pragma once
//////////////////////////////////////////////////////////////////////
// Renderer - Classic
// Classic Dark Forces (DOS) derived renderer.
//////////////////////////////////////////////////////////////////////

#include <TFE_System/types.h>
#include <TFE_Asset/colormapAsset.h>
#include "fixedPoint.h"
#include "../renderCommon.h"

namespace RendererClassic
{
	void setCamera(fixed16 cosYaw, fixed16 sinYaw, fixed16 sinPitch, s32 x, s32 y, s32 z, s32 sectorId);
	void draw(u8* display, const ColorMap* colormap);

	void setupLevel(s32 width, s32 height);
	void changeResolution(s32 width, s32 height);
}