#ifndef CLIENT_H
#define CLIENT_H

#ifdef WIN32

#include <winsock2.h>
#include <conio.h>
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

#else/* sinon vous êtes sur une plateforme non supportée */

#error not defined for this platform

#endif
#endif // CLIENT_H
