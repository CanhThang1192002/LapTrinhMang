#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

// kiểm tra xem đầu vào là một địa chỉ IP hay là một tên miền
int is_valid_ip(const char *input)
{
    struct sockaddr_in ip;                                 // struct sockaddr_in là một struct chứa địa chỉ IPv4
    return inet_pton(AF_INET, input, &(ip.sin_addr)) != 0; // Nếu địa chỉ IP hợp lệ thì hàm trả về 1, ngược lại trả về 0
}

// Hàm lấy tên miền chính và tên miền phụ từ địa chỉ IP
void getDomainsFromIP(const char *ip)
{
    struct in_addr addr;           // struct in_addr là một struct chứa một địa chỉ IPv4
    inet_pton(AF_INET, ip, &addr); // Chuyển đổi địa chỉ IP từ dạng chuỗi sang dạng struct in_addr và lưu vào biến addr

    struct hostent *host = gethostbyaddr(&addr, sizeof(addr), AF_INET); // Lấy thông tin tên miền từ địa chỉ IP và lưu vào biến host

    if (host == NULL)
    {
        printf("Not found information\n");
    }
    else
    {
        printf("Official name: %s\n", host->h_name); // In ra tên miền chính

        if (host->h_aliases) // Nếu có tên miền phụ
        {
            char **alias = host->h_aliases; // Lấy danh sách tên miền phụ
            printf("Alias name:\n");
            while (*alias != NULL) // In ra ds tên miền phụ
            {
                printf("%s\n", *alias);
                alias++;
            }
        }
    }
}

// Hàm lấy địa chỉ IP chính và địa chỉ IP phụ từ tên miền
void getIPfromDomain(const char *domain)
{
    struct hostent *host;         // struct hostent chứa thông tin về tên miền
    host = gethostbyname(domain); // Lấy thông tin về tên miền
    if (host == NULL)
    {
        printf("Not found information.\n");
        return;
    }
    printf("Official IP: %s\n", inet_ntoa(*(struct in_addr *)host->h_addr)); // In ra địa chỉ IP chính
    // Tìm các địa chỉ IP phụ
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints)); // Khởi tạo struct hints với các giá trị 0
    hints.ai_family = AF_INET;        // Địa chỉ IP có thể là IPv4 hoặc IPv6
    hints.ai_socktype = SOCK_STREAM;  // Kiểu socket là SOCK_STREAM (TCP)

    if (getaddrinfo(domain, NULL, &hints, &res) != 0) // Lấy thông tin về địa chỉ IP từ tên miền
    {
        return;
    }
    else
    {
        // Hiển thị các địa chỉ IP phụ
        printf("Alias IP:\n");
        for (struct addrinfo *p = res; p != NULL; p = p->ai_next)
        {
            printf("    %s\n", p->ai_addr->sa_data);
        }
    }
    freeaddrinfo(res); // Giải phóng
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <domain or IP>\n", argv[0]);
        return 1;
    }
    char *input = argv[1];
    if (is_valid_ip(input)) // Kiểm tra xem đầu vào là một địa chỉ IP hay là một tên miền
    {
        getDomainsFromIP(input); // Nếu là địa chỉ IP thì lấy tên miền
    }
    else
    {
        getIPfromDomain(input); // Nếu là tên miền thì lấy địa chỉ IP
    }

    return 0;
}
