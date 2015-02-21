#include "syscall.h"

int lock1;
int lock2;


int main()
{
	StartSimulation();
	Write( "\n#### begin ###\n\0" , 17, 1 );
	lock1 = CreateLock("l1",3) ;
	lock2 = CreateLock("l2",3);
    	Write( "\n#### fork1 ###\n\0" , 16, 1 );

	Exit (33);

}
