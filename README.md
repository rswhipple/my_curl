# my_curl
A recreation of the curl command.

## Task
Create a my_curl command (very similar to the UNIX command curl).
my_curl is a tool to get data from a server, using HTTP. 
The command is designed to work without user interaction.

How does it work?
You send a url as parameter and it will print the html content of a web page.

my_curl only supports HTTP.

## Description
To make an http request for the raw html data of any given website requires the url address to be correctly formatted. 
Next, a socket must be created and a connection initiated with the host server. 
An http request is then sent via write() and the response is received via read().

Complexity:
To correctly format the url address, one must remove prefixes if any were present in the argument ("http://" etc.),
and tokenize the rest of address into the hostname and path.

Before creating a socket one must retreive the IP address and related network info via getaddrinfo().
getaddrinfo() requires 4 parameters, the hostname, the port (in this case "80" as we are requesting data from a standard website), 
and 2 struct addrinfo datatypes that give a "hint" and store the results respectively.

Each website usually has multiple potential IP addresses, so to create and connect the socket, the program loops through the link list that points to the alternatives and uses the first option that functions.

After the socket connection is established a correctly formatted http request is written to the socket file descriptor (sockfd).
As the response is read, the end of the http header is located, and the content of the body of the response is written to STDOUT.


## Installation
All .h and .c files are contained in the inc/ and src/ directories. 
The Makefile is located in the project/ directory. 

List of required .h files:
format_url.h
header.h
helper.h
memory.h
tokenize.h

List of required .c files:
format_url.c
helper.c
memory.c
my_curl.c
tokenize.c

```
> make my_curl
```

## Usage
Usage is simple as functionality is limited.
Initiate the function and provide a url address as the parameter:

```
./my_curl <url>
```

### The Core Team

Rebecca Whipple Silverstein

<span><i>Made at <a href='https://qwasar.io'>Qwasar SV -- Software Engineering School</a></i></span>