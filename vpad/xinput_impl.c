#include <windef.h>
#include <winbase.h>
#include <winerror.h>
#include <cguid.h>
#include <xinput.h>
#include "vpad_common.h"
#include "vpad_pipe.h"
#include "vpad_debug.h"

// Not defined in MinGW's xinput.h for some reason
#define XINPUT_GAMEPAD_GUIDE 0x0400

XINPUT_GAMEPAD vpad_state;
XINPUT_STATE vpad_report_state;
bool vpad_connected = false;
bool vpad_xinput_enabled = true;

static void vpad_init();
static void vpad_update();
static void vpad_deinit();

BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
) {
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hinstDLL);
        vpad_init();
    }
    else if (fdwReason == DLL_PROCESS_DETACH)
        vpad_deinit();

    return TRUE;
}

void WINAPI XInputEnable(
    _In_  BOOL enable
) {
    // TODO: when vibration will be supported, there must be a more complicated logic
    vpad_xinput_enabled = enable != FALSE;
}

DWORD WINAPI XInputGetCapabilities(
    _In_   DWORD dwUserIndex,
    _In_   DWORD dwFlags,
    _Out_  XINPUT_CAPABILITIES *pCapabilities
) {
    if (dwUserIndex >= XUSER_MAX_COUNT)
        return ERROR_BAD_ARGUMENTS;
    if (!vpad_connected || dwUserIndex > 0)
        return ERROR_DEVICE_NOT_CONNECTED;

    pCapabilities->Type = XINPUT_DEVTYPE_GAMEPAD;
    pCapabilities->SubType = XINPUT_DEVSUBTYPE_GAMEPAD;

    pCapabilities->Flags = XINPUT_CAPS_FFB_SUPPORTED;

    pCapabilities->Vibration = (XINPUT_VIBRATION){0};
    pCapabilities->Gamepad = vpad_report_state.Gamepad;

    return ERROR_SUCCESS;
}

DWORD WINAPI XInputGetState(
    _In_   DWORD dwUserIndex,
    _Out_  XINPUT_STATE *pState
) {
    if (dwUserIndex >= XUSER_MAX_COUNT)
        return ERROR_BAD_ARGUMENTS;
    if (!vpad_connected || dwUserIndex > 0)
        return ERROR_DEVICE_NOT_CONNECTED;

    vpad_update();
    *pState = vpad_report_state;

    // The only difference between ex and non-ex function is:
    pState->Gamepad.wButtons &= ~XINPUT_GAMEPAD_GUIDE;
    return ERROR_SUCCESS;
}

// Actually requires XINPUT_STATE_EX structure, 
// but the difference is only in padding bytes at the end so don't care about it
DWORD WINAPI XInputGetStateEx(
    _In_   DWORD dwUserIndex,
    _Out_  XINPUT_STATE *pState
) {
    if (dwUserIndex >= XUSER_MAX_COUNT)
        return ERROR_BAD_ARGUMENTS;
    if (!vpad_connected || dwUserIndex > 0)
        return ERROR_DEVICE_NOT_CONNECTED;

    vpad_update();
    *pState = vpad_report_state;
    return ERROR_SUCCESS;
}

DWORD WINAPI XInputSetState(
    _In_     DWORD dwUserIndex,
    _Inout_  XINPUT_VIBRATION *pVibration
) {
    if (dwUserIndex >= XUSER_MAX_COUNT)
        return ERROR_BAD_ARGUMENTS;
    if (!vpad_connected || dwUserIndex > 0)
        return ERROR_DEVICE_NOT_CONNECTED;

    // TODO: bzzzzz
    return ERROR_SUCCESS;
}

DWORD WINAPI XInputGetKeystroke(
    _In_   DWORD dwUserIndex,
    _In_   DWORD dwReserved,
    _Out_  PXINPUT_KEYSTROKE pKeystroke
) {
    if (dwUserIndex >= XUSER_MAX_COUNT)
        return ERROR_BAD_ARGUMENTS;
    if (!vpad_connected || (dwUserIndex > 0 && dwUserIndex != XUSER_INDEX_ANY))
        return ERROR_DEVICE_NOT_CONNECTED;

    // TODO: ?
    return ERROR_EMPTY;
}

DWORD WINAPI XInputGetBatteryInformation(
    _In_   DWORD dwUserIndex,
    _In_   BYTE devType,
    _Out_  XINPUT_BATTERY_INFORMATION *pBatteryInformation
) {
    if (dwUserIndex >= XUSER_MAX_COUNT)
        return ERROR_BAD_ARGUMENTS;
    if (!vpad_connected && dwUserIndex > 0)
        return ERROR_DEVICE_NOT_CONNECTED;

    pBatteryInformation->BatteryLevel = BATTERY_LEVEL_FULL;
    pBatteryInformation->BatteryType = BATTERY_TYPE_WIRED;
    return ERROR_SUCCESS;
}

DWORD WINAPI XInputGetAudioDeviceIds(
    _In_         DWORD dwUserIndex,
    _Out_opt_    LPWSTR pRenderDeviceId,
    _Inout_opt_  UINT *pRenderCount,
    _Out_opt_    LPWSTR pCaptureDeviceId,
    _Inout_opt_  UINT *pCaptureCount
) {
    if (dwUserIndex >= XUSER_MAX_COUNT)
        return ERROR_BAD_ARGUMENTS;
    if (!vpad_connected || dwUserIndex > 0)
        return ERROR_DEVICE_NOT_CONNECTED;

    *pRenderDeviceId = '\0';
    *pCaptureDeviceId = '\0';
    return ERROR_SUCCESS;
}

DWORD WINAPI XInputGetDSoundAudioDeviceGuids(
    _In_   DWORD dwUserIndex,
    _Out_  GUID* pDSoundRenderGuid,
    _Out_  GUID* pDSoundCaptureGuid
) {
    if (dwUserIndex >= XUSER_MAX_COUNT)
        return ERROR_BAD_ARGUMENTS;
    if (!vpad_connected || dwUserIndex > 0)
        return ERROR_DEVICE_NOT_CONNECTED;

    *pDSoundRenderGuid = GUID_NULL;
    *pDSoundCaptureGuid = GUID_NULL;
    return ERROR_SUCCESS;
}

static void vpad_init()
{
    int res = vpad_open_pipe();
    if (res < 0) {
        vpad_connected = false;
        VPAD_ERROR("Vpad is not connected");
        return;
    }

    vpad_connected = true;
    memset(&vpad_report_state.Gamepad, 0, sizeof(XINPUT_GAMEPAD));
    vpad_report_state.dwPacketNumber = 0;
    VPAD_LOG("Vpad is connected as gamepad 0");
}

static void vpad_update()
{
    // Update actual state

    VpadEvent event;
    while (vpad_get_next_event(&event))
    {
        switch (event.type)
        {
        case VpadDummyEvent:
            VPAD_DEBUG("Received VpadDummyEvent");
            break;

        case VpadButtonsPress:
            vpad_state.wButtons &= event.buttons;
            VPAD_DEBUG("Received VpadButtonsPress");
            break;

        case VpadButtonsRelease:
            vpad_state.wButtons |= ~event.buttons;
            VPAD_DEBUG("Received VpadButtonsRelease");
            break;

        case VpadLeftTriggerMove:
            vpad_state.bLeftTrigger = event.trigger_value;
            VPAD_DEBUG("Received VpadLeftTriggerMove");
            break;

        case VpadRightTriggerMove:
            vpad_state.bRightTrigger = event.trigger_value;
            VPAD_DEBUG("Received VpadRightTriggerMove");
            break;

        case VpadStickLXMove:
            vpad_state.sThumbLX = event.axis_value;
            VPAD_DEBUG("Received VpadStickLXMove");
            break;

        case VpadStickLYMove:
            vpad_state.sThumbLY = event.axis_value;
            VPAD_DEBUG("Received VpadStickLYMove");
            break;

        case VpadStickRXMove:
            vpad_state.sThumbRX = event.axis_value;
            VPAD_DEBUG("Received VpadStickRXMove");
            break;

        case VpadStickRYMove:
            vpad_state.sThumbRY = event.axis_value;
            VPAD_DEBUG("Received VpadStickRYMove");
            break;

        default:
            VPAD_ERROR("Unknown event");
            break;
        }
    }

    // Update report state (depends on XInputEnable)

    if (!vpad_xinput_enabled)
        memset(&vpad_report_state.Gamepad, 0, sizeof(XINPUT_GAMEPAD));

    if (memcmp(&vpad_state, &vpad_report_state.Gamepad, sizeof(XINPUT_GAMEPAD)) != 0)
    {
        vpad_report_state.Gamepad = vpad_state;
        vpad_report_state.dwPacketNumber++;
        VPAD_DEBUG("Reported new state");
    }
}

static void vpad_deinit()
{
    vpad_close_pipe();
}
