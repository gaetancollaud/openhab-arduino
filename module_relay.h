#include <arduino.h>

void moduleRelayCallback(String &topic, String &value);

void moduleRelayLoad() {
#ifdef MODULE_RELAY_PIN1
	pinMode(MODULE_RELAY_PIN1, OUTPUT);
	digitalWrite(MODULE_RELAY_PIN1, HIGH);
	moduleTopicRegister("Relay1", (void*) moduleRelayCallback);
#endif
#ifdef MODULE_RELAY_PIN2
	pinMode(MODULE_RELAY_PIN2, OUTPUT);
	digitalWrite(MODULE_RELAY_PIN2, HIGH);
	moduleTopicRegister("Relay2", (void*) moduleRelayCallback);
#endif
}

void moduleRelayCallback(String &item, String &value) {
	moduleStream->print("relay received : ");
	moduleStream->print(item);
	moduleStream->print(" = ");
	moduleStream->println(value);

	int relayNumber = item.substring(5).toInt();
	int pinValue = utilIsON(value) ? LOW : HIGH;
	
	switch(relayNumber){
		case 1:
			digitalWrite(MODULE_RELAY_PIN1, pinValue);
			break;
		case 2:
			digitalWrite(MODULE_RELAY_PIN2, pinValue);
			break;
	};
}

