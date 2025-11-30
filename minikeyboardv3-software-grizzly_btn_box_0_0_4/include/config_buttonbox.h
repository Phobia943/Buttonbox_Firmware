//
// Created by Simon on 24.10.2023.
//
#pragma once

#if defined (BOARD_TYPE_BUTTON_BOX_1FUNKY) && defined (BOARD_TYPE_BUTTON_BOX_2FUNKY)
#error "Only one button box type can be defined!"
#endif

#define SSR_IN_READ                 28
#define SSR_CLK                     27
#define SSR_IN                      20
#define SSR_OUT                     21
#define SSR_OUT_WRITE               26
#define SSR_OUT_OE                  22

#define FUNKY1_A                    2
#define FUNKY1_B                    3
#define FUNKY1_C                    4
#define FUNKY1_D                    5
#define FUNKY1_BTN                  6
#define FUNKY1_ENC_A                0
#define FUNKY1_ENC_B                1

#if defined (BOARD_TYPE_BUTTON_BOX_2FUNKY)
#define FUNKY2_A                    9
#define FUNKY2_B                    10
#define FUNKY2_C                    11
#define FUNKY2_D                    12
#define FUNKY2_BTN                  13
#define FUNKY2_ENC_A                7
#define FUNKY2_ENC_B                8
#endif

#define ENC1_A                      19
#define ENC1_B                      18

#define ENC2_A                      17
#define ENC2_B                      16

#define ENC3_A                      14
#define ENC3_B                      15

#if defined (BOARD_TYPE_BUTTON_BOX_1FUNKY)
#define ENC4_A                      7
#define ENC4_B                      8

#define ENC5_A                      9
#define ENC5_B                      10
#endif

#if defined(ENC4_A) && defined (FUNKY2_ENC_A) \
 || defined(ENC4_B) && defined (FUNKY2_ENC_B) \
 || defined(ENC5_A) && defined (FUNKY2_A)     \
 || defined(ENC5_B) && defined (FUNKY2_B)
#error Funky2 and Encoder 4/5 use the same pins!
#endif

