#include <arduino.h>

#include "WS2812.h"

WS2812 LED(MODULE_LED_STRIP_NUMBER);

cRGB value;

void moduleLedStripCallback(String &topic, String &value);

void moduleLedStripLoad() {

	LED.setOutput(MODULE_LED_STRIP_PIN);

	LED.setColorOrderRGB(); // Uncomment for RGB color order

	moduleTopicRegister("Strip", (void*) moduleLedStripCallback);
}

void convertHSVtoRGB(float h, float s, float v, byte rgb[]) {

	double r = 0, g = 0, b = 0;

	moduleStream->println("========= HSV");
	moduleStream->println(h);
	moduleStream->println(s);
	moduleStream->println(v);

	s /= 100;
	v /= 100;

	float c = v*s;
	float x = c * (1 - abs(((int)(h / 60)) % 2) - 1);
	float m = v - c;

	if (h < 60) {
		r = c;
		g = x;
	} else if (h < 120) {
		r = x;
		g = c;
	} else if (h < 180) {
		g = c;
		b = x;
	} else if (h < 240) {
		g = x;
		b = c;
	} else if (h < 300) {
		r = x;
		b = c;
	} else {
		r = c;
		b = x;
	}

	moduleStream->println("========= RGB double");
	moduleStream->println(r);
	moduleStream->println(g);
	moduleStream->println(b);

	rgb[0] = (byte) (r * 255);
	rgb[1] = (byte) (g * 255);
	rgb[2] = (byte) (b * 255);
}

void moduleLedStripCallback(String &item, String &value) {
	moduleStream->print("led strip received : ");
	moduleStream->print(item);
	moduleStream->print(" = ");
	moduleStream->println(value);

	int split1 = value.indexOf(',');
	int split2 = value.indexOf(',', split1 + 1);
	float h =  value.substring(0, split1).toFloat();
	float s =  value.substring(split1 + 1, split2).toFloat();
	float v =  value.substring(split2 + 1, value.length()).toFloat();

	byte rgb[3];
	convertHSVtoRGB(h, s, v, rgb);
	moduleStream->println("========= RGB byte (hex)");
	moduleStream->println(rgb[0], HEX);
	moduleStream->println(rgb[1], HEX);
	moduleStream->println(rgb[2], HEX);



	//TODO
}

void moduleLedStripLoop() {
	//TODO
	//	LED.sync(); // Sends the data to the LEDs
}
