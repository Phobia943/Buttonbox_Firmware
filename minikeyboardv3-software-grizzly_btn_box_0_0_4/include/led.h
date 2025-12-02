//
// Created by Simon on 29.10.2023.
//

#pragma once

#include <cstdint>
#include <utility>
#include <vector>
#include "config.h"
#include <map>
#include <Adafruit_NeoPixel.h>
#include <string>

//#define RAINBOW_MODE

typedef union{
    uint32_t colorPacked;
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t white;
    };
}neoPixelColor_t;

class OutputLightDevice{
public:
    struct entityCombo_s{
        uint8_t entity;
        uint8_t subEntity;
        // those operators are needed for the working as key in a map
        bool operator==(const entityCombo_s& other) const{
            return this->entity == other.entity && this->subEntity == other.subEntity;
        }
        bool operator<(const entityCombo_s& other) const{
            return this->entity < other.entity || (this->entity == other.entity && this->subEntity < other.subEntity);
        }
    };
    typedef struct entityCombo_s entityCombo_t;

    typedef enum{
        red,
        green,
        blue,
        white,
        yellow,
        off,
        any,
        noColor
    }ledColor_t;

protected:
    uint8_t brightness = 255;
    bool isValid = false;
    ledColor_t activeColor = noColor;
    ledColor_t inactiveColor = noColor;

public:
    static ledColor_t getColorFromString(const std::string& color) {
        if (color == "red") return red;
        if (color == "green") return green;
        if (color == "blue") return blue;
        if (color == "yellow") return yellow;
        if (color == "white") return white;
        if (color == "any") return any;
        if (color == "noColor") return noColor;
        if (color == "off") return off;
        return noColor;
    }

    static std::string getStringFromColor(const ledColor_t color){
        switch(color){
            case red:
                return "red";
            case green:
                return "green";
            case blue:
                return "blue";
            case yellow:
                return "yellow";
            case white:
                return "white";
            case off:
                return "off";
            case any:
                return "any";
            case noColor:
                return "noColor";
            default:
                return "";
        }
    }

    [[maybe_unused]] [[nodiscard]] uint8_t getBrightness() const{ return this->brightness; }

    [[maybe_unused]] [[nodiscard]] bool getIsValid() const { return this->isValid; }

    virtual ~OutputLightDevice() = default;
    virtual void begin() = 0;

    [[maybe_unused]] virtual void setInactiveColor(ledColor_t color) = 0;
    [[maybe_unused]] virtual void setActiveColor(ledColor_t color) = 0;
    [[maybe_unused]] virtual void setBrightness(uint8_t val) = 0;
    [[maybe_unused]] virtual void setKeyToInactiveColor(uint8_t entity, uint8_t subEntity) = 0;
    [[maybe_unused]] virtual void setKeyToActiveColor(uint8_t entity, uint8_t subEntity) = 0;
    [[maybe_unused]] virtual void setKeyToColor(uint8_t entity, uint8_t subEntity, ledColor_t color) = 0;
    [[maybe_unused]] virtual void setAllToColor(ledColor_t color) = 0;
    [[maybe_unused]] virtual void setIdToColor(uint8_t id, ledColor_t color, bool dontSend) = 0;

    [[maybe_unused]] virtual void cyclic() = 0;

protected:
    virtual void update() = 0;

};

typedef std::map<OutputLightDevice::entityCombo_t, std::vector<uint8_t>> entityToIdMap_t;

typedef struct{
    const uint16_t numOfLeds;
    const uint16_t numOfBrightLeds;
    const int16_t pin;
    const neoPixelType type;
    const uint8_t defaultBrightness;
    const OutputLightDevice::ledColor_t defaultActiveColor;
    const OutputLightDevice::ledColor_t defaultInactiveColor;
    entityToIdMap_t entityToIdMap;
}lightOutDeviceNeopixelConf_t;

typedef struct{
    const uint8_t pinClk;
    const uint8_t pinDataOut;
    const uint8_t pinDataWriteCmd;
    const uint8_t pinOE;
    const uint8_t channelsActive;
    const uint8_t invertChannels;
    const uint8_t order[8];
    const uint8_t defaultBrightness;
    entityToIdMap_t entityToIdMap;
}outputDeviceSSROutConf_t;

class LED: public OutputLightDevice{
private:
    std::map<ledColor_t, uint32_t> colorConversionTable = {
            {red, Adafruit_NeoPixel::Color(255, 0, 0)},
            {green, Adafruit_NeoPixel::Color(0, 255, 0)},
            {blue, Adafruit_NeoPixel::Color(0, 0, 255)},
            {white, Adafruit_NeoPixel::Color(255, 255, 255)},
            {yellow, Adafruit_NeoPixel::Color(255, 255, 0)},
            {off, Adafruit_NeoPixel::Color(0, 0, 0)},
    };
    lightOutDeviceNeopixelConf_t conf;
    Adafruit_NeoPixel* strip;

public:
    explicit LED(lightOutDeviceNeopixelConf_t conf):
        conf(std::move(conf)),
        strip(new Adafruit_NeoPixel(this->conf.numOfLeds, this->conf.pin, this->conf.type))
    {
        this->activeColor = this->conf.defaultActiveColor;
        this->inactiveColor = this->conf.defaultInactiveColor;
        this->brightness = this->conf.defaultBrightness;
    }

    void begin() override{
        this->strip->begin();
        this->setBrightness(this->brightness);
        this->setAllToColor(this->inactiveColor);
    }

    void setAllToColor(ledColor_t color) override{
        this->strip->fill(this->colorConversionTable[color]);
        for (uint8_t i = this->conf.numOfLeds - this->conf.numOfBrightLeds; i < this->conf.numOfLeds; i++) {
            this->strip->setPixelColor(i, adjustBrightLedColor(this->strip->getPixelColor(i)));
        }
        this->strip->setBrightness(this->brightness);
        delay(10);
        this->strip->show();
    }

    void setInactiveColor(ledColor_t color) override {
        this->inactiveColor = color;
    }

    void setActiveColor(ledColor_t color) override {
        this->activeColor = color;
    }

    void setBrightness(uint8_t val) override {
        this->brightness = val;
        this->strip->setBrightness(this->brightness);
        this->strip->show();
    }

    void setKeyToInactiveColor(uint8_t entity, uint8_t subEntity) override {
        this->setKeyToColor(entity, subEntity, this->inactiveColor);
    }

    void setKeyToActiveColor(uint8_t entity, uint8_t subEntity) override {
        this->setKeyToColor(entity, subEntity, this->activeColor);
    }

    void setKeyToColor(uint8_t entity, uint8_t subEntity, ledColor_t color) override {
        if (this->conf.entityToIdMap.find({entity, subEntity}) == this->conf.entityToIdMap.end()) return;
        for (auto id: this->conf.entityToIdMap[{entity, subEntity}]){
            this->setIdToColor(id, color, true);
        }
        this->update();
    }

    void setIdToColor(uint8_t id, ledColor_t color, bool dontSend) override {
        if (id >= this->conf.numOfLeds - this->conf.numOfBrightLeds){
            this->strip->setPixelColor(id, adjustBrightLedColor(this->colorConversionTable[color]));
        } else {
            this->strip->setPixelColor(id, this->colorConversionTable[color]);
        }
        this->strip->setBrightness(this->brightness);
        if (!dontSend)
            this->strip->show();
    }

    void cyclic() override{
        static unsigned long lastTime = 0;
#ifdef RAINBOW_MODE
        static uint16_t lastHue = 0;
        if (millis() - lastTime > 10){
            lastTime = millis();
            this->strip->rainbow(lastHue, 1, 255, 255, true);
            lastHue += 100;
            this->strip->show();
        }
#endif
    }

private:
    void update() override{
        this->strip->show();
    }

    static uint32_t adjustBrightLedColor(uint32_t colorIn){
        neoPixelColor_t tmp;
        tmp.colorPacked = colorIn;
        tmp.red >>= 2;
        tmp.green >>= 2;
        tmp.blue >>= 2;
        tmp.white >>= 2;
        return tmp.colorPacked;
     }


};

class ShiftRegisterOut: public OutputLightDevice{
private:
    outputDeviceSSROutConf_t conf;
    uint8_t dataBuf = 0;
    bool firstTime = true;
    const uint8_t numOfChannels = 8;

public:
    explicit ShiftRegisterOut(outputDeviceSSROutConf_t conf): conf(std::move(conf)){
        this->activeColor = noColor;
        this->inactiveColor = any;
        this->brightness = this->conf.defaultBrightness;
    }

    void begin() override{
        pinMode(conf.pinClk, OUTPUT);
        pinMode(conf.pinDataOut, OUTPUT);
        pinMode(conf.pinDataWriteCmd, OUTPUT);
        pinMode(conf.pinOE, OUTPUT);
        digitalWrite(conf.pinOE, HIGH);
        digitalWrite(conf.pinDataWriteCmd, LOW);
        this->setBrightness(this->brightness);
        this->setAllToColor(this->inactiveColor);
    }

    void setInactiveColor(ledColor_t color) override{
        if (color != noColor){
            this->inactiveColor = any;
            this->activeColor = noColor;
        } else {
            this->inactiveColor = noColor;
            this->activeColor = any;
        }
    }

    void setActiveColor(ledColor_t color) override{
        if (color == noColor){
            this->inactiveColor = any;
            this->activeColor = noColor;
        } else {
            this->inactiveColor = noColor;
            this->activeColor = any;
        }
    }

    void setBrightness(uint8_t val) override{
        this->brightness = brightness;
        analogWrite(this->conf.pinOE, 255 - brightness);
    }

    void setKeyToInactiveColor(uint8_t entity, uint8_t subEntity) override{
        this->setKeyToColor(entity, subEntity, this->inactiveColor);
    }

    void setKeyToActiveColor(uint8_t entity, uint8_t subEntity) override{
        this->setKeyToColor(entity, subEntity, this->activeColor);
    }

    void setKeyToColor(uint8_t entity, uint8_t subEntity, ledColor_t color) override{
        if (this->conf.entityToIdMap.find({entity, subEntity}) == this->conf.entityToIdMap.end()) return;
        for (auto id: this->conf.entityToIdMap[{entity, subEntity}])
            this->setIdToColor(id, color, false);
    }

    void setAllToColor(ledColor_t color) override{
        if (color == noColor){
            this->dataBuf = 0;
        } else {
            this->dataBuf = 0xFF & this->conf.channelsActive;
        }
        this->update();
    }

    void setIdToColor(uint8_t id, ledColor_t color, bool dontSend) override{
        if (id >= this->numOfChannels) return;
        if (color == noColor){
            this->dataBuf &= ~(1 << id);
        } else {
            this->dataBuf |= 1 << id;
        }
        if (!dontSend)
            this->update();
    }

    void cyclic() override{

    }

private:
    void update() override{
        uint8_t data_corrected = this->dataBuf ^ this->conf.invertChannels;
        data_corrected &= this->conf.channelsActive;

        uint8_t data_ordered = 0;
        for (uint8_t i = 0; i < 8; i++){
            if (data_corrected & (1 << i)){
                data_ordered |= (1 << this->conf.order[i]);
            }
        }

        for (uint8_t i = 0; i < 8; i++){
            digitalWrite(this->conf.pinClk, LOW);
            delayMicroseconds(100);
            digitalWrite(this->conf.pinDataOut, (data_ordered & (1 << i)) ? HIGH : LOW);
            delayMicroseconds(100);
            digitalWrite(this->conf.pinClk, HIGH);
            delayMicroseconds(100);
        }
        digitalWrite(this->conf.pinDataWriteCmd, HIGH);
        delayMicroseconds(100);
        digitalWrite(this->conf.pinDataWriteCmd, LOW);
        if (this->firstTime){
            digitalWrite(this->conf.pinOE, LOW);
            this->firstTime = false;
        }
    }
};
































