#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct client_args
{
    SOCKET sock;
    struct sockaddr_in service;
};

void *handle_client_conn(void *client_args)
{
    struct client_args *args = client_args;
    SOCKET client_sock = args->sock;
    struct sockaddr_in client_service = args->service;
    free(args);

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_service.sin_addr, client_ip, sizeof(client_ip));
    unsigned short port = ntohs(client_service.sin_port);
    printf("Accepted connection from: %s:%hu\n", client_ip, port);

    // echo loop
    char buffer[512];
    int len = recv(client_sock, buffer, sizeof(buffer), 0);
    while (len > 0)
    {
        send(client_sock, buffer, len, 0);
        len = recv(client_sock, buffer, sizeof(buffer), 0);
    }

    printf("Client %s:%hu disconnected\n", client_ip, port);
    closesocket(client_sock);
    return NULL;
}

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
        // wait for a connection
        struct sockaddr_in client_service;
        int addr_len = sizeof(client_service);
        SOCKET client_sock = accept(sock, (struct sockaddr *)&client_service, &addr_len);
        if (client_sock == INVALID_SOCKET)
        {
            printf("Error during socket accept\n");
            break;
        }

        // allocate args on heap to send to thread
        struct client_args *args = malloc(sizeof(struct client_args));
        if (!args)
        {
            closesocket(client_sock);
            continue;
        }
        args->sock = client_sock;
        args->service = client_service;

        // create client connection thread;
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client_conn, args) != 0)
        {
            printf("Error creating client connection thread");
            closesocket(client_sock);
            free(args);
            continue;
        }
        pthread_detach(thread_id);
    }

    WSACleanup();
    return 0;
}