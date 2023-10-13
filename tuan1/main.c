#include "AccountManagement.h"
int main()
{
    list l;
    initList(&l); // khởi tạo danh sách
    readFile(&l); // đọc file
    int choice;
    while (1)
    {
        printf("\nUSER MANAGEMENT PROGRAM \n");
        printf("-----------------------------------\n");
        printf("1. Register\n");
        printf("2. Sign in\n");
        printf("3. Search\n");
        printf("4. Sign out\n");
        printf("Your choice (1-4, other to quit): ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1: // đăng ký
            registerAccount(&l);
            break;
        case 2: // đăng nhập
            signIn(&l);
            break;
        case 3: // tìm kiếm user
            search(&l);
            break;
        case 4: // đăng xuất
            signOut(&l);
            break;
        default:
            return 0;
        }
    }
    return 0;
}