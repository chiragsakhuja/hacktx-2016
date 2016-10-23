#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT  0x501

#include "windows.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "json/json.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define BUFLEN 512
#define PORT 1221

class Client
{
    public:

        struct sockaddr_in si_other;
        int s, slen;
        struct in_addr addr;
        WSADATA wsa;
        void send_message(char * buf)
        {
            //send the message
            if (sendto(s, buf, strlen(buf) , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
            {
                printf("sendto() failed with error code : %d" , WSAGetLastError());
                exit(EXIT_FAILURE);
            }

        }

        void recieve_message(char * buf, int len)
        {
            if (recvfrom(s, buf, len, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
            {
                printf("recvfrom() failed with error code : %d" , WSAGetLastError());
                exit(EXIT_FAILURE);
            }
        }

        Client()
        {
            //Initialise winsock
            slen = sizeof(si_other);
            printf("\nInitialising Winsock...");
            if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
            {
                printf("Failed. Error Code : %d",WSAGetLastError());
                exit(EXIT_FAILURE);
            }
            printf("Initialised.\n");

            //create socket
            if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
            {
                printf("socket() failed with error code : %d" , WSAGetLastError());
                exit(EXIT_FAILURE);
            }

            //setup address structure
            memset((char *) &si_other, 0, sizeof(si_other));
            si_other.sin_family = AF_INET;
            si_other.sin_port = htons(PORT);
            si_other.sin_addr.S_un.S_addr = inet_addr("45.20.198.107");

        }

        ~Client()
        {
            closesocket(s);
            WSACleanup();
        }
};

int main(int argc, char **argv) 
{

    Client c;

    char buf[BUFLEN];
    char message[BUFLEN];
    
    //start communication
    sprintf(message, "hel0\n");
    c.send_message(message);
    while(1)
    {
        memset(buf,'\0', BUFLEN);
        c.recieve_message(buf, 100);

        puts(buf);

        Sleep(5000);
    }

    return 0;
}

