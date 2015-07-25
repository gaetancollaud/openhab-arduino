bool utilIsON(String &data) {
	if (data.equalsIgnoreCase("ON")) {
		return true;
	} else if (data.equalsIgnoreCase("OFF")) {
		return false;
	} else {
		moduleStream->print("Unknown ON/OFF value '");
		moduleStream->print(data);
	}
	return false;
}