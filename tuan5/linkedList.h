#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Account
{
    char username[1024];
    char password[1024];
    int is_active;
    int attempts; // Number of failed login attempts
} Account;

// tạo node cho danh sách liên kết đơn
struct Node
{
    Account data;
    struct Node *next;
};
typedef struct Node node;
// tạo danh sách liên kết đơn
struct List
{
    node *head;
    node *tail;
};
typedef struct List list;
// khởi tạo danh sách liên kết đơn
void initList(list *l)
{
    l->head = l->tail = NULL;
}
// tạo account
Account createUser()
{
    Account account;
    printf("username: ");
    scanf("%s", account.username);
    printf("password: ");
    scanf("%s", account.password);
    account.is_active = 1;
    account.attempts = 0;
    return account;
}
// tạo node
node *makeNode(Account account)
{
    node *p = (node *)malloc(sizeof(node));
    p->data = account;
    p->next = NULL;
    return p;
}
// thêm account vào danh sách
void addAccount(list *l, node *p)
{
    if (l->head == NULL)
    {
        l->head = l->tail = p;
    }
    else
    {
        l->tail->next = p;
        l->tail = p;
    }
}
// tìm Account trong danh sách
node *searchAccount(list *l, char username[])
{
    node *p;
    for (p = l->head; p != NULL; p = p->next)
    {
        if (strcmp(p->data.username, username) == 0)
        {
            return p;
        }
    }
    return NULL;
}