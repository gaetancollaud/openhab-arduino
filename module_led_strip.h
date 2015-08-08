#include <arduino.h>

#include "WS2812.h"

WS2812 LED(MODULE_LED_STRIP_NUMBER); // 1 LED

cRGB value;
byte intensity;
byte sign;

void moduleLedStripCallback(String &topic, String &value);

void moduleLedStripLoad() {
	
	LED.setOutput(MODULE_LED_STRIP_PIN);
	
	LED.setColorOrderRGB();  // Uncomment for RGB color order
	//LED.setColorOrderBRG();  // Uncomment for BRG color order
	//LED.setColorOrderGRB();  // Uncomment for GRB color order (Default; will be used if none other is defined.)

	intensity = 0;
	sign = 1;
}

void moduleLedStripCallback(String &item, String &value) {
	moduleStream->print("led strip received : ");
	moduleStream->print(item);
	moduleStream->print(" = ");
	moduleStream->println(value);

	//TODO
}


void moduleLedStripLoop(){
	byte i = 0;
	if (sign) {
		intensity++;
		if (intensity == 255) 
			sign = 0; 
	}
	else {
		intensity--;
		if (intensity == 0)
			sign = 1;
	}

	while (i < MODULE_LED_STRIP_NUMBER){
		if ((i % 3) == 0) {  // First LED, and every third after that
			value.b = 0; 
			value.g = 0; 
			value.r = intensity; // RGB Value -> Red Only
			LED.set_crgb_at(i, value); // Set value at LED found at index 0
		}
		else if ((i % 3) == 1) { // Second LED, and every third after that
			value.b = 0; 
			value.g = intensity; 
			value.r = 0; // RGB Value -> Green Only
			LED.set_crgb_at(i, value); // Set value at LED found at index 0
		}
		else  { // Third LED, and every third after that
			value.b = intensity; 
			value.g = 0; 
			value.r = 0; // RGB Value -> Blue Only
			LED.set_crgb_at(i, value); // Set value at LED found at index 0
		}
		i++;
	}

	LED.sync(); // Sends the data to the LEDs
	delay(10); // Wait (ms)
}
