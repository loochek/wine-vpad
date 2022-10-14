#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "vpad_pipe.h"
#include "vpad_debug.h"

static const char* fifo_path = "/home/user/fifo";
static int fifo_fd = -1;

int vpad_open_pipe()
{
    int res = mkfifo(fifo_path, S_IRUSR | S_IWUSR);
    if (res < 0)
    {
        VPAD_ERROR("Unable to create named pipe %s: %s", fifo_path, strerror(errno)); 
        return -1;
    }

    fifo_fd = open(fifo_path, O_RDONLY | O_NONBLOCK);
    if (fifo_fd < 0)
    {
        VPAD_ERROR("Unable to open named pipe %s: %s", fifo_path, strerror(errno)); 
        return -1;
    }

    VPAD_LOG("Created named pipe %s", fifo_path);
    return 0;
}

bool vpad_get_next_event(VpadEvent* event)
{
    if (fifo_fd < 0)
        return false;
        
    int bytes_available = 0;
    int res = ioctl(fifo_fd, FIONREAD, &bytes_available);
    if (res < 0)
    {
        VPAD_ERROR("Pipe ioctl() error: %s", strerror(errno)); 
        return false;
    }

    if (bytes_available < sizeof(VpadEvent))
        return false;

    // Event is available to be read at the moment

    VPAD_DEBUG("Event is present");

    res = read(fifo_fd, &event, sizeof(VpadEvent));
    if (res < 0)
    {
        VPAD_ERROR("Pipe read() error: %s", strerror(errno)); 
        return false;
    }

    if (res != sizeof(VpadEvent))
    {
        VPAD_ERROR("Bad read size! Something went really bad"); 
        return false;
    }

    VPAD_DEBUG("Event read");

    return true;
}

void vpad_close_pipe()
{
    close(fifo_fd);
    fifo_fd = -1;

    int res = remove("/home/user/fifo");
    if (res < 0)
    {
        VPAD_ERROR("Unable to remove named pipe %s: %s", fifo_path, strerror(errno));
        return;
    }

    VPAD_LOG("Removed named pipe %s", fifo_path);
}
