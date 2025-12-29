#include <winsock2.h>
#include <ws2tcpip.h>
#include <pthread.h>
#include <stdio.h>

#define SERVER_PORT 80

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

    const char *body = "Mimine"; // TODO: change to actual file
    char res_buffer[256];
    snprintf(
        res_buffer,
        sizeof(res_buffer),
        "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %zu\r\n\r\n%s",
        strlen(body), body);
    send(client_sock, res_buffer, strlen(res_buffer), 0);

    printf("Closing connection: %s:%hu\n", client_ip, port);
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
    service.sin_port = htons(SERVER_PORT);
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