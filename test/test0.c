/* test0.c
 *	Fork 4 threads 
     main(): create two new lock(and try to destroy one lock approprately),  create new condition and 4 threads 
     A(): 2 threads need to be woken up
	 B(): Signal two times to wake up the these two threads
	 C(): ask for a lock which is not included in this process
 */

#include "syscall.h"

   	int l1,l2,lock;
	int c,c2;


int main() 
{
	StartSimulation();
    l1= CreateLock("Lock1",5);

	AcquireLock(l1);
	Write("I'm in CR \n",14,1);
	ReleaseLock(l1);
	Write("End --Main \n",14,1);

    
	Exit(11);
}

