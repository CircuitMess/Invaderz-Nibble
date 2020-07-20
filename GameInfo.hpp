#ifndef GAME_INFO
#define GAME_INFO
#include <Arduino.h>
#include <Display/Display.h>
#include "../bitmaps/icon.hpp"
#include "src/SpaceInvaders.h"

class Context;
struct GameInfo {
	const char* title;
	const char* description;
	const uint16_t* icon;
	Context* (*launch)(Display& display);
};
#endif
const GameInfo SnakeInfo {
		"Invaderz",
		"A remake of the classic space invaders.",
		icon,
		[](Display& display) -> Context* { return new SpaceInvaders(display); }
};