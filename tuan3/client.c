#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Cú pháp: ./client IPAddress PortNumber\n");
        return 1;
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int sockfd;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];

    // Tạo socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(server_port);
    serverAddr.sin_addr.s_addr = inet_addr(server_ip);

    while (1) {
        char message[BUFFER_SIZE];

        printf("Nhập một xâu (hoặc nhập '@' hoặc '#' để thoát): ");
        fgets(message, BUFFER_SIZE, stdin);

        if (message[0] == '@' || message[0] == '#') {
            break;
        }

        sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

        addr_size = sizeof(serverAddr);
        memset(buffer, '\0', BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serverAddr, &addr_size);
        printf("Kết quả từ server:\n%s", buffer);
    }

    close(sockfd);
    return 0;
}
