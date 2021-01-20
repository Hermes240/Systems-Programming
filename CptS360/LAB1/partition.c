/********************************************************************
Programmer: Hermes Obiang
Class: CptS 360, Fall 2019
Programming Assignment: LAB 1
Date: August 28, 2019

Description:   Write myprintf() that behaves exactly as de bulit in 
               printf() function in C
*********************************************************************/
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

struct partition {
	u8 drive;             /* drive number FD=0, HD=0x80, etc. */

	u8  head;             /* starting head */
	u8  sector;           /* starting sector */
	u8  cylinder;         /* starting cylinder */

	u8  sys_type;         /* partition type: NTFS, LINUX, etc. */

	u8  end_head;         /* end head */
	u8  end_sector;       /* end sector */
	u8  end_cylinder;     /* end cylinder */

	u32 start_sector;     /* starting sector counting from 0 */
	u32 nr_sectors;       /* number of of sectors in partition */
};

char buf[512];


int main(void)
{

  int fd = open("vdisk",O_RDONLY); // open vdisk for read only
  read(fd,buf,512); // read MBR into buf

  int n = 1;
  int extended = 0;
    
  struct partition *p = (struct partition *)&buf[0x1BE]; // access partition table

  printf("Device\tStart\tEnd\tSectors\tType\n");


  while(p->start_sector != 0)
    {
  printf("vdisk%d\t%d\t%d\t%d\t%x\n",
  n,p->start_sector, p->start_sector+p->nr_sectors-1,p->nr_sectors,p->sys_type);
  extended = p->start_sector;
  p++; // increment pointer
  n++;  

    }


// print extended partition P4

  
    lseek(fd,(long)extended*512, SEEK_SET); // lseek to P4 begin
    read(fd, buf, 512);    // read local partitions
    p = (struct partition *)&buf[0x1BE]; // access local ptable

    for(int i = 0;p->start_sector !=0;i++)
    {

      printf("vdisk%d\t%d\t%d\t%d\t%x\n",
      n,p->start_sector+extended + i, extended+p->start_sector+p->nr_sectors-1,p->nr_sectors,p->sys_type);
      p++; // increment pointer
      n++;
    }
      
   
   
  return 0;
}
