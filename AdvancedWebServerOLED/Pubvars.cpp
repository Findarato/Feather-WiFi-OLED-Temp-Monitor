/**
 * Variable Defines for Feather OLED Temp
 * Created by Joseph D. Harry, January 31, 2017.
 * Released GPL 3
 */

#include "Arduino.h"
#include "Pubvars.h"

const char* Varstore::readSSID(){
    return _ssid;
}

const char* Varstore::readPassword(){
    return _password;
}

const char* Varstore::readDeviceName(){
    return _deviceName;
}

const int Varstore::readDeviceID(){
    return _deviceID;
}

const int Varstore::readButtonPinIP(){
    return _buttonPinIP;
}

const int Varstore::readButtonPinTemp(){
    return _buttonPinTemp;
}

const int Varstore::readServerPort(){
    return _webServerPort;
}
