#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/sha.h>

#define MAX_ATTEMPTS 3
#define MAX_ACCOUNTS 3

typedef struct Account {
    char username[100];
    char password[100];
    int is_active;
    int attempts;
} Account;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s PortNumber\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        perror("Socket creation error");
        exit(1);
    }

    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind error");
        exit(1);
    }

    char buffer[256];
    socklen_t len = sizeof(client_addr);
    recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &len);

    // Load account data from account.txt
    Account accounts[MAX_ACCOUNTS];
    // Initialize account data

    while (1) {
        // Receive username and password from the client
        // Verify the username and password
        // Send appropriate response
        // Implement password change, signout, and other functionality

        // Close the server socket on some condition
    }

    close(server_socket);
    return 0;
}

