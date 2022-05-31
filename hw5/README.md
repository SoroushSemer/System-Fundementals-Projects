# HW5 - ```PBX``` TELEPHONE SYSTEM #

```pbx``` is a server that simulates the behavior of a Private Branch Exchange Telephone System.

The goal of this project was to gain an understanding of the following:
  * socket programming
  * thread execution
  * mutexes and semaphores
  * POSIX threads
  * the design of concurrent data structures

## Guide to ```pbx``` ##

### Commands from Client to Server ###

  * pickup
  * hangup
  * dial #, where # is the number of the extension to be dialed.
  * chat ...arbitrary text...

### Responses from Server to Client ###

  * ON HOOK #, where # reports the extension number of the client.
  * RINGING
  * DIAL TONE
  * RING BACK
  * CONNECTED #, where # is the number of the extension to which the connection exists.
  * ERROR
  * CHAT ...arbitrary text...
