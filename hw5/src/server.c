/*
 * "PBX" server module.
 * Manages interaction with a client telephone unit (TU).
 */
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include "debug.h"
#include "pbx.h"
#include "server.h"

#define PICK "pick"
#define HANG "hang"
/*
 * Thread function for the thread that handles interaction with a client TU.
 * This is called after a network connection has been made via the main server
 * thread and a new thread has been created to handle the connection.
 */
// #if 0

void *pbx_client_service(void *arg)
{
    // TO BE IMPLEMENTED
    int *client_fd = calloc(1, sizeof(int));
    *client_fd = *((int *)arg);
    free(arg);
    debug("client fd freed %d", *((int *)arg));

    if (pthread_detach((client_fd) < 0))
    {
        debug("detach failed!");
        return NULL;
    }

    TU *client_tu = tu_init(*client_fd);

    pbx_register(pbx, client_tu, *client_fd);

    for (;;)
    {
        char buf[7];
        read(*client_fd, buf, 4);
        if (!strcmp(buf, tu_command_names[TU_DIAL_CMD]))
        {
            debug("DIAL");
        }
        else if (!strcmp(buf, tu_command_names[TU_CHAT_CMD]))
        {
            debug("CHAT");
        }
        else if (!strcmp(buf, PICK) || !strcmp(buf, HANG))
        {
            read(*client_fd, buf, 2);
            if (!strcmp(buf, tu_command_names[TU_PICKUP_CMD]))
            {
                debug("PICKUP");
            }
            else
            {
                read(*client_fd, buf, 1);
                if (!strcmp(buf, tu_command_names[TU_HANGUP_CMD]))
                {
                    debug("HANGUP");
                }
                else
                {
                    debug("INVALID CMD");
                    return NULL;
                }
            }
        }
    }
    return NULL;
    // abort();
}
// #endif
