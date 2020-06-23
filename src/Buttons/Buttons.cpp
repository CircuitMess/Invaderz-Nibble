#include "Buttons.h"
#include <Wire.h>
void Buttons::begin() {
	Wire.begin(4, 5);
	kpd.begin();


	// kpd.setDebounceTime(20);
	// kpd.setHoldTime(0);

	for(int i = 0;i<NUM_BTN;i++) {
		kpd.pin_mode(i, INPUT_PULLUP);
	}
}

void Buttons::update() {
	buttonsData = 0;
	// delay(1);
	// delayMicroseconds(1000);
	// delay(1);
/* 	 ads.readADC_SingleEnded(0);
	 ads.readADC_SingleEnded(1);
	 ads.readADC_SingleEnded(1);
	 ads.readADC_SingleEnded(2);
	 ads.readADC_SingleEnded(2);
	 ads.readADC_SingleEnded(3);
	 ads.readADC_SingleEnded(3); */
	// delayMicroseconds(1000);
	
	// Serial.print("Joystick X");
	// Serial.println(joystick_x);
	// Serial.print("Joystick Y");
	// Serial.println(joystick_y);
	// Serial.print("button a");
	// Serial.println(button_a);
	// Serial.print("Button B");
	// Serial.println(button_b);
	

	int tmp_value = kpd.port_read();

	for (uint8_t i = 0; i < NUM_BTN; i++) {
		bool pressed;
		pressed = !bitRead(tmp_value, i);
		if (pressed) //if button pressed
		{
			if (states[i] < 0xFFFE) { // we want 0xFFFE to be max value for the counter
				states[i]++; //increase button hold time
			}
			else if (states[i] == 0xFFFF) { // if we release / hold again too fast
				states[i] = 1;
			}
		}
		else
		{
			if (states[i] == 0) { //button idle
				continue;
			}
			if (states[i] == 0xFFFF) { //if previously released
				states[i] = 0; //set to idle
			}
			else {
				states[i] = 0xFFFF; //button just released
			}
		}
	}
	if(holdForUnlock && states[(uint8_t)11] == 0xFFFF)
	{
		Serial.println("Release ignored");
		holdForUnlock = 0;
		Buttons::update();
	}
}

bool Buttons::repeat(uint8_t button, uint16_t period) {
	if (period <= 1) {
		if ((states[(uint8_t)button] != 0xFFFF) && (states[(uint8_t)button])) {
			return true;
		}
	}
	else {
		if ((states[(uint8_t)button] != 0xFFFF) && ((states[(uint8_t)button] % period) == 1)) {
			return true;
		}
	}
	return false;
}

bool Buttons::pressed(uint8_t button) {
	return states[(uint8_t)button] == 1;
}

bool Buttons::released(uint8_t button) {
	return states[(uint8_t)button] == 0xFFFF;
}

bool Buttons::held(uint8_t button, uint16_t time) {
	return states[(uint8_t)button] == (time + 1);
}

char Buttons::getKey() {
	for(int i = 0; i < 22; i++) {
		if(Buttons::released(i)) {
			if(i == 9) {
				return '*';
			}else if(i == 10)
				return '0';
			else if(i == 11) {
				return '#';
			} else if(i < 11) {
				return '1' + i;
			} else {
				switch(i) {
					case 12:
						return 'A';
						break;
					case 13:
						return 'B';
						break;
					case 14:
						return 'C';
						break;
					case 15:
						return 'D';
						break;
					default:
						return NO_KEY;
						break;
				}
			}
		}
	}
	return NO_KEY;
}

uint16_t Buttons::timeHeld(uint8_t button) {
	if (states[(uint8_t)button] != 0xFFFF) {
		return states[(uint8_t)button];
	}
	else {
		return 0;
	}
}