#include <winsock2.h>
#include <ws2tcpip.h>
#include <unistd.h>
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

void tcp_server()
{
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
    service.sin_addr.s_addr = htonl(INADDR_ANY);
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
        exit(-1);
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
}

void udp_server()
{
    // TODO: try to make this work with IPV6 (ncat default)
    // ipv4, tcp
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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
    service.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (SOCKADDR *)&service, sizeof(service)) == SOCKET_ERROR)
    {
        printf("Error during socket bind\n");
        closesocket(sock);
        WSACleanup();
        exit(-1);
    }

    // loop for connections
    while (1)
    {
        struct sockaddr client_service;
        int from_len = sizeof(client_service);
        char buffer[512];

        int len = recvfrom(sock, buffer, sizeof(buffer), 0, &client_service, &from_len);
        sendto(sock, buffer, len, 0, &client_service, from_len);
    }

    WSACleanup();
}

int main(int argc, char *argv[])
{
    int option;
    int flag_t = 0;
    int flag_u = 0;
    int both_flags = 0;

    while ((option = getopt(argc, argv, "tu")) != -1)
    {
        switch (option)
        {
        case 't':
            flag_t = 1;
            break;
        case 'u':
            flag_u = 1;
            break;
        }
    }
    both_flags = flag_t && flag_u;

    if (argc - optind != 0)
    {
        printf("You must not provide anything other than the -u or -t parameters.\n");
        exit(-1);
    }
    if (!flag_t && !flag_u)
    {
        printf("You must specify whether to use TCP (-t) or UDP (-u).\n");
        exit(-1);
    }
    if (both_flags)
    {
        printf("Cannot use both TCP and UDP.\n");
        exit(-1);
    }

    // init WSA (windows socket api)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
    {
        printf("Error starting up WSA\n");
        exit(-1);
    }

    if (flag_t)
    {
        tcp_server();
    }
    else if (flag_u)
    {
        udp_server();
    }

    return 0;
}