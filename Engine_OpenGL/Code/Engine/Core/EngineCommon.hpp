#pragma once
#pragma warning( error: 4172 ) // Return address of local variable
#pragma warning( error: 4715 ) // Not all paths return a value

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#define UNUSED(x) (void)(x);


class DevConsole;
extern DevConsole* g_theDevConsole;
typedef unsigned int DevConsoleChannel;

class EventSystem;
extern EventSystem* g_theEventSystem;

class NamedStrings;
extern NamedStrings g_theGameConfigBlackboard;

struct Vec2;
extern Vec2 ALIGN_CENTER;
extern Vec2 ALIGN_BOTTOM_LEFT;
extern Vec2 ALIGN_BOTTOM_RIGHT;
extern Vec2 ALIGN_TOP_LEFT;
extern Vec2 ALIGN_TOP_RIGHT;
extern Vec2 ALIGN_TOP_CENTER;
extern Vec2 ALIGN_BOTTOM_CENTER;
extern Vec2 ALIGN_CENTER_LEFT;
extern Vec2 ALIGN_CENTER_RIGHT;