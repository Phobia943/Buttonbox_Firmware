#include <Arduino.h>
#include "config.h"
#include "buttons.h"
#include <list>
#include "usb.h"
#include "app_config_handler.h"
#include "InterfaceProvider.h"
#include "hw_objects.h"
#if defined (BOARD_TYPE_MINIKEYBOARD)   //only minikeyboard has a display
#include "display.h"
#endif

// upload with pio.exe run -t upload
// workaround as long as CLion Plugin is broken

AppConfigHandler appConfigHandler = AppConfigHandler();
USB usb = USB();
InterfaceProvider* interfaceProvider = new SerialInterfaceProvider();

void device_callback (uint8_t entity, InputDeviceType_t type, uint8_t subId, bool value, int32_t value_alt){
    Serial.print("__INPUT_EVENT__:");
    Serial.print(entity); Serial.print(":");
    Serial.print(subId); Serial.print(":");
    Serial.println(value);
    appConfigHandler.handleActions(usb, lightDevices, type, entity, subId, value, value_alt);
}

void setup() {
    Serial.begin(115200);
    usb.begin();
    usb.enable();
    for (auto device: lightDevices){
        device->begin();
    }
    for (auto device: inputDevices){
        device->setCallback(&device_callback);
    }
    appConfigHandler.begin();
    appConfigHandler.load();
}

void loop() {
    for (auto device : inputDevices){
        device->update();
    }
    for (auto device : lightDevices){
        device->cyclic();
    }
    appConfigHandler.periodic(interfaceProvider);
}
