#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char *myargv[64];
char cmd[128];
char mypath[128],path[128],home[128], *path2[128];
int n;


int main(int argc,char *argv[], char *env[])
{

    int i = 0;
    int pid, status;
    char cmd[16],param[64];
    char line[128];


   welcome();

    while(1)
    {

        printf("hosh: ");
        fgets(line,128,stdin);
        line[strlen(line)-1] = 0;
        cmd[0] = 0;
        param[0] = 0;
        int j = 0;

        sscanf(line,"%s %s",cmd,param);
        printf("cmd = %s  param = %s\n", cmd,param);
        //getchar();
        
        if(strcmp(cmd,"cd") == 0)
        {
            chdir(param);
        }

        if(strcmp(cmd,"exit") == 0)
        {
            exit(0);
        }

        myargv[0] = cmd;
        myargv[1]= param;
        myargv[2] = 0;
        i = 0;
            //printf("\nTEST:%s\n", myargv[0]);


        while(myargv[i])
        {
      
            printf("myargv[%d] = %s\n",i, myargv[i]);
            i++;
        }
        
        /* strcpy(line,path2[j]);
        strcat(line,"/");
        strcat(line,myargv[0]);
        printf("line = %s\n",line);
        j++;
       // getchar();
        int r = execve(line, myargv,env);
        printf("execve faild r = %d\n",r);
        exit(1); */

        
         pid = fork();

        if(pid)
        {
            pid = wait(&status);
            printf("dead child = %d status = %x\n",pid,status);
            printf("main sh repeats loop\n");
          
            
        }
        else
        {
           myargv[0] = cmd;
           myargv[1]= param;
           myargv[2] = 0;
           i = 0;
            printf("\nTEST:%s\n", myargv[0]);


           while(myargv[i])
           {
      
               printf("myargv[%d] = %s\n",i, myargv[i]);
               i++;
           }
        
        strcpy(line,"/bin/");
        strcat(line,myargv[0]);
        printf("line = %s\n",line);
       // getchar();
        int r = execve(line, myargv,env);
        printf("execve faild r = %d\n",r);
        exit(1);
        } 

    //     strcpy(line,"/bin/");
    //     strcat(line,myargv[0]);
    //     printf("line = %s\n",line);
    //    // getchar();
    //     int r = execve(line, myargv,env);
    //     printf("execve faild r = %d\n",r);
    //     exit(1);
        
    }
}

void welcome()
{
     //gets path and home directory
    strcpy(path,getenv("PATH"));
    strcpy(home,getenv("HOME"));


    printf("**************************Welcome to hosh********************************\n");

    printf("Show PATH = %s\n\n",path);

    printf("Show HOME directory = %s\n\n",home);

    printf("Decompose PATH into dir string: ");

    tokenize_Path();
    print_path();
    printf("\n");
    
    printf("**************************hosh processing loop****************************\n");
}

void tokenize_Path()
{
    char *s;
     n = 0;
    s = strtok(path,":");


    while(s)
    {

        path2[n] = s;
        
        n++;
        s = strtok(NULL,":");
    }
}

void print_path()
{
    for(int i = 0;i<n;i++)
    {
        printf("%s ",path2[i]);
    }
}
