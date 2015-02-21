/* testfiles.c
 *	Simple program to test the file handling system calls
 */

#include "syscall.h"

int main() {
	int a,b,c;

	StartSimulation();
	
	a = CreateCondition("condition1",10);


	b = CreateLock("aaa",3);

	AcquireLock(b);

	Write("Waiting \n",14,1);

	WaitCondition(a,b);

	Write("Doing  \n",14,1);

	ReleaseLock(b);
	
	Write("Release Lock\n",24,1);


	Exit(33);
}

