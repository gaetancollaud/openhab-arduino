#include <arduino.h>

#include "RunningMedian.h"

#define MEDIAN_SIZE 10

String AMP_METER_AMP_TOPIC = "/openhab/in/" ARDUINO_NAME "Amp1/state";
String AMP_METER_WATT_TOPIC = "/openhab/in/" ARDUINO_NAME "Watt1/state";



RunningMedian moduleAmpMeterMedian = RunningMedian(MEDIAN_SIZE);

float moduleAmpMeterlastValue = -1;
unsigned long moduleAmpMeterDelayCapture = 200000;
unsigned long moduleAmpMeterNextTimeCapture = 0;
unsigned long moduleAmpMeterDelaySend = moduleAmpMeterDelayCapture*MEDIAN_SIZE;
unsigned long moduleAmpMeterNextTimeSend = moduleAmpMeterDelaySend;

float moduleAmpMetergetAmp() {

	int readValue; //value read from the sensor
	int maxValue = 0; // store max value here
	int minValue = 1024; // store min value here

	uint32_t start_time = millis();
	while ((millis() - start_time) <= 20) {
		readValue = analogRead(MODULE_AMP_METER_PIN);
		if (readValue > maxValue) {
			maxValue = readValue;
		}
		if (readValue < minValue) {
			minValue = readValue;
		}
	}

	// Subtract min from max
	// peak = (max-min)/2
	// v = peak*5v/1024
	// rms = v*0.707
	// a = rms*1000/mVperAmp
	// => (max-min)*1.7260/mVperAmp

	return (((maxValue - minValue) * 1.7260) / MODULE_AMP_METER_MV_PER_AMP);
}


//void moduleAmpMeterCallback(String &topic, String &value);

void moduleAmpMeterLoad() {
}

void moduleAmpMeterSendIfChanged() {
	if (mqttConnected) {
		float amp = moduleAmpMeterMedian.getMedian();
		if (amp != moduleAmpMeterlastValue) {
			moduleAmpMeterlastValue = amp;
//			mqttPublish(AMP_METER_AMP_TOPIC, String(amp, 3));
			mqttPublish(AMP_METER_WATT_TOPIC, String(amp * 230, 3));
		}
	}
}

void moduleAmpMeterLoop(unsigned long *now) {
	if (*now > moduleAmpMeterNextTimeCapture) {
		moduleAmpMeterNextTimeCapture += moduleAmpMeterDelayCapture;
		moduleAmpMeterMedian.add(moduleAmpMetergetAmp());
		if (*now > moduleAmpMeterNextTimeSend) {
			moduleAmpMeterNextTimeSend += moduleAmpMeterDelaySend;
			moduleAmpMeterSendIfChanged();
		}
	}
}
