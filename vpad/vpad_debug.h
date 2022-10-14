#ifndef VPAD_DEFUG_H
#define VPAD_DEBUG_H

#include <stdarg.h>
#include <stdio.h>

typedef enum _VpadLogLevel
{
    VpadDebug = 0,
    VpadLog = 1,
    VpadError = 2
} VpadLogLevel;

static void vpad_log(FILE* file, VpadLogLevel log_level, const char* fmt, ...)
{
    switch (log_level)
    {
    case VpadDebug:
        fprintf(file, "[Vpad][Debug] ");
        break;

    case VpadLog:
        fprintf(file, "[Vpad][Log] ");
        break;

    case VpadError:
        fprintf(file, "[Vpad][Error] ");
        break;

    default:
        fprintf(file, "[Vpad] ");
        break;
    }
    

    va_list args;
    va_start(args, fmt);
    vfprintf(file, fmt, args);
    va_end(args);

    fprintf(file, "\n");
}

#define VPAD_ERROR(...) vpad_log(stderr, VpadError, __VA_ARGS__)
#define VPAD_LOG(...) vpad_log(stdout, VpadLog, __VA_ARGS__)

#ifndef NDEBUG
#define VPAD_DEBUG(...) vpad_log(stdout, VpadDebug, __VA_ARGS__)
#else
#define VPAD_DEBUG
#endif

#endif
