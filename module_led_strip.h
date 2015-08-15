#include <arduino.h>

#include "WS2812.h"

WS2812 LED(MODULE_LED_STRIP_NUMBER);

cRGB lastColor = {255, 255, 255};

void moduleLedStripCallback(String &topic, String &value);

void moduleLedStripLoad() {

	LED.setOutput(MODULE_LED_STRIP_PIN);

	LED.setColorOrderRGB(); // Uncomment for RGB color order

	moduleTopicRegister("StripColor", (void*) moduleLedStripCallback);
}

cRGB convertHSVtoRGB(float h, float s, float v) {
	double r = 0, g = 0, b = 0;
	s /= 100;
	v /= 100;

	float t = (float) (((int) (h / 60 * 1000)) % 2000) / 1000;
	float c = v*s;
	float x = c * (1 - abs(t - 1));
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

	return
	{
		((r + m) * 255),
				((g + m) * 255),
				((b + m) * 255)
	};
}

void setLedStripColor(cRGB &color) {
	for (int i = 0; i < MODULE_LED_STRIP_NUMBER; i++) {
		LED.set_crgb_at(i, color);
	}
	LED.sync();
}

void moduleLedStripCallback(String &item, String &value) {
	//	moduleStream->print("led strip received : ");
	//	moduleStream->print(item);
	//	moduleStream->print(" = ");
	//	moduleStream->println(value);

	if (utilIsOn(value)) {
		setLedStripColor(lastColor);
	} else if (utilIsOff(value)) {
		cRGB c = {0, 0, 0};
		setLedStripColor(c);
	} else {
		int split1 = value.indexOf(',');
		int split2 = value.indexOf(',', split1 + 1);
		float h = value.substring(0, split1).toFloat();
		float s = value.substring(split1 + 1, split2).toFloat();
		float v = value.substring(split2 + 1, value.length()).toFloat();

		//		moduleStream->println("========= RGB byte (hex)");
		//		moduleStream->println(rgb.r, HEX);
		//		moduleStream->println(rgb.g, HEX);
		//		moduleStream->println(rgb.b, HEX);

		lastColor = convertHSVtoRGB(h, s, v);
		setLedStripColor(lastColor);
	}

	mqttPublish("/openhab/in/" ARDUINO_NAME "StripColor/state", value);
}

void moduleLedStripLoop() {
	//TODO
}
