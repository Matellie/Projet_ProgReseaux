#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "client.h"
#include "defi.h"
#include "awale.h"

static void init(void)
{
   printf("init\n");
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

static void end(void)
{
   printf("end\n");
#ifdef WIN32
   WSACleanup();
#endif
}

static void app(void)
{
   printf("app\n");
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual = 0;
   int max = sock;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];
   /* an array for all game requests */
   ListeDefi listeDeDefis; // IL FAUDRAIT UNE HASHMAP
   listeDeDefis.actual = 0;
   /* an array for all games */
   ListeAwale listeDeAwale;
   listeDeAwale.actual = 0;

   fd_set rdfs;

   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         socklen_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if(read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = { csock };
         strncpy(c.name, buffer, BUF_SIZE - 1);
         clients[actual] = c;
         actual++;
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            /* a client is talking */
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client client = clients[i];
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */
               if(c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual);
                  strncpy(buffer, client.name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(clients, client, actual, buffer, 1);
                  send_message_to_self(client, buffer);
               }
               else
               {
                  parse_message(clients, &listeDeDefis, &listeDeAwale, client, i, buffer, actual);
                  // for(int i=0; i<listeDeDefis.actual; i++)
                  // {
                  //    printf("%d %s %s  |||  ", i, listeDeDefis.listeDefis[i]->pseudoQuiDefie, listeDeDefis.listeDefis[i]->pseudoEstDefie);
                  // }
               }
               break;
            }
         }
      }
   }

   clear_clients(clients, actual);
   end_connection(sock);
}

static void clear_clients(Client *clients, int actual)
{
   printf("clear_clients\n");
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   printf("remove_client\n");
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server)
{
   printf("send_message_to_all_clients\n");
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {
         if(from_server == 0)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
   }
}

static void send_message_to_self(Client sender, const char *buffer)
{
   printf("send_message_to_self\n");
   char message[BUF_SIZE];
   message[0] = 0;
   strncpy(message, sender.name, BUF_SIZE - 1);
   strncat(message, " : ", sizeof message - strlen(message) - 1);
   strncat(message, buffer, sizeof message - strlen(message) - 1);
   printf("%s\n",message);
}

static void send_message_to_client(Client * clients, Client sender, char *receiver, const char *buffer, int actual)
{
   printf("send_message_to_client");
   
   // Find index of the receiver
   int indexReceiver = -1;
   for(int i=0; i<actual; i++)
   {
      if(strcmp(receiver, clients[i].name) == 0)
      {
         indexReceiver = i;
         break;
      }
   }

   char message[BUF_SIZE];
   message[0] = 0;
   if(indexReceiver != -1) // Si le receiver est connecté envoyer message
   {
      strncpy(message, sender.name, BUF_SIZE - 1);
      strncat(message, " : ", sizeof message - strlen(message) - 1);
      strncat(message, buffer, sizeof message - strlen(message) - 1);
      write_client(clients[indexReceiver].sock, message);
   }
   else // Si le receiver n est pas dans la liste des gens connectés envoyer message d erreur au sender
   {
      strncpy(message, "Cette personne n'est pas connectée ! :(\n", BUF_SIZE - 1);
      write_client(sender.sock, message);
   }
}

static int init_connection(void)
{
   printf("init_connection\n");
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   printf("end_connection\n");
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
{
   printf("read_client\n");
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

static void write_client(SOCKET sock, const char *buffer)
{
   printf("write_client\n");
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

// static void play_awale_move(Client client, int slot)
// {
//    // Recuperer la bonne partie
//    // Jouer le coup
//    // Si coup ok et bon sender -> renvoyer nouveau plateau
//    // Sinon -> renvoyer plateau et message erreur
// }

static void parse_message(Client * clients, ListeDefi * defis, ListeAwale * awales, Client sender, int indexClient, char *buffer, int actual)
{
   printf("parse_message\n");
   char * cmd = strtok(buffer, " ");

   if(strcmp(cmd, "HELP") == 0)
   {
      /* 
      Format de la requete:
      HELP
      */
      char * message = "Commandes valides:\nHELP, LISTE_PSEUDO, SET_BIO, GET_BIO, CHAT, DEFIER, ACCEPTER, REFUSER, JOUER";
      write_client(sender.sock, message);
   }
   else if(strcmp(cmd, "LISTE_PSEUDO") == 0)
   {
      /* 
      Format de la requete:
      LISTE_PSEUDO
      */
      char message[BUF_SIZE];
      message[0] = 0;
      for(int i=0; i<actual; i++)
      {
         strncat(message, clients[i].name, sizeof message - strlen(message) - 1);
         strncat(message, ", ", sizeof message - strlen(message) - 1);
      }
      write_client(sender.sock, message);
   }
   else if(strcmp(cmd, "SET_BIO") == 0)
   {
      /* 
      Format de la requete:
      SET_BIO texte_bio
      */
      char * texte_bio = strtok(NULL, "\0");

      // Security in case not enough arguments have been provided
      if (texte_bio == NULL){
         write_client(clients[indexClient].sock, "Format invalide !\nFormat : SET_BIO [text_bio]");
         return;
      }

      strncpy(clients[indexClient].bio, texte_bio, BUF_SIZE - 1);
   }
   else if(strcmp(cmd, "GET_BIO") == 0)
   {
      /* 
      Format de la requete:
      GET_BIO pseudo
      */
      char * pseudo = strtok(NULL, "\0");

      // Security in case not enough arguments have been provided
      if (pseudo == NULL){
         write_client(clients[indexClient].sock, "Format invalide !\nFormat : GET_BIO [pseudo]");
         return;
      }

      // Find index of the pseudo
      int indexPseudo = -1;
      for(int i=0; i<actual; i++)
      {
         if(strcmp(pseudo, clients[i].name) == 0)
         {
            indexPseudo = i;
            break;
         }
      }

      char message[BUF_SIZE];
      message[0] = 0;
      if(indexPseudo != -1)
      {
         strncpy(message, clients[indexPseudo].bio, BUF_SIZE - 1);
         write_client(sender.sock, message);
      }
      else
      {
         strncpy(message, "Cette personne n'est pas connectée ! :(", BUF_SIZE - 1);
         write_client(sender.sock, message);
      }
   }
   else if(strcmp(cmd, "CHAT") == 0)
   {
      /* 
      Format de la requete:
      CHAT pseudo message
      */
      char * receiver = strtok(NULL, " ");
      char * message = strtok(NULL, "\0");

      // Security in case not enough arguments have been provided
      if (receiver == NULL || receiver == NULL){
         write_client(clients[indexClient].sock, "Format invalide !\nFormat : CHAT [pseudo] [message]");
         return;
      }

      send_message_to_client(clients, sender, receiver, message, actual);
   }
   else if(strcmp(cmd, "DEFIER") == 0)
   {
      /* 
      Format de la requete:
      DEFIER pseudo
      */
      char * pseudo = strtok(NULL, "\0");

      // Security in case not enough arguments have been provided
      if (pseudo == NULL){
         write_client(clients[indexClient].sock, "Format invalide !\nFormat : DEFIER [pseudo]");
         return;
      }

      // Find index of the pseudo
      int indexPseudo = -1;
      for(int i=0; i<actual; i++)
      {
         if(strcmp(pseudo, clients[i].name) == 0)
         {
            indexPseudo = i;
            break;
         }
      }

      char message[BUF_SIZE];
      message[0] = 0;
      if(indexPseudo != -1)
      {
         strncpy(message, "!!! ", BUF_SIZE - 1);
         strncat(message, sender.name, sizeof message - strlen(message) - 1);
         strncat(message, " vous défie au jeu d'Awale !!!\nRépondez par 'ACCEPTER ", sizeof message - strlen(message) - 1);
         strncat(message, sender.name, sizeof message - strlen(message) - 1);
         strncat(message, "' pour lancer la partie\nRépondez par 'REFUSER ", sizeof message - strlen(message) - 1);
         strncat(message, sender.name, sizeof message - strlen(message) - 1);
         strncat(message, "' pour refuser la partie", sizeof message - strlen(message) - 1);
         write_client(clients[indexPseudo].sock, message);

         // Créer un défi
         Defi * newDefi = malloc(sizeof(Defi));
         strncpy(newDefi->pseudoQuiDefie, sender.name, BUF_SIZE - 1);
         strncpy(newDefi->pseudoEstDefie, pseudo, BUF_SIZE - 1);
         defis->listeDefis[defis->actual] = newDefi;
         (defis->actual)++;
      }
      else
      {
         strncpy(message, "Cette personne n'est pas connectée ! :(", BUF_SIZE - 1);
         write_client(sender.sock, message);
      }
   }
   else if(strcmp(cmd, "ACCEPTER") == 0)
   {
      /* 
      Format de la requete:
      ACCEPTER pseudo
      */
      char * pseudo = strtok(NULL, "\0");

      // Security in case not enough arguments have been provided
      if (pseudo == NULL){
         write_client(clients[indexClient].sock, "Format invalide !\nFormat : ACCEPTER [pseudo]");
         return;
      }

      // Find index of the pseudo
      int indexPseudo = -1;
      for(int i=0; i<actual; i++)
      {
         if(strcmp(pseudo, clients[i].name) == 0)
         {
            indexPseudo = i;
            break;
         }
      }

      char message[BUF_SIZE];
      message[0] = 0;
      // Si la personne qui défie est connectée
      if(indexPseudo != -1)
      {
         // Find index of the defi
         int indexDefi = -1;
         for(int i=0; i<defis->actual; i++)
         {
            if(strcmp(pseudo, defis->listeDefis[i]->pseudoQuiDefie) == 0 && 
               strcmp(sender.name, defis->listeDefis[i]->pseudoEstDefie) == 0)
            {
               indexDefi = i;
               break;
            }
         }

         // Si le défi existe bien
         if(indexDefi != -1)
         {
            strncpy(message, "!!! Defi de ", BUF_SIZE - 1);
            strncat(message, pseudo, sizeof message - strlen(message) - 1);
            strncat(message, " accepté par ", sizeof message - strlen(message) - 1);
            strncat(message, sender.name, sizeof message - strlen(message) - 1);
            strncat(message, " !!! La partie va bientôt commencer !", sizeof message - strlen(message) - 1);
            write_client(sender.sock, message);
            write_client(clients[indexPseudo].sock, message);

            // Créer le jeu d'Awale
            AwaleGame * newAwale = malloc(sizeof(AwaleGame));
            // TODO : PLAYER 1 ET PLAYER 2 ALEATOIRE
            strcpy(newAwale->player1, sender.name);
            strcpy(newAwale->player2, pseudo);
            createGame(newAwale);

            // Mettre le jeu dans la liste de parties
            awales->listeAwales[awales->actual] = newAwale;
            (awales->actual)++;

            // Détruire le défi
            free(defis->listeDefis[indexDefi]);
            memmove(defis->listeDefis + indexDefi, defis->listeDefis + indexDefi + 1, (defis->actual - indexDefi - 1) * sizeof(*(defis->listeDefis)));
            (defis->actual)--;

            /* Lancement de la partie */
            char * gameString = malloc(sizeof(char)*BUF_SIZE);
            gameToString(newAwale, gameString);
            write_client(sender.sock, gameString);
            write_client(clients[indexPseudo].sock, gameString);
            free(gameString);
         }
         else
         {
            strncpy(message, "Cette personne ne vous a pas défié ! xP", BUF_SIZE - 1);
            write_client(sender.sock, message);
         }
      }
      else
      {
         strncpy(message, "Cette personne n'est pas connectée ! :(", BUF_SIZE - 1);
         write_client(sender.sock, message);
      }
   }
   else if(strcmp(cmd, "REFUSER") == 0)
   {
      /* 
      Format de la requete:
      REFUSER pseudo
      */
      char * pseudo = strtok(NULL, "\0");
      
      // Security in case not enough arguments have been provided
      if (pseudo == NULL){
         write_client(clients[indexClient].sock, "Format invalide !\nFormat : REFUSER [pseudo]");
         return;
      }

      // Find index of the defi
      int indexDefi = -1;
      for(int i=0; i<defis->actual; i++)
      {
         if(strcmp(pseudo, defis->listeDefis[i]->pseudoQuiDefie) == 0 && 
            strcmp(sender.name, defis->listeDefis[i]->pseudoEstDefie) == 0)
         {
            indexDefi = i;
            break;
         }
      }

      char message[BUF_SIZE];
      message[0] = 0;
      // Si le défi existe
      if(indexDefi != -1)
      {
         // Détruire le défi
         free(defis->listeDefis[indexDefi]);
         memmove(defis->listeDefis + indexDefi, defis->listeDefis + indexDefi + 1, (defis->actual - indexDefi - 1) * sizeof(*(defis->listeDefis)));
         (defis->actual)--;

         strncpy(message, "Le défi a été réduit au silence ! :)", BUF_SIZE - 1);
         write_client(sender.sock, message);

         // Find index of the pseudo
         int indexPseudo = -1;
         for(int i=0; i<actual; i++)
         {
            if(strcmp(pseudo, clients[i].name) == 0)
            {
               indexPseudo = i;
               break;
            }
         }

         if(indexPseudo != -1)
         {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " a rejeté votre défi ! ^^'", sizeof message - strlen(message) - 1);
            write_client(clients[indexPseudo].sock, message);
         }
      }
      else
      {
         strncpy(message, "Ce défi n'existe pas ! :O", BUF_SIZE - 1);
         write_client(sender.sock, message);
      }
   }
   else if(strcmp(cmd, "JOUER") == 0)
   {
      /* 
      Format de la requete:
      JOUER slot
      */
      
   } 
   else if(strcmp(cmd, "LISTE_DEFI") == 0)
   {
      /* 
      Format de la requete:
      LISTE_DEFI
      */
      char message[BUF_SIZE];
      message[0] = 0;
      strncat(message, "Personne qui defie -> Personne defiee\n", sizeof message - strlen(message) - 1);
      for(int i=0; i<defis->actual; i++)
      {
         strncat(message, defis->listeDefis[i]->pseudoQuiDefie, sizeof message - strlen(message) - 1);
         strncat(message, " -> ", sizeof message - strlen(message) - 1);
         strncat(message, defis->listeDefis[i]->pseudoEstDefie, sizeof message - strlen(message) - 1);
         strncat(message, "\n", sizeof message - strlen(message) - 1);
      }
      write_client(sender.sock, message);
   }
   else if(strcmp(cmd, "LISTE_PARTIE") == 0)
   {
      printf("\nListe Partie\n");
      /* 
      Format de la requete:
      LISTE_PARTIE
      */
      char message[BUF_SIZE];
      message[0] = 0;
      for(int i=0; i<awales->actual; i++)
      {
         strncat(message, "Partie ", sizeof message - strlen(message) - 1);
         char * number = malloc(10*sizeof(char));
         sprintf(number,"%d",i);
         strncat(message, number, sizeof message - strlen(message) - 1);
         free(number);
         strncat(message, " : ", sizeof message - strlen(message) - 1);
         strncat(message, awales->listeAwales[i]->player1, sizeof message - strlen(message) - 1);
         strncat(message, " vs ", sizeof message - strlen(message) - 1);
         strncat(message, awales->listeAwales[i]->player2, sizeof message - strlen(message) - 1);
         strncat(message, "\n", sizeof message - strlen(message) - 1);
      }
      write_client(sender.sock, message);
   }
   else
   {
      write_client(sender.sock, "Commande non reconnue - Entrer HELP pour voir la liste des commandes disponibles\n");
      printf("%s : CMD non reconnue\n", sender.name);
   }
}

int main(int argc, char **argv)
{
   printf("main\n");
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
