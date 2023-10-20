#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

void calculate_md5(const char *str, char *md5)
{
    MD5_CTX context;
    MD5_Init(&context);
    MD5_Update(&context, str, strlen(str));
    MD5_Final(md5, &context);
}

void calculate_sha1(const char *str, char *sha1)
{
    SHA_CTX context;
    SHA1_Init(&context);
    SHA1_Update(&context, str, strlen(str));
    SHA1_Final(sha1, &context);
}

void handle_client1(int sockfd, struct sockaddr_in *client1Addr, struct sockaddr_in *client2Addr)
{                                                            // xử lý dữ liệu từ client 1
    socklen_t addr_size = sizeof(*client1Addr);              // thực hiện gán kích thước địa chỉ của client
    char client1Buffer[BUFFER_SIZE];                         // bộ đệm cho dữ liệu
    char client2Buffer[BUFFER_SIZE];                         // bộ đệm cho dữ liệu
    char md5Hash[33], sha1Hash[41];                          // bộ đệm cho dữ liệu
    char alphaChars[BUFFER_SIZE], numericChars[BUFFER_SIZE]; // bộ đệm cho dữ liệu

    int alphaCount = 0, numericCount = 0; // số lượng ký tự chữ cái và chữ số

    memset(client1Buffer, 0, BUFFER_SIZE); // xóa bộ nhớ đệm cho client1Buffer

    recvfrom(sockfd, client1Buffer, BUFFER_SIZE, 0, (struct sockaddr *)client1Addr, &addr_size); // thực hiện nhận dữ liệu từ client

    for (int i = 0; i < strlen(client1Buffer); i++)
    { // duyệt qua từng ký tự trong chuỗi
        if (isalpha(client1Buffer[i]))
        {                                                // nếu là chữ cái
            alphaChars[alphaCount++] = client1Buffer[i]; // thêm vào bộ đệm chữ cái
        }
        else if (isdigit(client1Buffer[i]))
        {                                                    // nếu là chữ số
            numericChars[numericCount++] = client1Buffer[i]; // thêm vào bộ đệm chữ số
        }
    }

    alphaChars[alphaCount] = '\0';     // kết thúc chuỗi
    numericChars[numericCount] = '\0'; // kết thúc chuỗi

    if (strlen(alphaChars) == 0 || strlen(numericChars) == 0)
    {                                                                                                                                  // nếu không có chữ cái hoặc chữ số
        sendto(sockfd, "Xâu không chứa chữ cái hoặc chữ số. Hãy thử lại.", BUFFER_SIZE, 0, (struct sockaddr *)client1Addr, addr_size); // gửi thông báo cho client
    }
    else
    {
        calculate_md5(client1Buffer, md5Hash);   // tính toán MD5
        calculate_sha1(client1Buffer, sha1Hash); // tính toán SHA1

        sendto(sockfd, alphaChars, BUFFER_SIZE, 0, (struct sockaddr *)client2Addr, addr_size);   // gửi chữ cái cho client 2
        sendto(sockfd, numericChars, BUFFER_SIZE, 0, (struct sockaddr *)client2Addr, addr_size); // gửi chữ số cho client 2
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Cú pháp: ./server PortNumber\n");
        return 1;
    }

    int port = atoi(argv[1]); // chuyển port từ chuỗi sang số

    int sockfd;                                // socket
    struct sockaddr_in serverAddr, clientAddr; // thông tin server và client
    socklen_t addr_size;                       // kích thước địa chỉ của client
    char buffer[BUFFER_SIZE];                  // bộ đệm cho dữ liệu

    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // tạo socket

    memset(&serverAddr, 0, sizeof(serverAddr)); // xóa bộ nhớ đệm cho serverAddr
    serverAddr.sin_family = AF_INET;            // AF_INET: sử dụng IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY;    // địa chỉ IP của máy chủ
    serverAddr.sin_port = htons(port);          // cổng 5550

    bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)); // gán địa chỉ cho socket

    printf("Server đang lắng nghe trên cổng %d\n", port); // in ra màn hình cổng đang lắng nghe

    struct sockaddr_in client1Addr, client2Addr; // thông tin client 1 và client 2
    int client1Connected = 0;                    // biến kiểm tra client 1 đã kết nối chưa
    int client2Connected = 0;                    // biến kiểm tra client 2 đã kết nối chưa

    while (1)
    {
        addr_size = sizeof(clientAddr);                                                       // thực hiện gán kích thước địa chỉ của client
        memset(buffer, 0, BUFFER_SIZE);                                                       // xóa bộ nhớ đệm cho buffer
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &addr_size); // thực hiện nhận dữ liệu từ client

        if (!client1Connected)
        {                                                                                                                            // nếu client 1 chưa kết nối
            client1Connected = 1;                                                                                                    // đánh dấu client 1 đã kết nối
            memcpy(&client1Addr, &clientAddr, sizeof(clientAddr));                                                                   // gán thông tin client 1
            sendto(sockfd, "Bạn là client 1, hãy nhập một xâu bất kỳ.", BUFFER_SIZE, 0, (struct sockaddr *)&client1Addr, addr_size); // gửi thông báo cho client 1
        }
        else if (!client2Connected)
        {                                                                                                                                    // nếu client 2 chưa kết nối
            client2Connected = 1;                                                                                                            // đánh dấu client 2 đã kết nối
            memcpy(&client2Addr, &clientAddr, sizeof(clientAddr));                                                                           // gán thông tin client 2
            sendto(sockfd, "Bạn là client 2, đợi để nhận kết quả từ client 1.", BUFFER_SIZE, 0, (struct sockaddr *)&client2Addr, addr_size); // gửi thông báo cho client 2
        }
        else
        {
            if (clientAddr.sin_port == client1Addr.sin_port)
            {                                                       // nếu client 1 gửi dữ liệu
                handle_client1(sockfd, &client1Addr, &client2Addr); // xử lý dữ liệu
            }
        }
    }

    return 0;
}
