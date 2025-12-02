//
// Created by Simon on 24.10.2023.
//
#pragma once


#define DEBOUNCE_DEFAULT_TIME_MS    50
#define DEFAULT_BRIGHTNESS          30

////#define BOARD_TYPE_MINIKEYBOARD
//#define BOARD_TYPE_BUTTON_BOX_1FUNKY
//#define BOARD_TYPE_BUTTON_BOX_2FUNKY

#if defined(BOARD_TYPE_MINIKEYBOARD)
#include "config_minikeyboard.h"
#elif defined(BOARD_TYPE_BUTTON_BOX_1FUNKY) || defined(BOARD_TYPE_BUTTON_BOX_2FUNKY)
#include "config_buttonbox.h"
#else
#error "No board type defined!"
#endif

