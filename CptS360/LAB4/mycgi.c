#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>

struct stat mystat, *sp, dirent, *ep;
DIR *dp;
char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";
char linkname[1024];


FILE *fp, *gp;
char buf[4096];
#define BLKSIZE 4096
#define MAX 10000
typedef struct {
    char *name;
    char *value;
} ENTRY;

ENTRY entry[MAX];

main(int argc, char *argv[]) 
{
  int i, m, r;
  char cwd[128];

  m = getinputs();    // get user inputs name=value into entry[ ]
  getcwd(cwd, 128);   // get CWD pathname

  printf("Content-type: text/html\n\n");
  printf("<p>pid=%d uid=%d cwd=%s\n", getpid(), getuid(), cwd);

  printf("<H1>Echo Your Inputs</H1>");
  printf("You submitted the following name/value pairs:<p>");
 
  for(i=0; i <= m; i++)
     printf("%s = %s<p>", entry[i].name, entry[i].value);
  printf("<p>");


  if(strcmp(entry[0].value,"mkdir") == 0)
    {
          
	  r = mkdir(entry[1].value,0755);
	  if(r == 0){ 
		printf("mkdir OK: <p>");
    }

    }

   else if(strcmp(entry[0].value,"rmdir") == 0)
     {
	  r = rmdir(entry[1].value);
	if(r==0){ printf("rmdir OK <p>");}	
     }

   else if(strcmp(entry[0].value,"rm") == 0)

	{
		r = unlink(entry[1].value);
		if (r ==0){printf("rm OK: <p>");}
	}

else if(strcmp(entry[0].value, "cat") == 0)
{
	FILE *fp;
	int c;
	if(strlen(entry[0].value)<2){ exit(1);};
	fp = fopen(entry[1].value, "r");
	if (fp==0) {exit(2);}
	while ((c = fgetc(fp))!= EOF){
		putchar(c);
}
	
}

else if(strcmp(entry[0].value,"cp") == 0)
{
	int n, total=0;
	if (entry[0].value < 3) exit(1);
	fp = fopen(entry[1].value, "r");
	if (fp == NULL) exit(2);
	gp = fopen(entry[2].value, "w");
	while(n=fread(buf,1,BLKSIZE,fp))
	{
		fwrite(buf, 1, n, gp);
		total += n;
	}
		printf("total = %d\n", total);
		fclose(fp); fclose(gp);
}

else if(strcmp(entry[0].value,"ls") == 0)
 {	
struct stat mystat, *sp = &mystat;
  int r;
  char *filename, path[1024], cwd[256];
  filename = "./";
  if (strlen(entry[1].value) >= 1)
  {
    filename = entry[1].value;
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


  /*****************************************************************
   Write YOUR C code here to processs the command
         mkdir dirname
         rmdir dirname
         rm    filename
         cat   filename
         cp    file1 file2
         ls    [dirname] <== ls CWD if no dirname
  *****************************************************************/
 
  // create a FORM webpage for user to submit again 
  printf("</title>");
  printf("</head>");
  printf("<body bgcolor=\"#FF0000\" link=\"#330033\" leftmargin=8 topmargin=8");
  printf("<p>------------------ DO IT AGAIN ----------------\n");
  
  printf("<FORM METHOD=\"POST\" ACTION=\"http://cs360.eecs.wsu.edu/~obiang/cgi-bin/mycgi.bin\">");

  //------ NOTE : CHANGE ACTION to YOUR login name ----------------------------
  //printf("<FORM METHOD=\"POST\" ACTION=\"http://cs360.eecs.wsu.edu/~YOURNAME/cgi-bin/mycgi.bin\">");
  
  printf("Enter command : <INPUT NAME=\"command\"> <P>");
  printf("Enter filename1: <INPUT NAME=\"filename1\"> <P>");
  printf("Enter filename2: <INPUT NAME=\"filename2\"> <P>");
  printf("Submit command: <INPUT TYPE=\"submit\" VALUE=\"Click to Submit\"><P>");
  printf("</form>");
  printf("------------------------------------------------<p>");

  printf("</body>");
  printf("</html>");
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
