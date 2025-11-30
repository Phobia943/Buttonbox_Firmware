//
// Created by Simon on 11.02.2024.
// Modified for SimHub Integration
//

#include "InterfaceProvider.h"
#include <Arduino.h>
#include "algorithm"

SerialInterfaceProvider::SerialInterfaceProvider() {
    connected = true;
    inputBuffer.reserve(64); // Reserve some memory to avoid frequent reallocations
}

InterfaceProvider::interfaceUpdateResult_t SerialInterfaceProvider::update() {
    while (Serial.available()) {
        char c = (char)Serial.read();
        if (c == '\n') {
            // Process complete line
            std::string line = inputBuffer;
            inputBuffer.clear();

            // Remove Carriage Return if present (SimHub might send \r\n)
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            if (line.empty()) continue;

            // --- System Commands ---
            if (line.find("__REBOOT__") != std::string::npos){
                rp2040.reboot();
            }
            else if (line.find("__BOOTLOADER__") != std::string::npos){
                rp2040.rebootToBootloader();
            }
            else if (line.find("__NEW_CONFIG__") != std::string::npos){
                return ResultReadNewConfig;
            }
            else if (line.find("__RELOAD__") != std::string::npos){
                return ResultReload;
            }
            else if (line.find("__PRINT_CONFIG__") != std::string::npos){
                return ResultPrintCurrentConfig;
            }
            else if (line.find("__CHANGE_CONFIG_FILE__") != std::string::npos || line.find("__CHANGE_DEFAULT_CONFIG__") != std::string::npos){
                // Note: This remains blocking, which is acceptable for maintenance commands
                this->lastConfFileName = Serial.readStringUntil('\n').c_str();
                this->lastConfFileName.erase( remove(this->lastConfFileName.begin(), this->lastConfFileName.end(), '\r'), this->lastConfFileName.end() );
                this->lastConfFileName.erase( remove(this->lastConfFileName.begin(), this->lastConfFileName.end(), '\n'), this->lastConfFileName.end() );
                if (line.find("__CHANGE_CONFIG_FILE__") != std::string::npos){
                    return ResultChangeConfig;
                } else {
                    return ResultChangeDefaultConfig;
                }
            }
            // --- SimHub / Custom Commands ---
            else {
                processSimHubCommand(line);
            }

        } else {
            // Add character to buffer
            inputBuffer += c;
        }
    }
    return ResultNothing;
}

void SerialInterfaceProvider::processSimHubCommand(const std::string& line) {
    // SimHub Protocol: KEY=VALUE
    size_t separatorPos = line.find('=');
    if (separatorPos == std::string::npos) {
        // Optional: Handle commands without value
        return;
    }

    std::string key = line.substr(0, separatorPos);
    std::string value = line.substr(separatorPos + 1);

    // TODO: Implement your hardware control logic here
    // Example:
    // if (key == "RPM") {
    //     int rpm = std::stoi(value);
    //     // setLeds(rpm);
    // }
}

bool SerialInterfaceProvider::readLine(std::string &line) {
    line = "";
    while (line.empty()) {
        line = Serial.readStringUntil('\n').c_str();
    }
    return true;
}

std::string SerialInterfaceProvider::getNewConfigName() {
    return "";
}