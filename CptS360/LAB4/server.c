#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>

#define SIZE 256
#define MAX   256
#define PORT 1122

char command[20], pathname[20];

struct stat mystat, *sp, dirent, *ep;
struct hostent *host;
struct sockaddr_in server_addr, sockaddr, saddr, caddr;
#define h_addr h_addr_list[0]
DIR *dp;
char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";
char linkname[1024];
char line[MAX], BUFF[SIZE];
int n,sfd, len,cfd;

char *cmd[] = {"mkdir", "ls", "quit","cd","pwd","creat","rmdir", 0};



int initialize(char *hostname)
{
   printf("=================server init=================\n");

   
   
   // 1. get hostname
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

   //printf("3. bind socket with server address\n");

   if ((bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr))) != 0) { 
      printf("socket bind failed\n"); 
      exit(2);
    }

   len = sizeof(caddr);

   getsockname(sfd, (struct sockaddr *)&caddr, &len);

   printf("hostname=%s, port=%d\n", host->h_name, ntohs(caddr.sin_port));


   return 0;
}
int main(int argc, char *argv[]) 
{
   int r;
   //int sfd, cfd, len; 
   //struct sockaddr_in saddr, caddr; 
  
   //printf("1. create a TCP socket\n");
  

   /* printf("2. fill in [localhost IP port=%d] as server address\n", PORT);
   bzero(&saddr, sizeof(saddr)); 
   saddr.sin_family = AF_INET; 
   saddr.sin_addr.s_addr = htonl(INADDR_ANY); 
   saddr.sin_port = htons(PORT);  */
  
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
      while(1){
         printf("server: ready for next request\n");
	      n = read(cfd, line, MAX);

         if (n==0){
            printf("server: client died, server loops\n");
            close(cfd);
            break;
         }

	  


      // show the line contents
      printf("server: read  n=%d bytes; line=[%s]\n", n, line);
	
	   sscanf(line, "%s %s",&command, &pathname);
      strcat(line, " ECHO");

	   if(strcmp(command,"cd") == 0){
	      r = chdir(pathname);}

	   else if(strcmp(command,"pwd") == 0){
	      getcwd(BUFF,SIZE);}
      
       else if(strcmp(command,"rm") == 0){
         r = unlink(pathname);
      }
      
      else if(strcmp(command,"mkdir") == 0){
         r = mkdir(pathname,0755);

         if(r == 0){ printf("mkdir ok:");}
      }

      else if(strcmp(command,"rmdir") == 0){
         r = rmdir(pathname);
      }

      else if(strcmp(command,"ls") == 0){
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
  printf("path: %s<p>", path);
  if (S_ISDIR(sp->st_mode))
  {
    ls_dir(path);
  }
  else
  {
    ls_file(path);
  }
      }

          // send the echo line to client 
          n = write(cfd, BUFF, MAX);
          printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, BUFF);
       }
    }
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
                 printf("<p>");
               }
         }
	
	return 0;
      }


int ls_command()
{
   /* struct stat mystat, *sp = &mystat;
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
  printf("path: %s<p>", path);
  if (S_ISDIR(sp->st_mode))
  {
    ls_dir(path);
  }
  else
  {
    ls_file(path);
  } */
}

