#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "pbx.h"
#include "server.h"
#include "debug.h"

static void terminate(int status);

int get_port(int argc, char *argv[])
{
    int c, port;
    while ((c = getopt(argc, argv, "p:")) != -1)
    {
        switch (c)
        {
        case 'p':
            port = atoi(optarg);
            break;

        default:
            debug("get_opt failed");
            return -1;
            break;
        }
    }
    return port;
}

/*
 * "PBX" telephone exchange simulation.
 *
 * Usage: pbx <port>
 */
int main(int argc, char *argv[])
{
    // Option processing should be performed here.
    // Option '-p <port>' is required in order to specify the port number
    // on which the server should listen.

    int port = get_port(argc, argv);
    if (port < 0)
        return -1;

    // Perform required initialization of the PBX module.
    debug("Initializing PBX...");
    pbx = pbx_init();
    if (!pbx)
    {
        return -1;
    }
    // TODO: Set up the server socket and enter a loop to accept connections
    // on this socket.  For each connection, a thread should be started to
    // run function pbx_client_service().  In addition, you should install
    // a SIGHUP handler, so that receipt of SIGHUP will perform a clean
    // shutdown of the server.

    struct sigaction sa;
    sa.sa_handler = &terminate;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
    {
        debug("sigaction failed");
        return -1;
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        debug("socket failed");
        return -1;
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        debug("setsockopt failed");
        close(server_fd);
        return -1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        debug("bind failed");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 256) < 0)
    {
        debug("listen failed");
        close(server_fd);
        return -1;
    }

    struct sockaddr_storage server_storage;
    int addrlen;
    // int client[256];
    pthread_t tid[256];
    for (int i = 0;; i++)
    {
        int *client = calloc(1, sizeof(int));
        addrlen = sizeof(server_storage);
        debug("listening on port %d for client #%d", port, i + 1);
        *client = accept(server_fd, (struct sockaddr *)&server_storage, (socklen_t *)&addrlen);
        if (*client < 0)
        {
            debug("accept failed");
            return -1;
        }

        debug("client #%d accepted", i + 1);
        debug("creating thread #%d", i + 1);
        if (pthread_create(&tid[i], NULL, &pbx_client_service, (void *)client) < 0)
        {
            debug("client #%d thread failed", i + 1);
            return -1;
        }
        debug("client #%d thread created", i + 1);
    }

    fprintf(stderr, "You have to finish implementing main() "
                    "before the PBX server will function.\n");

    terminate(EXIT_FAILURE);
}

/*
 * Function called to cleanly shut down the server.
 */
static void terminate(int status)
{
    debug("Shutting down PBX...");
    pbx_shutdown(pbx);
    debug("PBX server terminating");
    exit(status);
}
