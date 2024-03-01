#include "../inc/helper.h" 


int my_strlen(char* str) 
{
    int i = 0;

    do {
        i += 1;
    } while (str[i] != '\0');

    return i;
}

char *my_strncpy(char* dst, char* src, int n)
{
    char* original_dst = dst;
    int i;

    for (i = 0; i < n; i++) {
        *dst++ = *src++;
    }

    *--dst = '\0';

    return original_dst;
}


char *my_strcpy(char* dst, char* src)
{
    int i;

    for (i = 0; i < my_strlen(src); i++) {
        dst[i] = src[i]; 
    }

    dst[i] = '\0';
    
    return dst;
}

char *my_strcat(char *dst, char *src) 
{ 
    // Save original destination pointer
    char *original_dst = dst; 
    
    // Move destination pointer to the end of the first string
    while (*dst != '\0') {
        dst += 1;
    }

    // Copy characters from src to dst
    while (*src != '\0') {
        *dst = *src;
        dst += 1;
        src += 1;
    }

    // Null-terminate 
    *dst = '\0';

    return original_dst;
}

char* my_strchr(char* str_1, char char_1)
{
    int i;

    for (i = 0; i <= my_strlen(str_1); i++) {
        if (str_1[i] == char_1) {
            return &str_1[i];
        }
    }

    return "";
}

int my_strchr_index(char* str_1, char char_1)
{
    int i;

    for (i = 0; i <= my_strlen(str_1); i++) {
        if (str_1[i] == char_1) {
            return i + 1;
        }
    }

    return 0;
}

void my_putstr(char* str)
{
    write(STDOUT_FILENO, str, my_strlen(str));
}

void my_putstrerr(char* str)
{
    write(STDERR_FILENO, str, my_strlen(str));
}

