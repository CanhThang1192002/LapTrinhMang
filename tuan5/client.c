#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./client <IPAddress> <PortNumber>\n");
        return 1;
    }

    char *ipAddress = argv[1];
    int port = atoi(argv[2]);
    int sockfd;
    struct sockaddr_in server;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ipAddress);
    char request[1024];
    socklen_t len = sizeof(server);
    while (1)
    {
        memset(request, 0, sizeof(request));
        fgets(request, sizeof(request), stdin);
        request[strlen(request) - 1] = '\0';
        if (strlen(request) == 0 || strcmp(request, "\n") == 0)
        {
            break;
        }
        sendto(sockfd, request, sizeof(request), 0, (struct sockaddr *)&server, len);

        char response[1024];
        memset(response, 0, sizeof(response));
        recvfrom(sockfd, response, sizeof(response), 0, (struct sockaddr *)&server, &len);
        printf("%s\n", response);
    }
    close(sockfd);
    return 0;
}
