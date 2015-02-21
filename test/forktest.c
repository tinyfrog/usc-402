/* forktest.c
 *	Simple program to test the file handling fork
 */

#include "syscall.h"


int main(){
	StartSimulation();
	Write("Thread x create!\n",17,1);
    Exit(100);

}
