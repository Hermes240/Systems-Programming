/*Programmer: Hermes Obiang
 *Class: CptS 360 - FALL 2019
 *Date: 9/12/2019 
 *Programming Assignment: Linux file system tree
 *
 */
#include <stdio.h>             // for I/O
#include <stdlib.h>            // for I/O
#include <libgen.h>            // for dirname()/basename()
#include <string.h>

typedef struct node{
          char  name[64];       // node's name string
          char  type;
   struct node *child, *sibling, *parent;
}NODE;


NODE *root, *cwd, *start, *CWD;
char line[128];
char command[16], pathname[64];
char dname[64], bname[64];
//char *name[64];

//               0       1      2      3    4      5       6
char *cmd[] = {"cd", "pwd", "mkdir","rmdir","pwd","get","rmdir","put", 0};


char *gpath[256];
char *name[64];           // OR   char *name[16];
int n;



int findCmd(char *command)
{
   int i = 0;
   while(cmd[i]){
     if (strcmp(command, cmd[i])==0)
         return i;
     i++;
   }
   return -1;
}

// finds a node in the tree
NODE *search_child(NODE *parent, char *name)
{
  NODE *p;
  printf("search for %s in parent DIR\n", name);
  p = parent->child;
  if (p==0)
    return 0;
  while(p){
    if (strcmp(p->name, name)==0)
      return p;
    p = p->sibling;
  }
  return 0;
}

//insert a node into the file system tree
int insert_child(NODE *parent, NODE *q)
{
  NODE *p;
  printf("insert NODE %s into parent child list\n", q->name);
  p = parent->child;
  if (p==0)
    parent->child = q;
  else{
    while(p->sibling)
      p = p->sibling;
    p->sibling = q;
  }
  q->parent = parent;
  q->child = 0;
  q->sibling = 0;
}

// traverse the tree following the giving pathname
NODE *path2node(char *pathname)
{
  NODE *start, *p;
  int i;

  if (pathname[0]=='/'){ start = root;}
   
  else{start = cwd;}

  tokenize(pathname);

  if(strlen(name) == 0 || strcmp(name[0],".") == 0 || strcmp(name[0],"") == 0)
  {
    return start;
  }

  for (int i=0; i < n; i++){

        p = search_child(start, name[i]);
        if (p==0){
          printf("mkdir: cannot create directory %s: No such file or directory\n",pathname);
          return 0;
        }
        
        if (p->type == 'F' )
        {
          return 0;
        }
           
        start = p;
     }  
  
  return p;
}

// parse data into tokens
int tokenize(char *pathname)
{
char *s;
n = 0;
*name = NULL;

s = strtok(pathname, "/"); // first call to strtok()
while(s){
name[n] = s; 
s = strtok(0, "/"); // call strtok() until it returns NULL
n++;
}
}

/***************************************************************
 This mkdir(char *name) makes a DIR in the current directory
 You MUST improve it to mkdir(char *pathname) for ANY pathname
****************************************************************/
int mkdir(char *pathname)
{
  NODE *p, *q;
  strcpy(gpath,pathname);
  strcpy(bname,basename(pathname));
  strcpy(dname,dirname(pathname));

  printf("mkdir: name=%s\n", bname);

  if (!strcmp(bname, "/") || !strcmp(bname, ".") || !strcmp(bname, "..")){
    printf("can't mkdir with %s\n", bname);
    return -1;
  }
  
  start = path2node(dname);

  if(start == 0){ 
    printf("cannot create directory : Permission denied");
    return -1;
    }

  printf("check whether %s already exists\n", bname);
  p = search_child(start, bname);
  if (p){
    printf("name %s already exists, mkdir FAILED\n", bname);
    return -1;
  }
  printf("--------------------------------------\n");
  printf("ready to mkdir %s\n", bname);
  q = (NODE *)malloc(sizeof(NODE));
  q->type = 'D';
  strcpy(q->name, bname);
  insert_child(start, q);
  printf("mkdir %s OK\n", bname);
  printf("--------------------------------------\n");
    
  return 0;
}

// This ls() list CWD. You MUST improve it to ls(char *pathname)
int ls(char *pathname)
{
  NODE *p = path2node(pathname);
  if(p == 0)
  {
    p = cwd->child;
  }else{    p = p->child;
  }
  
  printf("cwd contents = ");

  while(p){
    printf("[%c %s] ", p->type, p->name);
    p = p->sibling;
  }
  printf("\n");
}

int quit()
{
  printf("Program exit\n");
  exit(0);
  // improve quit() to SAVE the current tree as a Linux file
  // for reload the file to reconstruct the original tree
}

// takes a pathname and changes de cwd to wherever the pathname leads you to
int cd(char *pathname)
{
  NODE *p;
  if (!pathname || strcmp(pathname,"") == 0)
  {
    cwd = root;
    return 1;
  }

  if(strcmp(pathname,"../") == 0 || strcmp(pathname,"..") == 0)
  {
    cwd = cwd->parent;
    return 1;
  }

  tokenize(pathname);

  for (int i=0; i < n; i++){

        p = search_child(cwd, name[i]);

        if (p==0 || p->type != 'D'){
          printf("bash: cd: %s: No such file or directory\n",pathname);
          return 0;
        }
           
        cwd = p;
     }  

  
}

// implemented using recursion. Prints the complete path from the 
//cwd to the root
int pwd()
{
  if (CWD == root)
  {
    printf("\n");
    return;
  }

  printf("/%s",CWD);
  CWD = CWD->parent;
  pwd(); 
  
}

//creats a file in the given pathname
int creat(char *pathname)
{
  NODE *p, *q;
  strcpy(gpath,pathname);
  strcpy(bname,basename(pathname));
  strcpy(dname,dirname(pathname));

  printf("mkdir: name=%s\n", bname);

  if (!strcmp(bname, "/") || !strcmp(bname, ".") || !strcmp(bname, "..")){
    printf("can't mkdir with %s\n", bname);
    return -1;
  }
  
  start = path2node(dname);

  if(start == 0){ 
    printf("cannot create file : Permission denied");
    return -1;
    }

  printf("check whether %s already exists\n", bname);
  p = search_child(start, bname);
  if (p){
    printf("name %s already exists, creat FAILED\n", bname);
    return -1;
  }
  printf("--------------------------------------\n");
  printf("ready to creat %s\n", bname);
  q = (NODE *)malloc(sizeof(NODE));
  q->type = 'F';
  strcpy(q->name, bname);
  insert_child(start, q);
  printf("creat %s OK\n", bname);
  printf("--------------------------------------\n");
    
  return 0;
}

// deletes a directory from the file system tree
int rmdir(char *pathname)
{
  NODE *p = path2node(pathname);

  if(!p)
  {
    return 0;
  }

  if(p->child)
  {
    printf("rmdir: failed to remove %s: Directory not empty\n",p->name);
    return 0;
  }

  
}

int initialize()
{
    root = (NODE *)malloc(sizeof(NODE));
    strcpy(root->name, "/");
    root->parent = root;
    root->sibling = 0;
    root->child = 0;
    root->type = 'D';
    cwd = root;
    printf("Root initialized OK\n");
}

int main()
{
  int index;

  initialize();
  

  printf("NOTE: commands = [mkdir|ls|quit|cd|pwd|creat]\n");

  while(1){
      printf("Enter command line : ");
      fgets(line, 128, stdin);
      line[strlen(line)-1] = 0;

      command[0] = pathname[0] = 0;
      sscanf(line, "%s %s", command, pathname);
      printf("command=%s pathname=%s\n", command, pathname);
      
      if (command[0]==0) 
         continue;

      index = findCmd(command);

      CWD = cwd;

      switch (index){
        case 0: mkdir(pathname); break;
        case 1: ls(pathname);    break;
        case 2: quit();          break;
        case 3: cd(pathname);    break;
        case 4: pwd();           break;
        case 5: creat(pathname); break;
        case 6: rmdir(pathname); break;
      }
  }
}

