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
char *read_fd(int fd)
{
    int size = 0;
    char *buf = malloc(1024);
    size += read(fd, buf, 1024);
    if (!size)
    {
        return NULL;
    }
    if (size < 1024)
    {
        return realloc(buf, size);
    }
    int read_bytes;
    char s[1024];
    while ((read_bytes = read(fd, s, 1024)) != 0)
    {
        size += read_bytes;
        strcat(buf, s);
    }
    return realloc(buf, size);
}
void *pbx_client_service(void *arg)
{
    // TO BE IMPLEMENTED
    int *client_fd = calloc(1, sizeof(int));
    *client_fd = *((int *)arg);
    free(arg);
    debug("client fd freed %d", *((int *)arg));

    // if (pthread_detach((client_fd) < 0))
    // {
    //     debug("detach failed!");
    //     return NULL;
    // }

    TU *client_tu = tu_init(*client_fd);
    if (!client_tu)
    {
        debug("tu_init failed");
        return NULL;
    }
    if (pbx_register(pbx, client_tu, *client_fd) < 0)
    {
        debug("pbx_register failed");
        return NULL;
    }

    for (;;)
    {
        char *cmd = read_fd(*client_fd);
        if (!cmd)
        {
            break;
        }

        char cmd4[5];
        memcpy(cmd4, cmd, 4);
        cmd4[4] = '\0';
        char cmd6[7];
        memcpy(cmd6, cmd, 6);
        cmd6[6] = '\0';
        if (!strcmp(cmd6, tu_command_names[TU_PICKUP_CMD]))
        {
            debug("PICKUP");
            tu_pickup(client_tu);
        }
        else if (!strcmp(cmd6, tu_command_names[TU_HANGUP_CMD]))
        {
            debug("HANGUP");
            tu_hangup(client_tu);
        }
        else if (!strcmp(cmd4, tu_command_names[TU_DIAL_CMD]))
        {
            int ext;
            strtok(cmd, " \t");
            char *arg = strtok(NULL, EOL);
            ext = atoi(arg);
            debug("DIAL ext #%d", ext);
            pbx_dial(pbx, client_tu, ext);
        }
        else if (!strcmp(cmd4, tu_command_names[TU_CHAT_CMD]))
        {
            strtok(cmd, " \t");
            char *arg = strtok(NULL, EOL);
            debug("CHAT %s", arg);
            tu_chat(client_tu, arg);
        }
        else
        {
            debug("INVALID CMD");
        }
        free(cmd);
    }

    return NULL;
    // abort();
}
// #endif
