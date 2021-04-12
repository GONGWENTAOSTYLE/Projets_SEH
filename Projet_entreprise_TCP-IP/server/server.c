#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "server.h"

#define IP "127.0.0.1" //Define the IP of current server
#define PORT 1314  // Define the communication port
#define MAX_CLIENTS     100 //Max number of client

/* Initialize the DLL required for the sock operation */
static void init(void)
{
#ifdef WIN32 //  If the compiler is WIN32, not needed in Linux
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }

#endif
}
/* End the operation */
static void end(void)
{
#ifdef WIN32 //  If the compiler is WIN32, not needed in Linux
   WSACleanup();
   printf("0");
#endif
}
/* Initialize the server  */
static int init_connection(void)
{
    /* Create server socket */
   SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }
   /* Set server information */
   memset(&sin, 0, sizeof(sin)); // Clear the structure sin
   sin.sin_addr.s_addr = inet_addr(IP); // IP
   sin.sin_port = htons(PORT); // port
   sin.sin_family = AF_INET; // Use IPv4 address

   /* Bind socket */
   if(bind(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   /* Start monitoring */
   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

/* End the connection with a client */
static void end_connection(int sock)
{
   closesocket(sock);
}

/* Clear all clients */
static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      end_connection(clients[i].sock);
   }
}

/* Remove the client that has disconnected from the array of client */
static void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

/* Read data from client  */
static int read_client(SOCKET sock, char *buffer)
{
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

/* Print the messages from client in the terminal  */
static int print_client(Client client, char *buffer)
{
   int i = 0;

   char message[BUF_SIZE];
   message[0] = 0;
   strncpy(message, client.name, BUF_SIZE - 1);
   strncat(message, " : ", sizeof message - strlen(message) - 1);
   strncat(message, buffer, sizeof message - strlen(message) - 1);
   printf( "%s\n", message);
   memset(message, 0, sizeof(message));
}

/* Send data to the client*/
static void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}
/* Send the data from a certain client to others*/
static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer)
{
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock)
      {

         strncpy(message, sender.name, BUF_SIZE - 1);
         strncat(message, " : ", sizeof message - strlen(message) - 1);

         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);
      }
   }
}

/* Send the data to a certain client */
static void send_message_to_a_client(Client *clients, Client sender, Client receiver, int actual, const char *buffer)
{
   int i = 0;
   char message[BUF_SIZE];
   const char *err = "This client has disconnected.\n";
   message[0] = 0;
   for(i = 0; i < actual; i++)
   {
      /* we don't send message to the sender */
      if(receiver.sock == clients[i].sock)
      {
         strncpy(message, sender.name, BUF_SIZE - 1);
         strncat(message, " : ", sizeof message - strlen(message) - 1);

         strncat(message, buffer, sizeof message - strlen(message) - 1);
         write_client(clients[i].sock, message);

         break;
      }
      if (i == actual)
      {
          write_client(sender.sock, err);
      }
   }
}

/* Main operation of server*/
static void server(void)
{
    SOCKET sock = init_connection();
    char buffer[BUF_SIZE];
    char message[BUF_SIZE];
    memset(buffer, 0, sizeof(buffer));
    memset(message, 0, sizeof(buffer));

    /* the index for the array */
    int actual = 0;
    int max = sock;
    /* an array for all clients and their receiver */
    Client clients[MAX_CLIENTS];
    Client receivers[MAX_CLIENTS];

    fd_set rdfs;
    struct timeval timeout={3,0};//


    while(1)
    {
        int i = 0;
        FD_ZERO(&rdfs);
        #ifdef linux
        FD_SET(STDIN_FILENO, &rdfs);
        #endif
        /* add the connection socket */
        FD_SET(sock, &rdfs);

        /* add socket of each client */
        for(i = 0; i < actual; i++)
        {
            FD_SET(clients[i].sock, &rdfs);
        }
        if(select(max + 1, &rdfs, NULL, NULL, &timeout) == -1)
        {
            perror("select()");
            exit(errno);
        }
        /*In windows system, Ctrl + C to stop the server
        In Linux stop process when type on keyboard "quit"*/
        #ifdef linux
        if(FD_ISSET(STDIN_FILENO, &rdfs))
        {
            fgets(buffer, BUF_SIZE - 1, stdin);
            {
                char *p = NULL;
                p = strstr(buffer, "\n");
                if(p != NULL)
                {
                    *p = 0;
                }
                else
                {
               /* fclean */
                    buffer[BUF_SIZE - 1] = 0;
                }
            }
            if(strncmp(buffer,"quit",4) == 0) break;
            memset(buffer, 0, sizeof(buffer));
        }
        else
        #endif
        if(FD_ISSET(sock, &rdfs))
        {
            /* new client */
            SOCKADDR_IN csin = { 0 };
            int sinsize = sizeof csin;
            int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
            if(csock == SOCKET_ERROR)
            {
                perror("accept()");
            }
            /* after connecting, the client sends its name */
            if(read_client(csock, buffer) == -1)
            {
                /* disconnected */
                continue;
            }

            /* what is the new maximum fd ? */
            max = csock > max ? csock : max;

            FD_SET(csock, &rdfs);

            Client c = { csock };
            strncpy(c.name, buffer, BUF_SIZE - 1); // Remember client's name
            c.state = 0;
            c.VIP = 0;
            printf(" %s connected! \n ", buffer);
            clients[actual] = c;
            memset(buffer, 0, sizeof(buffer));
            receivers[actual].sock = 0;
            memset((char *)&(receivers[actual].name), 0, sizeof(receivers[actual].name));
            actual++;
        }
        else
        {
            int i = 0;
            for(i = 0; i < actual; i++)
            {
                /*If a client is talking */
                if(FD_ISSET(clients[i].sock, &rdfs))
                {

                    Client client = clients[i];
                    int c = read_client(clients[i].sock, buffer);
                    /* If client disconnected */
                    if(c == 0 || strncmp(buffer,"disconnect",10) == 0)
                    {
                        if(c == 0)
                            printf(" %s disconnected abnormally! \n ", client.name);
                        else
                            printf(" %s disconnected normally! \n ", client.name);
                        /* All disconnected clients can't be the receiver anymore */
                        int n = 0;
                        for(n = 0; n < actual; n++)
                        {
                            if ( receivers[n].sock = clients[i].sock )
                            {
                                receivers[n].sock = 0;
                                memset((char *)&(receivers[n].name), 0, sizeof(receivers[n].name));
                                clients[n].state = 0;
                            }
                        }
                         /* to remove disconnected client and tell all other clients that someone disconnected */
                        closesocket(clients[i].sock);
                        remove_client(clients, i, &actual); // Remove disconnected client
                        strncpy(buffer, client.name, BUF_SIZE - 1);
                        strncat(buffer, " disconnected! \n", BUF_SIZE - strlen(buffer) - 1);
                        send_message_to_all_clients(clients, client, actual, buffer);
                        memset(buffer, 0, sizeof(buffer));
                    }
                     /* If a client need help or in urgent situation, send the urgent message to all clients */
                    else if (strncmp(buffer,"urgent",6) == 0 || strncmp(buffer,"help",4) == 0)
                    {
                        print_client(client,buffer);
                        strncpy(message, client.name, BUF_SIZE - 1);
                        strncat(message, " need help!\n", BUF_SIZE - strlen(message) - 1);
                        send_message_to_all_clients(clients, client, actual, message);
                        memset(message, 0, sizeof(message));
                        memset(buffer, 0, sizeof(buffer));
                        break;
                    }

                    /* If a client enter the correct code "IMTAtlantiquestudentsarethebest", then he can become a VIP client.
                    As a VIP client, he get the authority to send his messages to all connected clients */
                    else if (strncmp(buffer,"I want to be a VIP\n",18) == 0)
                    {
                        print_client(client,buffer);
                        clients[i].VIP = 1; //To be the VIP state
                        const char *toVIP = "You have entered VIP status and can send yous messages to everyone and stop the server. ";
                        write_client(clients[i].sock, toVIP);

                    }

                    else
                   {
                        print_client(client,buffer);
                        if (clients[i].VIP == 1)
                        {
                            if(strncmp(buffer,"quit",4) == 0)
                            {
                               clients[i].VIP = 0; //Quit VIP state
                               const char *quitVIP = "You have quit the VIP status. \n";
                               write_client(clients[i].sock, quitVIP);
                            }
                            else if (strncmp(buffer,"stop the server",15) == 0 )
                            {
                                clear_clients(clients, actual);
                                end_connection(sock);
                            }

                            else send_message_to_all_clients(clients, client, actual, buffer);
                            memset(buffer, 0, sizeof(buffer));

                        }

                        /* If the state of client is 0, it means he hasn't specify his receiver or his receiver has
                        disconnected. The receiver should be a connected client. If the state is 1, then the client can send
                         messages to his receiver. Clients can say "bye" to return to the state "0" and specify other receiver. */

                        else if (client.state == 0)
                       {
                            int j = 0;
                            for(j=0; j < actual; j++)
                            {
                                if (strcmp(buffer, clients[j].name) == 0&&strcmp(buffer, client.name) != 0)
                                {
                                     receivers[i] = clients[j];
                                     clients[i].state = 1;  /* Specify the receiver */
                                     const char *to = "Client's name correct, now you can send your messages to that client. \n";
                                     write_client(clients[i].sock, to);
                                     memset(buffer, 0, sizeof(buffer));
                                     break;
                                }
                            }
                            if (j == actual)
                            {
                                const char *ask_name = "This client doesn't exist or has disconnected, please enter again. \n";
                                write_client(clients[i].sock, ask_name);
                                memset(buffer, 0, sizeof(buffer));
                            }
                       }
                       else
                         /* The client says "bye" to return to the state "0" and can specify other receiver */
                       {
                           if (strncmp(buffer,"bye",6) == 0)
                           {
                               clients[i].state = 0;
                               send_message_to_a_client(clients,client,receivers[i],actual,buffer);
                               receivers[i].sock = 0;
                               memset((char *)&(receivers[i].name), 0, sizeof(receivers[i].name));
                           }
                           send_message_to_a_client(clients,client,receivers[i],actual,buffer);
                           memset(buffer, 0, sizeof(buffer));
                       }
                    }
                    break;
                }
            }
        }
    }
    clear_clients(clients, actual);
    end_connection(sock);
}

int main(int argc, char **argv)
{
    init();

    server();

    end();

    return EXIT_SUCCESS;
}
