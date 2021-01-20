/* Programmer: Hermes Obiang
 * Class: CptS360 - Fall 2019
 * Date: October 24, 2019
 */

//libraries needed
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>

// struct declaration for EXT2
typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc GD;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

#define BLKSIZE 1024

char buf[1024], dbuf[1024],sbuf[256];
char *tokenizedpath[64];
char ibuf[1024];
typedef unsigned u32;
u32 ubuf[256],*p,*q;

SUPER *sp;
GD *gd;
INODE *ip;
DIR *dp;
int bmap, imap, inode_starts;
int fd,blksize,inodesize,n;

// reads a disk block into a char buffer
int get_block(int dev, int blk, char *buf)
    {   
        lseek(dev, blk*BLKSIZE, SEEK_SET);
        return read(dev, buf, BLKSIZE);
    }

// read super block (block #1) to verify disk image is an EXT2 file system
int super(char *disk)
{
   
    
    lseek(fd, (long)1024*1, 0); // block 1 on FD, offset 1024 on HD
    read(fd, buf, 1024);
    sp = (SUPER *)buf; // as a super block structure

    if(sp->s_magic == 0xEF53) // verify the magic number
    {
        printf("(1). verified it's an ext2 file system\n");
    }

    else
    {
        {
            printf("It is not an EXT2 file");
            exit(2);
        }
    }
    
}

// read in group descriptor (block #2) to get the block # of bmap, imap and inodes_start
int group(char *disk)
{

    printf("(2). Read group descriptor 0 to get bmap, imap and inodes_start\n");
    get_block(fd,2,buf);
    gd = (GD*)buf;
    bmap = gd->bg_block_bitmap; // get bmap from group descriptor
    imap = gd->bg_inode_bitmap; // get imap
    inode_starts = gd->bg_inode_table; // get inodes_start
    printf("GD info: %d %d %d %d %d %d\n",
    gd->bg_block_bitmap,
    gd->bg_inode_bitmap,
    gd->bg_inode_table,
    gd->bg_free_blocks_count,
    gd->bg_free_inodes_count,
    gd->bg_used_dirs_count); // print out group descriptot information
    printf("bmap = %d  imap = %d  inode_start = %d\n",bmap,imap,inode_starts); // print out bmap, imap and inodes_start values
}

// show all directories under the file system
int show_dir()
{
    char* cp;
    

    get_block(fd,inode_starts,buf); 

    printf("************ get root inode ************\n");
    ip = (INODE *)buf;
    ip++;

    printf("(3). Show roor DIR contents\n");

    for(int i = 0; i<15;i++)
    {
        if(ip->i_block[i])
        {
            printf("root inode data block = %d\n\n",ip->i_block[i]);
        }
    }
    printf("i_number  rec_len  name_len  name\n");
    for(int i = 0; i<12; i++) // assume DIR at most 12 direct blocks
    {
        if(ip->i_block[i] == 0)
        {
            break;
        }

        get_block(fd,ip->i_block[i],dbuf);

        dp = (DIR*)dbuf;
        cp = dbuf;

        while(cp < dbuf + 1024)
        {
            strncpy(sbuf,dp->name,dp->name_len);
            sbuf[dp->name_len] = 0;

            printf("  %4d    %4d     %4d      %s\n",
            dp->inode,dp->rec_len,dp->name_len,sbuf);

            cp +=dp->rec_len;
            dp = (DIR*)cp;
        }
        
    }
}

// tokenize the path
int tokenize(char *pathname)
{
    char *s;
    n = 0;

    s = strtok(pathname, "/"); // first call to strtok()
    while(s)
    {
        printf("    %s\n",s);
    tokenizedpath[n] = s; 
    s = strtok(0, "/"); // call strtok() until it returns NULL
    n++;
    }
}

int search()
{
    int ino, blk, offset;
    for (int i=0; i < n; i++)
    {
        ino = search_ino(tokenizedpath[i]);
        
        if (ino == 0)
        {
            printf("can't find %s\n", tokenizedpath[i]); 
            exit(1);
        }

        else
        {
            printf("found %s: ino = %d\n",tokenizedpath[i],ino);
        }
  
             // Mailman's algorithm: Convert (dev, ino) to INODE pointer
             blk    = (ino - 1) / 8 + inode_starts; 
             offset = (ino - 1) % 8;        
             get_block(fd, blk, ibuf);
             ip = (INODE *)ibuf + offset;   // ip -> new INODE
         }

         printf("size = %d\n",ip->i_size);

         for(int i = 0; i<13;i++)
         {
             printf("i_block[%d] = %d\n",i,ip->i_block[i]);
         }
         // print indirect block 
         if(ip->i_block[12])
         {
            get_block(fd, ip->i_block[12], ibuf);  
            p = (u32 *)ibuf;
            
            for(int i = 0; i<BLKSIZE/sizeof(int);i++)
            {
                if(*p)
                {
                   printf("i_block[%d] = %d\n",i,*p);
                }
                p++;                             
            }     
         }


         // print double indirect block
         char temp[1024];
        if(ip->i_block[13])
        {
            get_block(fd, 334, ibuf);  
            p = (u32 *)ibuf;
            
            for(int i = 0; i<256;i++)
            {
                if(*p)
                {
                    get_block(fd, *p, temp);  
                    q = (u32 *)temp;

                    for(int j = 0; j<256;j++)
                    {
                        if(*q)
                        {
                            printf("i_block[%d][%d] = %d\n",i, j,*q);
                        }
                        q++;                             
                    }  
                                         
                }   
                p++;  
            }
        }
}

search_ino(char *pathname)
{
    char* cp;
    int ino;


    for(int i = 0; i<12; i++)
    {
        if(ip->i_block[i] == 0)
        {
            break;
        }

        get_block(fd,ip->i_block[i],dbuf);

        dp = (DIR*)dbuf;
        cp = dbuf;

        while(cp < dbuf + 1024)
        {
            strncpy(sbuf,dp->name,dp->name_len);
            sbuf[dp->name_len] = 0;

            if(strcmp(sbuf,pathname) == 0)
            {
                return dp->inode;
            }

            cp +=dp->rec_len;
            dp = (DIR*)cp;
        }
        
    }
}


int main(int argc, char *argv[])
{
    
    fd = open(argv[1],O_RDONLY); // open the disk for read mode

    if( fd< 0)
    {
        printf("Failed to open %s",argv[1]);
        exit(1);
    }

    super(argv[1]);
    group(argv[1]);
    
    show_dir();
    printf("\ntokenize %s\n",argv[2]);
    tokenize(argv[2]);
    search();

}
