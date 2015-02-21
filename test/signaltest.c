/* testfiles.c
 *	Simple program to test the file handling system calls
 */

#include "syscall.h"

int main() {
	int a,b,c;


	int i=0;
	StartSimulation();
	/*
	for(i=0;i<200000;i++){
		;
		}
    */
	a = CreateCondition("condition1",10);

	b = CreateLock("aaa",3);

	
	AcquireLock(b);


	Write("Signal \n",14,1);

	SignalCondition(a,b);


	ReleaseLock(b);
	
	Write("Release Lock\n",16,1);
	


	Exit(34);
}

