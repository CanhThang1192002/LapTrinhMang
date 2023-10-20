#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#define BUFFER_SIZE 1024

// Hàm tính giá trị MD5 hash của một chuỗi
void calculate_md5(char *input, char *output) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((unsigned char *)input, strlen(input), digest);

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(output + 2 * i, "%02x", digest[i]);
    }
}

// Hàm tính giá trị SHA-1 hash của một chuỗi
void calculate_sha1(char *input, char *output) {
    unsigned char digest[SHA_DIGEST_LENGTH];
    SHA1((unsigned char *)input, strlen(input), digest);

    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(output + 2 * i, "%02x", digest[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Cú pháp: ./server PortNumber\n");
        return 1;
    }

    int port = atoi(argv[1);

    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];

    // Tạo socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // Gắn socket với địa chỉ và cổng
    bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr);

    printf("Server đang lắng nghe trên cổng %d\n", port);

    struct sockaddr_in client1Addr, client2Addr;
    char client1Buffer[BUFFER_SIZE];
    char client2Buffer[BUFFER_SIZE];
    char md5Hash[33], sha1Hash[41];
    char alphaChars[BUFFER_SIZE], numericChars[BUFFER_SIZE];

    int client1Connected = 0;
    int client2Connected = 0;

    while (1) {
        addr_size = sizeof(clientAddr);
        memset(buffer, '\0', BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &addr_size);

        if (!client1Connected) {
            client1Connected = 1;
            memcpy(&client1Addr, &clientAddr, sizeof(clientAddr));
            sendto(sockfd, "Bạn là client 1, hãy nhập một xâu bất kỳ.", BUFFER_SIZE, 0, (struct sockaddr *)&client1Addr, addr_size);
        } else if (!client2Connected) {
            client2Connected = 1;
            memcpy(&client2Addr, &clientAddr, sizeof(clientAddr));
            sendto(sockfd, "Bạn là client 2, đợi để nhận kết quả từ client 1.", BUFFER_SIZE, 0, (struct sockaddr *)&client2Addr, addr_size);
        } else {
            if (clientAddr.sin_port == client1Addr.sin_port) {
                // Xử lý dữ liệu từ client 1
                memcpy(client1Buffer, buffer, BUFFER_SIZE);
                int alphaCount = 0, numericCount = 0;

                for (int i = 0; i < strlen(client1Buffer); i++) {
                    if (isalpha(client1Buffer[i])) {
                        alphaChars[alphaCount++] = client1Buffer[i];
                    } else if (isdigit(client1Buffer[i])) {
                        numericChars[numericCount++] = client1Buffer[i];
                    }
                }

                alphaChars[alphaCount] = '\0';
                numericChars[numericCount] = '\0';

                if (strlen(alphaChars) == 0 || strlen(numericChars) == 0) {
                    sendto(sockfd, "Xâu không chứa chữ cái hoặc chữ số. Hãy thử lại.", BUFFER_SIZE, 0, (struct sockaddr *)&client1Addr, addr_size);
                } else {
                    calculate_md5(client1Buffer, md5Hash);
                    calculate_sha1(client1Buffer, sha1Hash);

                    sendto(sockfd, md5Hash, BUFFER_SIZE, 0, (struct sockaddr *)&client2Addr, addr_size);
                    sendto(sockfd, sha1Hash, BUFFER_SIZE, 0, (struct sockaddr *)&client2Addr, addr_size);
                }
            }
        }
    }

    return 0;
}
