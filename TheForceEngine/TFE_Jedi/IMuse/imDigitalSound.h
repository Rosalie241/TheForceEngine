#pragma once
//////////////////////////////////////////////////////////////////////
// iMuse System
// The IMuse system used by Dark Forces.
//
// Copyright note:
// While the project as a whole is licensed under GPL-2.0, some of the
// code under TFE_Jedi/ was derived from reverse-engineered
// code from "Dark Forces" (DOS) which is copyrighted by LucasArts.
// The original renderer is contained in RClassic_Fixed/ and the
// RClassic_Float/ and RClassic_GPU/ sub-renderers are derived from
// that code.
//
// I consider the reverse-engineering to be "Fair Use" - a means of 
// supporting the games on other platforms and to improve support on
// existing platforms without claiming ownership of the games
// themselves or their IPs.
//
// That said using this code in a commercial project is risky without
// permission of the original copyright holders (LucasArts).
//////////////////////////////////////////////////////////////////////
#include <TFE_System/types.h>
#include "imuse.h"

struct iMuseInitData;

namespace TFE_Jedi
{
	s32 ImInitializeDigitalAudio(iMuseInitData* initData);
	s32 ImSetWaveParam(ImSoundId soundId, s32 param, s32 value);
	s32 ImGetWaveParam(ImSoundId soundId, s32 param);
	s32 ImStartDigitalSound(ImSoundId soundId, s32 priority);
}  // namespace TFE_Jedi