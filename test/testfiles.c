/* testfiles.c
 *	Simple program to test the file handling system calls
 */

#include "syscall.h"

   	int l;
	int c;
 a()
{


	Write("xxxxxxx\n",8,1);
   
	AcquireLock(l);
    WaitCondition(c,l);
	Write("FUCK\n",5,1);
    ReleaseLock(l);
	/*DestroyCondition(l);*/

  
	Write("END---xxxxxxx\n",14,1);

     Exit(17);
    /* not reached */
}
 



b()
{

	Write("yyyyyyyy\n",8,1);
 
	AcquireLock(l);
	Write("Signal\n",8,1);
	
	SignalCondition(c,l);
   SignalCondition(c,l);
	ReleaseLock(l);

	Write("END:yyyyyyyy\n",14,1);
/*	l = CreateLock("AAA",3);
	AcquireLock(l);


	c = CreateCondition("hello",5);
    


	SignalCondition(l,c);
	DestroyCondition(l);

  */
     Exit(9);
    /* not reached */
}

y(){
	Write("END:cccccc\n",14,1);
	/*Exit(100);*/

}

int main() {
		/*Yield();*/
  int a,b,c;
  a=CreateLock("AAA",3);
  b=CreateCondition("F",1);
  AcquireLock(a);
  WaitCondition(b,a);
  Exit(0);
}

