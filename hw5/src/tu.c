/*
 * TU: simulates a "telephone unit", which interfaces a client with the PBX.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include "pbx.h"
#include "debug.h"
// #include <errno.h>
// #if 0
#define CHAT "CHAT"
#define SPACE " "
struct tu
{
    int ext;
    int fileno;
    TU_STATE state;
    pthread_mutex_t mutex;
    int ref;

    TU *peer;
};
void set_state(TU *tu, TU_STATE state)
{
    // if (!tu)
    // {
    //     return;
    // }
    tu->state = state;
    char c[PBX_MAX_EXTENSIONS + 7];
    if (tu->state == TU_CONNECTED)
    {
        snprintf(c, PBX_MAX_EXTENSIONS + 7, "%s %d%s", tu_state_names[tu->state], tu->peer->ext, EOL);
    }
    else if (tu->state == TU_ON_HOOK)
    {
        snprintf(c, PBX_MAX_EXTENSIONS + 7, "%s %d%s", tu_state_names[tu->state], tu->ext, EOL);
    }
    else
    {
        snprintf(c, PBX_MAX_EXTENSIONS + 7, "%s%s", tu_state_names[tu->state], EOL);
    }
    if (write(tu->fileno, c, strlen(c)) <= 0)
    {
        debug("look i got here2");
        tu->state = TU_ERROR;
        return;
    }
}
void double_lock(pthread_mutex_t *tu, pthread_mutex_t *tu2)
{
    if (tu > tu2)
    {
        pthread_mutex_lock(tu2);
        pthread_mutex_lock(tu);
    }
    else
    {
        pthread_mutex_lock(tu);
        pthread_mutex_lock(tu2);
    }
}

void double_unlock(pthread_mutex_t *tu, pthread_mutex_t *tu2)
{
    if (tu > tu2)
    {
        pthread_mutex_unlock(tu2);
        pthread_mutex_unlock(tu);
    }
    else
    {
        pthread_mutex_unlock(tu);
        pthread_mutex_unlock(tu2);
    }
}
// dprintf(tu->fileno, "%s", c);

// if (tu->fileno == -1 || errno)
// {
//     debug("look i got here");
//     return;
// }
// tu->state = state;
// debug("look i got here1");
// if (write(tu->fileno, tu_state_names[state], strlen(tu_state_names[state])) <= 0 || errno)
// {
//     debug("look i got here2");
//     tu->state = TU_ERROR;
//     return;
// }
// debug("look i got here3: %s, ext %d", tu_state_names[tu->state], tu->ext);
// if (tu->state == TU_CONNECTED)
// {
//     if (write(tu->fileno, SPACE, strlen(SPACE)) <= 0 || errno)
//     {
//         tu->state = TU_ERROR;
//         debug("look i got here2");
//         return;
//     }
//     char c[PBX_MAX_EXTENSIONS];
//     snprintf(c, PBX_MAX_EXTENSIONS, "%d", tu->peer->ext);
//     if (write(tu->fileno, c, strlen(c)) <= 0 || errno)
//     {
//         debug("look i got here2");
//         tu->state = TU_ERROR;
//         return;
//     }
// }
// else if (tu->state == TU_ON_HOOK)
// {
//     debug("look i got here4: %s, ext %d", tu_state_names[tu->state], tu->ext);
//     if (write(tu->fileno, SPACE, strlen(SPACE)) <= 0 || errno)
//     {
//         debug("look i got here2");
//         tu->state = TU_ERROR;
//         return;
//     }
//     debug("look i got here5");
//     char c[PBX_MAX_EXTENSIONS];
//     snprintf(c, PBX_MAX_EXTENSIONS, "%d", tu->ext);
//     if (write(tu->fileno, c, strlen(c)) <= 0 || errno)
//     {
//         debug("look i got here2");
//         tu->state = TU_ERROR;
//         return;
//     }
//     debug("look i got here6");
// }
// if (write(tu->fileno, EOL, strlen(EOL)) <= 0 || errno)
// {
//     debug("look i got here2");
//     tu->state = TU_ERROR;
//     return;
// }

/*
 * Initialize a TU
 *
 * @param fd  The file descriptor of the underlying network connection.
 * @return  The TU, newly initialized and in the TU_ON_HOOK state, if initialization
 * was successful, otherwise NULL.
 */
// #if 0

TU *tu_init(int fd)
{
    debug("tu_init");
    TU *tu = malloc(sizeof(TU));
    tu->fileno = tu->ext = fd;
    tu->ref = 0;
    set_state(tu, TU_ON_HOOK);

    pthread_mutex_init(&tu->mutex, NULL);
    // pthread_mutex_lock(&tu->mutex);
    // pthread_mutex_unlock(&tu->mutex);
    return tu;
}
// #endif

/*
 * Increment the reference count on a TU.
 *
 * @param tu  The TU whose reference count is to be incremented
 * @param reason  A string describing the reason why the count is being incremented
 * (for debugging purposes).
 */
// #if 0
void tu_ref(TU *tu, char *reason)
{
    debug("tu_ref: %s", reason);
    if (!tu)
    {
        return;
    }
    // TO BE IMPLEMENTED
    // abort();
    pthread_mutex_lock(&tu->mutex);
    tu->ref++;
    pthread_mutex_unlock(&tu->mutex);
    return;
}
// #endif

/*
 * Decrement the reference count on a TU, freeing it if the count becomes 0.
 *
 * @param tu  The TU whose reference count is to be decremented
 * @param reason  A string describing the reason why the count is being decremented
 * (for debugging purposes).
 */
// #if 0
void tu_unref(TU *tu, char *reason)
{
    debug("tu_unref: %s", reason);
    if (!tu)
    {
        return;
    }
    // TO BE IMPLEMENTED
    // abort();
    pthread_mutex_lock(&tu->mutex);
    tu->ref--;
    if (!tu->ref)
    {
        // pthread_mutex_unlock(&tu->mutex);
        pthread_mutex_destroy(&tu->mutex);
        free(tu);
        return;
    }
    pthread_mutex_unlock(&tu->mutex);

    return;
}
// #endif

/*
 * Get the file descriptor for the network connection underlying a TU.
 * This file descriptor should only be used by a server to read input from
 * the connection.  Output to the connection must only be performed within
 * the PBX functions.
 *
 * @param tu
 * @return the underlying file descriptor, if any, otherwise -1.
 */
// #if 0
int tu_fileno(TU *tu)
{
    debug("tu_fileno");
    if (!tu)
    {
        return -1;
    }
    return tu->fileno;
}
// #endif

/*
 * Get the extension number for a TU.
 * This extension number is assigned by the PBX when a TU is registered
 * and it is used to identify a particular TU in calls to tu_dial().
 * The value returned might be the same as the value returned by tu_fileno(),
 * but is not necessarily so.
 *
 * @param tu
 * @return the extension number, if any, otherwise -1.
 */
// #if 0
int tu_extension(TU *tu)
{
    if (!tu)
    {
        return -1;
    }
    // pthread_mutex_lock(&tu->mutex);
    int e = tu->ext;
    debug("tu_ext %d", e);
    // pthread_mutex_unlock(&tu->mutex);
    return e;
}
// #endif

/*
 * Set the extension number for a TU.
 * A notification is set to the client of the TU.
 * This function should be called at most once one any particular TU.
 *
 * @param tu  The TU whose extension is being set.
 */
// #if 0
int tu_set_extension(TU *tu, int ext)
{
    debug("tu_set_ext");
    // if (!tu)
    // {
    //     return -1;
    // }
    pthread_mutex_lock(&tu->mutex);
    tu->ext = ext;
    pthread_mutex_unlock(&tu->mutex);
    return 0;
}
// #endif

/*
 * Initiate a call from a specified originating TU to a specified target TU.
 *   If the originating TU is not in the TU_DIAL_TONE state, then there is no effect.
 *   If the target TU is the same as the originating TU, then the TU transitions
 *     to the TU_BUSY_SIGNAL state.
 *   If the target TU already has a peer, or the target TU is not in the TU_ON_HOOK
 *     state, then the originating TU transitions to the TU_BUSY_SIGNAL state.
 *   Otherwise, the originating TU and the target TU are recorded as peers of each other
 *     (this causes the reference count of each of them to be incremented),
 *     the target TU transitions to the TU_RINGING state, and the originating TU
 *     transitions to the TU_RING_BACK state.
 *
 * In all cases, a notification of the resulting state of the originating TU is sent to
 * to the associated network client.  If the target TU has changed state, then its client
 * is also notified of its new state.
 *
 * If the caller of this function was unable to determine a target TU to be called,
 * it will pass NULL as the target TU.  In this case, the originating TU will transition
 * to the TU_ERROR state if it was in the TU_DIAL_TONE state, and there will be no
 * effect otherwise.  This situation is handled here, rather than in the caller,
 * because here we have knowledge of the current TU state and we do not want to introduce
 * the possibility of transitions to a TU_ERROR state from arbitrary other states,
 * especially in states where there could be a peer TU that would have to be dealt with.
 *
 * @param tu  The originating TU.
 * @param target  The target TU, or NULL if the caller of this function was unable to
 * identify a TU to be dialed.
 * @return 0 if successful, -1 if any error occurs that results in the originating
 * TU transitioning to the TU_ERROR state.
 */
// #if 0
int tu_dial(TU *tu, TU *target)
{
    debug("tu_dial");
    // if (!tu)
    // {
    //     return -1;
    // }
    // TO BE IMPLEMENTED
    // abort();
    pthread_mutex_lock(&tu->mutex);
    if (tu->state != TU_DIAL_TONE)
    {
        pthread_mutex_unlock(&tu->mutex);
        return 0;
    }
    if (!target)
    {
        set_state(tu, TU_ERROR);
        pthread_mutex_unlock(&tu->mutex);
        return -1;
    }

    pthread_mutex_unlock(&tu->mutex);
    // pthread_mutex_lock(&target->mutex);
    double_lock(&tu->mutex, &target->mutex);
    if (tu == target)
    {
        set_state(tu, TU_BUSY_SIGNAL);
        // tu->state = TU_BUSY_SIGNAL;
    }
    else if (tu->peer || target->state != TU_ON_HOOK)
    {
        // if (tu->peer)
        //     debug("busy %d", tu->ext);
        set_state(tu, TU_BUSY_SIGNAL);
        // tu->state = TU_BUSY_SIGNAL;
    }
    else
    {
        target->peer = tu;
        tu->peer = target;
        // tu->state = TU_RING_BACK;
        set_state(tu, TU_RING_BACK);
        // target->state = TU_RINGING;
        set_state(target, TU_RINGING);
        // pthread_mutex_unlock(&target->mutex);
        // pthread_mutex_unlock(&tu->mutex);

        double_unlock(&tu->mutex, &target->mutex);
        tu_ref(tu, "dialing");
        tu_ref(target, "ringing");
        return 0;
    }
    // pthread_mutex_unlock(&target->mutex);
    // pthread_mutex_unlock(&tu->mutex);

    double_unlock(&tu->mutex, &target->mutex);
    return 0;
}
// #endif

/*
 * Take a TU receiver off-hook (i.e. pick up the handset).
 *   If the TU is in neither the TU_ON_HOOK state nor the TU_RINGING state,
 *     then there is no effect.
 *   If the TU is in the TU_ON_HOOK state, it goes to the TU_DIAL_TONE state.
 *   If the TU was in the TU_RINGING state, it goes to the TU_CONNECTED state,
 *     reflecting an answered call.  In this case, the calling TU simultaneously
 *     also transitions to the TU_CONNECTED state.
 *
 * In all cases, a notification of the resulting state of the specified TU is sent to
 * to the associated network client.  If a peer TU has changed state, then its client
 * is also notified of its new state.
 *
 * @param tu  The TU that is to be picked up.
 * @return 0 if successful, -1 if any error occurs that results in the originating
 * TU transitioning to the TU_ERROR state.
 */
// #if 0
int tu_pickup(TU *tu)
{
    debug("tu_pickup");
    // TO BE IMPLEMENTED
    // abort();
    // if (!tu)
    // {
    //     return -1;
    // }
    pthread_mutex_lock(&tu->mutex);
    // if (tu->state != TU_RINGING && tu->state != TU_ON_HOOK)
    // {
    //     pthread_mutex_unlock(&tu->mutex);
    //     return 0;
    // }
    if (tu->state == TU_ON_HOOK)
    {
        set_state(tu, TU_DIAL_TONE);
        pthread_mutex_unlock(&tu->mutex);
        return 0;
        // tu->state = TU_DIAL_TONE;
    }
    else if (tu->state == TU_RINGING)
    {
        set_state(tu, TU_CONNECTED);
        // tu->state = TU_CONNECTED;

        pthread_mutex_unlock(&tu->mutex);
        // pthread_mutex_lock(&tu->peer->mutex);
        double_lock(&tu->mutex, &tu->peer->mutex);

        set_state(tu->peer, TU_CONNECTED);
        // tu->peer->state = TU_CONNECTED;
        // pthread_mutex_unlock(&tu->peer->mutex);

        double_unlock(&tu->mutex, &tu->peer->mutex);
        return 0;
    }
    pthread_mutex_unlock(&tu->mutex);
    return 0;
}
// #endif

/*
 * Hang up a TU (i.e. replace the handset on the switchhook).
 *
 *   If the TU is in the TU_CONNECTED or TU_RINGING state, then it goes to the
 *     TU_ON_HOOK state.  In addition, in this case the peer TU (the one to which
 *     the call is currently connected) simultaneously transitions to the TU_DIAL_TONE
 *     state.
 *   If the TU was in the TU_RING_BACK state, then it goes to the TU_ON_HOOK state.
 *     In addition, in this case the calling TU (which is in the TU_RINGING state)
 *     simultaneously transitions to the TU_ON_HOOK state.
 *   If the TU was in the TU_DIAL_TONE, TU_BUSY_SIGNAL, or TU_ERROR state,
 *     then it goes to the TU_ON_HOOK state.
 *
 * In all cases, a notification of the resulting state of the specified TU is sent to
 * to the associated network client.  If a peer TU has changed state, then its client
 * is also notified of its new state.
 *
 * @param tu  The tu that is to be hung up.
 * @return 0 if successful, -1 if any error occurs that results in the originating
 * TU transitioning to the TU_ERROR state.
 */
// #if 0
int tu_hangup(TU *tu)
{
    debug("tu_hangup");
    // TO BE IMPLEMENTED
    // abort();
    // if (!tu)
    // {
    //     return -1;
    // }
    pthread_mutex_lock(&tu->mutex);
    if (tu->state == TU_CONNECTED || tu->state == TU_RINGING)
    {
        debug("set state on hook");
        set_state(tu, TU_ON_HOOK);
        // if (!tu->peer)
        // {
        //     set_state(tu, TU_ERROR);
        //     return -1;
        // }
        pthread_mutex_unlock(&tu->mutex);
        // pthread_mutex_lock(&tu->peer->mutex);
        TU *peer = tu->peer;
        double_lock(&tu->mutex, &peer->mutex);
        set_state(tu->peer, TU_DIAL_TONE);
        peer->peer = NULL;
        tu->peer = NULL;
        // pthread_mutex_unlock(&peer->mutex);
        // pthread_mutex_unlock(&tu->mutex);
        double_unlock(&tu->mutex, &peer->mutex);
        tu_unref(peer, "HANGING UP");
        tu_unref(tu, "HANGING UP");
        return 0;
    }
    else if (tu->state == TU_RING_BACK)
    {
        set_state(tu, TU_ON_HOOK);
        // if (!tu->peer)
        // {
        //     set_state(tu, TU_ERROR);
        //     return -1;
        // }
        pthread_mutex_unlock(&tu->mutex);
        // pthread_mutex_lock(&tu->peer->mutex);
        TU *peer = tu->peer;
        double_lock(&tu->mutex, &peer->mutex);
        set_state(tu->peer, TU_ON_HOOK);
        peer->peer = NULL;
        tu->peer = NULL;
        // pthread_mutex_unlock(&peer->mutex);
        // pthread_mutex_unlock(&tu->mutex);
        double_unlock(&tu->mutex, &peer->mutex);
        tu_unref(peer, "HANGING UP");
        tu_unref(tu, "HANGING UP");
        return 0;
    }
    else if (tu->state == TU_DIAL_TONE || tu->state == TU_BUSY_SIGNAL || tu->state == TU_ERROR)
    {
        set_state(tu, TU_ON_HOOK);
    }

    pthread_mutex_unlock(&tu->mutex);
    return 0;
}
// #endif

/*
 * "Chat" over a connection.
 *
 * If the state of the TU is not TU_CONNECTED, then nothing is sent and -1 is returned.
 * Otherwise, the specified message is sent via the network connection to the peer TU.
 * In all cases, the states of the TUs are left unchanged and a notification containing
 * the current state is sent to the TU sending the chat.
 *
 * @param tu  The tu sending the chat.
 * @param msg  The message to be sent.
 * @return 0  If the chat was successfully sent, -1 if there is no call in progress
 * or some other error occurs.
 */
// #if 0
int tu_chat(TU *tu, char *msg)
{
    debug("tu_chat");
    // TO BE IMPLEMENTED
    // abort();

    pthread_mutex_lock(&tu->mutex);
    if (tu->state != TU_CONNECTED)
    {
        set_state(tu, tu->state);
        pthread_mutex_unlock(&tu->mutex);
        return -1;
    }
    if (!tu->peer)
    {
        set_state(tu, TU_ERROR);
        return -1;
    }
    pthread_mutex_unlock(&tu->mutex);
    double_lock(&tu->mutex, &tu->peer->mutex);
    write(tu->peer->fileno, CHAT, strlen(CHAT));
    write(tu->peer->fileno, SPACE, strlen(SPACE));
    write(tu->peer->fileno, msg, strlen(msg));
    write(tu->peer->fileno, EOL, strlen(EOL));
    set_state(tu, tu->state);
    // pthread_mutex_unlock(&tu->peer->mutex);
    // pthread_mutex_unlock(&tu->mutex);

    double_unlock(&tu->mutex, &tu->peer->mutex);

    return 0;
}
// #endif
