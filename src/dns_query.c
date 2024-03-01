//Header Files
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <stdlib.h>    //malloc
#include <sys/socket.h>    
#include <arpa/inet.h> //inet_addr , inet_ntoa , ntohs etc
#include <netinet/in.h>
#include <unistd.h>    //getpid
 
// Types of DNS resource records
// #define T_A 1 //Ipv4 address
// #define T_NS 2 //Nameserver
// #define T_CNAME 5 // canonical name
// #define T_SOA 6 /* start of authority zone */
// #define T_PTR 12 /* domain name pointer */
// #define T_MX 15 // Mail server

// Macros
#define MAX_LINE_LENGTH 512
#define BUF_SIZE 65536

// STRUCTS 
// DNS header struct
struct DNS_HEADER
{
    short id; // identification number
 
    char rd :1; // recursion desired
    char tc :1; // truncated message
    char aa :1; // authoritive answer
    char opcode :4; // purpose of message
    char qr :1; // query/response flag
 
    char rcode :4; // response code
    char cd :1; // checking disabled
    char ad :1; // authenticated data
    char z :1; // its z! reserved
    char ra :1; // recursion available
 
    short q_count; // number of question entries
    short ans_count; // number of answer entries
    short auth_count; // number of authority entries
    short add_count; // number of resource entries
};

// Constant sized fields of query structure
struct QUESTION
{
	short qtype;
	short qclass;
};

// Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct R_DATA
{
    short type;
    short _class;
    int ttl;
    short data_len;
};
#pragma pack(pop)
 
// Resource record contents struc
struct R_RECORD
{
    char *name;
    struct R_DATA *resource;
    char *rdata;
};
 
// Structure of a Query
typedef struct
{
    unsigned char *name;
    struct QUESTION *ques;
} QUERY;

// Global Variables
char dns_servers[10][100];
int dns_server_count = 0;
 
// Function Prototypes
void my_get_host_by_name(char *host, int query_type);
void to_dns_name_format(char *str_1, char *str_2);
char* read_name(char *str_1, char *str_2, int *num);
void get_dns_servers(void);


void get_dns_servers()
{
    FILE *fp;
    char line[200] , *p;
    char *search_str = "nameserver";

    if ((fp = fopen("/etc/resolv.conf" , "r")) == NULL)
    {
        printf("Failed opening /etc/resolv.conf file \n");
    }
     
    while(fgets(line , 200 , fp))
    {
        if(line[0] == '#')
        {
            continue;
        }
        if(strncmp(line, search_str, 10) == 0)
        {
            p = strtok(line , " ");
            p = strtok(NULL , " ");
             
            //p now is the dns ip :)
            //????
        }
    }
     
    strcpy(dns_servers[0], "208.67.222.222");
    strcpy(dns_servers[1], "208.67.220.220");
}

void my_get_host_by_name(char *host, int query_type)
{
	char buf[BUF_SIZE];
    char *qname;
    char *reader;
	int i , j , stop , s;

	struct sockaddr_in a;

	struct R_RECORD answers[20], auth[20], addit[20]; // the replies from the DNS server
	struct sockaddr_in dest;

	struct DNS_HEADER *dns = NULL;
	struct QUESTION *qinfo = NULL;

	printf("Resolving %s" , host);

	s = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP); // UDP packet for DNS queries

	dest.sin_family = AF_INET;
	dest.sin_port = htons(53);
	dest.sin_addr.s_addr = inet_addr(dns_servers[0]); // dns servers

	// Set the DNS structure to standard queries
	dns = (struct DNS_HEADER *)&buf;

	dns->id = (unsigned short) htons(getpid());
	dns->qr = 0; // This is a query
	dns->opcode = 0; // This is a standard query
	dns->aa = 0; // Not Authoritative
	dns->tc = 0; // This message is not truncated
	dns->rd = 1; // Recursion Desired
	dns->ra = 0; // Recursion not available! hey we dont have it (lol)
	dns->z = 0;
	dns->ad = 0;
	dns->cd = 0;
	dns->rcode = 0;
	dns->q_count = htons(1); // we have only 1 question
	dns->ans_count = 0;
	dns->auth_count = 0;
	dns->add_count = 0;

	// point to the query portion
	qname = (char*)&buf[sizeof(struct DNS_HEADER)];

	to_dns_name_format(qname, host);
	qinfo =(struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)]; // fill it

	qinfo->qtype = htons( query_type ); //type of the query , A , MX , CNAME , NS etc
	qinfo->qclass = htons(1); // its internet (lol)

	printf("\nSending Packet...");
	if ( sendto(s,(char*)buf,sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION),0,(struct sockaddr*)&dest,sizeof(dest)) < 0)
	{
		perror("sendto failed");
	}
	printf("Done");
	
	//Receive the answer
	i = sizeof dest;
	printf("\nReceiving answer...");
	if (recvfrom (s,(char*)buf , 65536 , 0 , (struct sockaddr*)&dest , (socklen_t*)&i ) < 0)
	{
		perror("recvfrom failed");
	}
	printf("Done");

	dns = (struct DNS_HEADER*) buf;

	//move ahead of the dns header and the query field
	reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];

	printf("\nThe response contains : ");
	printf("\n %d Questions.",ntohs(dns->q_count));
	printf("\n %d Answers.",ntohs(dns->ans_count));
	printf("\n %d Authoritative Servers.",ntohs(dns->auth_count));
	printf("\n %d Additional records.\n\n",ntohs(dns->add_count));

	//Start reading answers
	stop=0;

	for(i=0;i<ntohs(dns->ans_count);i++)
	{
		answers[i].name=ReadName(reader,buf,&stop);
		reader = reader + stop;

		answers[i].resource = (struct R_DATA*)(reader);
		reader = reader + sizeof(struct R_DATA);

		if(ntohs(answers[i].resource->type) == 1) //if its an ipv4 address
		{
			answers[i].rdata = (unsigned char*)malloc(ntohs(answers[i].resource->data_len));

			for(j=0 ; j<ntohs(answers[i].resource->data_len) ; j++)
			{
				answers[i].rdata[j]=reader[j];
			}

			answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';

			reader = reader + ntohs(answers[i].resource->data_len);
		}
		else
		{
			answers[i].rdata = ReadName(reader,buf,&stop);
			reader = reader + stop;
		}
	}

	//read authorities
	for(i=0;i<ntohs(dns->auth_count);i++)
	{
		auth[i].name=ReadName(reader,buf,&stop);
		reader+=stop;

		auth[i].resource=(struct R_DATA*)(reader);
		reader+=sizeof(struct R_DATA);

		auth[i].rdata=ReadName(reader,buf,&stop);
		reader+=stop;
	}

	//read additional
	for(i=0;i<ntohs(dns->add_count);i++)
	{
		addit[i].name=ReadName(reader,buf,&stop);
		reader+=stop;

		addit[i].resource=(struct R_DATA*)(reader);
		reader+=sizeof(struct R_DATA);

		if(ntohs(addit[i].resource->type)==1)
		{
			addit[i].rdata = (unsigned char*)malloc(ntohs(addit[i].resource->data_len));
			for(j=0;j<ntohs(addit[i].resource->data_len);j++)
			addit[i].rdata[j]=reader[j];

			addit[i].rdata[ntohs(addit[i].resource->data_len)]='\0';
			reader+=ntohs(addit[i].resource->data_len);
		}
		else
		{
			addit[i].rdata=ReadName(reader,buf,&stop);
			reader+=stop;
		}
	}

	//print answers
	printf("\nAnswer Records : %d \n" , ntohs(dns->ans_count) );
	for(i=0 ; i < ntohs(dns->ans_count) ; i++)
	{
		printf("Name : %s ",answers[i].name);

		if( ntohs(answers[i].resource->type) == T_A) //IPv4 address
		{
			long *p;
			p=(long*)answers[i].rdata;
			a.sin_addr.s_addr=(*p); //working without ntohl
			printf("has IPv4 address : %s",inet_ntoa(a.sin_addr));
		}
		
		if(ntohs(answers[i].resource->type)==5) 
		{
			//Canonical name for an alias
			printf("has alias name : %s",answers[i].rdata);
		}

		printf("\n");
	}

	//print authorities
	printf("\nAuthoritive Records : %d \n" , ntohs(dns->auth_count) );
	for( i=0 ; i < ntohs(dns->auth_count) ; i++)
	{
		
		printf("Name : %s ",auth[i].name);
		if(ntohs(auth[i].resource->type)==2)
		{
			printf("has nameserver : %s",auth[i].rdata);
		}
		printf("\n");
	}

	//print additional resource records
	printf("\nAdditional Records : %d \n" , ntohs(dns->add_count) );
	for(i=0; i < ntohs(dns->add_count) ; i++)
	{
		printf("Name : %s ",addit[i].name);
		if(ntohs(addit[i].resource->type)==1)
		{
			long *p;
			p=(long*)addit[i].rdata;
			a.sin_addr.s_addr=(*p);
			printf("has IPv4 address : %s",inet_ntoa(a.sin_addr));
		}
		printf("\n");
	}
	return;
}
