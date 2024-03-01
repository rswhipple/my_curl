#include "../inc/memory.h" 

char* init_str(int size)  
{
    char* str = malloc(sizeof(char) * size + 1);

    for (int i = 0; i < size + 1; i++) { str[i] = '\0'; }

    return str;
}

char** init_string_arr(int size)
{
    char** new_array = malloc(sizeof(char*) * size + 1);

    for (int i = 0; i <= size; i++) { new_array[i] = NULL; }

    return new_array;
}

void free_string_arr(char** string_array)
{
    int i = 0;
    while (string_array[i]) {
        free(string_array[i]) ;
        i++;
    }
    
    free(string_array);

}

void print_array(char** array)
{
    int i = 0;

    while (array[i]) {
        printf("char* array[%i] = %s\n", i, array[i]);
        i++;
    }
}
