//
// Created by Simon on 20.01.2024.
//

#pragma once

#define INIT_VAL_FILE "__default_config.txt"

#include "led.h"
#include "usb.h"
#include "buttons.h"
#include "actions.h"
#include <ArduinoJson.h>
#include "InterfaceProvider.h"

class SinglePageAppConfigHandler{
private:
    const std::string pageName;
    std::vector<action_t> actions;

public:
    SinglePageAppConfigHandler(JsonObject& configPage, JsonObject& entityNames, JsonObject& subEntityNames, std::string pageName, bool generateDefaultLed = true);
    ~SinglePageAppConfigHandler();

    void handleActions(USB& usb, std::vector<OutputLightDevice*>& leds, InputDeviceType_t type, uint8_t entity, uint8_t subId, bool value, int32_t value_alt);

private:
    static void handleDefaultLed(action_t& action);

};

class AppConfigHandler{
private:
    std::vector<SinglePageAppConfigHandler*> pages;
    bool littleFSStarted = false;
    std::string currentFileName;
    int currentPage = 0;

public:
    AppConfigHandler();
    ~AppConfigHandler();

    void handleActions(USB& usb, std::vector<OutputLightDevice*>& leds, InputDeviceType_t type, uint8_t entity, uint8_t subId, bool value, int32_t value_alt);
    bool begin();
    bool load(const std::string& filenameOverride = "");
    bool periodic(InterfaceProvider* interfaceProvider);

private:
    bool initLittleFS();
    bool loadDefaultVals();

    bool handleReload(InterfaceProvider* interfaceProvider);
    bool handleChangeConfig(InterfaceProvider* interfaceProvider);
    bool handleChangeDefaultConfig(InterfaceProvider* interfaceProvider);
    bool handleReadNewConfig(InterfaceProvider* interfaceProvider);
    bool handlePrintCurrentConfig(InterfaceProvider* interfaceProvider);
};