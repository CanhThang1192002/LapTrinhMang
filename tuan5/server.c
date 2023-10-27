#include "linkedList.h"
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/sha.h>

#define MAX_SIZE 1024

void readFile(list *l); // Read data from file saved to list

void writeFile(list *l); // Write data from list to file

int create_socket(); // create socket

struct sockaddr_in create_server_addr(int port); // create server address

void bind_socket(int sockfd, struct sockaddr_in server_addr); // bind socket to server address and port

void sha256(const char *input, char output[65]); // Function to hash a string using SHA-256

int isAlphanumeric(const char *str); // Function to check if a string contains only letters and digits

void changePassword(char buffer[], node *p, char response[]); // Function to change password

void logout(node *p, int *login_status, char response[]); // Function to logout

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "--> Usage: %s PortNumber\n", argv[0]);
        exit(1);
    }
    list accounts;
    initList(&accounts);
    readFile(&accounts);

    struct sockaddr_in server_addr, client_addr;
    int port = atoi(argv[1]);
    int server_socket = create_socket();
    server_addr = create_server_addr(port);
    bind_socket(server_socket, server_addr);

    char buffer[MAX_SIZE];
    socklen_t len = sizeof(client_addr);
    int login_status = 0;
    node *p = NULL;
    while (1)
    {
        memset(buffer, '\0', sizeof(buffer));
        recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &len);
        if (login_status != 0)
        {
            if (strcmp(buffer, "bye") == 0)
            {
                char response_bye[MAX_SIZE];
                logout(p, &login_status, response_bye);
                sendto(server_socket, response_bye, sizeof(response_bye), 0, (struct sockaddr *)&client_addr, len);
            }
            else
            {
                if (isAlphanumeric(buffer))
                {
                    char response[MAX_SIZE * 2];
                    changePassword(buffer, p, response);
                    writeFile(&accounts);
                    sendto(server_socket, response, sizeof(response), 0, (struct sockaddr *)&client_addr, len);
                }
                else
                {
                    sendto(server_socket, "--> Error", MAX_SIZE, 0, (struct sockaddr *)&client_addr, len);
                }
            }
        }
        else
        {
            char username[MAX_SIZE];
            char password[MAX_SIZE];
            socklen_t len = sizeof(client_addr);
            memcpy(username, buffer, MAX_SIZE);
            // Check if the account exists and is active
            p = searchAccount(&accounts, username);
            if (p != NULL && p->data.is_active == 1)
            {
                sendto(server_socket, "--> Insert password: ", MAX_SIZE, 0, (struct sockaddr *)&client_addr, len);
                memset(password, '\0', sizeof(password));
                recvfrom(server_socket, password, sizeof(password), 0, (struct sockaddr *)&client_addr, &len);
                // Check if the password is correct
                if (strcmp(password, p->data.password) == 0)
                {
                    sendto(server_socket, "--> Ok", MAX_SIZE, 0, (struct sockaddr *)&client_addr, len);
                    p->data.attempts = 0;
                    login_status = 1;
                }
                else
                {
                    p->data.attempts++;
                    if (p->data.attempts >= 3)
                    {
                        p->data.is_active = 0;
                        writeFile(&accounts);
                        sendto(server_socket, "--> Account is blocked", MAX_SIZE, 0, (struct sockaddr *)&client_addr, len);
                    }
                    else
                    {
                        sendto(server_socket, "--> not OK", MAX_SIZE, 0, (struct sockaddr *)&client_addr, len);
                    }
                }
            }
            else
            {
                sendto(server_socket, "--> account not ready", MAX_SIZE, 0, (struct sockaddr *)&client_addr, len);
            }
        }
    }
    close(server_socket);
    return 0;
}

void readFile(list *l)
{
    FILE *f = fopen("account.txt", "r");
    if (f == NULL)
    {
        perror("Unable to open account.txt");
        exit(1);
    }
    while (!feof(f))
    {
        Account a;
        fscanf(f, "%s %s %d", a.username, a.password, &a.is_active);
        a.attempts = 0;
        node *p = makeNode(a);
        addAccount(l, p);
    }
    fclose(f);
}

void writeFile(list *l)
{
    FILE *f = fopen("account.txt", "w");
    if (f == NULL)
    {
        perror("Unable to open account.txt");
        exit(1);
    }
    node *p;
    for (p = l->head; p != l->tail; p = p->next)
    {
        fprintf(f, "%s %s %d\n", p->data.username, p->data.password, p->data.is_active);
    }
    fprintf(f, "%s %s %d", p->data.username, p->data.password, p->data.is_active);
    fclose(f);
}

int create_socket()
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
    return sockfd;
}

struct sockaddr_in create_server_addr(int port)
{
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    // server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    return server_addr;
}

void bind_socket(int sockfd, struct sockaddr_in server_addr)
{
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind error");
        exit(1);
    }
}

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

int isAlphanumeric(const char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (!isalnum(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

void changePassword(char buffer[], node *p, char response[])
{
    char hashed_password[100];
    sha256(buffer, hashed_password);
    strcpy(p->data.password, buffer);
    char alphaChars[MAX_SIZE], numericChars[MAX_SIZE];
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
    response[0] = '\0';
    char response_alpha[] = "--> AlphaChars: ";
    strcat(response_alpha, alphaChars);
    strcat(response, response_alpha);
    strcat(response, "\n");
    char response_numeric[] = "--> NumericChars: ";
    strcat(response, response_numeric);
    strcat(response, numericChars);
}

void logout(node *p, int *login_status, char response[])
{
    response[0] = '\0';
    strcat(response, "--> Goodbye ");
    strcat(response, p->data.username);
    p = NULL;
    *login_status = 0;
}