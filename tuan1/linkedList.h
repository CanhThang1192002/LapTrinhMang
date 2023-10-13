#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tạo cấu trúc dữ liệu cho user
struct User
{
    char username[50];  // tên đăng nhập
    char password[50]; // mật khẩu
    int status; // 0: blocked, 1: active
    int numberOfLogin; // số lần đăng nhập sai
    int statusLogin; // 0: chưa đăng nhập, 1: đã đăng nhập
};
typedef struct User user;

// tạo danh sách liên kết đơn
struct Node
{
    user data; // dữ liệu của node
    struct Node *next; // con trỏ trỏ đến node tiếp theo
};
typedef struct Node node;

struct List 
{
    node *head; // node đầu 
    node *tail; // node cuối
};
typedef struct List list;

void initList(list *l)  // khởi tạo danh sách liên kết đơn
{
    l->head = l->tail = NULL; // node đầu và node cuối đều trỏ đến NULL
}

user createUser() // tạo user
{
    user u;
    printf("username: ");
    scanf("%s", u.username);
    printf("password: ");
    scanf("%s", u.password);
    u.status = 1;
    u.numberOfLogin = 0;
    u.statusLogin = 0;
    return u;
}

node *makeNode(user u) // tạo node
{
    node *p = (node *)malloc(sizeof(node)); // cấp phát bộ nhớ cho node
    p->data = u;                           // gán giá trị cho node
    p->next = NULL;                       // khởi tạo node trỏ đến NULL
    return p;
}

void addUser(list *l, node *p) // thêm user vào danh sách
{
    if (l->head == NULL) // nếu danh sách rỗng thì node đầu và node cuối đều là p
    {
        l->head = l->tail = p;    
    }
    else
    {
        l->tail->next = p; // node cuối trỏ đến p
        l->tail = p;      // node cuối là p
    }
}

node *searchUser(list *l, char username[]) // tìm user trong danh sách
{
    node *p;
    for (p = l->head; p != NULL; p = p->next) // duyệt danh sách
    {
        if (strcmp(p->data.username, username) == 0) // nếu tìm thấy user thì trả về node
        {
            return p;
        }
    }
    return NULL;
}