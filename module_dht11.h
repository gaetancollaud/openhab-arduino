

#include "dht11.h"

String TEMP_TOPIC = "/openhab/in/" ARDUINO_NAME "Temp1/state";
String HUMIDITY_TOPIC = "/openhab/in/" ARDUINO_NAME "Humi1/state";

#define MODULE_DHT11_DELAY 10000

dht11 DHT11;

unsigned long lastDHT11 = 0;

void moduleDHT11Loop() {
	unsigned long now = millis();
	if (now - lastDHT11 > MODULE_DHT11_DELAY && isConnected()) {
		lastDHT11 = now;

		int chk = DHT11.read(MODULE_DHT11_PIN);

		Serial.print("Read sensor: ");
		switch (chk) {
			case DHTLIB_OK:
				Serial.println("OK");
				break;
			case DHTLIB_ERROR_CHECKSUM:
				Serial.println("Checksum error");
				break;
			case DHTLIB_ERROR_TIMEOUT:
				Serial.println("Time out error");
				break;
			default:
				Serial.println("Unknown error");
				break;
		}

		//		Serial.print("Humidity (%):\t\t");
		//		Serial.println((float) DHT11.humidity, 2);
		//
		//		Serial.print("Temperature (C):\t");
		//		Serial.println((float) DHT11.temperature, 2);

		mqttPublish(TEMP_TOPIC, String(DHT11.temperature));
		mqttPublish(HUMIDITY_TOPIC, String(DHT11.humidity));
	}
}
