#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/sha.h>

#define MAX_ATTEMPTS 3
#define MAX_ACCOUNTS 3

typedef struct Account
{
    char username[100];
    char password[100];
    int is_active;
    int attempts; // Number of failed login attempts
} Account;

// Function to load account data from a file
int loadAccounts(Account accounts[])
{
    FILE *file = fopen("account.txt", "r");
    if (file == NULL)
    {
        perror("Unable to open account.txt");
        exit(1);
    }
    int num_accounts = 0;
    while (!feof(file))
    {
        fscanf(file, "%s %s %d", accounts[num_accounts].username, accounts[num_accounts].password, &accounts[num_accounts].is_active);
        accounts[num_accounts].attempts = 0;
        num_accounts++;
    }
    fclose(file);
    return num_accounts;
}

// Function to hash a string using SHA-256
void sha256(const char *input, char output[65])
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input, strlen(input));
    SHA256_Final(hash, &sha256);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(output + 2 * i, "%02x", hash[i]);
    }
    output[64] = 0;
}

// Function to check if a string contains only letters and digits
int isAlphanumeric(const char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (!isalnum(str[i]))
        {
            return 0; // Not alphanumeric
        }
    }
    return 1; // Alphanumeric
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s PortNumber\n", argv[0]);
        exit(1);
    }
    Account accounts[MAX_ACCOUNTS]; // account data
    int num_accounts = 0;
    // Load account data from account.txt
    num_accounts = loadAccounts(accounts);

    int port = atoi(argv[1]);

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation error");
        exit(1);
    }

    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind error");
        exit(1);
    }

    char buffer[256];
    socklen_t len = sizeof(client_addr);
    int login_status = 0, account_index = -1;
    ;
    while (1)
    {
        memset(buffer, '\0', sizeof(buffer));
        recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &len);
        printf("Received: %s\n", buffer);
        if (login_status != 0)
        {
            printf("da login\n");
            if (strcmp(buffer, "bye") == 0)
            {
                char response[] = "Goodbye ";
                strcat(response, accounts[account_index].username);
                sendto(server_socket, response, sizeof(response), 0, (struct sockaddr *)&client_addr, len);
                account_index = -1;
                login_status = 0;
            }
            else if (strcmp(buffer, "change_password") == 0)
            {
                char hashed_password[100];
                if (isAlphanumeric(buffer))
                {
                    // Mật khẩu hợp lệ, mã hóa bằng SHA-256
                    sha256(buffer, hashed_password);
                    strcpy(accounts[account_index].password, buffer);
                    char alphaChars[256], numericChars[256];
                    int numAlphaChars = 0, numNumericChars = 0;
                    for (int i = 0; i < strlen(hashed_password); i++)
                    {
                        if (isalpha(hashed_password[i]))
                        {
                            alphaChars[numAlphaChars++] = hashed_password[i];
                        }
                        else if (isdigit(hashed_password[i]))
                        {
                            numericChars[numNumericChars++] = hashed_password[i];
                        }
                    }
                    alphaChars[numAlphaChars] = '\0';
                    numericChars[numNumericChars] = '\0';
                    sendto(server_socket, alphaChars, sizeof(alphaChars), 0, (struct sockaddr *)&client_addr, len);
                    sendto(server_socket, numericChars, sizeof(numericChars), 0, (struct sockaddr *)&client_addr, len);
                }
                else
                {
                    char response[] = "Error";
                    sendto(server_socket, response, sizeof(response), 0, (struct sockaddr *)&client_addr, len);
                }
            }
        }
        else
        {
            printf("login\n");
            char username[100];
            char password[100];
            strcpy(username, buffer);
            for (int i = 0; i < num_accounts; i++)
            {
                if (strcmp(username, accounts[i].username) == 0)
                {
                    account_index = i;
                    break;
                }
            }
            // Kiểm tra xem tài khoản có tồn tại và có hoạt động không
            printf("account_index: %d\n", account_index);
            if (account_index >= 0 && accounts[account_index].is_active == 1)
            {
                char response1[] = "Insert password: ";
                sendto(server_socket, response1, sizeof(response1), 0, (struct sockaddr *)&client_addr, len);
                memset(password, '\0', sizeof(password));
                recvfrom(server_socket, password, sizeof(password), 0, (struct sockaddr *)&client_addr, &len);
                // Kiểm tra mật khẩu
                if (strcmp(password, accounts[account_index].password) == 0)
                {
                    char response[] = "OK";
                    sendto(server_socket, response, sizeof(response), 0, (struct sockaddr *)&client_addr, len);
                    accounts[account_index].attempts = 0;
                    login_status = 1;
                }
                else
                {
                    char response[] = "not OK";
                    sendto(server_socket, response, sizeof(response), 0, (struct sockaddr *)&client_addr, len);
                    accounts[account_index].attempts++;
                    if (accounts[account_index].attempts >= 3)
                    {
                        accounts[account_index].is_active = 0;
                    }
                }
            }
            else
            {
                char response[] = "account not ready";
                sendto(server_socket, response, sizeof(response), 0, (struct sockaddr *)&client_addr, len);
            }
        }
    }

    close(server_socket);
    return 0;
}
