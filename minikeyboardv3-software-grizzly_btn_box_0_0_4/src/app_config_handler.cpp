//
// Created by Simon on 20.01.2024.
//
#include "app_config_handler.h"
#include <LittleFS.h>

#include <utility>

std::string conf = "{\n"
                   "    \"version\": 1.1,\n"
                   "    \"entities\": {\n"
                   "        \"FunkySwitch1\": 0,\n"
                   "        \"FunkySwitch2\": 1,\n"
                   "        \"encoder1\": 2,\n"
                   "        \"encoder2\": 3,\n"
                   "        \"encoder3\": 4,\n"
                   "        \"ssr1\": 7\n"
                   "    },\n"
                   "    \"sub_entities\": {\n"
                   "        \"FunkySwitch1\": {\n"
                   "            \"Up\": 0,\n"
                   "            \"Left\": 1,\n"
                   "            \"Down\": 2,\n"
                   "            \"Right\": 3,\n"
                   "            \"Center\": 4,\n"
                   "            \"Encoder\": 5\n"
                   "        },\n"
                   "        \"FunkySwitch2\": {\n"
                   "            \"Up\": 0,\n"
                   "            \"Left\": 1,\n"
                   "            \"Down\": 2,\n"
                   "            \"Right\": 3,\n"
                   "            \"Center\": 4,\n"
                   "            \"Encoder\": 5\n"
                   "        }\n"
                   "    },\n"
                   "    \"pages\": [{\n"
                   "        \"FunkySwitch1\": {\n"
                   "            \"Up\": {\n"
                   "                \"press\": [{\"press\": \"G0\"}],\n"
                   "                \"release\": [{\"release\": \"G0\"}]\n"
                   "            },\n"
                   "            \"Left\": {\n"
                   "                \"press\": [{\"press\": \"G1\"}],\n"
                   "                \"release\": [{\"release\": \"G1\"}]\n"
                   "            },\n"
                   "            \"Down\": {\n"
                   "                \"press\": [{\"press\": \"G2\"}],\n"
                   "                \"release\": [{\"release\": \"G2\"}]\n"
                   "            },\n"
                   "            \"Right\": {\n"
                   "                \"press\": [{\"press\": \"G3\"}],\n"
                   "                \"release\": [{\"release\": \"G3\"}]\n"
                   "            },\n"
                   "            \"Center\": {\n"
                   "                \"press\": [{\"press\": \"G4\"}],\n"
                   "                \"release\": [{\"release\": \"G4\"}]\n"
                   "            },\n"
                   "            \"Encoder\": {\n"
                   "                \"increase\": [{\"push\": \"G5\"}],\n"
                   "                \"decrease\": [{\"push\": \"G6\"}]\n"
                   "            }\n"
                   "        },\n"
                   "        \"FunkySwitch2\": {\n"
                   "            \"Up\": {\n"
                   "                \"press\": [{\"press\": \"G7\"}],\n"
                   "                \"release\": [{\"release\": \"G7\"}]\n"
                   "            },\n"
                   "            \"Left\": {\n"
                   "                \"press\": [{\"press\": \"G8\"}],\n"
                   "                \"release\": [{\"release\": \"G8\"}]\n"
                   "            },\n"
                   "            \"Down\": {\n"
                   "                \"press\": [{\"press\": \"G9\"}],\n"
                   "                \"release\": [{\"release\": \"G9\"}]\n"
                   "            },\n"
                   "            \"Right\": {\n"
                   "                \"press\": [{\"press\": \"G10\"}],\n"
                   "                \"release\": [{\"release\": \"G10\"}]\n"
                   "            },\n"
                   "            \"Center\": {\n"
                   "                \"press\": [{\"press\": \"G11\"}],\n"
                   "                \"release\": [{\"release\": \"G11\"}]\n"
                   "            },\n"
                   "            \"Encoder\": {\n"
                   "                \"increase\": [{\"push\": \"G12\"}],\n"
                   "                \"decrease\": [{\"push\": \"G13\"}]\n"
                   "            }\n"
                   "        },\n"
                   "        \"encoder1\": {\n"
                   "            \"0\": {\n"
                   "                \"increase\": [{\"push\": \"G14\"}],\n"
                   "                \"decrease\": [{\"push\": \"G15\"}]\n"
                   "            }\n"
                   "        },\n"
                   "        \"encoder2\": {\n"
                   "            \"0\": {\n"
                   "                \"increase\": [{\"push\": \"G16\"}],\n"
                   "                \"decrease\": [{\"push\": \"G17\"}]\n"
                   "            }\n"
                   "        },\n"
                   "        \"encoder3\": {\n"
                   "            \"0\": {\n"
                   "                \"increase\": [{\"push\": \"G18\"}],\n"
                   "                \"decrease\": [{\"push\": \"G19\"}]\n"
                   "            }\n"
                   "        },\n"
                   "        \"ssr1\": {\n"
                   "            \"0\": {\n"
                   "                \"press\": [{\"press\": \"G20\"}],\n"
                   "                \"release\": [{\"release\": \"G20\"}]\n"
                   "            },\n"
                   "            \"1\": {\n"
                   "                \"press\": [{\"press\": \"G21\"}],\n"
                   "                \"release\": [{\"release\": \"G21\"}]\n"
                   "            },\n"
                   "            \"2\": {\n"
                   "                \"press\": [{\"press\": \"G22\"}],\n"
                   "                \"release\": [{\"release\": \"G22\"}]\n"
                   "            },\n"
                   "            \"3\": {\n"
                   "                \"press\": [{\"press\": \"G23\"}],\n"
                   "                \"release\": [{\"release\": \"G23\"}]\n"
                   "            },\n"
                   "            \"4\": {\n"
                   "                \"press\": [{\"press\": \"G24\"}],\n"
                   "                \"release\": [{\"release\": \"G24\"}]\n"
                   "            },\n"
                   "            \"5\": {\n"
                   "                \"press\": [{\"press\": \"G25\"}],\n"
                   "                \"release\": [{\"release\": \"G25\"}]\n"
                   "            },\n"
                   "            \"6\": {\n"
                   "                \"press\": [{\"press\": \"G26\"}],\n"
                   "                \"release\": [{\"release\": \"G26\"}]\n"
                   "            },\n"
                   "            \"7\": {\n"
                   "                \"press\": [{\"press\": \"G27\"}],\n"
                   "                \"release\": [{\"release\": \"G27\"}]\n"
                   "            }\n"
                   "        }\n"
                   "    }]\n"
                   "}";



// TODO make this a little easier to read
SinglePageAppConfigHandler::SinglePageAppConfigHandler(JsonObject& configPage, JsonObject& entityNames, JsonObject& subEntityNames, std::string pageName, const bool generateDefaultLed /* = true */):
    pageName(std::move(pageName)){
    for (auto entry : configPage){                       // iterate over all entities e.g. KeyMatrix, FunkySwitch, ...
        uint8_t entity_id = 0;
        std::string entity_name;
        if (entityNames.containsKey(entry.key())) {
            entity_id = entityNames[entry.key()].as<int>();
            entity_name = entry.key().c_str();
            Serial.print("Found Entity: ");
            Serial.print(entry.key().c_str());
            Serial.print(" with ID: ");
            Serial.println(entity_id);
        } else {
            char* end_ptr;
            entity_id = strtol(entry.key().c_str(), &end_ptr, 10);
            if (end_ptr == entry.key().c_str()){
                Serial.print("Error while parsing entity (skipping: ");
                Serial.println(entry.key().c_str());
                continue;
            }
        }
        auto sub_entities = entry.value().as<JsonObject>();
        for (auto sub_entity : sub_entities){   // iterate over all sub entities e.g. 0, 1, 2, 3...
            uint8_t sub_entity_id = 0;
            bool sub_entity_good = false;
            if (!entity_name.empty()){
                if (subEntityNames.containsKey(entity_name.c_str())) {
                    if (subEntityNames[entity_name.c_str()].containsKey(sub_entity.key().c_str())){
                        sub_entity_id = subEntityNames[entity_name.c_str()][sub_entity.key().c_str()].as<int>();
                        sub_entity_good = true;
                        Serial.print("Found Sub Entity: "); Serial.print(sub_entity_id); Serial.print(" for Entity: "); Serial.println(sub_entity.key().c_str());
                    }
                }
            }
            if (!sub_entity_good){
                char *end_ptr;
                sub_entity_id = strtol(sub_entity.key().c_str(), &end_ptr, 10);
                if (end_ptr == sub_entity.key().c_str()){
                    Serial.print("Error while parsing sub entity (skipping: ");
                    Serial.println(sub_entity.key().c_str());
                    continue;
                }
                Serial.print("Found Sub Entity: "); Serial.println(sub_entity_id);
            }
            action_t temp = {
                    .entity = entity_id,
                    .sub_id = sub_entity_id,
            };
            for (auto action_trigger : sub_entity.value().as<JsonObject>()) { // iterate over all triggers e.g. press, release, increase, decrease
                std::string trigger_word = action_trigger.key().c_str();
                Serial.print("Found Trigger: "); Serial.println(trigger_word.c_str());
                actionArray_t* array_ptr = nullptr;
                if (trigger_word == "press"){ array_ptr = &temp.press; Serial.println("Press Confirmed"); }
                if (trigger_word == "release"){ array_ptr = &temp.release; Serial.println("Release Confirmed"); }
                if (trigger_word == "increase"){ array_ptr = &temp.increase; Serial.println("Increase Confirmed"); }
                if (trigger_word == "decrease"){ array_ptr = &temp.decrease; Serial.println("Decrease Confirmed"); }
                if (array_ptr == nullptr) continue;
                for (auto action : action_trigger.value().as<JsonArray>()) { // iterate over all actions e.g. push, wait, led, write
                    Action *tmp = ActionFactory::parseSingleAction(action);
                    if (tmp){
                        array_ptr->push_back(tmp);
                        Serial.println(tmp->toString().c_str());
                    }
                }
            }
            if (generateDefaultLed){
                SinglePageAppConfigHandler::handleDefaultLed(temp);
            }
            this->actions.push_back(temp);
        }
    }
}

SinglePageAppConfigHandler::~SinglePageAppConfigHandler() {
    for (const auto& action : this->actions){
        for (auto a : action.press){
            delete a;
        }
        for (auto a : action.release){
            delete a;
        }
        for (auto a : action.increase){
            delete a;
        }
        for (auto a : action.decrease){
            delete a;
        }
    }
}

void SinglePageAppConfigHandler::handleActions(USB &usb, std::vector<OutputLightDevice *> &leds, InputDeviceType_t type,
                                               uint8_t entity, uint8_t subId, bool value, int32_t value_alt) {
    for (auto& action : this->actions){
        if (action.entity == entity && action.sub_id == subId){
            actionArray_t* array_ptr = nullptr;
            if (type == INPUT_DEVICE_ENCODER && value)  array_ptr = &action.increase;
            if (type == INPUT_DEVICE_ENCODER && !value) array_ptr = &action.decrease;
            if (type != INPUT_DEVICE_ENCODER && value)  array_ptr = &action.press;
            if (type != INPUT_DEVICE_ENCODER && !value) array_ptr = &action.release;


            if (array_ptr == nullptr) continue;
            for (auto a : *array_ptr){
//                Serial.print("-----------------------Executing: entity: ");
//                Serial.print(entity); Serial.print(", subEntity: ");
//                Serial.print(subId); Serial.print(", value: ");
//                Serial.print(value); Serial.print(", value_alt: ");
//                Serial.print(value_alt); Serial.println(" :----------------------------------------");
//                Serial.println(a->toString().c_str());
                try{
                    a->execute(usb, leds, entity, subId, value, value_alt);
                } catch (std::exception& e) {
                    Serial.print("Exception while executing action: ");
                    Serial.println(e.what());
                } catch (...) {
                    Serial.println("Unknown exception while executing action!");
                }
            }
            break;
        }
    }
}

void SinglePageAppConfigHandler::handleDefaultLed(action_t &action) {
    bool found = false;
    for (auto& a : action.press){ if (a->getType() == Action::actionType_t::ledAction) found = true; }
    if (!found){
        Action *tmp = new LEDAction();
        JsonDocument doc;
        doc["led"] = "active";
        tmp->parse(doc.as<JsonObject>());
        action.press.insert(action.press.begin(), tmp);
    }
    found = false;
    for (auto& a : action.release){ if (a->getType() == Action::actionType_t::ledAction) found = true; }
    if (!found){
        Action *tmp = new LEDAction();
        JsonDocument doc;
        doc["led"] = "inactive";
        tmp->parse(doc.as<JsonObject>());
        action.release.insert(action.release.begin(), tmp);
    }
}

AppConfigHandler::AppConfigHandler() = default;

AppConfigHandler::~AppConfigHandler() {
    for (auto page : this->pages){
        delete page;
    }
    if (this->littleFSStarted)
        LittleFS.end();
}

bool AppConfigHandler::initLittleFS() {
    if (this->littleFSStarted) return true;

    LittleFSConfig cfg;
    cfg.setAutoFormat(false);
    LittleFS.setConfig(cfg);
    if (!LittleFS.begin()){
        Serial.println("LittleFS failed to start!");
        return false;
    }
    this->littleFSStarted = true;
    return true;
}

void AppConfigHandler::handleActions(USB &usb, std::vector<OutputLightDevice *> &leds, InputDeviceType_t type,
                                     uint8_t entity, uint8_t subId, bool value, int32_t value_alt) {
    if (this->currentPage < this->pages.size()){
        this->pages.at(this->currentPage)->handleActions(usb, leds, type, entity, subId, value, value_alt);
    }
}

bool AppConfigHandler::begin() {
    if (this->littleFSStarted) return true;
    return this->initLittleFS() && this->loadDefaultVals();
}

bool AppConfigHandler::load(const std::string& filenameOverride) {
//    if (!this->littleFSStarted){
//        Serial.println("Cannot load from LittleFS LittleFS is not started");
//        return false;
//    }
//    std::string filename = filenameOverride.empty() ? this->currentFileName : filenameOverride;
//    File configFile = LittleFS.open(filename.c_str(), "r");
//    if (!configFile){
//        Serial.println("Could not open config file!");
//        return false;
//    }
//    Serial.println("Config file opened successfully!");
    DeserializationError err;
    JsonDocument jsonConf;
    err = deserializeJson(jsonConf, conf);
    //configFile.close();
    if (err){
        Serial.print("Error while parsing config file: ");
        Serial.println(err.c_str());
        return false;
    }
    auto root = jsonConf.as<JsonObject>();
    JsonObject entityNames = root["entities"].as<JsonObject>();
    JsonObject subEntityNames = root["sub_entities"].as<JsonObject>();
    JsonObject templates = root["templates"].as<JsonObject>();
    ActionFactory::parseTemplates(templates);
    for (auto page : root["pages"].as<JsonArray>()){
        JsonObject pageObject = page.as<JsonObject>();
        this->pages.push_back(new SinglePageAppConfigHandler(pageObject, entityNames, subEntityNames, page["name"].as<std::string>()));
    }
    return true;
}

bool AppConfigHandler::loadDefaultVals(){
    if (!this->littleFSStarted){
        Serial.println("Cannot load default values LittleFS is not started");
        return false;
    }
    File default_config = LittleFS.open(INIT_VAL_FILE, "r");
    if (!default_config){
        Serial.println("Could not open default config file!");
        return false;
    }
    this->currentFileName = default_config.readStringUntil('\n').c_str();
    Serial.print("Default File Name: "); Serial.println(this->currentFileName.c_str());
    return true;
}

bool AppConfigHandler::periodic(InterfaceProvider* interfaceProvider) {
    auto result = interfaceProvider->update();
    switch(result){
        case InterfaceProvider::ResultChangeConfig:
            this->handleChangeConfig(interfaceProvider);
            break;
        case InterfaceProvider::ResultChangeDefaultConfig:
            this->handleChangeDefaultConfig(interfaceProvider);
            break;
        case InterfaceProvider::ResultReadNewConfig:
            this->handleReadNewConfig(interfaceProvider);
            break;
        case InterfaceProvider::ResultReload:
            this->handleReload(interfaceProvider);
            break;
        case InterfaceProvider::ResultPrintCurrentConfig:
            this->handlePrintCurrentConfig(interfaceProvider);
            break;
        case InterfaceProvider::ResultNothing:      // fall through
        default:
            break;
    }
    return false;
}

bool AppConfigHandler::handleReload(InterfaceProvider* interfaceProvider){
    (void*)interfaceProvider;
    for (auto page : this->pages){
        delete page;
    }
    this->pages.clear();
    return this->load();
}

bool AppConfigHandler::handleChangeConfig(InterfaceProvider *interfaceProvider) {
    this->currentFileName = interfaceProvider->getLastConfFileName();
    return true;
}

bool AppConfigHandler::handleChangeDefaultConfig(InterfaceProvider *interfaceProvider) {
    this->handleChangeConfig(interfaceProvider);
    if (!this->littleFSStarted){
        Serial.println("Cannot change default config LittleFS is not started");
        return false;
    }
    File default_config = LittleFS.open(INIT_VAL_FILE, "w");
    if (!default_config){
        Serial.println("Could not open default config file!");
        return false;
    }
    default_config.write(this->currentFileName.c_str());
    default_config.close();
    return true;
}

bool AppConfigHandler::handleReadNewConfig(InterfaceProvider *interfaceProvider) {
    std::string tmp;
    Serial.println("Reading new config file");
    File new_config = LittleFS.open(this->currentFileName.c_str(), "w");
    if (!new_config){
        Serial.println("Could not open new config file!");
        return false;
    }
    while(tmp.find("__START__") == std::string::npos){
        interfaceProvider->readLine(tmp);
    }
    Serial.print("Found Start. Starting to Read file to: "); Serial.println(this->currentFileName.c_str());
    while(true){
        interfaceProvider->readLine(tmp);
        if (tmp.find("__END__") != std::string::npos){
            new_config.write("\n");
            break;
        }
        tmp.erase( remove(tmp.begin(), tmp.end(), '\r'), tmp.end() );
        new_config.write(tmp.c_str());
        Serial.print(": "); Serial.println(tmp.c_str());
    }
    Serial.println("End");
    new_config.close();
    return true;
}

bool AppConfigHandler::handlePrintCurrentConfig(InterfaceProvider *interfaceProvider) {
    (void*)interfaceProvider;
    File configFile = LittleFS.open(this->currentFileName.c_str(), "r");
    if (!configFile){
        Serial.println("Could not open config file!");
        return false;
    }
    Serial.println("Config file opened successfully!");
    while(configFile.available()){
        Serial.print(configFile.readStringUntil('\n').c_str());
    }
    configFile.close();
    return true;
}








































