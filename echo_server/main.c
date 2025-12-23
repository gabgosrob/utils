#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // init WSA (windows socket api)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
    {
        printf("Error starting up WSA\n");
        exit(-1);
    }

    // ipv4, tcp
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        printf("Error during socket initialization\n");
        WSACleanup();
        exit(-1);
    }

    // bind socket to localhost on port 7
    struct sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(7);
    inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
    if (bind(sock, (SOCKADDR *)&service, sizeof(service)) == SOCKET_ERROR)
    {
        printf("Error during socket bind\n");
        closesocket(sock);
        WSACleanup();
        exit(-1);
    }

    // set socket to listen
    if (listen(sock, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Error setting socket to listen\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // loop for connections
    while (1)
    {
        printf("Waiting for connections...\n");
        struct sockaddr_in client_service;
        int addr_len = sizeof(client_service);
        SOCKET client = accept(sock, (struct sockaddr *)&client_service, &addr_len);
        if (client == INVALID_SOCKET)
        {
            printf("Error during socket accept\n");
            break;
        }
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, (struct sockaddr *)&client_service.sin_addr, client_ip, sizeof(client_ip));
        unsigned short port = ntohs(client_service.sin_port);

        printf("Accepted connection from: %s:%hu\n", client_ip, port);
        closesocket(client);
    }

    WSACleanup();
    return 0;
}