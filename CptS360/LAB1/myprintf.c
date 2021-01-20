
/********************************************************************
Programmer: Hermes Obiang
Class: CptS 360, Fall 2019
Programming Assignment: LAB 1
Date: August 28, 2019

Description:   Write myprintf() that behaves exactly as de bulit in 
               printf() function in C
*********************************************************************/


#include<stdio.h>

typedef unsigned int u32;

char *ctable = "0123456789ABCDEF";
int  BASE = 10; 

int myprintf(char *fmt,...);
int main ()
{
   
  myprintf("cha=%c string=%s      dec=%d hex=%x oct=%o neg=%d\n", 
	       'A', "this is a test", 100,    100,   100,  -100);
  
  printf("\n");
  return 0;
}

int rpu(u32 x)
{  
    char c;
    if (x){
       c = ctable[x % BASE];
       rpu(x / BASE);
       putchar(c);
    }
}

int myrpu(u32 x, int myBASE)
{
	char c;
	if (x) {
		c = ctable[x % myBASE];
		myrpu(x / myBASE,myBASE);
		putchar(c);
	}
}

int printu(u32 x)
{
   (x==0)? putchar('0') : rpu(x);
   putchar(' ');
}

int prints(char *s)
{
   // traverse the string until encounter NULL
   while (*s != NULL)
   {
      //Fecth each character, print it and increment pointer
      putchar(*s++);

   }
}

int printx(u32 x)
{
   
   putchar('O');
   putchar('x');

   myrpu(x,16);


}
int printo(u32 x)
{
   
   myrpu(x,8);


}

int printd(int x)
{
   // if the argument is less than 0,
   // print a dash to indicate that it is negative number
   if (x < 0) 
   {
      putchar('-');
      x = -x;
      }

      myrpu(x,BASE);

}

int myprintf(char *fmt,...)
{
   char *cp = fmt;  // set *cp to point to the string ("CPTS %d",360)
   int *ip = (int *)&fmt; //ip points to the arguments past to myprintf

   
   // traverse the string until it is null
   while(cp[0] != NULL)
   {
      // if the fetched character is %,
      // the next character must be one of these "c","s","d","o","x"
      if(cp[0] == '%')
      {
         cp++;
		

		 if (cp[0] == 'c') { putchar(*(++ip)); cp++; }
		 else if (cp[0] == 's')
        { 
           prints(*(++ip)); 
           cp++; 
         }

		 else if (cp[0] == 'u') { printu(*(++ip)); cp++; }
		 else if (cp[0] == 'd') { printd(*(++ip)); cp++; }
		 else if (cp[0] == 'o') { printo(*(++ip)); cp++; }
		 else if (cp[0] == 'x') { printx(*(++ip)); cp++; }

       // if the fetched character is not one of the above, 
       // print a message
		 else
		 {
			 char * s = "Wrong format";
			 prints(s);
			 break;
		 }
      }

      //if the fetched character is not any of the special characters mentioned above
      // print it and advace to the next character
      else
      {
         putchar(*cp);

		 cp++;
      }
   }
}