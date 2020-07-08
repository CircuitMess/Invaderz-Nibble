#include "ByteBoy.hpp"
Game::Game(Display &display, char* title, unsigned short* gameIcon, char* description) : 
		Context(display), gameIcon(gameIcon), description(description), title(title)
{
}

const unsigned short* Game::getIcon()
{
	return gameIcon;
}
const char* Game::getTitle()
{
	return title;
}
const char* Game::getDescription()
{
	return description;
}