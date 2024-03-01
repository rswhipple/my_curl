#include "../inc/memory.h"
#include "../inc/helper.h"

char **split_string(char* src, char separator)  
{
    char **tokens = NULL;

    // create tokens
    if (!(my_strchr(src, separator))) {
        
        tokens = init_t_array(1);  
        tokens->array[0] = src;
        return tokens;
    }

    int num_tokens = 2;

    // Initialize token array and run lexer
    tokens = init_t_array(num_tokens);  
    if ((status = lexer(&tokens, src, separator, num_tokens))) { 
        free_t_array(tokens);
        return NULL; 
    }

    return tokens;
}

