/*
	Invaderz
	Copyright (C) 2020 CircuitMess
	original game:
	Invaders by Yoda Zhang
	http://www.yodasvideoarcade.com/images/gamebuino-invaders.zip
	Ported to MAKERphone by CircuitMess, then ported to ByteBoy.
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
//----------------------------------------------------------------------------
// define variables && constants
//----------------------------------------------------------------------------    
#include <Arduino.h>
#include "src/SpaceInvaders.h"
#include <CircuitOS.h>
#include <Input/I2cExpander.h>
#include <Input/InputI2C.h>
#include <Update/UpdateManager.h>
#include <ArduinoJson.h>
#include <spiffs_api.h>
#include <gpio.h>
#include <avr/pgmspace.h>
#include "ByteBoy.hpp"

Display display(128, 128, BL_PIN, 0);
I2cExpander i2c;
InputI2C buttons(&i2c);
SpaceInvaders game(display);

/*
//----------------------------------------------------------------------------
// setup
//----------------------------------------------------------------------------
*/

void setup() {
	gpio_init();
	i2c.begin(0x74, 4, 5);
	display.begin();
	Serial.begin(115200);
	Serial.println("BL on");
	display.getBaseSprite()->clear(TFT_BLACK);
	display.commit();
	Serial.println("display ok");
	Serial.println("buttons begin");
	SPIFFS.begin();
	Serial.println("spiffs begin");
	UpdateManager::addListener(&buttons);
	
	game.unpack();
	game.start();
}
//----------------------------------------------------------------------------    
// loop
//----------------------------------------------------------------------------    
void loop() {
	UpdateManager::update();
	yield();
}