/* halt.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

#include "syscall.h"
int a[3];
int b, c;
int l;
int c;
 X()
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

Y()
{

	Write("yyyyyyyy\n",8,1);
 
	AcquireLock(l);
	Write("Signal\n",8,1);
	
	SignalCondition(c,l);
   /* SignalCondition(c,l);*/
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


P(){
	Write("Thread P\n",10,1);
	
    Exit(18);
}

int
main()
{
  /*
	Fork(P);
	/* l = CreateLock("AAA",3);
	c = CreateCondition("hello",5);
	Fork(X);
	Fork(X);

	Yield();
	Fork(Y);
	Yield();
	Write("UUUCCC\n",7,1);
	
/*	a[0]=CreateLock("AAA",3);
   
	AcquireLock(a[0]);

    b = CreateCondition("hello",5);
    
	SignalCondition(b,a[0]);
	DestroyCondition(b);   

  

	Exit(11);*/

    /* not reached */
}

