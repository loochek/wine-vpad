 # Vpad
This project provides a virtual Xbox controller for applications running in Wine that can be controlled with named pipe accessible from host Linux environment. As this project is highly specialized, only one virtual controller is supported

## How it works
It's just a Xinput reimplementation for Wine. When Wine application starts, it creates a named pipe (`/tmp/vpad` by default) and waits for events. Events are just binary structures that are read sequentially. Events format is described in [vpad_common.h](vpad/vpad_common.h).

For testing purposes there is a client that listens to the gamepad using PyGame and emits corresponding events to the pipe. Client is designed to be used with DualShock 4, but can be easily remapped to another gamepad.

## Installation
Prebuilt DLLs are available in *Releases* section
- Put DLLs to Wine's `system32` and `syswow64` folders
- Set `xinput_*.dll` override to `Native (Windows)` in Wine configuration

## Possible problems
This project is extremely experimental. Only `xinput1_3.dll` (both 32 and 64 bits) is really tested by the author

## Building
Just `make`. Requires `winegcc` (`wine-{stable,devel,staging}-dev` and its `i386` version in Ubuntu)
