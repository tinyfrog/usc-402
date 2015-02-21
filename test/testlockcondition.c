/* test0.c
 *	Fork 4 threads 
     main(): create two new lock(and try to destroy one lock approprately),  create new condition and 4 threads 
     A(): 2 threads need to be woken up
	 B(): Signal two times to wake up the these two threads
	 C(): ask for a lock which is not included in this process
 */

#include "syscall.h"

   	int l1,l2,lock;
	int c1,c2;
 A()
{



	Write("new Thread(A)\n",20,1);
    Write("A: ",2,1);
	AcquireLock(l1);
	Write("A: ",2,1);
    WaitCondition(c1,l1);
	Write(" (A)DO MY WORK\n",15,1);
	Write("A: ",2,1);
	SignalCondition(c1,l1);
	Write("A: ",2,1);
	WaitCondition(c1,l1);
	Write("A: ",2,1);
    ReleaseLock(l1);
	/*DestroyCondition(l);*/

  
	Write("END---Thread(A)\n",20,1);

     

     Exit(1);
}
 



B()
{

	Write("new Thread(B) \n",20,1);
	AcquireLock(l1);
	Write("Signal two times to wake up waiting threads\n",8,1);
	SignalCondition(c1,l1);
	SignalCondition(c1,l1);
	WaitCondition(c1,l1);
	ReleaseLock(l1);

	Write("END---Thread(B)\n",20,1);

    Exit(2);
    /* not reached */
}

C(){
	int k=1;
	Write( "Ask for a unincluded lock\n",20,1 );
	lock=9;
	AcquireLock(lock);
	Write("END:-----Thread(C)\n",20,1);
	Exit(3);
   
}

int main() 
{
    l1= CreateLock("Lock1",5);
    l2 = CreateLock("Lock2",5);
	c1 = CreateCondition("Condition1",5);
	c2 = CreateCondition("Condition2",5);
	Fork(A);
	Fork(A);
	DestroyLock(l1);
	
	/*Fork(A);*/
	Fork(C);
	Fork(B);


	Write("End --Main \n",14,1);

    

}

