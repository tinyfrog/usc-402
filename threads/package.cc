//---------------------------------------------
// CLASS:PACKAGE, by pei
//--------------------------------------------

#include "synch.h"
#include "restaurant.h"
#include "package.h"



package::package(){
	for(int i=0;i<NUM_OF_FOOD;i++)
	{
		food[i]=0;	
	}
	choice = -1;
	token = -1;
	status= -1;
    ot = -1;
}


void
package::addPackage(int SIXDOLLARBURGER,int THREEDOLLARBURGER,int VIGGIEBURGER,int FRYS,int SODA,int EATIN_TOGO,int t){
	food[0]=SIXDOLLARBURGER;
	food[1]=THREEDOLLARBURGER;
	food[2]=VIGGIEBURGER;
	food[3]=FRYS;
	food[4]=SODA;
	choice=EATIN_TOGO;
	token = t;
	status=NOTREADY;
	ot = -1;
}

int
package::getChoice(){  
      return choice;
}
void
package::setChoice(int c){
      choice=c;
}


int
package::getOT(){
      return ot;
}
void
package::setOT(int packOT){
      ot=packOT;
}



int
package::getToken(){
      return token;
}
void
package::setToken(int t){
      token=t;
}

int
package::getStatus(){
      return status;
}

void
package::setStatus(int s){
      status=s;
}


int
package::getPrice(){
      return 11111;
}





bool
	package::enoughFood(){   //judge if ordertaker or manager could pack the Food.
	for(int i=0;i<NUM_OF_FOOD-1;i++)
	{
		
		if(food[i]>FoodCount[i]){
		  //		printf("not enough food! %d    %d       ", food[i],FoodCount[i]);
			return 0;
			}
	}
	//	printf(" enough food!   ");
	return 1;
}

void
	package::getFood(){
    for(int i=0;i<NUM_OF_FOOD-1;i++)
	{ 
	   FoodCount[i] -= food[i];  // NEED DEBUG
	}  
}
