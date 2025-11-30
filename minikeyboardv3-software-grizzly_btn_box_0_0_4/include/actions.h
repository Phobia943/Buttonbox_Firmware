//
// Created by Simon on 28.10.2023.
//

#pragma once

#include <cstdint>
#include "usb.h"
#include "buttons.h"
#include "led.h"
#include <string>
#include <ArduinoJson.h>
#include <algorithm>
#include <Arduino.h>

class Action{
public:
    typedef enum{
        writeAction,
        delayAction,
        ledAction,
        repeatAction,
        keyAction,
        mouseAction,
        templateAction,
        conditionalChangeAction,
        conditionAction,
        undefinedAction
    }actionType_t;
protected:
    bool isValid = false;
    actionType_t type = undefinedAction;

public:
    Action()= default;
    virtual ~Action()= default;
    [[nodiscard]] bool getIsValid() const{ return this->isValid; }

    virtual void parse(JsonObjectConst conf) = 0;

    virtual void execute(USB& usb, std::vector<OutputLightDevice*>& leds, uint8_t entity, uint8_t subEntity, bool value, int32_t value_alt) const = 0;

    [[nodiscard]] virtual std::string toString() const = 0;
    [[maybe_unused]] [[nodiscard]] actionType_t getType() const { return this->type; }

};

static std::map<std::string, std::vector<Action*>> templateActions;
static std::map<std::string, bool> conditionals;

class ActionFactory{
public:
    ActionFactory() = default;
    static Action* parseSingleAction(JsonObjectConst conf);
    static void parseTemplates(JsonObject & conf);
    static std::vector<Action*>* getTemplate(const std::string& name){
        if (templateActions.find(name) != templateActions.end()){
            return &templateActions[name];
        }
        return nullptr;
    }
};

class ConditionalChangeAction: public Action{
private:
    typedef enum{
        conditionCommandSet,
        conditionCommandUnset,
        conditionCommandToggle,
        conditionCommandUndefined
    }conditionCommand_t;
    std::string conditionName;
    conditionCommand_t command = conditionCommandUndefined;
    int stateLedId = -1;

public:
    ConditionalChangeAction() = default;
    void parse(JsonObjectConst conf) override{
        if (conf.containsKey("conditional")){
            this->conditionName = conf["conditional"]["name"].as<std::string>();
            std::string tmp = conf["conditional"]["newState"].as<std::string>();
            if (tmp == "set") this->command = conditionCommandSet;
            if (tmp == "unset") this->command = conditionCommandUnset;
            if (tmp == "toggle") this->command = conditionCommandToggle;
            if (this->command != conditionCommandUndefined) this->isValid = true;
            if (conf["conditional"].containsKey("stateLed")){
                this->stateLedId = conf["conditional"]["stateLed"].as<int>();
            }
        }
        this->type = conditionalChangeAction;
    }

    void execute(USB& usb, std::vector<OutputLightDevice*>& leds, uint8_t entity, uint8_t subEntity, bool value, int32_t value_alt) const override{
        if (this->command == conditionCommandSet) conditionals[this->conditionName] = true;
        if (this->command == conditionCommandUnset) conditionals[this->conditionName] = false;
        if (this->command == conditionCommandToggle) conditionals[this->conditionName] = !conditionals[this->conditionName];
        Serial.print("Conditional "); Serial.print(this->conditionName.c_str()); Serial.print(" is now "); Serial.println(conditionals[this->conditionName]);
        if (this->stateLedId != -1){
            for (auto& led : leds){
                led->setIdToColor(this->stateLedId, conditionals[this->conditionName] ? OutputLightDevice::ledColor_t::green : OutputLightDevice::ledColor_t::red, false);
            }
        }
    }

    [[nodiscard]] std::string toString() const override{
        std::string tmp = "ConditionalChangeAction: ";
        tmp += this->conditionName;
        tmp += ", command: ";
        if (this->command == conditionCommandSet) tmp += "set";
        if (this->command == conditionCommandUnset) tmp += "unset";
        if (this->command == conditionCommandToggle) tmp += "toggle";
        return tmp;
    }
};

class ConditionAction: public Action{
private:
    std::string conditionName;
    std::vector<Action*> actionsTrue;
    std::vector<Action*> actionsFalse;

public:
    ConditionAction() = default;
    void parse(JsonObjectConst conf) override{
        if (conf.containsKey("ifCondition")){
            auto config = conf["ifCondition"].as<JsonObjectConst>();
            this->conditionName = config["conditional"].as<std::string>();
            if (config.containsKey("true")){
                for (auto action : config["true"].as<JsonArrayConst>()){
                    Action *parsedAction = ActionFactory::parseSingleAction(action.as<JsonObjectConst>());
                    if (parsedAction != nullptr){
                        this->actionsTrue.push_back(parsedAction);
                    }
                }
            }
            if (config.containsKey("false")){
                for (auto action : config["false"].as<JsonArrayConst>()){
                    Action *parsedAction = ActionFactory::parseSingleAction(action.as<JsonObjectConst>());
                    if (parsedAction != nullptr){
                        this->actionsFalse.push_back(parsedAction);
                    }
                }
            }
            this->isValid = true;
        }
        this->type = conditionAction;
    }

    void execute(USB& usb, std::vector<OutputLightDevice*>& leds, uint8_t entity, uint8_t subEntity, bool value, int32_t value_alt) const override{
        if (conditionals.find(this->conditionName) == conditionals.end()){
            conditionals[this->conditionName] = false;
            Serial.print("Condition "); Serial.print(this->conditionName.c_str()); Serial.print(" not found..."); Serial.println("Creating and setting to false");
        }
        if (conditionals[this->conditionName]){
            for (auto action : this->actionsTrue){
                action->execute(usb, leds, entity, subEntity, value, value_alt);
            }
        } else {
            for (auto action : this->actionsFalse){
                action->execute(usb, leds, entity, subEntity, value, value_alt);
            }
        }
    }

    [[nodiscard]] std::string toString() const override{
        std::string tmp = "ConditionAction: ";
        tmp += this->conditionName;
        tmp += "\ntrue:\n";
        for (auto action : this->actionsTrue){
            tmp += action->toString();
            tmp += "\n\t";
        }
        tmp += "\nfalse:\n";
        for (auto action : this->actionsFalse){
            tmp += action->toString();
            tmp += "\n\t";
        }
        return tmp;
    }
};

class WriteAction: public Action{
private:
    KeyReportData* report = nullptr;

public:
    WriteAction() = default;

    void parse(JsonObjectConst conf) override{
        if (conf.containsKey("write")) {
            this->report = new KeyReportData(conf["write"].as<std::string>(), "", KeyReportData::reportActionTypeWrite);
            this->isValid = true;
        }
        this->type = writeAction;
    }

    void execute(USB& usb, std::vector<OutputLightDevice*>& leds, const uint8_t entity, const uint8_t subEntity, const bool value, const int32_t value_alt) const override{
        usb.execute(this->report);
    }

    [[nodiscard]] std::string toString() const override{
        std::string tmp = "WriteAction: ";
        tmp += this->report->toString();
        return tmp;
    }
};

class DelayAction: public Action{
private:
    uint32_t delayTime = 0;

public:
    DelayAction() = default;

    void parse(JsonObjectConst conf) override{
        if (conf.containsKey("wait")) {
            this->delayTime = conf["wait"].as<uint32_t>();
            this->isValid = true;
        }
        this->type = delayAction;
    }

    void execute(USB& usb, std::vector<OutputLightDevice*>& leds, const uint8_t entity, const uint8_t subEntity, const bool value, const int32_t value_alt) const override{
        delay(this->delayTime);
    }

    [[nodiscard]] std::string toString() const override{
        std::string tmp = "DelayAction: ";
        tmp += std::to_string(this->delayTime);
        tmp += "ms";
        return tmp;
    }
};

class LEDAction: public Action{
private:
    OutputLightDevice::ledColor_t color = OutputLightDevice::ledColor_t::noColor;
    bool setActive = false;
    bool setInactive = false;

public:
    LEDAction() = default;
    void parse(JsonObjectConst conf) override{
        if (conf.containsKey("led")) {
            std::string colorString = conf["led"].as<std::string>();
            if (colorString.find("inactive") != std::string::npos) this->setInactive = true;
            if (colorString.find("active") != std::string::npos && !this->setInactive) this->setActive = true;
            this->color = OutputLightDevice::getColorFromString(colorString);
        }
        if ((this->color != LED::ledColor_t::noColor) && !this->setActive && !this->setInactive) this->isValid = true;
        this->type = ledAction;
    }

    void execute(USB& usb, std::vector<OutputLightDevice*>& leds, const uint8_t entity, const uint8_t subEntity, const bool value, const int32_t value_alt) const override{
        for (auto& led : leds){
            if (this->setActive) led->setKeyToActiveColor(entity, subEntity);
            if (this->setInactive) led->setKeyToInactiveColor(entity, subEntity);
            if (!this->setActive && !this->setInactive) led->setKeyToColor(entity, subEntity, this->color);
        }
    }

    [[nodiscard]] std::string toString() const override{
        std::string tmp = "LEDAction: ";
        tmp += LED::getStringFromColor(this->color);
        tmp += ", setActive: ";
        tmp += std::to_string(this->setActive);
        tmp += ", setInactive: ";
        tmp += std::to_string(this->setInactive);
        return tmp;
    }
};

class RepeatAction: public Action{
private:
    uint8_t repeatCount = 0;
    uint32_t interval = 0;
    Action* action = nullptr;

public:
    RepeatAction() = default;
    void parse(JsonObjectConst conf) override{
        JsonObjectConst actionConf = conf["repeat"];
        if (actionConf.containsKey("times")){
            this->repeatCount = actionConf["times"].as<uint8_t>();
        }
        if (actionConf.containsKey("interval")){
            this->interval = actionConf["interval"].as<uint32_t>();
        }
        if (actionConf.containsKey("action")){
            this->action = ActionFactory::parseSingleAction(actionConf["action"]);
        }
        if (this->repeatCount > 0 && this->interval > 0 && this->action != nullptr) this->isValid = true;
        this->type = repeatAction;
    }

    void execute(USB& usb, std::vector<OutputLightDevice*>& leds, const uint8_t entity, const uint8_t subEntity, const bool value, const int32_t value_alt) const override{
        for (uint8_t i = 0; i < this->repeatCount; i++){
            this->action->execute(usb, leds, entity, subEntity, value, value_alt);
            delay(this->interval);
        }
    }

    [[nodiscard]] std::string toString() const override{
        std::string tmp = "RepeatAction: ";
        tmp += std::to_string(this->repeatCount);
        tmp += " times, interval: ";
        tmp += std::to_string(this->interval);
        tmp += "ms, action: ";
        if (this->action != nullptr)
            tmp += this->action->toString();
        else
            tmp += "nullptr";
        return tmp;
    }
};

class TemplateAction: public Action{
private:
    std::string templateName;
    std::vector<Action*>* actions = nullptr;

public:
    TemplateAction() = default;
    void parse(JsonObjectConst conf) override{
        if (conf.containsKey("template")){
            this->templateName = conf["template"].as<std::string>();
            this->actions = ActionFactory::getTemplate(this->templateName);
            if (this->actions != nullptr) this->isValid = true;
        }
        this->type = templateAction;
    }

    void execute(USB& usb, std::vector<OutputLightDevice*>& leds, const uint8_t entity, const uint8_t subEntity, const bool value, const int32_t value_alt) const override{
        if (this->actions != nullptr){
            for (auto action : *this->actions){
                Serial.print("Executing template action: "); Serial.println(action->toString().c_str());
                action->execute(usb, leds, entity, subEntity, value, value_alt);
            }
        }
    }

    [[nodiscard]] std::string toString() const override{
        std::string tmp = "TemplateAction: ";
        tmp += this->templateName;
        return tmp;
    }
};

// TODO
class MouseAction: public Action{
private:
    MouseReportData* report = nullptr;

public:
    MouseAction() = default;

    void parse(JsonObjectConst conf) override{
        int x, y;
        int8_t wheel;
        uint8_t button;
        bool isAbsolute = false;
        MouseReportData::buttonAction_t buttonAction = MouseReportData::buttonAction_t::mouseButtonActionUndefined;
        std::string buttonString;
        if (conf.containsKey("mouse")){
            JsonObjectConst mouseConf = conf["mouse"];
            if (mouseConf.containsKey("x")){ x = mouseConf["x"].as<int>(); } else { x = 0; }
            if (mouseConf.containsKey("y")){ y = mouseConf["y"].as<int>(); } else { y = 0; }
            if (mouseConf.containsKey("wheel")){ wheel = mouseConf["wheel"].as<int8_t>(); } else { wheel = 0; }
            if (mouseConf.containsKey("absolute")){ isAbsolute = mouseConf["absolute"].as<bool>(); }
            if (mouseConf.containsKey("click") || mouseConf.containsKey("release")){
                if (mouseConf.containsKey("click")) { buttonString = mouseConf["click"].as<std::string>(); buttonAction = MouseReportData::buttonAction_t::mouseButtonActionClick; }
                if (mouseConf.containsKey("press")) { buttonString = mouseConf["press"].as<std::string>(); buttonAction = MouseReportData::buttonAction_t::mouseButtonActionPress; }
                if (mouseConf.containsKey("release")) { buttonString = mouseConf["release"].as<std::string>(); buttonAction = MouseReportData::buttonAction_t::mouseButtonActionRelease; }
            }
            this->report = new MouseReportData(x, y, wheel, buttonString, isAbsolute, buttonAction);
            this->isValid = true;
        }
        this->type = mouseAction;
    }

    void execute(USB& usb, std::vector<OutputLightDevice*>& leds, const uint8_t entity, const uint8_t subEntity, const bool value, const int32_t value_alt) const override{
        usb.execute(this->report);
    }

    [[nodiscard]] std::string toString() const override{
        std::string tmp = "MouseAction: ";
        tmp += this->report->toString();
        return tmp;
    }
};

class KeyAction: public Action{
private:
    KeyReportData* report = nullptr;

public:
    KeyAction() = default;

    void parse(JsonObjectConst conf) override{
        std::string kWord;
        std::string key;
        std::string modifiers;
        KeyReportData::reportKeyActionType_t actionType = KeyReportData::reportActionTypeUndefined;
        if (conf.containsKey("press")){ actionType = KeyReportData::reportActionTypePress; kWord = "press"; }
        if (conf.containsKey("push")){ actionType = KeyReportData::reportActionTypePush;; kWord = "push"; }
        if (conf.containsKey("release")){ actionType = KeyReportData::reportActionTypeRelease; kWord = "release"; }

        if (kWord.empty()) return;
        if (conf[kWord].is<const char*>()){
            key = conf[kWord].as<std::string>();
        } else {
            if (conf[kWord].containsKey("key")) {
                key = conf[kWord]["key"].as<std::string>();
            }
            if (conf[kWord].containsKey("modifier")){
                modifiers = conf[kWord]["modifier"].as<std::string>();
            }
        }
        if (key.empty()) return;
        this->report = new KeyReportData(key, modifiers, actionType);
        this->isValid = true;
        this->type = keyAction;
    }

    void execute(USB& usb, std::vector<OutputLightDevice*>& leds, const uint8_t entity, const uint8_t subEntity, const bool value, const int32_t value_alt) const override{
        if (!this->isValid) return;
        if (!usb.execute(this->report)){
            Serial.println("Error executing key action");
        }
    }

    [[nodiscard]] std::string toString() const override{
        std::string tmp = "Keyaction: ";
        tmp += this->report->toString();
        return tmp;
    }
};

typedef std::vector<Action*> actionArray_t;

typedef struct{
    actionArray_t press;
    actionArray_t release;
    actionArray_t increase;
    actionArray_t decrease;
    uint8_t entity;
    uint8_t sub_id;
} action_t;
