1 stdcall -private DllMain(long long ptr) DllMain
2 stdcall XInputGetState(long ptr) XInputGetState
3 stdcall XInputSetState(long ptr) XInputSetState
4 stdcall XInputGetCapabilities(long long ptr) XInputGetCapabilities
5 stdcall XInputEnable(long) XInputEnable
7 stdcall XInputGetBatteryInformation(long long ptr) XInputGetBatteryInformation
8 stdcall XInputGetKeystroke(long long ptr) XInputGetKeystroke
10 stub XInputGetAudioDeviceIds(long ptr ptr ptr ptr) XInputGetAudioDeviceIds
100 stdcall XInputGetStateEx(long ptr) XInputGetStateEx
