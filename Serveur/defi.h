#ifndef LISTEDEFI_H
#define LISTEDEFI_H

#include "server.h"

#define MAX_DEFIS     100

typedef struct
{
   char pseudoQuiDefie[BUF_SIZE];
   char pseudoEstDefie[BUF_SIZE];
}Defi;

typedef struct
{
   Defi * listeDefis[MAX_DEFIS];
   int actual;
}ListeDefi;

#endif /* guard */