//
// Created by Simon on 20.01.2024.
//

#pragma once

#include "config.h"

#if defined (BOARD_TYPE_MINIKEYBOARD)
inputDeviceFunkyConf_t funky1 = {
    .baseConf = {
        .entityId = 0,
        .entitySubId = 0,
        .callback = nullptr,
        .niceName = "Funky SW 1",
    },
    .pinA = PIN_ENCODER_BTN_A,
    .pinB = PIN_ENCODER_BTN_B,
    .pinC = PIN_ENCODER_BTN_C,
    .pinD = PIN_ENCODER_BTN_D,
    .pinBTN = PIN_ENCODER_BTN,
    .pinEncA = PIN_ENCODER_A,
    .pinEncB = PIN_ENCODER_B,
};

inputDeviceKeyMatrixConf_t keymatrix1 = {
    .baseConf = {
        .entityId = 1,
        .entitySubId = 0,
        .callback = nullptr,
        .niceName = "Keymatrix 1",
    },
    .rowPins = {PIN_ROW0, PIN_ROW1, PIN_ROW2},
    .columnPins = {PIN_COLUMN0, PIN_COLUMN1, PIN_COLUMN2, PIN_COLUMN3},
};
InputDevice* inputDevices[] = {
    new FunkySwitch(funky1),
    new KeyMatrix(keymatrix1),
};

lightOutDeviceNeopixelConf_t neopixel1 = {
    NUM_OF_LEDS,
    NUM_OF_BRIGHT_LEDS,
    PIN_NEOPIXEL,
    NEO_GRB + NEO_KHZ800,
    DEFAULT_BRIGHTNESS,
    OutputLightDevice::ledColor_t::green,
    OutputLightDevice::ledColor_t::white,
    {
            {{1, 0}, {4}},
            {{1, 1}, {5}},
            {{1, 2}, {6}},
            {{1, 3}, {7}},
            {{1, 4}, {8}},
            {{1, 5}, {9}},
            {{1, 6}, {10}},
            {{1, 7}, {11}},
            {{1, 8}, {12}},
            {{1, 9}, {13}},
            {{1, 10}, {14}},
            {{1, 11}, {15}},
            {{0, 0}, {1}}, //a
            {{0, 1}, {0}}, //b
            {{0, 2}, {3}}, //c
            {{0, 3}, {2}}, //d
            {{0, 4}, {0, 1, 2, 3}} //btn
    }
};

std::vector<OutputLightDevice*> lightDevices = {
    new LED(neopixel1),
};
#endif
#if defined (BOARD_TYPE_BUTTON_BOX_1FUNKY) || defined (BOARD_TYPE_BUTTON_BOX_2FUNKY)
inputDeviceFunkyConf_t funky1 = {
        .baseConf = {
                .entityId = 0,
                .entitySubId = 0,
                .callback = nullptr,
                .niceName = "Funky SW 1",
        },
        .pinA = FUNKY1_A,
        .pinB = FUNKY1_B,
        .pinC = FUNKY1_C,
        .pinD = FUNKY1_D,
        .pinBTN = FUNKY1_BTN,
        .pinEncA = FUNKY1_ENC_A,
        .pinEncB = FUNKY1_ENC_B,
};
inputDeviceEncoderConf_t encoder1 = {
        .baseConf = {
                .entityId = 2,
                .entitySubId = 0,
                .callback = nullptr,
                .niceName = "Encoder 1",
        },
        .pinA = ENC1_A,
        .pinB = ENC1_B,
        .latchMode = RotaryEncoder::LatchMode::TWO03,
};
inputDeviceEncoderConf_t encoder2 = {
        .baseConf = {
                .entityId = 3,
                .entitySubId = 0,
                .callback = nullptr,
                .niceName = "Encoder 2",
        },
        .pinA = ENC2_A,
        .pinB = ENC2_B,
        .latchMode = RotaryEncoder::LatchMode::TWO03,
};
inputDeviceEncoderConf_t encoder3 = {
        .baseConf = {
                .entityId = 4,
                .entitySubId = 0,
                .callback = nullptr,
                .niceName = "Encoder 3",
        },
        .pinA = ENC3_A,
        .pinB = ENC3_B,
        .latchMode = RotaryEncoder::LatchMode::TWO03,
};
inputDeviceSSRInConf_t ssrin1 = {
        .baseConf = {
                .entityId = 7,
                .entitySubId = 0,
                .callback = nullptr,
                .niceName = "SSR In 1",
        },
        .pinClk = SSR_CLK,
        .pinDataIn = SSR_IN,
        .pinDataRead = SSR_IN_READ,
        .channelsActive = 0b11111111,
        .invertChannels = 0b11111111,
        .order = {7,6,5,4,0,1,2,3},
};
outputDeviceSSROutConf_t ssrout1 = {
        .pinClk = SSR_CLK,
        .pinDataOut = SSR_OUT,
        .pinDataWriteCmd = SSR_OUT_WRITE,
        .pinOE = SSR_OUT_OE,
        .channelsActive = 0b11111111,
        .invertChannels = 0,
        .order = {7,0,1,2,3,4,5,6},
        .defaultBrightness = DEFAULT_BRIGHTNESS,
        .entityToIdMap = {
                {{7, 0}, {0}},
                {{7, 1}, {1}},
                {{7, 2}, {2}},
                {{7, 3}, {3}},
                {{7, 4}, {4}},
                {{7, 5}, {5}},
                {{7, 6}, {6}},
                {{7, 7}, {7}},
        }
};
std::vector<OutputLightDevice*> lightDevices = {
        new ShiftRegisterOut(ssrout1),
};
#endif
#if defined (BOARD_TYPE_BUTTON_BOX_1FUNKY)
inputDeviceEncoderConf_t encoder4 = {
        .baseConf = {
                .entityId = 5,
                .entitySubId = 0,
                .callback = nullptr,
                .niceName = "Encoder 4",
        },
        .pinA = ENC4_A,
        .pinB = ENC4_B,
        .latchMode = RotaryEncoder::LatchMode::TWO03,
};
inputDeviceEncoderConf_t encoder5 = {
        .baseConf = {
                .entityId = 6,
                .entitySubId = 0,
                .callback = nullptr,
                .niceName = "Encoder 5",
        },
        .pinA = ENC5_A,
        .pinB = ENC5_B,
        .latchMode = RotaryEncoder::LatchMode::TWO03,
};
InputDevice* inputDevices[] = {
        new FunkySwitch(funky1),
        new Encoder(encoder1),
        new Encoder(encoder2),
        new Encoder(encoder3),
        new Encoder(encoder4),
        new Encoder(encoder5),
        new ShiftRegisterIn(ssrin1),
};
#endif
#if defined (BOARD_TYPE_BUTTON_BOX_2FUNKY)
inputDeviceFunkyConf_t funky2 = {
        .baseConf = {
                .entityId = 1,
                .entitySubId = 0,
                .callback = nullptr,
                .niceName = "Funky SW 2",
        },
        .pinA = FUNKY2_A,
        .pinB = FUNKY2_B,
        .pinC = FUNKY2_C,
        .pinD = FUNKY2_D,
        .pinBTN = FUNKY2_BTN,
        .pinEncA = FUNKY2_ENC_A,
        .pinEncB = FUNKY2_ENC_B,
};
InputDevice* inputDevices[] = {
        new FunkySwitch(funky1),
        new FunkySwitch(funky2),
        new Encoder(encoder1),
        new Encoder(encoder2),
        new Encoder(encoder3),
        new ShiftRegisterIn(ssrin1),
};
#endif