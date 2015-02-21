/* forktest.c
 *	Simple program to test the file handling fork
 */

#include "syscall.h"


int main(){
	int a,b,c;

	a = CreateCondition("condition1",10);

	b = CreateLock("aaa",3);

	AcquireLock(b);

	BroadcastCondition(a,b);

	ReleaseLock(b);

	DestroyCondition(a);

	DestroyLock(b);

	Halt();

}
