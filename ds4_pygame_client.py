#! /usr/bin/env python3

import pygame
import struct
import stat, os

FIFO_PATH = "/tmp/vpad"

# Vpad buttons masks
VPAD_BUTTON_UP               = 0x0001
VPAD_BUTTON_DOWN             = 0x0002
VPAD_BUTTON_LEFT             = 0x0004
VPAD_BUTTON_RIGHT            = 0x0008
VPAD_BUTTON_START            = 0x0010
VPAD_BUTTON_BACK             = 0x0020
VPAD_BUTTON_LEFT_THUMB       = 0x0040
VPAD_BUTTON_RIGHT_THUMB      = 0x0080
VPAD_BUTTON_LEFT_SHOULDER    = 0x0100
VPAD_BUTTON_RIGHT_SHOULDER   = 0x0200
VPAD_BUTTON_GUIDE            = 0x0400
VPAD_BUTTON_A                = 0x1000
VPAD_BUTTON_B                = 0x2000
VPAD_BUTTON_Y                = 0x8000
VPAD_BUTTON_X                = 0x4000

# Vpad events
VPAD_DUMMY_EVENT = 0
VPAD_BUTTONS_PRESS = 1
VPAD_BUTTONS_RELEASE = 2
VPAD_LEFT_TRIGGER_MOVE = 3
VPAD_RIGHT_TRIGGER_MOVE = 4
VPAD_STICK_LX_MOVE = 5
VPAD_STICK_LY_MOVE = 6
VPAD_STICK_RX_MOVE = 7
VPAD_STICK_RY_MOVE = 8

# DS4 layout

ds2vpad_buttons = {
    0: VPAD_BUTTON_A,
    1: VPAD_BUTTON_B,
    2: VPAD_BUTTON_Y,
    3: VPAD_BUTTON_X,
    4: VPAD_BUTTON_LEFT_SHOULDER,
    5: VPAD_BUTTON_RIGHT_SHOULDER,
    8: VPAD_BUTTON_BACK,
    9: VPAD_BUTTON_START,
    10: VPAD_BUTTON_GUIDE,
    11: VPAD_BUTTON_LEFT_THUMB,
    12: VPAD_BUTTON_RIGHT_THUMB
}

ds2vpad_axis_st = {
    0: VPAD_STICK_LX_MOVE,
    3: VPAD_STICK_RX_MOVE
}

ds2vpad_axis_bw = {
    1: VPAD_STICK_LY_MOVE,
    4: VPAD_STICK_RY_MOVE
}

ds2vpad_triggers = {
    2: VPAD_LEFT_TRIGGER_MOVE,
    5: VPAD_RIGHT_TRIGGER_MOVE,
}

ds2vpad_hat = [
    {
        -1: VPAD_BUTTON_LEFT,
        1: VPAD_BUTTON_RIGHT
    },
    {
        -1: VPAD_BUTTON_DOWN,
        1: VPAD_BUTTON_UP
    }
]

# Translates [-1; 1] to [0; 255]
def translate_trigger(value):
    return int((value + 1) / 2 * 255)

# Translates [-1; 1] to [-32768; 32767]
def translate_axis(value, bw=False):
    norm = (value + 1) / 2
    if bw:
        norm = 1 - norm
        
    return int(norm * 65535 - 32768)

def main():
    pygame.init()
    pygame.joystick.init()
    controller = pygame.joystick.Joystick(0)
    controller.init()

    print("Gamepad connected!")
    print(f"Opening pipe {FIFO_PATH}")

    if not os.path.exists(FIFO_PATH):
        print(f"{FIFO_PATH} is not exist!")
        print("Seems that Vpad DLL side is not running yet")
        print("Start target application in Wine and try again")
        return
        
    if not stat.S_ISFIFO(os.stat(FIFO_PATH).st_mode):
        print(f"{FIFO_PATH} is not a pipe! Make sure that there is no file with that path")
        return

    fifo_file = open(FIFO_PATH, "wb")
    print(f"Opened pipe")

    def fifo_send(bytes):
        fifo_file.write(bytes)
        fifo_file.flush()

    old_hat_state = (0, 0)
    while True:
        for event in pygame.event.get():
            if event.type == pygame.JOYAXISMOTION:
                value = round(event.value, 2)
                if event.axis in ds2vpad_axis_st:
                    fifo_send(struct.pack("ihH", ds2vpad_axis_st[event.axis], translate_axis(value), 0))
                elif event.axis in ds2vpad_axis_bw:
                    fifo_send(struct.pack("ihH", ds2vpad_axis_bw[event.axis], translate_axis(value, bw=True), 0))
                elif event.axis in ds2vpad_triggers:
                    fifo_send(struct.pack("iBBH", ds2vpad_triggers[event.axis], translate_trigger(value), 0, 0))
            elif event.type == pygame.JOYBUTTONDOWN:
                if event.button not in ds2vpad_buttons:
                    continue

                fifo_send(struct.pack("iHH", VPAD_BUTTONS_PRESS, ds2vpad_buttons[event.button], 0))
            elif event.type == pygame.JOYBUTTONUP:
                if event.button not in ds2vpad_buttons:
                    continue

                fifo_send(struct.pack("iHH", VPAD_BUTTONS_RELEASE, ds2vpad_buttons[event.button], 0))
            elif event.type == pygame.JOYHATMOTION:
                hat_state = event.value
                for i in range(2):
                    if old_hat_state[i] != hat_state[i]:
                        if old_hat_state[i] != 0:
                            fifo_send(struct.pack("iHH", VPAD_BUTTONS_RELEASE, ds2vpad_hat[i][old_hat_state[i]], 0))
                        if hat_state[i] != 0:
                            fifo_send(struct.pack("iHH", VPAD_BUTTONS_PRESS, ds2vpad_hat[i][hat_state[i]], 0))
                old_hat_state = hat_state

if __name__ == "__main__":
    main()
