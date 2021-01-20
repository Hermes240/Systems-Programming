#include<stdio.h>
#include<string.h>


char mypath[128];


int main(int argc,char *argv[], char *env[])
{
    int i = 0;
    strcpy(mypath,getenv("PATH"));

    while(env[i])
    {
        printf("env[%d] = %s\n",i,env[i]);
        if(!strncmp(env[i],"PATH",4));
        {
            printf("found PATH\n");
            strcpy(mypath,&env[i][5]);
            printf("mypath = %s\n",mypath);
            break;
        }  
            
        i++;
    }
}
