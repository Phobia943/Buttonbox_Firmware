//
// Created by Simon on 25.10.2023.
//
#pragma once
#include "Arduino.h"
#include <cstdint>
#include <utility>
#include <config.h>
#include <RotaryEncoder.h>
#include <string>

#define FANCY_SWITCH_SAFETY_SKIP_INTERVAL_MS 10

typedef enum{
    INPUT_DEVICE_BUTTON,
    INPUT_DEVICE_KEYMATRIX,
    INPUT_DEVICE_ENCODER,
    INPUT_DEVICE_FUNKYSWITCH,
    INPUT_DEVICE_SHIFT_REGISTER,
    INPUT_DEVICE_NONE,
}InputDeviceType_t;

typedef void (*inputDeviceCallback)(uint8_t /*entity*/, InputDeviceType_t /*type*/, uint8_t /*id*/, bool /*value*/, int32_t /*value_alt*/);

typedef struct{
    uint8_t entityId;
    uint8_t entitySubId;
    inputDeviceCallback callback;
    std::string niceName;
}inputDeviceBaseConf_t;

typedef struct{
    inputDeviceBaseConf_t baseConf;
    uint32_t debounceTimeMS;
    uint32_t pinNumber;
}inputDeviceButtonConf_t;

typedef struct{
    inputDeviceBaseConf_t baseConf;
    std::vector<uint8_t> rowPins;
    std::vector<uint8_t> columnPins;
}inputDeviceKeyMatrixConf_t;

typedef struct{
    inputDeviceBaseConf_t baseConf;
    uint8_t pinA;
    uint8_t pinB;
    RotaryEncoder::LatchMode latchMode;
}inputDeviceEncoderConf_t;

typedef struct{
    inputDeviceBaseConf_t baseConf;
    uint8_t pinA;
    uint8_t pinB;
    uint8_t pinC;
    uint8_t pinD;
    uint8_t pinBTN;
    uint8_t pinEncA;
    uint8_t pinEncB;
}inputDeviceFunkyConf_t;

typedef struct{
    inputDeviceBaseConf_t baseConf;
    uint8_t pinClk;
    uint8_t pinDataIn;
    uint8_t pinDataRead;
    uint8_t channelsActive;
    uint8_t invertChannels;
    uint8_t order[8];
}inputDeviceSSRInConf_t;

class InputDevice{
protected:
    uint8_t entityId = 0;
    uint8_t entitySubId = 0;
    uint8_t entitySize = 0;
    InputDeviceType_t type = INPUT_DEVICE_NONE;
    inputDeviceCallback callback = nullptr;

public:
    virtual ~InputDevice() = default;
    virtual void update() = 0;
    virtual void setCallback(inputDeviceCallback callback_arg) = 0;

    [[nodiscard]] [[maybe_unused]] bool checkDevice() const{
        if (this->type          == INPUT_DEVICE_NONE)   { return false; }
        if (this->entityId      == 0)                   { return false; }
        if (this->entitySubId   == 0)                   { return false; }
        if (this->entitySize    == 0)                   { return false; }
        if (this->callback      == nullptr)             { return false; }
        return true;
    }

    [[maybe_unused]] [[nodiscard]] uint8_t getEntityId()         const{ return this->entityId; }
    [[maybe_unused]] [[nodiscard]] uint8_t getEntitySubId()      const{ return this->entitySubId; }
    [[maybe_unused]] [[nodiscard]] uint8_t getEntitySize()       const{ return this->entitySize; }
    [[maybe_unused]] [[nodiscard]] InputDeviceType_t getType()   const{ return this->type; }
};

class Button: public InputDevice{
private:
    inputDeviceButtonConf_t const conf;
    bool currentState = false;
    unsigned long lastChange = 0;
    const bool usePin;

public:
    explicit Button(inputDeviceButtonConf_t const& conf) :conf(conf), usePin(conf.pinNumber != 0){
        this->callback = conf.baseConf.callback;
        this->entityId = conf.baseConf.entityId;
        this->entitySubId = conf.baseConf.entitySubId;
        this->type = INPUT_DEVICE_BUTTON;
        if (usePin){
            pinMode(conf.pinNumber, INPUT);
        }
    }

    void setCallback(inputDeviceCallback callback_arg) override{
        this->callback = callback_arg;
    }

    bool update(bool newValue, bool dryrun = false){
        if (dryrun){
            return newValue != this->currentState;
        }
        if (this->lastChange + this->conf.debounceTimeMS < millis() && (newValue != this->currentState)){
            this->lastChange = millis();
            this->currentState = newValue;
            if (this->callback != nullptr){
                this->callback(this->entityId, this->type, this->entitySubId, newValue, newValue);
            }
        }
        return this->currentState;
    }

    void update() override{
        if (usePin){
            this->update(!digitalRead(this->conf.pinNumber));
        }
    }

    bool update_with_return(){
        if (usePin){
            return this->update(!digitalRead(this->conf.pinNumber));
        }
        return false; //wtf
    }

    bool update_dryrun(){
        if (usePin){
            return this->update(!digitalRead(this->conf.pinNumber), true);
        }
        return false;       // you fucked up big time my friend
    }

    [[maybe_unused]] [[nodiscard]] bool getState() const{ return this->currentState; }
};

class KeyMatrix: public InputDevice{
private:
    inputDeviceKeyMatrixConf_t const conf;
    std::vector<Button> buttons;

public:
    explicit KeyMatrix(inputDeviceKeyMatrixConf_t  conf): conf(std::move(conf)){
        this->callback = this->conf.baseConf.callback;
        this->entityId = this->conf.baseConf.entityId;
        this->entitySubId = this->conf.baseConf.entitySubId;
        this->type = INPUT_DEVICE_KEYMATRIX;
        for (auto pin : this->conf.rowPins){
            pinMode(pin, INPUT);
        }
        for (auto pin : this->conf.columnPins){
            pinMode(pin, OUTPUT);
            digitalWrite(pin, HIGH);
        }
        for (uint8_t column = 0; column < (uint8_t)this->conf.columnPins.size(); column++){
            for (uint8_t row = 0; row < (uint8_t)this->conf.rowPins.size(); row++){
                inputDeviceButtonConf_t tmp = {
                        .baseConf = {
                            .entityId = this->entityId,
                            .entitySubId = static_cast<uint8_t>(row+column*3),
                            .callback = this->callback,
                            .niceName = this->conf.baseConf.niceName + " " + std::to_string(row+column*3),
                        },
                        .debounceTimeMS = DEBOUNCE_DEFAULT_TIME_MS,
                        .pinNumber = 0,
                };
                this->buttons.emplace_back(tmp);
            }
        }
    }

    void setCallback(inputDeviceCallback callback_arg) override{
        this->callback = callback_arg;
        for (auto& button : this->buttons){
            button.setCallback(callback_arg);
        }
    }

    void update() override{
        for (uint8_t column = 0; column < (uint8_t)this->conf.columnPins.size(); column++){
            digitalWrite(this->conf.columnPins.at(column), LOW);
            for (uint8_t row = 0; row < (uint8_t)this->conf.rowPins.size(); row++){
                this->buttons.at(column*3 + row).update(!digitalRead(this->conf.rowPins.at(row)));
            }
            digitalWrite(this->conf.columnPins.at(column), HIGH);
        }
    }
};

class Encoder: public InputDevice{
private:
    const inputDeviceEncoderConf_t config;
    RotaryEncoder* encoder;
    int32_t encoderPosition = 0;
    bool skipFirstValue = true;

public:
    explicit Encoder(const inputDeviceEncoderConf_t& config): config(config){
        this->callback = config.baseConf.callback;
        this->entityId = config.baseConf.entityId;
        this->entitySubId = config.baseConf.entitySubId;
        this->type = INPUT_DEVICE_ENCODER;
        pinMode(config.pinA, INPUT);
        pinMode(config.pinB, INPUT);
        this->encoder = new RotaryEncoder(config.pinA, config.pinB, config.latchMode);
    }

    void setCallback(inputDeviceCallback callback_arg) override{
        this->callback = callback_arg;
    }

    void update() override{
        this->encoder->tick();
        int32_t enc = ~this->encoder->getPosition();
        if (enc != this->encoderPosition){
            bool increase = this->encoderPosition < enc;
            this->encoderPosition = enc;
            if (this->callback != nullptr){
                if (this->skipFirstValue){
                    this->skipFirstValue = false;
                    return;
                }
                this->callback(this->entityId, this->type, this->entitySubId, increase, this->encoderPosition);
            }
        }
    }

    [[maybe_unused]] [[nodiscard]] int32_t getLastEncoderPosition() const{ return this->encoderPosition; }
};

class FunkySwitch: public InputDevice{
private:
    const inputDeviceFunkyConf_t config;
    bool safetySkipActive = false;
    Encoder* encoder;
    Button* btnA;
    Button* btnB;
    Button* btnC;
    Button* btnD;
    Button* btnBTN;

public:
    explicit FunkySwitch(const inputDeviceFunkyConf_t& config): config(config){
        this->callback = config.baseConf.callback;
        this->entityId = config.baseConf.entityId;
        this->entitySubId = config.baseConf.entitySubId;
        this->type = INPUT_DEVICE_FUNKYSWITCH;
        this->btnA = new Button({
            .baseConf = {
                .entityId = this->entityId,
                .entitySubId = 0,
                .callback = config.baseConf.callback,
                .niceName = config.baseConf.niceName + " A",
            },
            .debounceTimeMS = DEBOUNCE_DEFAULT_TIME_MS,
            .pinNumber = config.pinA,
        });
        this->btnB = new Button({
            .baseConf = {
                .entityId = this->entityId,
                .entitySubId = 1,
                .callback = config.baseConf.callback,
                .niceName = config.baseConf.niceName + " B",
            },
            .debounceTimeMS = DEBOUNCE_DEFAULT_TIME_MS,
            .pinNumber = config.pinB,
        });
        this->btnC = new Button({
            .baseConf = {
                .entityId = this->entityId,
                .entitySubId = 2,
                .callback = config.baseConf.callback,
                .niceName = config.baseConf.niceName + " C",
            },
            .debounceTimeMS = DEBOUNCE_DEFAULT_TIME_MS,
            .pinNumber = config.pinC,
        });
        this->btnD = new Button({
            .baseConf = {
                .entityId = this->entityId,
                .entitySubId = 3,
                .callback = config.baseConf.callback,
                .niceName = config.baseConf.niceName + " D",
            },
            .debounceTimeMS = DEBOUNCE_DEFAULT_TIME_MS,
            .pinNumber = config.pinD,
        });
        this->btnBTN = new Button({
            .baseConf = {
                .entityId = this->entityId,
                .entitySubId = 4,
                .callback = config.baseConf.callback,
                .niceName = config.baseConf.niceName + " BTN",
            },
            .debounceTimeMS = DEBOUNCE_DEFAULT_TIME_MS,
            .pinNumber = config.pinBTN,
        });
        this->encoder = new Encoder({
            .baseConf = {
                .entityId = this->entityId,
                .entitySubId = 5,
                .callback = config.baseConf.callback,
                .niceName = config.baseConf.niceName + " Encoder",
            },
            .pinA = config.pinEncA,
            .pinB = config.pinEncB,
            .latchMode = RotaryEncoder::LatchMode::TWO03,
        });
    }

    void setCallback(inputDeviceCallback callback_arg) override{
        this->callback = callback_arg;
        this->btnA->setCallback(callback_arg);
        this->btnB->setCallback(callback_arg);
        this->btnC->setCallback(callback_arg);
        this->btnD->setCallback(callback_arg);
        this->btnBTN->setCallback(callback_arg);
        this->encoder->setCallback(callback_arg);
    }

    void update() override{
        uint8_t result = btnA->update_with_return();
        result += btnB->update_with_return();
        result += btnC->update_with_return();
        result += btnD->update_with_return();
        if (!result && btnBTN->update_dryrun() && !this->safetySkipActive){
            delay(FANCY_SWITCH_SAFETY_SKIP_INTERVAL_MS);
            this->safetySkipActive = true;
        } else {
            this->safetySkipActive = false;
        }
        if (!result && !this->safetySkipActive){
            btnBTN->update();
        }

        this->encoder->update();
    }

    [[maybe_unused]] [[nodiscard]] int32_t getLastEncoderPosition() const{ return this->encoder->getLastEncoderPosition(); }
};

class ShiftRegisterIn: public InputDevice{
private:
    const inputDeviceSSRInConf_t config;
    std::vector<Button> buttons;

public:
    explicit ShiftRegisterIn(const inputDeviceSSRInConf_t& config): config(config){
        this->callback = config.baseConf.callback;
        this->entityId = config.baseConf.entityId;
        this->entitySubId = config.baseConf.entitySubId;
        this->type = INPUT_DEVICE_SHIFT_REGISTER;
        pinMode(config.pinClk, OUTPUT);
        pinMode(config.pinDataIn, INPUT);
        pinMode(config.pinDataRead, OUTPUT);
        digitalWrite(config.pinClk, LOW);
        digitalWrite(config.pinDataRead, HIGH);
        for (uint8_t i = 0; i < 8; i++){
            inputDeviceButtonConf_t tmp = {
                    .baseConf = {
                            .entityId = this->entityId,
                            .entitySubId = i,
                            .callback = this->callback,
                            .niceName = config.baseConf.niceName + " " + std::to_string(i),
                    },
                    .debounceTimeMS = DEBOUNCE_DEFAULT_TIME_MS,
                    .pinNumber = 0,
            };
            this->buttons.emplace_back(tmp);
        }
    }

    void setCallback(inputDeviceCallback callback_arg) override{
        this->callback = callback_arg;
        for (auto& button : this->buttons){
            button.setCallback(callback_arg);
        }
    }

    void update() override{
        digitalWrite(this->config.pinDataRead, LOW);
        delay(1);
        digitalWrite(this->config.pinDataRead, HIGH);
        delay(1);
        // shift em in!
        uint8_t shift_data = 0;
        for (uint8_t i = 0; i < 8; i++){
            digitalWrite(this->config.pinClk, LOW);
            delayMicroseconds(100);
            shift_data |= (digitalRead(this->config.pinDataIn) << i);
            digitalWrite(this->config.pinClk, HIGH);
            delayMicroseconds(100);
        }
        // reorder
        uint8_t data_ordered = 0;
        for (uint8_t i = 0; i < 8; i++){
            data_ordered |= ((shift_data & (1 << this->config.order[i])) >> this->config.order[i]) << i;
        }
        // invert
        uint8_t data_corrected = 0;
        data_corrected |= data_ordered ^ this->config.invertChannels;

        for (uint8_t i = 0; i < 8; i++){
            if (this->config.channelsActive & (1 << i)){
                this->buttons.at(i).update(data_corrected & (1 << i));
            }
        }
    }
};

