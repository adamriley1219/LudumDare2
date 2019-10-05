#pragma once
#include "Engine/Core/EngineCommon.hpp"

//--------------------------------------------------------------------------
uint GetSystemCoreCount();


//--------------------------------------------------------------------------
// Load in files and read into output
unsigned long int LoadFile(const char* filePath, char*& output);