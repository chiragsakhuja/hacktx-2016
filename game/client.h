#ifndef __CLIENT_H__
#define __CLIENT_H_

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

        int recieve_message(char * buf, int len)
        {
            int num_bytes = recvfrom(s, buf, len, 0, (struct sockaddr *) &si_other, &slen);
            if (num_bytes == SOCKET_ERROR)
            {
                printf("recvfrom() failed with error code : %d" , WSAGetLastError());
                exit(EXIT_FAILURE);
            }
            return num_bytes;
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

#endif
