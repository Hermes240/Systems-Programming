#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>


#define MAX   256
#define PORT 1122
#define h_addr h_addr_list[0]
struct sockaddr_in server_addr, sock_addr,saddr;
struct hostent *hp;
char line[MAX], ans[MAX], buffer[256];
int n,SERVER_IP,SERVER_PORT, cfd;

int initialize(char *argv[])
{
    printf("argv[1] = %s  ", argv[1]);
    printf("argv[1] = %s\n", argv[2]);
    hp = gethostbyname(argv[1]);
    //printf("1. create a TCP socket\n");

    SERVER_IP = *(long *)hp->h_addr;
    SERVER_PORT = atoi(argv[2]);

     printf("1. create a TCP socket\n");
    cfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (cfd < 0) { 
        printf("socket creation failed\n"); 
        exit(1); 
    }


    // 3. fill server_addr with server IP and PORT#
   // printf("2. fill in [localhost IP port=%d] as server address\n", PORT);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = SERVER_IP;
    server_addr.sin_port = htons(SERVER_PORT);

    printf("3. connect client socket to server\n");
    if (connect(cfd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0) { 
        printf("connection to server failed\n"); 
        exit(2); 
    }
}

int main(int argc, char *argv[ ]) 
{ 

printf(" argv = %s",argv[1]);
    initialize(argv);
       
    printf("hostname = %s  PORT = %d\n ", hp->h_name, SERVER_PORT);
    /* bzero(&saddr, sizeof(saddr)); 
    saddr.sin_family = AF_INET; 
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    saddr.sin_port = htons(PORT); 
   */
    
    
    printf("********  processing loop  *********\n");
    while(1){
       printf("input a line : ");
       bzero(line, MAX);                // zero out line[ ]
       fgets(line, MAX, stdin);         // get a line (end with \n) from stdin

       line[strlen(line)-1] = 0;        // kill \n at end
       if (line[0]==0)                  // exit if NULL line
          exit(0);
	
	if(line[0] == 'l')
	{
	
		char *ptr = getcwd(buffer,256);
	}
	else
{
       // Send ENTIRE line to server
       n = write(cfd, line, MAX);

       printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

       // Read a line from sock and show it
       n = read(cfd, ans, MAX);
}
       printf("client: read  n=%d bytes; echo=(%s)\n",n, ans);
    }
} 
