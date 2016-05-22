#include <SoftwareSerial.h>

#include "config_infinite_table.h"

bool wifiConnected = false;
bool mqttConnected = false;

#include "module.h"

#define PIN_WIFI_STATUS 13

typedef struct {
	String topicCommand;
	String topicState;
	int pin;
} RelayTopic;


SoftwareSerial wifiSerial(2, 3);// RX, TX
String buffer;

int ledDelay = 10;
bool ledState = LOW;


void setup() {
	pinMode(PIN_WIFI_STATUS, OUTPUT);
	digitalWrite(PIN_WIFI_STATUS, LOW);

	buffer.reserve(200);
	buffer = "";

	Serial.begin(115200);
	wifiSerial.begin(9600);

	//wait for the wifi module to start
	delay(1000);

	//avoir echo
	sendConfigCommand("uart.setup(0, 9600, 8, 0, 1, 0)");

	moduleLoad(&Serial);
}

void loop() {
	statusLed();
	checkNetworkStatus();
	readWifiSerial();

	moduleLoop();
}

void sendCommand(String cmd) {
//	Serial.print("Sending command : ");
//	Serial.println(cmd);
	wifiSerial.println(cmd);
	wifiSerial.flush();
	delay(200);
}

void sendConfigCommand(String cmd) {
	sendCommand(cmd);
	delay(200);
}

void mqttSubsribe(String topic) {
	sendConfigCommand("mqSubscribe(\"" + topic + "\")");
}

void mqttPublish(String topic, String data) {
	sendCommand("mqPublish(\"" + topic + "\",\"" + data + "\")");
}

void initMQTT() {
	sendCommand("mqClose()");
	sendCommand("mqConnect()");
}

void readWifiSerial() {
	if (wifiSerial.available()) {
		wifiSerialData();
	}
}

bool isConnected(){
	return wifiConnected && mqttConnected;
}

void newLine() {
	char first = buffer[0];
	while (first == '>') {
		buffer = buffer.substring(2);
		first = buffer[0];
	}
	buffer.trim();
	if (buffer.length() > 0) {
		if (first == 'm') {
			handleTopicReponse(buffer);
		} else if (first == 'i') {
			handleInformation(buffer);
		} else {
			Serial.print("Unknown buffer : ");
			Serial.println(buffer);
		}
	}
	buffer = "";
}

void wifiSerialData() {
	while (wifiSerial.available()) {
		char inChar = (char) wifiSerial.read();
		if (inChar == '\n') {
			newLine();
		} else {
			buffer += inChar;
		}
	}
}

unsigned long lastStatusLed = 0;

void statusLed() {
	unsigned long now = millis();
	if (now - lastStatusLed > ledDelay) {
		ledState = !ledState;
		digitalWrite(PIN_WIFI_STATUS, ledState ? LOW : HIGH);
		lastStatusLed = now;
	}
}

unsigned long lastWifiStatus = 0;

void checkNetworkStatus() {
	unsigned long now = millis();
	int delay = wifiConnected ? 10000 : 1000;
	if (now - lastWifiStatus > delay) {
		lastWifiStatus = now;
		sendCommand("wifiConnected()");
		if (wifiConnected && !mqttConnected) {
			initMQTT();
		}
	}
	adjustLedDelay();
}

void handleInformationMQTT(String &status) {
	Serial.print("MQTT state : ");
	Serial.println(status);
	bool oldMqttConnected = mqttConnected;
	mqttConnected = status.indexOf("1") != -1;
	if (!oldMqttConnected && mqttConnected) {
		moduleMQTTRegister();
	}
	adjustLedDelay();
}

void handleInformationWifi(String &status) {
	Serial.print("wifi state : ");
	Serial.println(status);
	bool oldWifiConnected = wifiConnected;
	wifiConnected = status.indexOf("1") != -1;
	if (!oldWifiConnected && wifiConnected) {
		initMQTT();
	}
	adjustLedDelay();
}

void adjustLedDelay() {
	ledDelay = isConnected() ? 1000 : 100;
}

void handleInformation(String &cmd) {
	int index = cmd.indexOf("\t", 3);
	String what = cmd.substring(2, index);
	what.trim();
	String data = cmd.substring(index + 1);
	data.trim();
	if (what.equalsIgnoreCase("wifi")) {
		handleInformationWifi(data);
	} else if (what.equalsIgnoreCase("mqtt")) {
		handleInformationMQTT(data);
	} else if (what.equalsIgnoreCase("sub")) {
		//subscription ok, nothing to handle
	} else if (what.equalsIgnoreCase("publish")) {
		//publish ok, nothing to handle
	} else {
		Serial.print("Unknown information : ");
		Serial.println(cmd);
	}
}

void handleTopicReponse(String &cmd) {
	int index = cmd.indexOf("\t", 3);
	String topic = cmd.substring(2, index);
	topic.trim();
	String data = cmd.substring(index + 1);
	data.trim();

	int indexOfArduinoName = topic.indexOf(ARDUINO_NAME)+sizeof(ARDUINO_NAME);
	int indexOfEndItem = topic.indexOf("/", indexOfArduinoName);
	String item = topic.substring(indexOfArduinoName-1, indexOfEndItem);

	moduleItemReceived(item, data);
}
