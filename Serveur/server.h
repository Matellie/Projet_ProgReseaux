#ifndef SERVER_H
#define SERVER_H

#ifdef WIN32

#include <winsock2.h>

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF        "\r\n"
#define PORT        1977
#define MAX_CLIENTS     15

#define BUF_SIZE    1024

#include "client.h"
#include "defi.h"
#include "awale.h"

static void init(void);
static void end(void);
static void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(Client *clients, Client client, int actual, const char *buffer, char from_server);
static void send_message_to_self(Client client, const char *buffer);
static void send_message_to_client(Client * clients, Client sender, char *receiver, const char *buffer, int actual);
static void remove_client(Client *clients, ListeDefi *defis, ListeAwale *awales, int to_remove, int *actual);
static void clear_clients(Client *clients, int actual);
static void parse_message(Client * clients, ListeDefi * defis, ListeAwale * awales, Client sender, int indexClient, char *buffer, int actual);
int checkIndexPartie(ListeAwale * awales, char joueur1[BUF_SIZE], char joueur2[BUF_SIZE]);
int checkIndexDefi(ListeDefi * defis, char joueur1[BUF_SIZE], char joueur2[BUF_SIZE]);
int checkIndexClient(Client * clients, int actual, char pseudo[BUF_SIZE]);

#endif /* guard */
