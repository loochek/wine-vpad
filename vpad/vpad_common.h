#ifndef VPAD_COMMON_H
#define VPAD_COMMON_H

#include <stdint.h>

//#include <xinput.h>

// #define VPAD_BUTTON_UP               XINPUT_GAMEPAD_DPAD_UP
// #define VPAD_BUTTON_DOWN             XINPUT_GAMEPAD_DPAD_DOWN
// #define VPAD_BUTTON_LEFT             XINPUT_GAMEPAD_DPAD_LEFT
// #define VPAD_BUTTON_RIGHT            XINPUT_GAMEPAD_DPAD_RIGHT
// #define VPAD_BUTTON_START            XINPUT_GAMEPAD_START
// #define VPAD_BUTTON_BACK             XINPUT_GAMEPAD_BACK
// #define VPAD_BUTTON_LEFT_THUMB       XINPUT_GAMEPAD_LEFT_THUMB
// #define VPAD_BUTTON_RIGHT_THUMB      XINPUT_GAMEPAD_RIGHT_THUMB
// #define VPAD_BUTTON_LEFT_SHOULDER    XINPUT_GAMEPAD_LEFT_SHOULDER
// #define VPAD_BUTTON_RIGHT_SHOULDER   XINPUT_GAMEPAD_RIGHT_SHOULDER
// #define VPAD_BUTTON_A                XINPUT_GAMEPAD_A
// #define VPAD_BUTTON_B                XINPUT_GAMEPAD_B
// #define VPAD_BUTTON_X                XINPUT_GAMEPAD_X
// #define VPAD_BUTTON_Y                XINPUT_GAMEPAD_Y

typedef enum _VpadEventType
{
    VpadDummyEvent = 0,
    VpadButtonsPress = 1,
    VpadButtonsRelease = 2,
    VpadLeftTriggerMove = 3,
    VpadRightTriggerMove = 4,
    VpadStickLXMove = 5,
    VpadStickLYMove = 6,
    VpadStickRXMove = 7,
    VpadStickRYMove = 8,
} VpadEventType;

typedef struct _VpadEvent
{
    VpadEventType type;
    union
    {
        uint16_t buttons;
        uint8_t trigger_value;
        uint16_t axis_value;
        uint32_t __padding;
    };
} VpadEvent;

#endif
