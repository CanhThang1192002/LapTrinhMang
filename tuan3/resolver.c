#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "convert.h"
#include "craw.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <domain or IP>\n", argv[0]);
        return 1;
    }
    char *input = argv[1];

    FILE *file = fopen("file.csv", "a+");
    if (file == NULL)
    {
        printf("Cannot open file.\n");
        return 1;
    }
    if (is_valid_ip(input)) // Kiểm tra xem đầu vào là một địa chỉ IP hay là một tên miền
    {
        int x = getDomainsFromIP(input, file); // Nếu là địa chỉ IP thì lấy tên miền
        if(x == 0){
            return 0;
        }else{
            craw(input);
        }
    }
    else
    {
        int x = getIPfromDomain(input, file); // Nếu là tên miền thì lấy địa chỉ IP
        if(x == 0){
            return 0;
        }else{
            craw(input);
        }
    }
    fclose(file);
    
    return 0;
}
