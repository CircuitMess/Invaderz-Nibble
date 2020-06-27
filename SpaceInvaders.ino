/*
	Invaderz
	Copyright (C) 2019 CircuitMess
	original game:
	Invaders by Yoda Zhang
	http://www.yodasvideoarcade.com/images/gamebuino-invaders.zip
	Ported to MAKERphone by CircuitMess
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
#include <CircuitOS.h>
#include <Input/I2cExpander.h>
#include <Input/InputI2C.h>
#include <Update/UpdateManager.h>
#include "src/Star.h"
#include <ArduinoJson.h>
#include <spiffs_api.h>
#include <gpio.h>
#include <avr/pgmspace.h>
StaticJsonBuffer<8000> jb;

I2cExpander i2c;
InputI2C buttons(&i2c);
uint32_t lastFrameCount = millis();
uint32_t frameSpeed = 40;

#define BTN_UP 0
#define BTN_DOWN 1
#define BTN_LEFT 2
#define BTN_RIGHT 3
#define BTN_A 4
#define BTN_B 5
#define BTN_C 6
#define TT1 &TomThumb
#define BUZZ_PIN 12
#define BL_PIN 16
#define NO_KEY '\0'

Display display(128, 128, BL_PIN, 0);
Sprite *baseSprite = display.getBaseSprite();
uint32_t pixelsTimer=0;
bool pixelsState=0;
String gamestatus;
int score;
int lives;
int gamelevel;
int shipx;
int invaderx[55];
int invadery[55];
int invaders[55];
int invaderframe[55];
int invaderanz;
int invaderctr;
int invadersound;
int checkdir;
int nextxdir;
int nextydir;
int invaderxr;
int invaderyr;
int invadershotx[4];
int invadershoty[4];
bool invadershotframe = 0;
bool pastInvaderShotFrame = 0;
int invadershots;
int invadershottimer;
int bunkers[4];
int shotx;
int shoty;
int checkl;
int checkr;
int checkt;
int checkb;
int yeahtimer;
int infoshow;
int deadcounter;
int saucerx;
int saucerdir;
int saucers;
int saucertimer;
int saucerwait;
int delayBip;
uint32_t blinkMillis = millis();
bool blinkState = 0;
unsigned long elapsedMillis = millis();
uint16_t hiscoresSize = 0;
char nameArray[6][4];
uint16_t scoreArray[6];
uint8_t charCursor = 0;
String previous = "";
uint32_t hiscoreMillis = millis();
bool hiscoreBlink = 0;
// MPTrack *shootSound;
// MPTrack *invaderDestroyed;
// MPTrack *mainMusic;
// MPTrack *titleMusic;
// MPTrack *playerDestroyed;
// MPTrack *ufoSound;
// MPTrack *gameoverMusic;
const char *highscoresPath = "/Invaderz/hiscores.sav";
bool savePresent = 0;
uint16_t tempScore = 0;
String name = "";
uint8_t cursor = 0;
char key = NO_KEY;
#define STAR_COUNT 30            // Number of stars on the screen. Arduino UNO maxes out around 250.
#define BACKGROUND_COLOR 0x0000   // Background color in hex. 0x0000 is black.
#define STAR_SPEED_MIN 1          // Minimum movement in pixels per update. (value is inclusive)
#define STAR_SPEED_MAX 2         // Maximum movement in pixels per update. (value is inclusive)
#define STAR_COLOR 0xffff  
Star stars[STAR_COUNT]; 
void starsSetup()
{
	// Loop through each star.
	for(int i = 0; i < STAR_COUNT; i++)
	{
		// Randomize its position && speed.
		stars[i].randomize(0, baseSprite->width(), 0, baseSprite->height(), STAR_SPEED_MIN, STAR_SPEED_MAX);
	}
} 
void drawBitmap(int16_t x, int16_t y, const byte *bitmap, uint16_t color, uint8_t scale) {
	uint16_t w = *(bitmap++);
	uint16_t h = *(bitmap++);
	baseSprite->drawMonochromeIcon(bitmap, x, y, w, h, scale, color);
}
//ported images
//----------------------------------------------------------------------------
static const unsigned short titleLogo[0x438] ={
    0x0000, 0x632C, 0x632C, 0x0000, 0x0000, 0x632C, 0x632C, 0x0000, 0x0000, 0x0000, 0x632C, 0x632C, 0x0000, 0x0000, 0x632C, 0x632C,   // 0x0010 (16)
    0x0000, 0x0000, 0x0000, 0x632C, 0x632C, 0x632C, 0x0000, 0x0000, 0x632C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x632C, 0x632C,   // 0x0020 (32)
    0x632C, 0x632C, 0x632C, 0x0000, 0x0000, 0x0000, 0x0000, 0x632C, 0x632C, 0x632C, 0x632C, 0x632C, 0x0000, 0x632C, 0x632C, 0x632C,   // 0x0030 (48)
    0x632C, 0x632C, 0x632C, 0x0000, 0x0000, 0x632C, 0x632C, 0x632C, 0x632C, 0x632C, 0x632C, 0x632C, 0xF800, 0xF800, 0x632C, 0x0000,   // 0x0040 (64)
    0xF800, 0xF800, 0x632C, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0xF800,   // 0x0050 (80)
    0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C,   // 0x0060 (96)
    0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C, 0x632C,   // 0x0070 (112)
    0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0x632C,   // 0x0080 (128)
    0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800,   // 0x0090 (144)
    0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C, 0x632C, 0x0000, 0xF800, 0xF800,   // 0x00A0 (160)
    0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800,   // 0x00B0 (176)
    0xF800, 0xF800, 0xF800, 0x632C, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C,   // 0x00C0 (192)
    0x0000, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000,   // 0x00D0 (208)
    0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000,   // 0x00E0 (224)
    0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C,   // 0x00F0 (240)
    0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800,   // 0x0100 (256)
    0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C,   // 0x0110 (272)
    0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C,   // 0x0120 (288)
    0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000,   // 0x0130 (304)
    0xF800, 0xF800, 0xF800, 0xF800, 0x632C, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800,   // 0x0140 (320)
    0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800,   // 0x0150 (336)
    0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C,   // 0x0160 (352)
    0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0170 (368)
    0x632C, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800,   // 0x0180 (384)
    0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800,   // 0x0190 (400)
    0x632C, 0x632C, 0x632C, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x01A0 (416)
    0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C,   // 0x01B0 (432)
    0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C,   // 0x01C0 (448)
    0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x01D0 (464)
    0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000,   // 0x01E0 (480)
    0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800,   // 0x01F0 (496)
    0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C,   // 0x0200 (512)
    0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C,   // 0x0210 (528)
    0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000,   // 0x0220 (544)
    0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800,   // 0x0230 (560)
    0x632C, 0x0000, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800,   // 0x0240 (576)
    0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C,   // 0x0250 (592)
    0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0xF800,   // 0x0260 (608)
    0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800,   // 0x0270 (624)
    0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800,   // 0x0280 (640)
    0x632C, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800,   // 0x0290 (656)
    0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C,   // 0x02A0 (672)
    0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0xF800, 0xF800, 0x632C,   // 0x02B0 (688)
    0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000,   // 0x02C0 (704)
    0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000,   // 0x02D0 (720)
    0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800,   // 0x02E0 (736)
    0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C,   // 0x02F0 (752)
    0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C,   // 0x0300 (768)
    0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000,   // 0x0310 (784)
    0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C,   // 0x0320 (800)
    0x0000, 0x0000, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0xF800, 0xF800,   // 0x0330 (816)
    0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C,   // 0x0340 (832)
    0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000,   // 0x0350 (848)
    0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0xF800,   // 0x0360 (864)
    0x0000, 0xF800, 0xF800, 0xF800, 0x632C, 0xF800, 0xF800, 0x632C, 0x632C, 0x632C, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800,   // 0x0370 (880)
    0x632C, 0x632C, 0x632C, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C, 0x0000, 0xF800, 0xF800, 0x632C,   // 0x0380 (896)
    0x632C, 0x632C, 0x632C, 0x632C, 0x0000, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0x0000, 0x0000,   // 0x0390 (912)
    0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0xF800, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800,   // 0x03A0 (928)
    0x632C, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x03B0 (944)
    0x0000, 0x0000, 0xF800, 0x0000, 0x0000, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x03C0 (960)
    0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800,   // 0x03D0 (976)
    0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0x0000, 0xF800, 0x0000,   // 0x03E0 (992)
    0xF800, 0x0000, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0x0000, 0xF800, 0x0000, 0x0000,   // 0x03F0 (1008)
    0x0000, 0x0000, 0xF800, 0x0000, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0x0000,   // 0x0400 (1024)
    0x0000, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000,   // 0x0410 (1040)
    0x0000, 0x0000, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0x0000,   // 0x0420 (1056)
    0x0000, 0x0000, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0x0000, 0xF800, 0x0000, 0x0000, 0xF800, 0x0000, 0x0000,   // 0x0430 (1072)
    0x0000, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 0xF800, 0x0000, 
};
//----------------------------------------------------------------------------
static const byte invader[8][7]  = {
{8,5, B00111000,B11010110,B11111110,B00101000,B11000110},
{8,5, B00111000,B11010110,B11111110,B01000100,B00101000},
{8,5, B00010000,B10111010,B11010110,B01111100,B00101000},
{8,5, B00111000,B01010100,B11111110,B10010010,B00101000},
{8,5, B00111000,B01010100,B01111100,B01000100,B00101000},
{8,5, B00111000,B01010100,B01111100,B00101000,B01010100},
{8,5, B01010100,B10000010,B01010100,B10000010,B01010100},
{8,5, B01010100,B10000010,B01010100,B10000010,B01010100},
};
//----------------------------------------------------------------------------
static const byte playership[3][6]  = {
{8,4, B00010000,B01111100,B11111110,B11111110},
{8,4, B10001010,B01000000,B00000100,B10010010},
{8,4, B10010010,B00000100,B01000000,B10001010},
};
//----------------------------------------------------------------------------
static const byte bunker[5][7]  = {
{8,5, B01111110,B11111111,B11111111,B11100111,B11000011},
{8,5, B01111110,B11011011,B11111111,B10100101,B11000011},
{8,5, B01101110,B11011011,B01110110,B10100101,B11000011},
{8,5, B01100110,B11011001,B01010110,B10100101,B01000010},
{8,5, B00100010,B10001001,B01010010,B10100101,B01000010},
};
//----------------------------------------------------------------------------
static const byte bomb[2][6]  = {
{2,4, B10000000,B01000000,B10000000,B01000000},
{2,4, B01000000,B10000000,B01000000,B10000000},
};
//----------------------------------------------------------------------------
static const byte saucer[2][10]  = {
{11,4, B00011111,B00000000,B01101010,B11000000,B11111111,B11100000,B01100100,B11000000},
{11,4, B01011101,B11000000,B01010101,B01000000,B01010101,B01000000,B01011101,B11000000},
};
//----------------------------------------------------------------------------


bool update()
{
	yield();
	delay(5);
	// buttons.update();
	// if (millis() - lastFrameCount >= frameSpeed)
	// {
	// 	lastFrameCount = millis();
	display.commit();
	// 	return true;
	// }
	// else
	// 	return false;

}

//ported nonstandard
//----------------------------------------------------------------------------
void newgame() {
	score = 0;
	lives = 3;
	gamelevel = 0;
	shipx = 80;
	shotx = -1;
	shoty = -1;
	deadcounter = -1;
	saucers = -1;
	starsSetup();
	for (int i = 0; i < 4; i++) {
		invadershotx[i] = -1;
		invadershoty[i] = -1;
	}
	gamestatus = "newlevel";
	setButtonsCallbacks();
}
//----------------------------------------------------------------------------
void newlevel() {
	invaderanz = 30;
	invaderctr = 29;
	invaderxr = 1;
	invaderyr = 1;
	checkdir = 0;
	nextxdir = 4;
	nextydir = 0;
	yeahtimer = 0;
	delayBip = 0;
	invadershottimer = 120;
	saucertimer = 480;
	int down = gamelevel;
	if (gamelevel > 8) { down = 16*2; }
	for (int i = 0; i < 6; i++) {
		invaderx[i] = 10 + i * 8*2;
		invaderx[i + 6] = 10 + i * 8*2;
		invaderx[i + 12] = 10 + i * 8*2;
		invaderx[i + 18] = 10 + i * 8*2;
		invaderx[i + 24] = 10 + i * 8*2;
		invadery[i] = 14 + down;
		invadery[i + 6] = 13*2 + down;
		invadery[i + 12] = 19*2 + down;
		invadery[i + 18] = 25*2 + down;
		invadery[i + 24] = 31*2 + down;
		invaders[i] = 4;
		invaders[i + 6] = 2;
		invaders[i + 12] = 2;
		invaders[i + 18] = 0;
		invaders[i + 24] = 0;
		invaderframe[i] = 0;
		invaderframe[i + 6] = 0;
		invaderframe[i + 12] = 0;
		invaderframe[i + 18] = 0;
		invaderframe[i + 24] = 0;
		yield();
	}
	for (int i = 0; i < 4; i++) {
		bunkers[i] = 0;
		if (gamelevel > 5) { bunkers[i] = -1; }
		yield();
	}
	gamestatus = "running";
}
//----------------------------------------------------------------------------
void showscore() {
	if (infoshow == 1 && saucers == -1) {
		if (lives > 1) { drawBitmap(0, 0, playership[0], TFT_WHITE, 2); }
		if (lives > 2) { drawBitmap(18, 0, playership[0], TFT_WHITE, 2); }
		baseSprite->cursor_x= 84 - 4 * (score > 9) - 4 * (score > 99) - 4 * (score > 999);
		baseSprite->cursor_y = 10;
		baseSprite->print(score);
		baseSprite->cursor_x = 112;
		baseSprite->cursor_y = 10;
		baseSprite->print(gamelevel + 1);
	}
}
//----------------------------------------------------------------------------
void nextlevelcheck() {
	// increment timer after all invaders killed
	if (invaderanz == 0) {
		yeahtimer++;
		if (yeahtimer >= 90) {
			gamelevel++;
			gamestatus = "newlevel";
		}
	}
}
//----------------------------------------------------------------------------
void handledeath() {
	deadcounter--;
	if (deadcounter == 0) {
		deadcounter = -1;
		lives--;
		shipx = 0;
		if (lives == 0) { gamestatus = "gameover"; }
	}
}


//ported specific
//----------------------------------------------------------------------------
void setButtonsCallbacks() {
	buttons.setButtonHeldRepeatCallback(BTN_LEFT, 250, [](uint){
		if (shipx > 0 && deadcounter == -1) {
			shipx-=1;
		}
	});
	buttons.setButtonHeldRepeatCallback(BTN_LEFT, 250, [](uint){
		if (shipx < 111 && deadcounter == -1) {
			shipx+=1;
		}
	});
	buttons.setBtnPressCallback(BTN_A, [](){
		if (shotx == -1 && deadcounter == -1) {
			shotx = shipx + 6;
			shoty = 106;
			tone(BUZZ_PIN, 400, 50);
		}
	});
	buttons.setBtnPressCallback(BTN_B, [](){
		gamestatus = "paused";
	});
}
//----------------------------------------------------------------------------
void drawplayership() {
	if (deadcounter == -1) {
		drawBitmap(shipx, 110, playership[0], TFT_WHITE, 2);
	}
	else {
		drawBitmap(shipx, 110, playership[1 + invadershotframe], TFT_YELLOW, 2);
		handledeath();
	}
}
//----------------------------------------------------------------------------
void drawplayershot() {
	if (shotx != -1) {
		shoty = shoty - 2;
		baseSprite->drawLine(shotx, shoty, shotx, shoty + 6, TFT_YELLOW);
		baseSprite->drawLine(shotx+1, shoty, shotx+1, shoty + 6, TFT_YELLOW);
		if (shoty < 0) {
			shotx = -1;
			shoty = -1;
		}
	}
}
//----------------------------------------------------------------------------
void invaderlogic() {
	// increment invader counter
	if (invaderanz > 0) {
		checkdir = 0;
		do {
			invaderctr++;
			if (invaderctr > 30) {
				invaderctr = 0;
				checkdir = 1;
				invadersound = ++invadersound % 4;
				if (delayBip <= 0) {
					if (invaderanz < 6) {
						delayBip = 5;
					}
					else if (invaderanz < 11) {
						delayBip = 3;
					} if (invaderanz < 21) {
						delayBip = 2;
					}
				}
				else { delayBip--; }
			}
			yield();
		} while (invaders[invaderctr] == -1);

		// change direction?
		if (checkdir == 1) {
			if (nextydir != 0) {
				invaderxr = 0;
				invaderyr = 2;
			}
			else {
				invaderxr = nextxdir;
				invaderyr = 0;
			}
			checkdir = 0;
		}

		// change invader position
		invaderx[invaderctr] = invaderx[invaderctr] + invaderxr;
		invadery[invaderctr] = invadery[invaderctr] + invaderyr;

		// determine bunker removal if invaders are too low
		if (invadery[invaderctr] > 80) {
			for (int i = 0; i < 4; i++) {
				bunkers[i] = -1;
			}
		}

		// determine game over if invaders reach bottom
		if (invadery[invaderctr] > 100) {
			gamestatus = "gameover";
		}

		// determine screen border hit -> go down, then change direction
		if (invaderx[invaderctr] > 108 && invaderxr > 0) {
			nextxdir = -4;
			nextydir = 4;
		}
		if (invaderx[invaderctr] < 4 && invaderxr < 0) {
			nextxdir = 4;
			nextydir = 4;
		}
		if (invaderyr != 0) { nextydir = 0; }

		//change invader shape
		invaderframe[invaderctr] = ++invaderframe[invaderctr] % 2;

		// remove killed invader
		if (invaders[invaderctr] == 6) {
			invaders[invaderctr] = -1;
			invaderanz--;
		}

		// release invadershoot
		if (invadershottimer <= 0 && invadershots < gamelevel + 1 && invadershots < 4 && invadery[invaderctr] < 80) {
			invadershottimer = 120 - gamelevel * 10;
			invadershots++;
			int flag = 0;
			for (int u = 0; u < 4; u++) {
				if (flag == 0 && invadershotx[u] == -1) {
					invadershotx[u] = invaderx[invaderctr] + 2;
					invadershoty[u] = invadery[invaderctr];
					flag = 1;
				}
			}
		}
	}
}
//----------------------------------------------------------------------------
void drawinvaders() {
	infoshow = 1;
	for (int i = 0; i < 30; i++) {
		if (invaders[i] != -1) {
			if (invaders[i] == 0) drawBitmap(invaderx[i], invadery[i], invader[invaders[i] + invaderframe[i]], TFT_ORANGE, 2);
			if (invaders[i] == 2) drawBitmap(invaderx[i], invadery[i], invader[invaders[i] + invaderframe[i]], TFT_PINK, 2);
			if (invaders[i] == 4) drawBitmap(invaderx[i], invadery[i], invader[invaders[i] + invaderframe[i]], TFT_BLUE, 2);
			if (invaders[i] == 6) drawBitmap(invaderx[i], invadery[i], invader[invaders[i] + invaderframe[i]], TFT_YELLOW, 2);
			
			if (invadery[i] < 5) {
				infoshow = 0;
			}
		}

		// determine collission: invader & player shoot
		checkl = invaderx[i];
		checkr = invaderx[i] + 12;
		checkt = invadery[i];
		checkb = invadery[i] + 8;
		if (invaders[i] == 4) {
			checkl++;
			checkr--;
		}
		if (invaders[i] != -1 && invaders[i] != 6 && shotx >= checkl && shotx <= checkr && shoty + 2 >= checkt && shoty <= checkb) {
			score = score + invaders[i] * 10 + 10;
			invaders[i] = 6;
			shotx = -1;
			shoty = -1;
			// destroyed->note(10, 0.05);
			tone(BUZZ_PIN, 50, 50);

			// invaderDestroyed->play();
		}
		yield();
	}
}
//----------------------------------------------------------------------------
void invadershot() {
	// handle invadershoot timer & framecounter
	invadershottimer--;
	if(invadershotframe == pastInvaderShotFrame)
		invadershotframe = !invadershotframe;
	else
		pastInvaderShotFrame = !pastInvaderShotFrame;
	// move invadershots, draw & check collission
	for (int i = 0; i < 4; i++) {
		if (invadershotx[i] != -1) {
			invadershoty[i] = invadershoty[i] + 1;
			drawBitmap(invadershotx[i], invadershoty[i], bomb[invadershotframe], TFT_RED, 2);

			// check collission: invadershot & bunker
			for (int u = 0; u < 4; u++) {
				checkl = 22 + u * 36;
				checkr = 22 + u * 36 + 14;
				checkt = 90;
				checkb = 100;
				if (bunkers[u] != -1 && invadershotx[i] + 1 >= checkl && invadershotx[i] <= checkr && invadershoty[i] + 3 >= checkt && invadershoty[i] <= checkb) {
					bunkers[u]++;
					if (bunkers[u] > 4) { bunkers[u] = -1; }
					invadershotx[i] = -1;
					invadershoty[i] = -1;
					invadershots--;
				}
				yield();
			}

			// check collission: invadershot & player
			checkl = shipx;
			checkr = shipx + 12;
			checkt = 107;
			checkb = 110;
			if (deadcounter == -1 && invadershotx[i] + 1 >= checkl && invadershotx[i] <= checkr && invadershoty[i] + 3 >= checkt && invadershoty[i] <= checkb) {
				deadcounter = 60;
				// destroyed->note(10, 0.05);
				tone(BUZZ_PIN, 50, 50);
				// playerDestroyed->play();
			}

			//check collission: invadershot & playershoot
			checkl = invadershotx[i];
			checkr = invadershotx[i] + 1;
			checkt = invadershoty[i];
			checkb = invadershoty[i] + 3;
			if (shotx >= checkl && shotx <= checkr && shoty + 2 >= checkt && shoty <= checkb) {
				shotx = -1;
				shoty = -1;
				invadershotx[i] = -1;
				invadershoty[i] = -1;
				invadershots--;
			}

			// invadershoot on bottom off screen?
			if (invadershoty[i] > 128) {
				invadershotx[i] = -1;
				invadershoty[i] = -1;
				invadershots--;
			}
		}
		yield();
	}
}
//----------------------------------------------------------------------------
void drawbunkers() {
	for (int i = 0; i < 4; i++) {
		checkl = 12 + i * 30;
		checkr = 12 + i * 30 + 14;
		checkt = 90;
		checkb = 100;
		if (bunkers[i] != -1 && shotx >= checkl && shotx <= checkr && shoty + 2 >= checkt && shoty <= checkb) {
			bunkers[i]++;
			shotx = -1;
			shoty = -1;
			if (bunkers[i] > 4) { bunkers[i] = -1; }
		}

		if (bunkers[i] != -1) {
			drawBitmap(12 + i * 30, 90, bunker[bunkers[i]], TFT_GREEN, 2);
		}
		yield();
	}
}
//----------------------------------------------------------------------------
void saucerappears() {
	saucertimer--;
	if (saucertimer <= 0) {
		saucertimer = 480;
		if (infoshow == 1 && saucers == -1) {
			// mainMusic->pause();
			// removeTrack(mainMusic);
			// addTrack(ufoSound);
			// ufoSound->setSpeed(2);
			// ufoSound->setRepeat(1);
			// ufoSound->play();
			saucers = 0;
			int i = random(2);
			if (i == 0) {
				saucerx = 0;
				saucerdir = 1;
			}
			else {
				saucerx = 146;
				saucerdir = -1;
			}
		}
	}
}
//----------------------------------------------------------------------------
void movesaucer() {
	if (saucers == 0) {
		saucerx = saucerx + saucerdir;
		if (saucerx <= 0 or saucerx >= 146) {
	// removeTrack(ufoSound);
	// mainMusic->resume();
	// addTrack(mainMusic);
			saucers = -1;
		}

		// check collission: player shot & saucer  
		checkl = saucerx;
		checkr = saucerx + 20;
		checkt = 0;
		checkb = 6;
		if (shotx >= checkl && shotx <= checkr && shoty + 2 >= checkt && shoty <= checkb) {
			score += 100;
			saucers = 1;
			shotx = -1;
			shoty = -1;
			saucerwait = 30;
	// removeTrack(ufoSound);
	// invaderDestroyed->play();
	// mainMusic->resume();
	// addTrack(mainMusic);
		}
	}
}
//----------------------------------------------------------------------------
void drawsaucer() {
	if (saucers != -1) {
		drawBitmap(saucerx, 0, saucer[saucers], TFT_RED, 2);
		if (saucers == 1) {
			saucerwait--;
			if (saucerwait <= 0) {
		// removeTrack(ufoSound);
		// addTrack(mainMusic);
		// mainMusic->play();
		// mainMusic->setRepeat(1);
				saucers = -1;
			}
		}
	}
}


//----------------------------------------------------------------------------
void eraseDataSetup()
{
	elapsedMillis = millis();
	blinkState = 1;
	buttons.setBtnPressCallback(BTN_B, [](){
		gamestatus = "dataDisplay";

	});
	buttons.setBtnPressCallback(BTN_A, [](){
		JsonArray &empty = jb.createArray();
		File file = SPIFFS.open(highscoresPath, "w");
		empty.prettyPrintTo(file);
		file.close();
		gamestatus = "title";

	});
}
void eraseData()
{
	baseSprite->clear(TFT_BLACK);
	baseSprite->setTextFont(2);

	if (millis() - elapsedMillis >= 350) {
		elapsedMillis = millis();
		blinkState = !blinkState;
	}

	baseSprite->setTextColor(TFT_WHITE);
	baseSprite->setCursor(4, 5);
	baseSprite->printCenter("ARE YOU SURE?");
	baseSprite->setCursor(4, 25);
	baseSprite->printCenter("This cannot");
	baseSprite->setCursor(4, 41);
	baseSprite->printCenter("be reverted!");

	if (blinkState){
		baseSprite->drawRect((baseSprite->width() - 60)/2, 102, 30*2, 9*2, TFT_RED);
		baseSprite->setTextColor(TFT_RED);
		baseSprite->setCursor(28*2, 103);
		baseSprite->printCenter("DELETE");
	}
	else {
		baseSprite->fillRect((baseSprite->width() - 60)/2, 102, 30*2, 9*2, TFT_RED);
		baseSprite->setTextColor(TFT_WHITE);
		baseSprite->setCursor(28*2, 103);
		baseSprite->printCenter("DELETE");
	}
}

void dataDisplaySetup()
{
	jb.clear();
	File file = SPIFFS.open(highscoresPath, "r");
	JsonArray &hiscores = jb.parseArray(file);
	file.close();
	memset(scoreArray, 0, 6);
	hiscoresSize = hiscores.size();
	for(uint8_t i = 0; i < 6; i++)
	{
		for(JsonObject& element:hiscores)
		{
			if(element["Rank"] == i)
			{
				strncpy(nameArray[i], element["Name"], 4);
				scoreArray[i] = element["Score"];
			}
			yield();
		}
	}
	buttons.setBtnPressCallback(BTN_A, [](){
		gamestatus = "title";
	});
	buttons.setBtnPressCallback(BTN_B, [](){
		gamestatus = "title";
	});
	buttons.setBtnPressCallback(BTN_C, [](){
		gamestatus = "eraseData";
	});
}
void dataDisplay()
{
	baseSprite->clear(TFT_BLACK);
	baseSprite->setCursor(32, -2);
	baseSprite->setTextSize(1);
	baseSprite->setTextFont(2);
	baseSprite->setTextColor(TFT_RED);
	baseSprite->printCenter("HIGHSCORES");
	baseSprite->setCursor(3, 110);
	for (int i = 1; i < 6;i++)
	{
		baseSprite->setCursor(6, i * 20);
		if(i <= hiscoresSize)
			baseSprite->printf("%d.   %.3s    %04d", i, nameArray[i], scoreArray[i]);
		else
			baseSprite->printf("%d.    ---   ----", i);
	}
	baseSprite->setCursor(2, 115);
	baseSprite->print("Erase");
}
void showtitle() {
	baseSprite->clear(TFT_BLACK);
	for(int i = 0; i < STAR_COUNT; i++)
	{
		// Remove the star from the screen by changing its pixel to the background color.
		baseSprite->drawPixel(stars[i].x, stars[i].y, BACKGROUND_COLOR);

		// Update the position of the star.
		stars[i].update();

		// If the star's Y position is off the screen after the update.
		if(stars[i].y >= baseSprite->height())
		{
			// Randomize its position.
			stars[i].randomize(0, baseSprite->width(), 0, baseSprite->height(), STAR_SPEED_MIN, STAR_SPEED_MAX);
			// Set its Y position back to the top.
			stars[i].y = 0;
		}

		// Draw the star at its new coordinate.
		baseSprite->fillRect(stars[i].x, stars[i].y, 2, 2, STAR_COLOR);
	}
	if(millis() - blinkMillis >= 250)
	{
		blinkMillis = millis();
		blinkState = !blinkState;
	}
	baseSprite->setTextColor(TFT_WHITE);
	baseSprite->drawIcon(titleLogo, 4, 10, 60, 18, 2, TFT_BLACK);
	baseSprite->setTextColor(TFT_RED);
	baseSprite->setFreeFont(TT1);
	baseSprite->setTextSize(2);
	baseSprite->setCursor(10, 80);
	baseSprite->printCenter("START");
	baseSprite->setCursor(10, 100);
	baseSprite->printCenter("HIGHSCORES");
	baseSprite->setCursor(10, 120);
	baseSprite->printCenter("QUIT");
	if(blinkState)
	{
		baseSprite->drawRect(15, 67 + cursor * 20, 98, 16, TFT_RED);
		baseSprite->drawRect(14, 66 + cursor * 20, 100, 18, TFT_RED);
	}

	
	
}
void titleSetup()
{
	cursor = 0;
	blinkMillis = millis();
	blinkState = 0;
	buttons.setBtnPressCallback(BTN_UP, [](){
		if(cursor > 0)
		{
			cursor--;
			blinkMillis = millis();
			blinkState = 1;
		}
	});
	buttons.setBtnPressCallback(BTN_DOWN, [](){
		if(cursor < 2)
		{
			cursor++;
			blinkMillis = millis();
			blinkState = 1;
		}
	});
	
	buttons.setBtnPressCallback(BTN_A, [](){
		
		switch (cursor)
		{
			case 0:
				gamestatus = "newgame";
				break;
			case 1:
				gamestatus = "dataDisplay";
				break;
		}
	});
}

void enterInitialsSetup()
{
	name = "AAA";
	charCursor = 0;
	previous = "";
	elapsedMillis = millis();
	hiscoreMillis = millis();
	blinkState = 1;
	hiscoreBlink = 0;
	buttons.setButtonHeldRepeatCallback(BTN_UP, 800, [](uint){
		blinkState = 1;
		elapsedMillis = millis();
		name[charCursor]++;
		// A-Z 0-9 :-? !-/ ' '
		if (name[charCursor] == '0') name[charCursor] = ' ';
		if (name[charCursor] == '!') name[charCursor] = 'A';
		if (name[charCursor] == '[') name[charCursor] = '0';
		if (name[charCursor] == '@') name[charCursor] = '!';
	});
	buttons.setButtonHeldRepeatCallback(BTN_DOWN, 800, [](uint){
		blinkState = 1;
		elapsedMillis = millis();
		name[charCursor]--;
		if (name[charCursor] == ' ') name[charCursor] = '?';
		if (name[charCursor] == '/') name[charCursor] = 'Z';
		if (name[charCursor] == 31)  name[charCursor] = '/';
		if (name[charCursor] == '@') name[charCursor] = ' ';
	});
	buttons.setBtnPressCallback(BTN_LEFT, [](){
		if(charCursor > 0){
			charCursor--;
			blinkState = 1;
			elapsedMillis = millis();
		}
	});
	buttons.setBtnPressCallback(BTN_RIGHT, [](){
		if(charCursor < 2){
			charCursor++;
			blinkState = 1;
			elapsedMillis = millis();
		}
	});
	buttons.setBtnPressCallback(BTN_A, [](){
		charCursor++;
		blinkState = 1;
		elapsedMillis = millis();
	});
}
void enterInitials() {
  
    if (millis() - elapsedMillis >= 350) //cursor blinking routine
	{
		elapsedMillis = millis();
		blinkState = !blinkState;
	}
    if(millis()-hiscoreMillis >= 1000)
    {
      hiscoreMillis = millis();
      hiscoreBlink = !hiscoreBlink;
    }
    previous = name;

    if (previous != name)
    {
      blinkState = 1;
      elapsedMillis = millis();
    }
	
	baseSprite->clear(TFT_BLACK);
    baseSprite->setCursor(16, 8);
    baseSprite->setTextFont(2);
    baseSprite->setTextColor(TFT_WHITE);
    baseSprite->setTextSize(1);
    baseSprite->printCenter("ENTER NAME");
    baseSprite->setCursor(20, 80);
	
    if(hiscoreBlink && score > tempScore)
      baseSprite->printCenter("NEW HIGH!");
    else
      baseSprite->printf("SCORE: %04d", score);

    baseSprite->setCursor(40, 40);
    baseSprite->print(name[0]);
	baseSprite->setCursor(55, 40);
    baseSprite->print(name[1]);
	baseSprite->setCursor(70, 40);
    baseSprite->print(name[2]);
    // baseSprite->drawRect(30, 38, 100, 20, TFT_WHITE);
	if(blinkState){
		baseSprite->drawFastHLine(38 + 15*charCursor, 56, 12, TFT_WHITE);
	}


	if(charCursor >= 3)
	{
		File file = SPIFFS.open(highscoresPath, "r");
		jb.clear();
		JsonArray &hiscores2 = jb.parseArray(file);
		file.close();
		JsonObject &newHiscore = jb.createObject();
		newHiscore["Name"] = name;
		newHiscore["Score"] = score;
		newHiscore["Rank"] = 1;

		if(savePresent && hiscores2.size() > 0)
		{
			newHiscore["Rank"] = 999;
			Serial.println(hiscores2.size());
			uint16_t tempSize = hiscores2.size();
			for (int16_t i = 0; i < tempSize;i++)//searching for a place in the leaderboard for our new score
			{
				Serial.printf("i: %d\n", i);
				Serial.println((uint16_t)(hiscores2[i]["Rank"]));
				Serial.println((uint16_t)(hiscores2[i]["Score"]));
				delay(5);
				if(score >= (uint16_t)(hiscores2[i]["Score"]))
				{
					Serial.println("ENTERED");
					delay(5);
					if((uint16_t)(newHiscore["Rank"]) >  (uint16_t)(hiscores2[i]["Rank"]))
					{
						newHiscore["Rank"] = (uint16_t)(hiscores2[i]["Rank"]);
					}
					JsonObject &tempObject = jb.createObject();
					tempObject["Name"] = (const char *)(hiscores2[i]["Name"]);
					tempObject["Score"] = (uint16_t)(hiscores2[i]["Score"]);
					tempObject["Rank"] = (uint16_t)(hiscores2[i]["Rank"]) + 1;
					tempObject.prettyPrintTo(Serial);
					delay(5);
					hiscores2.remove(i);
					hiscores2.add(tempObject);
					tempSize--;
					i--;
				}
				else
				{
					if(newHiscore["Rank"] <= (uint16_t)(hiscores2[i]["Rank"]) || newHiscore["Rank"] == 999)
						newHiscore["Rank"] = (uint16_t)(hiscores2[i]["Rank"]) + 1;
				}
			}
		}
		hiscores2.add(newHiscore);
		file = SPIFFS.open(highscoresPath, "w");
		hiscores2.prettyPrintTo(file);
		file.close();
		gamestatus = "dataDisplay";
	}
}

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
	baseSprite->clear(TFT_BLACK);
	display.commit();
	Serial.println("display ok");
	Serial.println("buttons begin");
	SPIFFS.begin();
	Serial.println("spiffs begin");
	UpdateManager::addListener(&buttons);


	// shootSound = new MPTrack("/Invaderz/shoot.wav");
	// invaderDestroyed = new MPTrack("/Invaderz/invaderDestroyed.wav");
	// mainMusic = new MPTrack("/Invaderz/main.wav");
	// titleMusic = new MPTrack("/Invaderz/title.wav");
	// playerDestroyed = new MPTrack("/Invaderz/playerDestroyed.wav");
	// gameoverMusic = new MPTrack("/Invaderz/gameover.wav");
	// ufoSound = new MPTrack("/Invaderz/ufo.wav");

	baseSprite->clear(TFT_BLACK);
	// baseSprite->fillRect(0,0,20,20,TFT_RED);
	starsSetup();
	gamestatus = "title";
	File file = SPIFFS.open(highscoresPath, "r");
	JsonArray &hiscores = jb.parseArray(file);
	file.close();
	if(!SPIFFS.exists("/Invaderz"))
		SPIFFS.mkdir("/Invaderz");
	if(hiscores.success())
		savePresent = 1;
	else
	{
		Serial.println("No save present");
		JsonArray &hiscores = jb.createArray();
		JsonObject &test = jb.createObject();
		File file = SPIFFS.open(highscoresPath, "w");
		hiscores.prettyPrintTo(file);
		file.close();
	}
	hiscores.prettyPrintTo(Serial);
	Serial.println("saves ok");
}
String prevGamestatus = gamestatus;
bool screenChange = 0;
//----------------------------------------------------------------------------    
// loop
//----------------------------------------------------------------------------    
void loop() {
	if(gamestatus != prevGamestatus)
	{
		screenChange = 1;
		prevGamestatus = gamestatus;
	}
	else
	{
		screenChange = 0;
	}
	
	if (gamestatus == "title") {
		if(screenChange)
		{
			titleSetup();
		}
		showtitle();
	}
	if (gamestatus == "newgame") { newgame(); } // new game

	if (gamestatus == "newlevel") { newlevel(); } // new level

	if (gamestatus == "running") { // game running loop
		baseSprite->clear(TFT_BLACK);
		for(int i = 0; i < STAR_COUNT; i++)
		{
			// Remove the star from the screen by changing its pixel to the background color.
			baseSprite->drawPixel(stars[i].x, stars[i].y, BACKGROUND_COLOR);

			// Update the position of the star.
			stars[i].update();

			// If the star's Y position is off the screen after the update.
			if(stars[i].y >= baseSprite->height())
			{
				// Randomize its position.
				stars[i].randomize(0, baseSprite->width(), 0, baseSprite->height(), STAR_SPEED_MIN, STAR_SPEED_MAX);
				// Set its Y position back to the top.
				stars[i].y = 0;
			}

			// Draw the star at its new coordinate.
			baseSprite->fillRect(stars[i].x, stars[i].y, 2, 2, STAR_COLOR);
			yield();
		}
		showscore();
		// checkbuttons(); // check buttons && move playership
		drawplayership(); // draw player ship
		drawplayershot(); // move + draw player shoot
		invaderlogic(); // invader logic
		drawinvaders(); // draw invaders
		invadershot(); // invaders shoot
		nextlevelcheck(); // next level?
		drawbunkers(); // draw bunkers & check collission with player shot
		saucerappears(); // saucer appears?
		movesaucer(); // move saucer
		drawsaucer(); // draw saucer & remove if hit
		showscore(); // show lives, score, level
	}
	if (gamestatus == "gameover") { // game over

		if(screenChange){
			buttons.setBtnPressCallback(BTN_A, [](){
				File file = SPIFFS.open(highscoresPath, "r");
				JsonArray &hiscores = jb.parseArray(file);
				file.close();
				for (JsonObject& element : hiscores)
				{
					if(element["Rank"] == 1)
						tempScore = element["Score"].as<int>();
				}
				hiscores.end();
				Serial.println("HERE");
				delay(5);
				gamestatus = "enterInitials";
			});
			buttons.setBtnPressCallback(BTN_B, [](){
				File file = SPIFFS.open(highscoresPath, "r");
				JsonArray &hiscores = jb.parseArray(file);
				file.close();
				for (JsonObject& element : hiscores)
				{
					if(element["Rank"] == 1)
						tempScore = element["Score"].as<int>();
				}
				hiscores.end();
				Serial.println("HERE");
				delay(5);
				gamestatus = "enterInitials";
			});
		}
		baseSprite->setTextColor(TFT_RED);
		baseSprite->setTextSize(2);
		baseSprite->setTextFont(1);
		baseSprite->drawRect(5, 45, 118, 38, TFT_WHITE);
		baseSprite->drawRect(4, 44, 120, 40, TFT_WHITE);
		baseSprite->fillRect(6, 46, 116, 36, TFT_BLACK);
		baseSprite->setCursor(47, 57);
		baseSprite->printCenter("GAME OVER");
		
	}
	if(gamestatus == "paused")
	{
		if(screenChange){
			buttons.setBtnPressCallback(BTN_A, [](){
				gamestatus = "running";
			});
			buttons.setBtnPressCallback(BTN_B, [](){
				gamestatus = "title";
			});
		}
		baseSprite->clear(TFT_BLACK);
		baseSprite->setTextColor(TFT_RED);
		baseSprite->setCursor(0, baseSprite->height()/2 - 18);
		baseSprite->setTextFont(2);
		baseSprite->setTextSize(2);
		baseSprite->printCenter("Paused");
		baseSprite->setCursor(4, 110);
		baseSprite->setFreeFont(TT1);
		baseSprite->printCenter("A:RESUME  B:QUIT");
	}
	if(gamestatus == "eraseData")
	{
		if(screenChange){
			eraseDataSetup();
		}
		eraseData();
	}
	if(gamestatus == "displayData")
	{
		if(screenChange){
			dataDisplaySetup();
		}
		dataDisplay();
	}
	if(gamestatus == "enterInitials")
	{
		if(screenChange){
			enterInitialsSetup();
		}
		enterInitials();
	}
	UpdateManager::update();
	update();
}