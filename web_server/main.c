#include <winsock2.h>
#include <ws2tcpip.h>
#include <pthread.h>
#include <stdio.h>

#define SERVER_PORT 80
#define MAX_REQUEST_SIZE 8192

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

    // loop to fill buffer until http request end marker (\r\n\r\n)
    char buffer[MAX_REQUEST_SIZE];
    int pos = 0;
    int request_success = 0;
    while (1)
    {
        if (pos >= MAX_REQUEST_SIZE - 1)
        {
            break;
        }

        int len = recv(client_sock, buffer + pos, MAX_REQUEST_SIZE - pos, 0);
        if (len <= 0)
        {
            break;
        }
        int last_pos = pos;
        pos += len;

        // check for http request end marker
        // start scanning at last pos - 3, in case last 3 bytes of
        // previous request were \r\n\r and the current first is \n,
        // which would mean the request header is fully read
        int http_header_complete = 0;
        int start = last_pos - 3;
        if (start < 0)
        {
            start = 0;
        }
        for (int i = start; i + 3 < pos; i++)
        {
            if (buffer[i] == '\r' &&
                buffer[i + 1] == '\n' &&
                buffer[i + 2] == '\r' &&
                buffer[i + 3] == '\n')
            {
                http_header_complete = 1;
                break;
            }
        }
        if (http_header_complete)
        {
            request_success = 1;
            break;
        }
    }
    if (!request_success)
    {
        printf("request invalid\n");
        char *error_message = "HTTP/1.1 400 Not Found";
        send(client_sock, error_message, strlen(error_message), 0);
        closesocket(client_sock);
        return NULL;
    }

    // TODO: implement HTTP request parsing
    // (check that the request is )

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