#include <stdlib.h>
#include "customer.h"
#include "synch.h"
#include "restaurant.h"
#include "system.h"

using namespace std;

void SimulateCustomer(int customerIndex){
  printf("[%d]:Enter Restaurant\n", customerIndex);
  int MyOT=-1;
  CustLineLock->Acquire();             
  for(int i=0;i<MAX_OTS;i++){         //find if any ordertaker is available
    if(OTStatus[i]==0){
      OTStatus[i]=1;                     //if find ,make that ordertaker my OT
      MyOT=i;
      break;
    }
  }
  
  if(MyOT==-1){                          //this is if not find
    CustLineLength++;                    //customer join line ,linelength up
   // printf("Customer [%d]: is moving to waitingQueue\n",customerIndex+1);
    CustWaitingCV->Wait(CustLineLock);   //put this thread in sleep untill an ordertake to wake it up
  //  printf("Customer [%d]: moving OUT\n",customerIndex+1);
  }

  for(int i=0;i<MAX_OTS;i++){        //this could only happen when ordertaker wakes the customer up. or a manager who is doing the OT's job 
    if(OTStatus[i]==2 && MyOT == -1){                 //should find because ordertaker wakes him up
      MyOT=i;                           //set this OT to be my OT
      OTStatus[i]=1; 
      break;
    }
  }

 
	if(MyOT==-1){
		MyOT=-2;
		//printf("caaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
	}


  CustLineLock->Release();              //at this point the customer is no longer in line
  
  if(MyOT!=-2){                          //when it is not -1 means the customer is serviced with an order taker
 // printf("CUST[%d] is giving order to OT[%d]\n",customerIndex+1,MyOT+1);
  
  OrderTakerLock[MyOT]->Acquire();
  printf("Customer [%d]: is giving order to Ordertaker [%d]\n ",customerIndex+1,MyOT+1);}
 
  else{
    CustManagerLock->Acquire();        //at this point the manager is taking order
   printf("Customer [%d]: is giving order to manager\n",customerIndex+1);
  }
  
  int randOrder[NUM_OF_FOOD+1]={0,0,0,0,0,0};
  int *p=randOrder;
for (int i=0;i<6;i++){
    randOrder[i]=rand()%3;    //random
    if(randOrder[i]==2){
    randOrder[i]=randOrder[i]-1;
    }
  }

  //the following if statements are only to meet the output requirement of the assignment.
  //the explaination of the randOrder[] is in customer.h
       if(randOrder[0]==1)	{
	 printf("Customer [%d] is ordering 6-dollar burger\n", customerIndex+1); 
	}else	{
	 printf("Customer [%d] is not ordering 6-dollar burger\n", customerIndex+1); 
	}
      if(randOrder[1]==1)	{
	 printf("Customer [%d] is ordering 3-dollar burger\n",customerIndex+1); 
	}else	{
	 printf("Customer [%d] is not ordering 3-dollar burger\n",customerIndex+1); 
	}
      if(randOrder[2]==1)	{
	 printf("Customer [%d] is ordering veggie burger\n",customerIndex+1); 
	}else	{
	 printf("Customer [%d] is not ordering veggie burger\n",customerIndex+1); 
	} 
       if(randOrder[3]==1)	{
	 printf("Customer [%d] is ordering franch fries\n",customerIndex+1); 
	}else	{
	 printf("Customer [%d] is not ordering franch fries\n",customerIndex+1); 
	} 
       if(randOrder[4]==1)	{
	 printf("Customer [%d] is ordering soda\n",customerIndex+1); 
	}else	{
	 printf("Customer [%d] is not ordering soda\n",customerIndex+1); 
	}
       if(randOrder[5]==1)	{
	 printf("Customer [%d]:chooses to eat-in\n",customerIndex+1); 
	}else	{
	 printf("Customer [%d]:chooses to to go\n",customerIndex+1); 	 
	}

        
	 OTService[MyOT]=customerIndex;
	Packages[customerIndex].addPackage(randOrder[0],randOrder[1],randOrder[2],randOrder[3],randOrder[4],randOrder[5], customerIndex) ;   //here we use customer index to be the token number because each customer have a distinguished index, and in fact the number of order is equal to number of customer
        //forTest:
	 //  Packages[customerIndex].addPackage(1,1,1,0,0,1, customerIndex) ;
	
	
	// printf("the package status is %d %d %d %d %d %d \n",randOrder[0],randOrder[1],randOrder[2],randOrder[3],randOrder[4],randOrder[5]);
         //OrderTakerCV[MyOT]->Signal(OrderTakerLock[MyOT]);  
         //OrderTakerCV[MyOT]->Wait(OrderTakerLock[MyOT]); 
   

    //-------  for TEST, DELETE   
      //  OrderTakerLock[MyOT]->Release();
	//---- 
	


       
       if(MyOT!=-2){
       OTService[MyOT]=customerIndex;
       }else{
       ManagerCustNum=customerIndex;
       }

       if(MyOT!=-2){                                                 //if an ordertaker is taking the order not the manager 
       OrderTakerCV[MyOT]->Signal(OrderTakerLock[MyOT]);
        OrderTakerCV[MyOT]->Wait(OrderTakerLock[MyOT]); 
       //wake up the ordertaker to process the order
       

       if(randOrder[5]==0){                                          //take to-go
                         //choose to go, wait for the OT to pack
       if(Packages[customerIndex].getStatus()==DONE)    {                // if the food is ready
       printf("Customer %d receives food from the OrderTaker %d\n",customerIndex+1,MyOT+1);
	   OrderTakerLock[MyOT]->Release();
       }else{                                                         //if choose to go and food not ready
       printf("Customer %d is given token number %d from OrderTaker %d",customerIndex+1,customerIndex+1, MyOT+1);
       //ToGoCV->Wait(ToGoLock);                     // to go food not ready
       //need an get funtion on packages to see who packed the food
       OrderTakerLock[MyOT]->Release();
	   ToGoLock->Acquire();
       while(Packages[customerIndex].getStatus()!=DONE){                     //waiting for food, after the boardcast if it is not my food i keep waiting
       ToGoCV->Wait(ToGoLock);
       }
       ToGoLock->Release();
       printf("Custome [%d] is leaving restaurant after OrderTaker  %d  packed the food!\n ",customerIndex+1,MyOT+1);    //at this point customer find his food ready so he took food and go
       //the to go customer leaves restaurant
       }
	    
       }else{                                                        //an ordertaker took the order and the customer ordered eat-in
	 printf("Customer [%d\ receives token number %d from the OrderTaker %d \n",customerIndex+1,customerIndex+1,MyOT+1);
     /*
	* SeatCustLock[customerIndex]->Acquire();  
	* CustomerStatus[customerIndex]=WAITING_FOR_SEAT;
	* SeatCustCV[customerIndex]->Wait(SeatCustLock[customerIndex]);                              //wait for manager to give seat
	* if(CustTableNum[customerIndex]!=-1){                         //the customer get a seat
	*    printf("Customer is informed by the Manager-the restaurant is not full");
	*    printf("customer %d is seated at table number %d",customerIndex, CustTableNum[customerIndex]);
	* }else{
	*    printf("Customer is informed by the Manager-the restaurant is full");
	* }
	*/ 
	  OrderTakerLock[MyOT]->Release();
       }
      }else {
      CustManagerCV->Signal(CustManagerLock);
      CustManagerCV->Wait(CustManagerLock);
        if(randOrder[5]==0){
       if(Packages[customerIndex].getStatus()==2)    {                // if the food is ready
       printf("Customer %d receives food from the manager %d\n",customerIndex+1,MyOT+1);
       CustManagerLock->Release();
	   }else{                                                         //if choose to go and food not ready
       printf("Customer %d is given token number %d from OrderTaker %d \n",customerIndex+1,customerIndex+1, MyOT+1);
                  // to go food not ready
       //need an get funtion on packages to see who packed the food
       CustManagerLock->Release();
       ToGoLock->Acquire();
       while(Packages[customerIndex].getStatus()!=DONE){                     //waiting for food, after the boardcast if it is not my food i keep waiting
       ToGoCV->Wait(ToGoLock);
       }
       ToGoLock->Release();

       printf("Custome [%d] is leaving restaurant after manager packed the food \n",customerIndex+1);    //at this point customer find his food ready so he took food and go
       //the to go customer leaves restaurant
       }
       }else{
         printf("Customer [%d] receives token number %d from the manager\n ",customerIndex+1,customerIndex+1);
         CustManagerLock->Release();
		}

      }
	   //OrderTakerLock[MyOT]->Release();
	   
	   
	   
	   
	   


    //for seat in customer request seat from manager
    if(randOrder[5]==EAT_IN){
    CustomerStatus[customerIndex]=WAITING_FOR_SEAT;
    SeatCustLock[customerIndex]->Acquire();
    SeatCustCV[customerIndex]->Wait(SeatCustLock[customerIndex]);
    while(CustTableNum[customerIndex]==-1){                                  //table is not found for this customer                                                
       printf("Customer is informed by the Manager-the restaurant is full\n");
       SeatCustCV[customerIndex]->Wait(SeatCustLock[customerIndex]);
    }
    printf("Customer is informed by the Manager-the restaurant is not full\n");
    printf(":D--customer [%d] is SEATED at table number %d\n",customerIndex+1, CustTableNum[customerIndex]+1);
    SeatCustLock[customerIndex]->Release();
     // CustomerStatus[customerIndex]=SEATED;                       // do this in manager code
    
    

    
    //cust eat in
    EatInLock->Acquire();
    printf(" Customer %d is waiting for the waiter to serve the food\n",customerIndex+1);
    while(Packages[customerIndex].getStatus()!=SERVE){	
		EatInCV->Wait(EatInLock);

	}	

      int MyWaiter = WaiterService[customerIndex];
      CustWaiterCV[MyWaiter]->Signal(EatInLock);
      printf("Customer %d is Served by Waiter \n", customerIndex+1);
    
    EatInLock->Release();

     int  Time = abs (rand() % 5 ) ;   //eating
        for (int i=0 ; i< Time ; i++) {
            currentThread -> Yield() ;
        }
     



	SeatLock->Acquire();
      printf("Customer %d is Leaving table,(leaving restaurant) \n", customerIndex+1);
   
         tables[CustomerSeat[customerIndex]]= 0;

	 SeatLock->Release();
}
    




  /*
  printf("Customer"+customerIndex+": I've made my order,asking manager if there are seats available");
  CustomerStatus[customerIndex]=0;
  SeatLock->Require();
  SeatCV->Wait(SeatLock);
  printf("Customer"+customerIndex+": Infromed from manager that seat available! go seating.....");
 
  */
}
