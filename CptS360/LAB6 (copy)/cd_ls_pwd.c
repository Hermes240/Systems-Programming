/*
* Programmer: Hermes Obiang
* CptS 360 - Fall 2019
* Project Level 1
*/
/************* cd_ls_pwd.c file **************/

/**** globals defined in main.c file ****/
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;
extern char   gpath[256];
extern char   *name[64];
extern int    n;
extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, inode_start;
extern char line[256], cmd[32], pathname[256];
char *cp,myname[256], buf[1024];
MINODE *pip, *mip;
int myino, parentino;
#define OWNER  000700
#define GROUP  000070
#define OTHER  000007



change_dir()
{
  int ino;
  MINODE *mip;
  //printf("chage_dir(): to be constructed\n");
  if (strcmp(pathname,"") == 0)
  {
    strcpy(pathname,"/");
  }

  ino = getino(pathname);
  mip = iget(dev,ino);
  int temp = &(mip->INODE).i_mode;

  if(S_ISDIR(temp))
  {
    iput(running->cwd = mip);
    running->cwd = mip;
  }

  iput(running->cwd = mip);
  running->cwd = mip;
}


int list_file()
{
  
  if (strlen(pathname) == 0) // check if there is a pathname
  {
    mip = running->cwd; // if no path provided, set mip to the cwd
    ip = &(mip->INODE);
  }

  else
  {
    myino = getino(pathname);
    mip = iget(dev,myino);
    ip = &(mip->INODE);
  }

  if(S_ISDIR(ip->i_mode)) // check if it is a directory
  {
    ls_dir(ip);
  }

  else
  {
    ls_file(myino,name[n-1]);
  }
}

int ls_dir(INODE *s)
{
  char nam[32];
  get_block(dev,s->i_block[0],buf);

  dp = (DIR*)buf;
  cp = buf;

  while(cp < buf + BLKSIZE)
  {
    strncpy(nam,dp->name,dp->name_len);
    nam[dp->name_len] = 0;

    ls_file(dp->inode,nam);

    cp += dp->rec_len;
    dp = (DIR *)cp;
  }
}

int ls_file(MINODE *inno, char *n)
{
  char buff[516];
   char ftime[64];
  mip = iget(dev,inno);
  ip = &(mip->INODE);

  if(S_ISDIR(ip->i_mode))
  {
    printf("d");
  }
  if(S_ISLNK(ip->i_mode))
  {
    printf("l");
  }
  if(S_ISREG(ip->i_mode))
  {
    printf("-");
  }
  for (int i = 8; i >= 0; i--)
  {
    if (ip->i_mode & (1 << i)) // print r|w|x
    {
      printf("%c", t1[i]);
    }

    else
    {
        printf("%c", t2[i]); // or print -
    }

  }

  printf("%4d ",ip->i_links_count); // link count
  printf("%4d ",ip->i_gid); // gid
  printf("%4d ",ip->i_uid); // uid
  printf("%8d ",ip->i_size); // file size

   // print time
  strcpy(ftime, ctime(&ip->i_mtime)); // print time in calendar form
  ftime[strlen(ftime)-1] = 0; // kill \n at end
  printf("%s ",ftime);
  printf("%s",n);

  if(ip->i_mode == 0120000)
  {
    char *l = myreadlink(n);
    printf("-> %s", l);
    //printf("-> %s", ip->i_block);
  }
  

  printf("\n");

}


int pwd(MINODE *wd)
{
  if (wd == root)
  {
    printf("/");
  }
  else
  {
    rpwd(wd);
  }
  printf("\n");
  
}

void rpwd(MINODE *wd)
{
  char myname[256];
  

  if(wd == root)
  {
    return;
  }

  // get the block
  get_block(dev,wd->INODE.i_block[0],buf); // from i_block[0] get myino and parentino
  dp = (DIR *)buf;
  myino = dp->inode;
  cp = buf;

  cp = cp + dp->rec_len; // add to get the next dir
  dp = (DIR *)cp;
  parentino = dp->inode; // get parent ino 

  pip = iget(dev,parentino);
  get_block(dev, pip->INODE.i_block[0],buf);
  dp = (DIR*)buf;
  cp = buf;

  while(cp < buf + BLKSIZE)
  {
    if(myino == dp->inode)
    {
      strncpy(myname,dp->name,dp->name_len);
      myname[dp->name_len] = 0;
      break;
      
    }

    cp += dp->rec_len;
    dp = (DIR *)cp;
  }
  
  rpwd(pip);
  printf("/%s",myname);
  
  

}


int make_dir()
{
  char parent[128], child[64];
  MINODE *start;
  int isdir = 0;

  if(pathname[0]== '/')
  {
    start = root;
    dev = root->dev;
  }

  else
  {
    start = running->cwd;
    dev = running->cwd->dev;
  }

  // since basename and dirname destroys the path
  // we make a copy of the path and operate on it
  strcpy(parent,dirname(gpath));
  // since basename and dirname destroys the path
  // we make a copy of the path and operate on it
  strcpy(gpath,pathname);
  strcpy(child,basename(gpath));

  //Get the In_MEMORY minode of parent:
  int pino  = getino(parent);
  pip   = iget(dev, pino); 
  //Verify : (1). parent INODE is a DIR (HOW?)

  /* if(pip->mounted && pip->ino != root->ino)
  {
    pip = start;
    dev = pip->dev;
  }  */
  
  if(S_ISDIR(pip->INODE.i_mode))
  {
    iput(pip);
    isdir = 1;
  }

  else
  {
    iput(pip);
    isdir = 0;
  }
  
  //(2). child does NOT exists in the parent directory (HOW?);
  int exists = search(pip,child);

  if(isdir == 1 && exists == 0)
  {
    //4. call mymkdir(pip, child);
    mymkdir(pip,child);
    //increment parent inodes's link count by 1; 
    //touch its atime and mark it DIRTY
    pip->INODE.i_links_count++;
    pip->dirty = 1;
    pip->INODE.i_atime = time(0L);
    iput(pip);
    return 1;
  }

  iput(pip);
  return 0;
  
}

int mymkdir(MINODE *pip, char *name)
{
  MINODE *mip;
  int ino,bno;
  char buf[BLKSIZE];

  //allocate an inode and a disk block for the new directory;
  ino = ialloc(dev);    
  bno = balloc(dev);

  printf("ino = %d   bno = %d\n",ino,bno);
  mip = iget(dev, ino);  //load the inode into a minode[] (in order to wirte contents to the INODE in memory.
  //iput(mip); //Write contents to mip->INODE to make it as a DIR INODE.
  ip = &mip->INODE;

  ip->i_mode = 0x41ED;		// OR 040755: DIR type and permissions
  ip->i_uid = running->uid;	// Owner uid 
  ip->i_gid = running->gid;	// Group Id 
  ip->i_size = BLKSIZE;		// Size in bytes 
  ip->i_links_count = 2;	        // Links count=2 because of . and ..
  ip->i_atime = time(0L);// set to current time
  ip->i_ctime = time(0L);// set to current time
  ip->i_mtime = time(0L); // set to current time
  ip->i_blocks = 2; // LINUX: Blocks count in 512-byte chunks 
  ip->i_block[0] = bno;  // new DIR has one data block   

  for(int i = 1; i<=14;i++){ ip->i_block[i] = 0;} //i_block[1] to i_block[14] = 0;
  mip->dirty = 1;               // mark minode dirty
  iput(mip);                   // write INODE to disk

  //***** create data block for new DIR containing . and .. entries ******
  
  get_block(dev,bno,buf);
  dp = (DIR*)buf;
  cp = buf;
  //Write . entry into a buf[ ] of BLKSIZE
  dp->file_type = (u8)EXT2_FT_DIR;
  dp->inode = (u32)ino;
  dp->name[0] = '.';
  dp->name_len = (u8) 1;
  dp->rec_len = (u16) 12;

  //Write .. entry into a buf[ ] of BLKSIZE
  cp += dp->rec_len;
  dp = (DIR*)cp;
  dp->file_type = (u8)EXT2_FT_DIR;
  dp->inode = (u32)pip->ino;
  dp->name[0] = '.';
  dp->name[1] = '.';
  dp->name_len = (u8) 2;
  dp->rec_len = (u16) 1012;

  //Then, write buf[ ] to the disk block bno;
  put_block(dev,bno,buf);

  //Finally, enter name ENTRY into parent's directory by 
  enter_name(pip,ino,name);

  return 0;

}

int enter_name(MINODE *pip, int myino, char *myname)
{
  char buf[BLKSIZE];

  for(int i = 0; i<12; i++)
  {
    if(pip->INODE.i_block[i] == 0){break;}

    //get parent's data block into a buf[];
    get_block(dev,pip->INODE.i_block[i],buf);

    dp = (DIR*)buf;
    cp = buf;

    // step to LAST entry in block: int blk = parent->INODE.i_block[i];
    int blk = pip->INODE.i_block[i];
    printf("step to LAST entry in data block %d\n", blk);

    //Step to the last entry in a data block (HOW?).
    while(cp + dp->rec_len < buf + BLKSIZE)
    {
      cp += dp->rec_len;
      dp = (DIR*)cp;
    }

    int IDEAL_LEN = 4*((8 + dp->name_len + 3)/4);
    int need_length = 4*((8 + strlen(myname) + 3)/4);
    int remain = dp->rec_len - IDEAL_LEN;

    if(remain >= need_length)
    {
      dp->rec_len = IDEAL_LEN;

      cp += dp->rec_len;
      dp = (DIR*)cp;

      dp->file_type = (u8)EXT2_FT_DIR;
      dp->inode = (u32)myino;
      strcpy(dp->name,myname);
      dp->name_len = (u8) strlen(myname);
      dp->rec_len = (u16) remain;

      //Write data block to disk;
      put_block(dev,pip->INODE.i_block[i],buf);
      return 0;
    
    }

    // Reach here means: NO space in existing data block(s)
    //Allocate a new data block; INC parent's isze by BLKSIZE;
    int temp_bno = balloc(dev); //Allocate a new data block;
    pip->INODE.i_block[i] = temp_bno;
    pip->INODE.i_size += BLKSIZE; //INC parent's isze by BLKSIZE;
    pip->dirty = 1; // mark pip->dirty

     get_block(dev,temp_bno, buf); // get the new block

    //Enter new entry as the first entry in the new data block with rec_len=BLKSIZE.
    cp = buf;
    dp = (DIR*)buf;

    dp->file_type = (u8)EXT2_FT_DIR;
    dp->inode = (u32)myino;
    strcpy(dp->name,myname);
    dp->name_len = (u8) strlen(myname);
    dp->rec_len = (u16) BLKSIZE;

    put_block(dev,temp_bno,buf);
  }
}

int creat_file()
{
  char parent[128], child[64];
  MINODE *start;
  int isdir = 0;

  if(pathname[0]== '/')
  {
    start = root;
    dev = root->dev;
  }

  else
  {
    start = running->cwd;
    dev = running->cwd->dev;
  }

  // since basename and dirname destroys the path
  // we make a copy of the path and operate on it
  strcpy(parent,dirname(gpath));
  // since basename and dirname destroys the path
  // we make a copy of the path and operate on it
  strcpy(gpath,pathname);
  strcpy(child,basename(gpath));

  //Get the In_MEMORY minode of parent:
  int pino  = getino(parent);
  pip   = iget(dev, pino); 
  //Verify : (1). parent INODE is a DIR (HOW?)

  if(pip->mounted && pip->ino != root->ino)
  {
    pip = start;
    dev = pip->dev;
  } 
  
  if(S_ISDIR(pip->INODE.i_mode))
  {
    iput(pip);
    isdir = 1;
  }

  else
  {
    iput(pip);
    isdir = 0;
  }
  
  //(2). child does NOT exists in the parent directory (HOW?);
  int exists = search(pip,child);

  if(isdir == 1 && exists == 0)
  {
    //4. call mymkdir(pip, child);
    my_creat(pip,child);
    
    pip->dirty = 1;
    pip->INODE.i_atime = time(0L);
    iput(pip);
    return 1;
  }

  iput(pip);
  return 0;
  
}

int my_creat(MINODE *pip, char *name)
{
  MINODE *mip;
  int ino,bno;
  char buf[BLKSIZE];

  //allocate an inode and a disk block for the new directory;
  ino = ialloc(dev);    
  bno = balloc(dev);

  printf("ino = %d   bno = %d\n",ino,bno);
  mip = iget(dev, ino);  //load the inode into a minode[] (in order to wirte contents to the INODE in memory.
  //iput(mip); //Write contents to mip->INODE to make it as a DIR INODE.
  ip = &mip->INODE;

  ip->i_mode = 0x81A4;		// OR 040755: DIR type and permissions
  ip->i_uid = running->uid;	// Owner uid 
  ip->i_gid = running->gid;	// Group Id 
  ip->i_size = 0;		// Size in bytes 
  ip->i_links_count = 1;	        // Links count=2 because of . and ..
  ip->i_atime = time(0L);// set to current time
  ip->i_ctime = time(0L);// set to current time
  ip->i_mtime = time(0L); // set to current time
  ip->i_blocks = 2; // LINUX: Blocks count in 512-byte chunks 
  ip->i_block[0] = bno;  // new DIR has one data block   

  for(int i = 1; i<=14;i++){ ip->i_block[i] = 0;} //i_block[1] to i_block[14] = 0;
  mip->dirty = 1;               // mark minode dirty
  iput(mip);                   // write INODE to disk


  //Finally, enter name ENTRY into parent's directory by 
  enter_name(pip,ino,name);

  return 0;

}


int rmdir()
{
  char parent[128], child[64];
  //2. get inumber of pathname:
  int ino = getino(pathname);
  //3. get its minode[ ] pointer:
  mip = iget(dev, ino);

  //4. check ownership. super user : OK. not super user: uid must match
  if(running->uid != 0)
  {
    if(running->uid != mip->INODE.i_uid)
    {
      printf("cannot remove directory : Permission denied.\n");
      return -1;
    }
  }
  // check if it is a directory
  if(!S_ISDIR(mip->INODE.i_mode))
  {
    printf("rmdir: failed to remove %s: Not a directory");
    return -1;
  }

  // ckeck if the directory is empty
  if(isEmpty(mip) == -1)
  {
    return -1;
  }

  // since basename and dirname destroys the path
  // we make a copy of the path and operate on it
  strcpy(parent,dirname(gpath));
  // since basename and dirname destroys the path
  // we make a copy of the path and operate on it
  strcpy(gpath,pathname);
  strcpy(child,basename(gpath));

  //Get the In_MEMORY minode of parent:
  int pino  = getino(parent);
  pip   = iget(mip->dev, pino);

  //Deallocate its block and inode
  for (int i=0; i<12; i++)
  {
    if (mip->INODE.i_block[i]==0){continue;}
    bdealloc(mip->dev, mip->INODE.i_block[i]);
  }
  idealloc(mip->dev, mip->ino);
  iput(mip); //(which clears mip->refCount = 0);
  

  rm_child(pip, child);

  pip->INODE.i_links_count -= 1;
  //pip->refCount -= 1;
  pip->dirty = 1;
  pip->INODE.i_mtime = time(0L);
  pip->INODE.i_atime = time(0L);
  iput(pip);
  return 0;

}

int rm_child(MINODE *parent, char *name)
{
  DIR *last_dir, *last_dir2;
  char temp[128], buf[BLKSIZE];
  char *cp2, *cp;
  //Search parent INODE's data block(s) for the entry of name
  for(int i = 0; i<12; i++)
  {
    if(parent->INODE.i_block[i] == 0) {break;}

    get_block(parent->dev,parent->INODE.i_block[i],buf);
    cp = buf;
    dp = (DIR*)buf;

    while(cp < buf + BLKSIZE)
    {
      strncpy(temp, dp->name,dp->name_len);
      temp[dp->name_len] = 0;
      // found name
      if(strcmp(temp,name) == 0)
      {
        if(cp + dp->rec_len == buf + BLKSIZE)
        {
          last_dir->rec_len += dp->rec_len;
          put_block(dev,parent->INODE.i_block[i],buf);
          return 0;
        }
        else
        {
          last_dir2 = (DIR*)buf;
          cp2 = buf;

        while(cp2 + last_dir2->rec_len < buf + BLKSIZE)
        {
          cp2 += last_dir2->rec_len;
          last_dir2 = (DIR*)cp2;
        }
        last_dir2->rec_len += dp->rec_len;
        int start = cp + dp->rec_len;
        int end = buf + BLKSIZE;

        memmove(cp,start,end-start);

        put_block(dev,parent->INODE.i_block[i],buf);
        }
      }

      cp  += dp->rec_len;
      last_dir = dp;
      dp = (DIR*)cp;     
    }
  }
}

int link()
{
  char old_file[256], new_file[256],filename[256], path[256];
  int ino,new_ino;

  strcpy(new_file,pathname2);
  strcpy(old_file,pathname);

  //(1).get the INODE of /a/b/c into memory: mip->minode[ ]
  ino = getino(old_file);
  mip = iget(dev,ino);

  //(2). check /a/b/c is a REG or LNK file (link to DIR is NOT allowed).
  if(S_ISREG(mip->INODE.i_mode) || S_ISLNK(mip->INODE.i_mode))
  {
    strcpy(path,dirname(new_file));
    strcpy(new_file,pathname2);
    strcpy(filename,basename(new_file));
 

    //(3). check /x/y  exists and is a DIR but 'z' does not yet exist in /x/y/
    new_ino = getino(path);
    if(new_ino == 0)
    {
      return 1;
    }

    pip = iget(dev,new_ino);

    strcpy(new_file,pathname2);
    if(search(pip,filename) != 0)
    {
      printf("Path to %s already exist",path);
      return 1;
    }


    enter_name(pip,ino,filename);
    //(5). increment the i_links_count of INODE by 1
    mip->INODE.i_links_count++;
    iput(mip);
    return 0;

  }
  else
  {
    printf("link to DIR is NOT allowed\n");
    return 1;
  } 
}

int unlink()
{
  char path[256], *parent,*child;

  strcpy(path,pathname);

  int ino = getino(path);

  if(ino == 0)
  {
    return 1;
  }

  mip = iget(dev,ino);

  if(S_ISREG(mip->INODE.i_mode) || S_ISLNK(mip->INODE.i_mode))
  {
    mip->INODE.i_links_count--;

    //if i_links_count == 0 ==> rm pathname by
    if(mip->INODE.i_links_count == 0)
    {
      for(int i = 0; i<12; i++)
      {
        if(mip->INODE.i_block[i] == 0){break;}
        bdealloc(dev,mip->INODE.i_block[i]);
      }

      idealloc(dev,ino);
    }

    strcpy(path,pathname);
    parent = dirname(path);
    strcpy(path,pathname);
    child = basename(path);

    ino = getino(parent);
    pip = iget(dev,ino);
    rm_child(pip,child);
    return 0;

  }

  else
  {
    printf("link to DIR is NOT allowed\n");
    return 1;
  }
  
}

int symlink()
{
  char old_file[256], new_file[256],*filename, parent[256],child[256];
  strcpy(old_file,pathname);
  strcpy(new_file,pathname2);

//(1). check: old_file exist 
  if(getino(old_file) == 0)
  {
    printf("%s does not exist",old_file);
    return 1;
  }

//(2). check: new_file does not exist
  if(getino(new_file) != 0)
  {
    printf("%s already exist",new_file);
    return 1;
  }

  strcpy(parent,dirname(new_file));
  strcpy(new_file,pathname2);
  strcpy(child,basename(new_file));

  int pino = getino(parent);
  pip = iget(dev,pino);
  strcpy(new_file,pathname2);

  //(2). creat new_file; change new_file to LNK type;
  my_creat(pip,child);
  
  //(3).  assume length of old_file name <= 60 chars
  myino = search(pip,child);

  mip = iget(dev,myino);
  if(strlen(new_file) <= 60)
  {
    //store old_file name in newfile’s INODE.i_block[ ] area.
    strcpy((char*)mip->INODE.i_block,basename(pathname));
    //set file size to length of old_file name
    mip->INODE.i_size = strlen(basename(old_file));
    //mark new_file’s minode dirty;
    mip->dirty = 1;
    mip->INODE.i_mode = 0120000;
    iput(mip);

  }
  
}