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
	int a,b,c;
	StartSimulation();
	
	a = CreateMonitor("monitor1",10);

	SetMonitor(a,10);

	b = RetrieveMonitor(a);

	Write("the monitor value after first set is:",returnLen("the monitor value after first set is:"),1);
	WriteInt(b);
	Write("\n",3,1);

	SetMonitor(a,15);

	b = RetrieveMonitor(a);

	Write("the monitor value after second set is:",returnLen("the monitor value after second set is:"),1);
	WriteInt(b);
	Write("\n",3,1);

	Exit(18);
}

