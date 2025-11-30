//
// Created by Simon on 28.10.2023.
//

#include "actions.h"

Action *ActionFactory::parseSingleAction(ArduinoJson::JsonObjectConst conf) {
    Action *result = nullptr;
    if (conf.containsKey("push") || conf.containsKey("press") || conf.containsKey("release")){
        result = new KeyAction();
    }
    if (conf.containsKey("wait")){
        result = new DelayAction();
    }
    if (conf.containsKey("led")){
        result = new LEDAction();
    }
    if (conf.containsKey("write")){
        result = new WriteAction();
    }
    if (conf.containsKey("repeat")){
        result = new RepeatAction();
    }
    if (conf.containsKey("mouse")){
        result = new MouseAction();
    }
    if (conf.containsKey("template")){
        result = new TemplateAction();
    }
    if (conf.containsKey("conditional")){
        result = new ConditionalChangeAction();
    }
    if (conf.containsKey("ifCondition")){
        result = new ConditionAction();
    }
    if (result != nullptr){
        result->parse(conf);
        if (!result->getIsValid()){
            Serial.print("Action is not valid: "); Serial.println(result->toString().c_str());
            delete result;
            result = nullptr;
        }
    }
    return result;
}

void ActionFactory::parseTemplates(JsonObject & conf) {
    // clear template actions
    for (auto & templateElement : templateActions){
        for (auto & templateAction : templateElement.second){
            delete templateAction;
        }
    }
    templateActions.clear();
    for (auto templateElement : conf){
        Serial.print("Parsing template: "); Serial.println(templateElement.key().c_str());
        std::vector<Action*> actions;
        for (auto action : templateElement.value()["actions"].as<JsonArray>()){
            Action *parsedAction = parseSingleAction(action.as<JsonObject>());
            if (parsedAction != nullptr){
                actions.push_back(parsedAction);
                Serial.print("Parsed template action: "); Serial.println(parsedAction->toString().c_str());
            }
        }
        templateActions[templateElement.key().c_str()] = actions;
    }
}








































