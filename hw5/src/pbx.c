/*
 * PBX: simulates a Private Branch Exchange.
 */
#include <stdlib.h>
#include <semaphore.h>
#include <sys/socket.h>
#include "pbx.h"
#include "debug.h"

// #if 0
struct pbx
{
    TU *tu[PBX_MAX_EXTENSIONS];
    sem_t semaphore;
};
/*
 * Initialize a new PBX.
 *
 * @return the newly initialized PBX, or NULL if initialization fails.
 */
PBX *pbx_init()
{
    debug("pbx_init");
    // TO BE IMPLEMENTED
    pbx = calloc(1, sizeof(PBX));
    for (int pos = 0; pos < PBX_MAX_EXTENSIONS; pos++)
        pbx->tu[pos] = NULL;
    sem_init(&pbx->semaphore, 0, 1);
    return pbx;
}
// #endif

/*
 * Shut down a pbx, shutting down all network connections, waiting for all server
 * threads to terminate, and freeing all associated resources.
 * If there are any registered extensions, the associated network connections are
 * shut down, which will cause the server threads to terminate.
 * Once all the server threads have terminated, any remaining resources associated
 * with the PBX are freed.  The PBX object itself is freed, and should not be used again.
 *
 * @param pbx  The PBX to be shut down.
 */
// #if 0
void pbx_shutdown(PBX *pbx)
{
    debug("pbx_shut");
    sem_wait(&pbx->semaphore);
    // TO BE IMPLEMENTED
    for (int pos = 0; pos < PBX_MAX_EXTENSIONS; pos++)
    {
        TU *tu = pbx->tu[pos];
        if (tu)
        {
            if (shutdown(tu_fileno(tu), SHUT_RDWR))
            {
                debug("shutdown fail");
            }
            sem_post(&pbx->semaphore);
            if (pbx_unregister(pbx, tu) < 0)
            {
                continue;
            }
            sem_wait(&pbx->semaphore);
            // free(tu);
        }
    }
    sem_post(&pbx->semaphore);
    sem_destroy(&pbx->semaphore);
    debug("free here");
    free(pbx);
    // abort();
}
// #endif

/*
 * Register a telephone unit with a PBX at a specified extension number.
 * This amounts to "plugging a telephone unit into the PBX".
 * The TU is initialized to the TU_ON_HOOK state.
 * The reference count of the TU is increased and the PBX retains this reference
 *for as long as the TU remains registered.
 * A notification of the assigned extension number is sent to the underlying network
 * client.
 *
 * @param pbx  The PBX registry.
 * @param tu  The TU to be registered.
 * @param ext  The extension number on which the TU is to be registered.
 * @return 0 if registration succeeds, otherwise -1.
 */
// #if 0
int pbx_register(PBX *pbx, TU *tu, int ext)
{
    // if (!pbx || !tu)
    // {
    //     return -1;
    // }
    sem_wait(&pbx->semaphore);
    debug("pbx_reg %d", ext);
    for (int pos = 0; pos < PBX_MAX_EXTENSIONS; pos++)
    {
        if (!pbx->tu[pos])
        {
            tu_set_extension(tu, ext);
            pbx->tu[pos] = tu;
            tu_ref(pbx->tu[pos], "registering");
            sem_post(&pbx->semaphore);

            debug("TU registered");
            return 0;
        }
    }
    sem_post(&pbx->semaphore);
    return -1;
}
// #endif

/*
 * Unregister a TU from a PBX.
 * This amounts to "unplugging a telephone unit from the PBX".
 * The TU is disassociated from its extension number.
 * Then a hangup operation is performed on the TU to cancel any
 * call that might be in progress.
 * Finally, the reference held by the PBX to the TU is released.
 *
 * @param pbx  The PBX.
 * @param tu  The TU to be unregistered.
 * @return 0 if unregistration succeeds, otherwise -1.
 */
// #if 0
int pbx_unregister(PBX *pbx, TU *tu)
{
    debug("pbx_unreg");
    // if (!pbx || !tu)
    // {
    //     return -1;
    // }
    sem_wait(&pbx->semaphore);
    for (int pos = 0; pos < PBX_MAX_EXTENSIONS; pos++)
    {
        if (pbx->tu[pos] == tu)
        {
            tu_hangup(tu);

            pbx->tu[pos] = NULL;
            tu_unref(tu, "unregistering");
            // tu_set_extension(pbx->tu[pos], -1);
            sem_post(&pbx->semaphore);
            return 0;
        }
    }
    sem_post(&pbx->semaphore);
    return -1;
}
// #endif

/*
 * Use the PBX to initiate a call from a specified TU to a specified extension.
 *
 * @param pbx  The PBX registry.
 * @param tu  The TU that is initiating the call.
 * @param ext  The extension number to be called.
 * @return 0 if dialing succeeds, otherwise -1.
 */
// #if 0
int pbx_dial(PBX *pbx, TU *tu, int ext)
{
    // if (!pbx || !tu)
    //     return -1;
    sem_wait(&pbx->semaphore);
    debug("pbx_dial");
    for (int pos = 0; pos < PBX_MAX_EXTENSIONS; pos++)
    {

        if (tu_extension(pbx->tu[pos]) == ext)
        {
            int t = tu_dial(tu, pbx->tu[pos]);
            sem_post(&pbx->semaphore);
            return t;
        }
    }
    debug("uh oh");
    sem_post(&pbx->semaphore);
    return -1;
}
// #endif
