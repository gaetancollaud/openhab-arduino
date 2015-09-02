#include <arduino.h>

#define MODULE_AMP_METER_HALF_HIGH 512

unsigned long moduleAmpMeterDelay;
unsigned long moduleAmpMeterNextTime;
long moduleAmpMeterSum;
int moduleAmpMeterCount;
int moduleAmpMeterCountLimit;

String AMP_METER_TOPIC = "/openhab/in/" ARDUINO_NAME "Amp1/state";

//void moduleAmpMeterCallback(String &topic, String &value);

void moduleAmpMeterLoad() {
	//  1s/(Hz*2)
	moduleAmpMeterCountLimit = MODULE_AMP_METER_HZ;
	moduleAmpMeterDelay = 1000 / moduleAmpMeterCountLimit;
	moduleAmpMeterDelay *= 1000;
	moduleAmpMeterNextTime = micros();
	moduleAmpMeterSum = 0;
	moduleAmpMeterCount = 0;
}

void moduleAmpMeterComputeAndSend() {
	if (mqttConnected) {
		double amp = moduleAmpMeterSum;
		amp /= moduleAmpMeterCountLimit;
		amp /= MODULE_AMP_METER_HALF_HIGH;
		amp *= MODULE_AMP_METER_MAX_AMP;
		mqttPublish(AMP_METER_TOPIC, String(amp, 3));
	}
}

void moduleAmpMeterLoop(unsigned long *now) {
	if (*now > moduleAmpMeterNextTime) {
		moduleAmpMeterNextTime += moduleAmpMeterDelay;
		moduleAmpMeterSum += abs(analogRead(MODULE_AMP_METER_PIN) - MODULE_AMP_METER_HALF_HIGH);
		moduleAmpMeterCount++;
		if (moduleAmpMeterCount >= moduleAmpMeterCountLimit) {
			moduleAmpMeterComputeAndSend();
			moduleAmpMeterCount = 0;
			moduleAmpMeterSum = 0;
		}

	}
}
