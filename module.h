
bool isConnected();
void mqttPublish(String topic, String data);
void mqttSubsribe(String topic);
void moduleTopicRegister(String topic, const void *callback);
Stream *moduleStream;

#include "utils.h"

#ifdef MODULE_DHT11
#include "module_dht11.h"
#endif

#ifdef MODULE_RELAY
#include "module_relay.h"
#endif

#ifdef MODULE_LED_STRIP
#include "module_led_strip.h"
#endif

typedef void (*MODULE_TOPIC_CALLBACK)(String, String);

typedef struct {
	String item;
	MODULE_TOPIC_CALLBACK callback;
} ModuleItem;

ModuleItem moduleItems[5];
char moduleItemsIndex = 0;

void moduleTopicRegister(String item, const void *callback) {
	ModuleItem *mt = &moduleItems[moduleItemsIndex++];
	mt->item = item;
	mt->callback = *((MODULE_TOPIC_CALLBACK*) (&callback));

}

void moduleLoad(Stream* stream) {
	moduleStream = stream;
#ifdef MODULE_DHT11
	stream->println("Load module DHT11");
#endif
#ifdef MODULE_RELAY
	moduleRelayLoad();
	stream->println("Load module Relay");
#endif
#ifdef MODULE_LED_STRIP
	moduleLedStripLoad();
	stream->println("Load module LedStrip");
#endif
}

void moduleMQTTRegister() {
	for (int i = 0; i < moduleItemsIndex; i++) {
		ModuleItem *mt = &moduleItems[i];
		mqttSubsribe("/openhab/out/" ARDUINO_NAME "" + mt->item + "/command");
	}
}

void moduleLoop() {
#ifdef MODULE_DHT11
	moduleDHT11Loop();
#endif
#ifdef MODULE_LED_STRIP
	moduleLedStripLoop();
#endif
}

void moduleItemReceived(String &item, String &value){
	for (int i = 0; i < moduleItemsIndex; i++) {
		ModuleItem *mt = &moduleItems[i];
		if(mt->item.equals(item)){
			mt->callback(item, value);
			return;
		}
	}
	moduleStream->print("Unknown item : ");
	moduleStream->println(item);
}