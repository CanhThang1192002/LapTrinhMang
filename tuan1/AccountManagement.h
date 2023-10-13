#include "linkedList.h"

// đọc file
void readFile(list *l)
{
    FILE *f = fopen("account.txt", "r"); // mở file
    if (f == NULL)                       // nếu không mở được file thì thông báo lỗi
    {
        printf("error opening file");
        return;
    }
    while (!feof(f)) // đọc file cho đến khi hết file
    {
        user u;
        fscanf(f, "%s %s %d", u.username, u.password, &u.status); // đọc từng dòng trong file
        u.numberOfLogin = 0;
        u.statusLogin = 0;
        node *p = makeNode(u); // tạo node
        addUser(l, p);         // thêm node vào danh sách
    }
    fclose(f); // đóng file
}

// ghi file
void writeFile(list *l)
{
    FILE *f = fopen("account.txt", "w"); // mở file
    if (f == NULL)                       // nếu không mở được file thì thông báo lỗi
    {
        printf("error opening file");
        return;
    }
    node *p;
    for (p = l->head; p != NULL; p = p->next) // duyệt danh sách
    {
        fprintf(f, "%s %s %d\n", p->data.username, p->data.password, p->data.status); // ghi từng dòng vào file
    }
    fclose(f); // đóng file
}

//  Register a new account
void registerAccount(list *l)
{
    user u;
    printf("Username: ");
    scanf("%s", u.username);
    if (searchUser(l, u.username) != NULL) // nếu tìm thấy user thì thông báo đã tồn tại
    {
        printf("account existed\n");
    }
    else // nếu không tìm thấy user thì tạo user mới
    {
        printf("Password: ");
        scanf("%s", u.password);
        u.status = 1;
        u.numberOfLogin = 0;
        u.statusLogin = 0;
        node *p = makeNode(u); // tạo node
        addUser(l, p);        // thêm node vào danh sách
        writeFile(l);         // ghi file
        printf("Successful registration. \n");
    }
}

// sign in user
void signIn(list *l)
{
    user u;
    printf("Username: ");
    scanf("%s", u.username);
    node *p = searchUser(l, u.username); // tìm user trong danh sách
    if (p == NULL) // nếu không tìm thấy user thì thông báo không tìm thấy
    {
        printf("Cannot find account\n");
    }
    else // nếu tìm thấy user thì kiểm tra mật khẩu
    {
        if (p->data.statusLogin == 1) // nếu đã đăng nhập thì thông báo
        {
            printf("account is logged in");
        }
        else
        {
            printf("Password: ");
            scanf("%s", u.password);
            if (strcmp(p->data.password, u.password) == 0) // nếu mật khẩu đúng thì kiểm tra trạng thái tài khoản
            {
                if (p->data.status == 0) // nếu tài khoản bị khóa thì thông báo
                {
                    printf("Account is blocked\n");
                    p->data.numberOfLogin = 0;
                }
                else // nếu tài khoản không bị khóa thì thông báo đăng nhập thành công
                {
                    printf("Hello %s\n", p->data.username);
                    p->data.statusLogin = 1;
                }
            }
            else // nếu mật khẩu sai thì thông báo lỗi và tăng số lần đăng nhập sai
            {
                printf("Password is incorrect\n");
                p->data.numberOfLogin++;
                if (p->data.numberOfLogin == 3)
                {
                    p->data.status = 0;
                    writeFile(l);
                    printf("Password is incorrect. Account is blocked\n");
                }
            }
        }
    }
}

// search
void search(list *l)
{
    char username[50];
    printf("Username: ");
    scanf("%s", username);
    node *p = searchUser(l, username); // tìm user trong danh sách
    if (p == NULL)                      // nếu không tìm thấy user thì thông báo
    {
        printf("Cannot find account\n");
    }
    else // nếu tìm thấy user thì kiểm tra trạng thái tài khoản
    {
        if (p->data.statusLogin == 0) // nếu tài khoản chưa đăng nhập thì thông báo
        {
            printf("Account is not sign in\n");
        }
        else
        {
            if (p->data.status == 0) // nếu tài khoản bị khóa thì thông báo
            {
                printf("Account is blocked\n");
            }
            else // nếu tài khoản không bị khóa thì thông báo
            {
                printf("Account is active\n");
            }
        }
    }
}

// sign out user
void signOut(list *l)
{
    char username[50];
    printf("Username: ");
    scanf("%s", username);
    node *p = searchUser(l, username); // tìm user trong danh sách
    if (p == NULL)                // nếu không tìm thấy user thì thông báo
    {
        printf("Cannot find account\n");
    }
    else // nếu tìm thấy user thì kiểm tra trạng thái tài khoản
    {
        if (p->data.statusLogin == 0) // nếu tài khoản chưa đăng nhập thì thông báo
        {
            printf("Account is not sign in\n");
        }
        else // nếu tài khoản đã đăng nhập thì đăng xuất
        {
            p->data.statusLogin = 0; // đổi trạng thái đăng nhập
            printf("Goodbye %s\n", p->data.username);
        }
    }
}