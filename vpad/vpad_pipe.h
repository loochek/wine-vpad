#ifndef VPAD_PIPE_H
#define VPAD_PIPE_H

#include <stdint.h>
#include <stdbool.h>
#include "vpad_common.h"

/**
 * Vpad communication mechanism
 */

/**
 * Creates named pipe and opens it
 * \return 0 if success, -1 otherwise
 */
int vpad_open_pipe();

/**
 * Gets next event from the named pipe
 * \param event Event structure to fill in
 * \return true if success, false if no events present or pipe is not opened
 */
bool vpad_get_next_event(VpadEvent* event);

/**
 * Closes and removes named pipe
 */
void vpad_close_pipe();

#endif
