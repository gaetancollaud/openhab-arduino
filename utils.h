


bool utilIsOn(String &data){
	return data.equalsIgnoreCase("On");
}
bool utilIsOff(String &data){
	return data.equalsIgnoreCase("Off");
}

bool utilIsOnWithCheck(String &data) {
	if (utilIsOn(data)) {
		return true;
	} else if (utilIsOff(data)) {
		return false;
	} else {
		moduleStream->print("Unknown ON/OFF value '");
		moduleStream->print(data);
	}
	return false;
}