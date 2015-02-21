#include "syscall.h";
f(){
  Write("FFFFFFFF thread!\n",17,1);
  Exit(100);
}
y(){
  Write("yyyyyyyy thread!\n",17,1);
  Exit(50);
}
forkafork(){
  Write("testing fork a fork!\n",22,1);
 /* Fork(f);*/
  Write("Fork a fork success!\n",22,1);
  Exit(17);
}
int main(){
  
  Fork(f);
  Fork(y);
  Fork(forkafork);
}
