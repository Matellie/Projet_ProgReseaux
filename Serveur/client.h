#ifndef CLIENT_H
#define CLIENT_H

#include "server.h"

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   char bio[1000];
}Client;

#endif /* guard */
