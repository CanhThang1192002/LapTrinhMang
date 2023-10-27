#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_BUFFER_SIZE 1024
// Hàm xử lý kết quả trả về từ server
void handleResponse(int sockfd, struct sockaddr_in serverAddr)
{
    char buffer[MAX_BUFFER_SIZE];
    socklen_t addr_size;

    while (1)
    {
        addr_size = sizeof(serverAddr);
        memset(buffer, '\0', MAX_BUFFER_SIZE);
        int bytesReceived = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&serverAddr, &addr_size);
        if (bytesReceived > 0)
        {
            printf("\nResult from server: %s", buffer);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Syntax: ./client IPAddress PortNumber\n");
        exit(1);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int sockfd;
    struct sockaddr_in serverAddr;

    // Tạo socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(server_port);
    serverAddr.sin_addr.s_addr = inet_addr(server_ip);

    // Tạo một tiến trình con để xử lý in kết quả từ server
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("Fork failed");
        exit(1);
    }
    else if (pid == 0) // Tiến trình con
    {
        handleResponse(sockfd, serverAddr);
        close(sockfd);
        exit(0);
    }
    else // Tiến trình cha
    {
        while (1)
        {
            // Nhập chuỗi từ bàn phím và gửi lên server
            char message[MAX_BUFFER_SIZE];
            fgets(message, MAX_BUFFER_SIZE, stdin);
            if (message[0] == '\0')
            {
                break;
            }
            sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        }
        close(sockfd);
        wait(NULL); // Đợi tiến trình con kết thúc
    }

    return 0;
}
