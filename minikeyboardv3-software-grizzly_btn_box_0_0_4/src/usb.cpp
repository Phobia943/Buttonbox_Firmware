//
// Created by Simon on 28.10.2023.
//

#include "usb.h"
#include <Keyboard.h>
#include <Mouse.h>
#include <Joystick.h>

USB::USB() = default;

void USB::begin(){
    Keyboard.begin();
    Mouse.begin();
    Joystick.begin();
    this->enabled = true;
    this->beginCalled = true;
}

bool USB::execute(const std::vector<BaseReportData *> &reportData) const {
    for (auto data : reportData){
        if (!this->execute(data)){
            return false;
        }
    }
    return true;
}

bool USB::execute(BaseReportData *reportData) const {
    if (!this->enabled) return false;
    if (reportData->getType() == BaseReportData::reportType_t::REPORT_TYPE_KEYBOARD){
        auto keyReportData = dynamic_cast<KeyReportData*>(reportData);
        if (keyReportData == nullptr) return false;
        return this->executeKeyboard(keyReportData);
    }
    if (reportData->getType() == BaseReportData::reportType_t::REPORT_TYPE_MOUSE || reportData->getType() == BaseReportData::reportType_t::REPORT_TYPE_MOUSE_ABSOLUTE){
        auto mouseReportData = dynamic_cast<MouseReportData*>(reportData);
        if (mouseReportData == nullptr) return false;
        return this->executeMouse(mouseReportData);
    }
    return false;
}

void USB::enable() {
    if (this->beginCalled)
        this->enabled = true;
}

void USB::disable() {
    this->enabled = false;
    Keyboard.releaseAll();
    Mouse.release(MOUSE_ALL);
    Keyboard.consumerRelease();
    // TODO do joystick
}

bool USB::executeKeyboard(const KeyReportData *keyReportData) const {
    if (!this->enabled) return false;
    if (keyReportData->isKey()){
        if (keyReportData->getActionType() == KeyReportData::reportKeyActionType_t::reportActionTypePush){
            bool modifier = false;
            if (!keyReportData->getModifiers().empty()){
                for (auto modifierKey : keyReportData->getModifiers()) {
                    Keyboard.press(modifierKey);
                }
                modifier = true;
            }
            Keyboard.write(keyReportData->getKeyCode());
            if (modifier){
                for (auto modifierKey : keyReportData->getModifiers()) {
                    Keyboard.release(modifierKey);
                }
            }
        }
        if (keyReportData->getActionType() == KeyReportData::reportKeyActionType_t::reportActionTypePress){
            Keyboard.press(keyReportData->getKeyCode());
        }
        if (keyReportData->getActionType() == KeyReportData::reportKeyActionType_t::reportActionTypeRelease){
            Keyboard.release(keyReportData->getKeyCode());
        }
    }
    if (keyReportData->isConsumer()){
        if (keyReportData->getActionType() == KeyReportData::reportKeyActionType_t::reportActionTypePush){
            Keyboard.consumerPress(keyReportData->getKeyCode());
            delay(20);
            Keyboard.consumerRelease();
        }
        if (keyReportData->getActionType() == KeyReportData::reportKeyActionType_t::reportActionTypePress){
            Keyboard.consumerPress(keyReportData->getKeyCode());
        }
        if (keyReportData->getActionType() == KeyReportData::reportKeyActionType_t::reportActionTypeRelease){
            Keyboard.consumerRelease();
        }
    }
    if (keyReportData->isGamepad()){
        if (keyReportData->getActionType() == KeyReportData::reportKeyActionType_t::reportActionTypePush){
            Joystick.button(keyReportData->getKeyCode(), true);
            delay(20);
            Joystick.button(keyReportData->getKeyCode(), false);
        }
        if (keyReportData->getActionType() == KeyReportData::reportKeyActionType_t::reportActionTypePress){
            Joystick.button(keyReportData->getKeyCode(), true);
        }
        if (keyReportData->getActionType() == KeyReportData::reportKeyActionType_t::reportActionTypeRelease){
            Joystick.button(keyReportData->getKeyCode(), false);
        }
    }
    if (keyReportData->isText()){
        Keyboard.print(keyReportData->getWriteText().c_str());
    }
    return true;
}

// TODO build my own mouse absolute with blackjack and hookers
bool USB::executeMouse(const MouseReportData *reportData) const{
    if (!this->enabled) return false;
    if (reportData->getIsAbsolute()){
        // TODO implement absolute mouse
        Serial.println("Absolute mouse not implemented yet, doing a relative move instead.");
    }
    int x = reportData->getX();
    int y = reportData->getY();
    bool done = false;
    while (!done){
        int x_temp = 0;
        int y_temp = 0;
        if (x > 0){
            x_temp = x > MOUSE_INCREMENT ? MOUSE_INCREMENT : x;
            x -= x_temp;
        }
        if (x < 0){
            x_temp = x < -MOUSE_INCREMENT ? -MOUSE_INCREMENT : x;
            x -= x_temp;
        }
        if (y > 0){
            y_temp = y > MOUSE_INCREMENT ? MOUSE_INCREMENT : y;
            y -= y_temp;
        }
        if (y < 0){
            y_temp = y < -MOUSE_INCREMENT ? -MOUSE_INCREMENT : y;
            y -= y_temp;
        }
        Mouse.move(x_temp, y_temp, 0);
        if (x == 0 && y == 0){
            done = true;
        }
    }

    Mouse.move(0, 0, reportData->getWheel());

    if (reportData->getButtonAction() == MouseReportData::buttonAction_t::mouseButtonActionClick){
        Mouse.click(reportData->getButtons());
    }
    if (reportData->getButtonAction() == MouseReportData::buttonAction_t::mouseButtonActionPress){
        Mouse.press(reportData->getButtons());
    }
    if (reportData->getButtonAction() == MouseReportData::buttonAction_t::mouseButtonActionRelease){
        Mouse.release(reportData->getButtons());
    }
    return true;
}
