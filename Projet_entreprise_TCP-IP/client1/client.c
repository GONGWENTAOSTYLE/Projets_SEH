#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "client.h"

#define IP "127.0.0.1" //Define the server IP to connect
#define PORT 1314
#define BUF_SIZE 1024
const char *name = "Robin"; // name of this client (eg Luffy)

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
#ifdef WIN32
   WSACleanup();
   printf("0");
#endif
}

/* Initialize the client  */
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
   memset(&sin, 0, sizeof(sin));
   sin.sin_addr.s_addr = inet_addr(IP);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   /* Connect the target server */
   if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
   {
      perror("connect()");
      exit(errno);
   }
   else
   {
      printf("Connected to a server!\n");
   }

   return sock;
}

/* End the connection */
static void end_connection(int sock)
{
   closesocket(sock);
}

/* Read the returned data from the server */
static int read_server(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      exit(errno);
   }

   buffer[n] = 0;

   return n;
}

/* Send data to the server */
static void write_server(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

/* Main operation of client*/
static void client(void)
{
    SOCKET sock = init_connection();
    char buffer[BUF_SIZE];
    memset(buffer, 0, sizeof(buffer));
    fd_set rdfs;
    /* Send the name of client to server*/
    write_server(sock, name);
    struct timeval timeout={2,0};
 while(1)
    {

        FD_ZERO(&rdfs);
        #ifdef linux
       /* add STDIN_FILENO */
        FD_SET(STDIN_FILENO, &rdfs);
        #endif
        /* add the socket */
        FD_SET(sock, &rdfs);

        if(select(sock + 1, &rdfs, NULL, NULL, &timeout) == -1)
        {
            perror("select()");
            exit(errno);
        }
         memset(buffer, 0, sizeof(buffer));
        /* Input From keyboard*/
        #ifdef WIN32
        if(kbhit())
        {
            gets(buffer);
        }
        write_server(sock, buffer);
        if(strncmp(buffer,"disconnect",10) == 0) break;
        memset(buffer, 0, sizeof(buffer));
        #elif defined (linux)
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
            write_server(sock, buffer);
            if(strncmp(buffer,"disconnect",10) == 0) break;
            memset(buffer, 0, sizeof(buffer));
        }
        else
        #endif

        if(FD_ISSET(sock, &rdfs))
        {
            int n = read_server(sock, buffer);
            if(n == 0)
            {
                printf("Server disconnected!\n");
                break;
            }
            puts(buffer);

        }
    }
    end_connection(sock);
}

int main(int argc, char **argv)
{

    init();

    client();

    end();


    return EXIT_SUCCESS;
}
