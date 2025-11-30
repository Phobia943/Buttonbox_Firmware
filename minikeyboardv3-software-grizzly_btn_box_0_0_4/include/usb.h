//
// Created by Simon on 28.10.2023.
//

#pragma once

#include <string>
#include <vector>
#include "Arduino.h"
#include <Keyboard.h>
#include <Mouse.h>
#include <Joystick.h>
#include <algorithm>

#define MOUSE_INCREMENT 10

class BaseReportData{
public:
    typedef enum{
        REPORT_TYPE_KEYBOARD,
        REPORT_TYPE_MOUSE,
        REPORT_TYPE_CONSUMER,
        REPORT_TYPE_GAMEPAD,
        REPORT_TYPE_MOUSE_ABSOLUTE,
        REPORT_TYPE_UNDEFINED
    }reportType_t;
protected:
    reportType_t type = REPORT_TYPE_UNDEFINED;
public:
    BaseReportData() = default;
    virtual ~BaseReportData() = default;
    virtual std::string toString() = 0;
    [[nodiscard]] reportType_t getType() const { return this->type; }
};

class KeyReportData : public BaseReportData{
public:
    typedef enum{
        reportActionTypePush,
        reportActionTypeRelease,
        reportActionTypePress,
        reportActionTypeWrite,
        reportActionTypeUndefined
    }reportKeyActionType_t;

    typedef enum{
        reportDeviceTypeKey,
        reportDeviceTypeText,
        reportDeviceTypeConsumer,
        reportDeviceTypeGamepad,
        reportDeviceTypeUndefined
    }reportDeviceType_t;

private:
    std::vector<uint8_t> modifiers;
    uint16_t keycode;
    std::string writeText;
    reportKeyActionType_t actionType = reportActionTypeUndefined;
    reportDeviceType_t deviceType = reportDeviceTypeUndefined;

public:
    KeyReportData(const std::string& keyText, const std::string& modifiersText, reportKeyActionType_t actionType){
        this->type = REPORT_TYPE_KEYBOARD;
        this->actionType = actionType;
        this->modifiers = KeyReportData::getModifiersFromString(modifiersText);
        if (actionType == reportActionTypeWrite){
            this->writeText = keyText;
            this->deviceType = reportDeviceTypeText;
            return;
        }
        if (keyText.length() == 1){
            this->keycode = (uint16_t)keyText.at(0);
            this->deviceType = reportDeviceTypeKey;
        } else {
            this->keycode = this->getSpecialKeyFromString(keyText);
        }
    }

    [[nodiscard]] reportKeyActionType_t getActionType() const { return this->actionType; }
    [[nodiscard]] std::vector<uint8_t> getModifiers() const { return this->modifiers; }
    [[nodiscard]] uint16_t getKeyCode() const { return this->keycode; }
    [[nodiscard]] std::string getWriteText() const { return this->writeText; }
    [[nodiscard]] reportDeviceType_t getDeviceType() const { return this->deviceType; }
    [[nodiscard]] bool isConsumer() const { return this->deviceType == reportDeviceTypeConsumer; }
    [[nodiscard]] bool isGamepad() const { return this->deviceType == reportDeviceTypeGamepad; }
    [[nodiscard]] bool isKey() const { return this->deviceType == reportDeviceTypeKey; }
    [[nodiscard]] bool isText() const { return this->deviceType == reportDeviceTypeText; }


    // TODO: Translate codes to a string
    std::string toString() override{
        std::string tmp = "KeyReportData: ";
        if (this->actionType == reportActionTypePush){
            tmp += "Push ";
        } else if (this->actionType == reportActionTypeRelease){
            tmp += "Release ";
        } else if (this->actionType == reportActionTypePress){
            tmp += "Press ";
        } else if (this->actionType == reportActionTypeWrite){
            tmp += "Write ";
        } else {
            tmp += "Unknown ";
        }
        if (this->deviceType == reportDeviceTypeKey){
            tmp += "Key ";
        }
        if (this->deviceType == reportDeviceTypeConsumer){
            tmp += "Consumer ";
        }
        if (this->deviceType == reportDeviceTypeGamepad){
            tmp += "Gamepad ";
        }
        if (this->deviceType == reportDeviceTypeUndefined){
            tmp += "Undefined! ";
        }
        tmp += std::to_string(this->keycode);
        tmp += " with modifiers: ";
        for (auto modifier : this->modifiers){
            tmp += std::to_string(modifier);
            tmp += " ";
        }
        return tmp;
    }

private:
    [[nodiscard]] static std::vector<uint8_t> getModifiersFromString(const std::string& in) {
        std::vector<uint8_t> result;
        size_t last = 0;
        size_t next;
        while((next = in.find('+', last)) != std::string::npos){
            result.push_back(KeyReportData::getSingleModifierFromString(in.substr(last, next - last)));
            last = next + 1;
        }
        result.push_back(KeyReportData::getSingleModifierFromString(in.substr(last)));
        return result;
    }

    [[nodiscard]] static uint8_t getSingleModifierFromString(const std::string& in) {
        std::string local = in;
        std::transform(local.begin(), local.end(), local.begin(),[](unsigned char c){ return std::tolower(c); });
        if (local == "ctrl") { /*Serial.println("LEFT CTRL");*/ return KEY_LEFT_CTRL; }
        if (local == "alt")  { /*Serial.println("LEFT ALT");*/ return KEY_LEFT_ALT; }
        if (local == "shift") { /*Serial.println("LEFT SHIFT");*/ return KEY_LEFT_SHIFT; }
        if (local == "win") { /*Serial.println("LEFT WIN");*/ return KEY_LEFT_GUI; }
        if (local == "rctrl") { /*Serial.println("RIGHT CTRL");*/ return KEY_RIGHT_CTRL; }
        if (local == "ralt") { /*Serial.println("RIGHT ALT");*/ return KEY_RIGHT_ALT; }
        if (local == "rshift") { /*Serial.println("RIGHT SHIFT");*/ return KEY_RIGHT_SHIFT; }
        if (local == "rwin") { /*Serial.println("RIGHT WIN");*/ return KEY_RIGHT_GUI; }
        return 0;
    }

    [[nodiscard]] uint16_t getSpecialKeyFromString(const std::string& in){
        std::string local = in;
        std::transform(local.begin(), local.end(), local.begin(),[](unsigned char c){ return std::tolower(c); });
        this->deviceType = reportDeviceTypeKey;
        if (local == "tab") return KEY_TAB;
        if (local == "capslock") return KEY_CAPS_LOCK;
        if (local == "backspace") return KEY_BACKSPACE;
        if (local == "enter") return KEY_RETURN;
        if (local == "menu") return KEY_MENU;
        if (local == "space") return ' ';

        if (local == "win") return KEY_LEFT_GUI;
        if (local == "insert") return KEY_INSERT;
        if (local == "delete") return KEY_DELETE;
        if (local == "pageup") return KEY_PAGE_UP;
        if (local == "pagedown") return KEY_PAGE_DOWN;
        if (local == "home") return KEY_HOME;
        if (local == "end") return KEY_END;
        if (local == "up") return KEY_UP_ARROW;
        if (local == "down") return KEY_DOWN_ARROW;
        if (local == "left") return KEY_LEFT_ARROW;
        if (local == "right") return KEY_RIGHT_ARROW;

        if (local == "numlock") return KEY_NUM_LOCK;
        if (local == "kp/") return KEY_KP_SLASH;
        if (local == "kp*") return KEY_KP_ASTERISK;
        if (local == "kp-") return KEY_KP_MINUS;
        if (local == "kp+") return KEY_KP_PLUS;
        if (local == "kpenter") return KEY_KP_ENTER;
        if (local == "kp1") return KEY_KP_1;
        if (local == "kp2") return KEY_KP_2;
        if (local == "kp3") return KEY_KP_3;
        if (local == "kp4") return KEY_KP_4;
        if (local == "kp5") return KEY_KP_5;
        if (local == "kp6") return KEY_KP_6;
        if (local == "kp7") return KEY_KP_7;
        if (local == "kp8") return KEY_KP_8;
        if (local == "kp9") return KEY_KP_9;
        if (local == "kp0") return KEY_KP_0;
        if (local == "kp.") return KEY_KP_DOT;

        if (local == "escape") return KEY_ESC;
        if (local == "f1") return KEY_F1;
        if (local == "f2") return KEY_F2;
        if (local == "f3") return KEY_F3;
        if (local == "f4") return KEY_F4;
        if (local == "f5") return KEY_F5;
        if (local == "f6") return KEY_F6;
        if (local == "f7") return KEY_F7;
        if (local == "f8") return KEY_F8;
        if (local == "f9") return KEY_F9;
        if (local == "f10") return KEY_F10;
        if (local == "f11") return KEY_F11;
        if (local == "f12") return KEY_F12;
        if (local == "f13") return KEY_F13;
        if (local == "f14") return KEY_F14;
        if (local == "f15") return KEY_F15;
        if (local == "f16") return KEY_F16;
        if (local == "f17") return KEY_F17;
        if (local == "f18") return KEY_F18;
        if (local == "f19") return KEY_F19;
        if (local == "f20") return KEY_F20;
        if (local == "f21") return KEY_F21;
        if (local == "f22") return KEY_F22;
        if (local == "f23") return KEY_F23;
        if (local == "f24") return KEY_F24;

        if (local == "printscreen") return KEY_PRINT_SCREEN;
        if (local == "scrolllock") return KEY_SCROLL_LOCK;
        if (local == "pause") return KEY_PAUSE;

        this->deviceType = reportDeviceTypeConsumer;
        if (local == "playpause") return KEY_PLAY_PAUSE;
        if (local == "stop") return KEY_STOP;
        if (local == "next") return KEY_SCAN_NEXT;
        if (local == "prev") return KEY_SCAN_PREVIOUS;
        if (local == "mute") return KEY_MUTE;
        if (local == "volup") return KEY_VOLUME_INCREMENT;
        if (local == "voldown") return KEY_VOLUME_DECREMENT;
        if (local == "calculator") return KEY_AL_CALCULATOR;

        if (local.at(0) == 'g' && local.length() > 1){
            this->deviceType = reportDeviceTypeGamepad;
            return std::stoi(local.substr(1)) + 1;  // when using arduino usb gamepad library, the first button is 1
        }
        this->deviceType = reportDeviceTypeUndefined;
        return 0;
    }
};

class MouseReportData: public BaseReportData {
public:
    typedef enum {
        mouseButtonActionPress,
        mouseButtonActionRelease,
        mouseButtonActionClick,
        mouseButtonActionUndefined
    } buttonAction_t;

private:
    int x;
    int y;
    int8_t wheel;
    uint8_t buttons;
    bool isAbsolute = false;
    buttonAction_t buttonAction = mouseButtonActionUndefined;

public:
    explicit MouseReportData(int x = 0,
                             int y = 0,
                             int8_t wheel = 0,
                             const std::string& buttonString = "",
                             bool isAbsolute = false,
                             buttonAction_t buttonAction = mouseButtonActionUndefined){
        this->type = REPORT_TYPE_MOUSE;
        this->x = x;
        this->y = y;
        this->wheel = wheel;
        this->buttonAction = buttonAction;
        this->buttons = 0;
        this->isAbsolute = isAbsolute;
        if (buttonString.find("left") != std::string::npos){ this->buttons |= MOUSE_LEFT; }
        if (buttonString.find("right") != std::string::npos){ this->buttons |= MOUSE_RIGHT; }
        if (buttonString.find("middle") != std::string::npos){ this->buttons |= MOUSE_MIDDLE; }
//      TODO implement this or check if available at all
//      if (tmp.find("forward") != std::string::npos) button |= MOUSE_BUTTON_FORWARD;
//      if (tmp.find("backward") != std::string::npos) button |= MOUSE_BUTTON_BACKWARD;
    }

    [[nodiscard]] int getX() const { return this->x; }
    [[nodiscard]] int getY() const { return this->y; }
    [[nodiscard]] int8_t getWheel() const { return this->wheel; }
    [[nodiscard]] uint8_t getButtons() const { return this->buttons; }
    [[nodiscard]] bool getIsAbsolute() const { return this->isAbsolute; }
    [[nodiscard]] buttonAction_t getButtonAction() const { return this->buttonAction; }

    std::string toString() override{
        std::string tmp = "MouseReportData: ";
        tmp += "X: ";
        tmp += std::to_string(this->x);
        tmp += " Y: ";
        tmp += std::to_string(this->y);
        tmp += " Wheel: ";
        tmp += std::to_string(this->wheel);
        tmp += " Buttons: ";
        tmp += std::to_string(this->buttons);
        tmp += " ButtonAction: ";
        if (this->buttonAction == mouseButtonActionPress){
            tmp += "Press ";
        } else if (this->buttonAction == mouseButtonActionRelease){
            tmp += "Release ";
        } else if (this->buttonAction == mouseButtonActionClick){
            tmp += "Click ";
        } else {
            tmp += "Unknown ";
        }
        return tmp;
    }
};

class USB{
public:


private:
    bool enabled = false;
    bool beginCalled = false;

public:
    USB();
    void begin();
    void enable();
    void disable();

    [[nodiscard]] bool execute(const std::vector<BaseReportData*>& reportData) const;
    bool execute(BaseReportData* reportData) const;

    // TODO those two will probably not be const in the future!
    bool executeKeyboard(const KeyReportData* keyReportData) const;
    bool executeMouse(const MouseReportData* reportData) const;
};
