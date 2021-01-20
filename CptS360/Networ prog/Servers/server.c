/*
*Programmers: Hermes Obiang & Rodney Walton
* Lab Assignment 5: Network programming
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include<dirent.h>
#include<fcntl.h>


#define MAX   256
#define PORT 1199
#define h_addr h_addr_list[0]

struct stat mystat, *sp, dirent, *ep;
struct stat mystat, *sp, dirent, *ep;
struct hostent *host;
struct sockaddr_in server_addr, sockaddr, saddr, caddr;

DIR *dp;
char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";
char linkname[1024];
char line[MAX], command[64],pathname[256], RESULT[256];
int n, r, sfd, cfd, len;

int initialize(char *hostname)
{
   printf("=================server init=================\n");
   host = gethostbyname(hostname);

   if(host == 0)
   {
      printf("unknown host --> %s",hostname);
   }

   printf("1 : create a TCP STREAM socket\n");

    sfd = socket(AF_INET, SOCK_STREAM, 0); 
   
   if (sfd < 0) { 
      printf("socket creation failed\n"); 
      exit(1); 
   }

   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = *(long*)host->h_addr;
   server_addr.sin_port = 0;

   printf("2 : bind socket with server address\n");

   if ((bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr))) != 0) { 
      printf("socket bind failed\n"); 
      exit(2);
    }

   len = sizeof(caddr);

   getsockname(sfd, (struct sockaddr *)&caddr, &len);

   printf("hostname=%s, port=%d\n", host->h_name, ntohs(caddr.sin_port));


   return 0;
}
void check(int success)
{ 
    if(success  == 0 )
    {
        strcpy(RESULT,"");
        strcat(RESULT,command);
        strcat(RESULT," is OK ");
    }

    else
    {
        strcpy(RESULT,"");
        strcat(RESULT,command);
        strcat(RESULT," FAILED ");
    }
}
// this function changes the cwd
void CD()
{
    r = chdir(pathname);
    check(r);
}

//prints the current working directory
void PWD()
{
    bzero(RESULT, 256);
    getcwd(RESULT,256);
}

// create a directory
void MKDIR()
{
    r = mkdir(pathname, 0755);   
    check(r);
}

void RMDIR()
{
    r = rmdir(pathname);
    check(r);
}

void RM()
{
    r = unlink(pathname);
    check(r);
}

int ls_file(char *fname)
{
    struct stat fstat, *sp;
    int r, i;
    char ftime[64];
    sp = &fstat;
    if ( (r = lstat(fname, &fstat)) < 0){
    printf("can't stat %s\n", fname);
    exit(1);}

    if ((sp->st_mode & 0xF000) == 0x8000) // if (S_ISREG())
        printf("%c",'-');
    if ((sp->st_mode & 0xF000) == 0x4000) // if (S_ISDIR())
        printf("%c",'d');
    if ((sp->st_mode & 0xF000) == 0xA000) // if (S_ISLNK())
        printf("%c",'1');
    for (i=8; i >= 0; i--){
        if (sp->st_mode & (1 << i)) // print r|w|x
            printf("%c", t1[i]);
        else
            printf("%c", t2[i]); // or print -
    }
    printf("%4d ",sp->st_nlink); // link count
    printf("%4d ",sp->st_gid); // gid
    printf("%4d ",sp->st_uid); // uid
    printf("%8d ",sp->st_size); // file size
      // print time
    strcpy(ftime, ctime(&sp->st_ctime)); // print time in calendar form
    ftime[strlen(ftime)-1] = 0; // kill \n at end
    printf("%s ",ftime);
    // print name
    printf("%s", basename(fname)); // print file basename
    // print -> linkname if symbolic file
    if ((sp->st_mode & 0xF000)== 0xA000){
        if(readlink(fname,linkname,1024) > 0)
        {
            printf(" -> %s", linkname); // print linked name
            }
    printf("\n");
    }
}
int ls_dir(char *dname)
{
    DIR *dp;
	struct dirent *ep;
    
    dp = opendir(dname);
  

    while((ep = readdir(dp)) != NULL)
    {
        if (strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0)
        {
            ls_file(ep->d_name);
            printf("\n");
               }
         }

    return 0;
    }

void ls()
{
    struct stat mystat, *sp = &mystat;
   int r;
  char *filename, path[1024], cwd[256];
  filename = "./";
  if (strlen(pathname) >= 1)
  {
    filename = pathname;
  }
  if (r = lstat(filename, sp) < 0)
  {
    printf("no such file %s\n", filename);
    exit(1);
  }
  strcpy(path, filename);
  if (path[0] != '/')
  {
    getcwd(cwd, 256);
    strcpy(path, cwd);
    strcat(path, "/");
    strcat(path, filename);
  }
  printf("\n");
  printf("path: %s\n\n", path);
  if (S_ISDIR(sp->st_mode))
  {
    ls_dir(path);
  }
  else
  {
    ls_file(path);
  }
      }

int get_file(int cfd, char* filename)
{
    int size, count, n, file;
    count = 0;
    char buf[MAX] = {0};

    read(cfd, buf, MAX);
    sscanf(buf, "%d", &size);

    file = open(filename, O_WRONLY | O_CREAT);
    while(count < size)
    {
        n = read(cfd, buf, MAX);
        write(file, buf, n);
        count += n;
    }

    close(file);
    return 0;
}

int put_file(int cfd, char* filename)
{
    struct stat sb;
    int size, n, file;
    char buf[MAX] = {0};


    if (stat(filename, &sb) == 0)
        size = sb.st_size;
    else
        size = 0;

    sprintf(buf, "%d", size);
    write(cfd, buf, MAX);


    file = open(filename, O_RDONLY);

    while(n = read(file, buf, MAX)){
        write(cfd, buf, n);
    }

    close(file);
    return 0;
}


int main(int argc, char *argv[]) 
{   
    initialize("localhost");

    printf("4. server listens\n");
    if ((listen(sfd, 5)) != 0) { 
        printf("Listen failed\n"); 
        exit(3); 
    }

    len = sizeof(caddr);
    while(1){
       printf("server accepting connection\n");
       cfd = accept(sfd, (struct sockaddr *)&caddr, &len); 
       if (cfd < 0) { 
          printf("server acccept failed\n"); 
          exit(4); 
       }
       printf("server acccepted a client connection\n"); 

       // server Processing loop
       while(1)
       {
           printf("server: ready for next request\n");

           n = read(cfd, line, MAX);
           if (n==0){
             printf("server: client died, server loops\n");
             close(cfd);
             break;
             }
      
          // show the line contents
          printf("server: read  n=%d bytes; line=[%s]\n", n, line);

          // tokenize input
          sscanf(line,"%s %s",&command, &pathname);
           
          // change directory command
          if (strcmp(command,"cd") == 0)
          {
              CD();
          }
          else if(strcmp(command,"pwd") == 0)
          {
              PWD();
          }

          else if(strcmp(command,"mkdir") == 0)
          {
              MKDIR();
          }

          else if(strcmp(command,"rmdir") == 0)
          {
              RMDIR();
          }

          else if(strcmp(command,"rm") == 0)
          {
              RM();
          }

          else if(strcmp(command,"ls") == 0)
          {
              ls();
              check(0);
          }
          else if(strcmp(command,"get") == 0)
          {
              put_file(cfd,pathname);
              check(0);
          }

          else if(strcmp(command,"put") == 0)
          {
              get_file(cfd,pathname);
              check(0);
          }

          // send the echo line to client 
          bzero(&command,64);
          bzero(&pathname,64);
          n = write(cfd, RESULT, MAX);
          printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, RESULT);
       }
    }
} 
