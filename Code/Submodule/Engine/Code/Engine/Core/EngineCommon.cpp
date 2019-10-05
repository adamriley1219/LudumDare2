#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Strings/NamedStrings.hpp"

#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"
#include "Engine/Core/Debug/Log.hpp"

#include <thread>

NamedStrings g_gameConfigBlackboard;


EventSystem* g_theEventSystem = nullptr;

DevConsole* g_theConsole = nullptr;

DebugRenderSystem* g_theDebugRenderSystem = nullptr;
