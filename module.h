
bool isConnected();
void mqttPublish(String topic, String data);

#ifdef MODULE_DHT11
#include "module_dht11.h"
#endif

void loadModules() {
#ifdef MODULE_DHT11
	load_dht11();
	Serial.println("Load module DHT11");
#endif

}

void unloadModules() {
	//TODO
}

void loopModules() {
#ifdef MODULE_DHT11
	loop_dht11();
#endif
}