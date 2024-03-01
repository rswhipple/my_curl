#include "../inc/format_url.h"
#include "../inc/print_addr_info.h"
#include "../inc/read_line.h"
#include "../inc/helper.h"
#include "../inc/memory.h"
#include <netdb.h> 
#include <stdlib.h>


int my_curl(char* av) 
{
    // tokenize url into host and path
    char *url = remove_prefix(av);

    char **url_array = split_string(url);
    my_putstr(tokens->array[0]);
    my_putstr("\n");
    my_putstr(tokens->array[1]);
    my_putstr("\n");

    char *port = "80"; // default
    struct addrinfo *result;

    // get address info
    result = get_addr_info(tokens->array[0], port);
    if (!result) { 
        tokens->free_t_array(tokens);
        return EXIT_FAILURE; 
    }

    // create + bind socket
    int sockfd = create_socket(result); 
    freeaddrinfo(result);
    if (sockfd < 0) { 
        tokens->free_t_array(tokens);
        return EXIT_FAILURE; 
    }

    // create request
    char *request = create_request(url);
    

    // write + read data
    int status = write_read_socket_data(sockfd, request);
    tokens->free_t_array(tokens);
    close(sockfd); 

    if (status) { return EXIT_FAILURE; }

    return EXIT_SUCCESS;
}

char *create_request(char *hostname)
{
    char *request = init_str(REQUEST_BUF);
    char *get =  "GET / HTTP/1.1\r\nHost: ";
    strncpy(request,  get, my_strlen(get));
    strcat(request, hostname);
    strcat(request, "\r\nConnection: close\r\n\r\n");

    return request;
}

struct addrinfo *get_addr_info(char *hostname, char *port)
{
    struct addrinfo hints, *result;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;        // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // two-way stream
    hints.ai_flags = AI_PASSIVE;        // Fill in my IP for me

    if ((status = getaddrinfo(hostname, port, &hints, &result)) != 0) {
        my_putstrerr("could not resolve host: ");
        my_putstrerr(hostname);
        return NULL;
    }

    return result;
}

int create_socket(struct addrinfo *result)
{
    struct addrinfo *p;
    int sockfd;

    // find the first working IP address 
    for (p = result; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("webserver (socket)");
            continue;
        }

        // connect
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) != 0) {
            close(sockfd);
            perror("webserver (connect)");
            continue;
        }
        break;
    }

    if (p == NULL) {
        my_putstrerr("failed to connect socket\n");
        return -1;
    }

    return sockfd;
}


int write_read_socket_data(int sockfd, char *request)
{
    if ((write(sockfd, request, my_strlen(request))) <= 0 ) {
        perror("write error");
        free(request);
        close(sockfd);
        return EXIT_FAILURE;
    }

    free(request);

    read_line_t *buf = NULL;
    char *str = NULL;

    while ((str = my_read_line(sockfd, &buf)) != NULL) {
        my_putstr(str);
        my_putstr("\n");
        free(str);
    }

    return EXIT_SUCCESS;
}


int main(int ac, char** av)
{
    // check for the correct number of arguments
    if (ac < 2) {
        printf("./my_curl expects you to provide a url: no url parameter provided.\n");
        return EXIT_FAILURE;
    }
    else if (ac == 2) {
        if (my_curl(av[1])) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
