/* testfiles.c
 *	Simple program to test the file handling system calls
 */

#include "syscall.h"

int myexp ( int count ) {
  int i;

  int val=1;
  for (i=0; i<count; i++ ) {
    val = val * 10;
  }
  return val;
}

void itoa( char arr[], int size, int val ) {
  int i;

  int max, dig, subval, loc;

  for (i=0; i<size; i++ ) {
	arr[i] = '\0';
  }

  for ( i=1; i<=2; i++ ) {
	if (( val / myexp(i) ) == 0 ) {
	  max = i-1;
	  break;
	}
  }

  subval = 0;
  loc = 0;
  for ( i=max; i>=0; i-- ) {
	dig = 48 + ((val-subval) / myexp(i));
	subval = (dig-48) * myexp(max);
	arr[loc] = dig;
	loc++;
  }

  return;
}

void WriteInt(int nachos){
	char buf[6];
	
	itoa( buf, 6, nachos);
	Write( buf, 6, 1 );

}

int returnLen(char *a){

	int n = 0;
    while(*(a++)!=0)
		n++;

	return n;
}

int main() {
	int a,b,c,d,e,f,g;
	
	
	a = CreateCondition("condition1",10);

	b = CreateCondition("condition2",10);

	c = CreateLock("Lock1",5);

	d = CreateLock("Lock2",5);

	/* just to test if we create the lock/condition with already existing name*/
	e = CreateCondition("condition1",10);

	if(a == e){
		Write("It returns the existing index when I request CONDITION with same name!",returnLen("It returns the existing index when I request CONDITION with same name!"),1);
		Write("\n",3,1);

	}

	f = CreateLock("Lock1",5);

	if(c == f){
		Write("It returns the existing index when I request LOCK with same name!",returnLen("It returns the existing index when I request LOCK with same name!"),1);
		Write("\n",3,1);
	}

	
	DestroyLock(c);

	DestroyLock(c);

	DestroyLock(d);

	DestroyCondition(a);

	DestroyCondition(a);

	DestroyCondition(b);


	Halt();
}

