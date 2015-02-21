//---------------------------------------------
// Ordertakers, by pei
//--------------------------------------------


#include "system.h"
#include "synch.h"
#include "restaurant.h"
#include "ordertaker.h"
#include <stdlib.h>
using namespace std;

void SimulateOrdertaker(int index){
         int count =0;    //for debug       
	 int MyIndex=index;       //
 
	 int MyCUST=-1;          //indicate the customer who receives Ordertaker's service
	 int MyPack=-1;          //indicate which package would be packed at this time.

	while(true){	
         //for(int i=0;i<1000000;i++);
	 // printf("WORKING");
	CustLineLock->Acquire();
	
	if(CustLineLength>0){
	    CustWaitingCV->Signal(CustLineLock);
		OTStatus[MyIndex]=2;  //wait for the customer to come and take an order.

	    CustLineLength--;
	   // printf("\n-----------LEFT----------CUSTS: %d\n",CustLineLength);
	}
	
       else if (UnpackedFood>0){
           
           
           FoodTableLock->Acquire();
	   OTStatus[MyIndex]=1;
	   CustLineLock->Release();
	
	 //   printf("\n++++++++++++++UnpackedFood+%d+++++++++++++++++\n",UnpackedFood);
		
				
                if(UnpackedFood<1) {              //if food has been locked, move back
			//OTStatus[MyIndex]=FREE;
			FoodTableLock->Release();		
			continue;
		}
		//find a order which is unpacked
	    for(int i=0;i<NUM_OF_CUSTS;i++){           
	                
			int st= Packages[i].getStatus();
			int b=  Packages[i].enoughFood();
			
			//printf("abcdsafdas");
			
			//when current counts of food could satisfy the request of one order, then odertaker packs this
			if( (st==0) && (b==1) ){						
					MyPack=i;
					Packages[MyPack].getFood();
					Packages[MyPack].setOT(MyIndex);
					UnpackedFood--;
					//printf("\n-------------+UnpackedFood+%d---------------\n",UnpackedFood);
                     
					printf("OrderTaker [%d] packed the food for Customer [%d]\n",MyIndex+1,MyPack+1);
				    //if this package is for "To Go" customers, ordertaker directly asks customer to take the food
					if(Packages[MyPack].getChoice() == TO_GO ){                        
								ToGoLock->Acquire();
								Packages[MyPack].setStatus(DONE);
								ToGoCV->Broadcast(ToGoLock);			   
								ToGoLock->Release();
								printf("OrderTaker [%d] gives food to Customer [%d](Customer has chosen to-go and wait for a while)\n",MyIndex+1,MyCUST+1);
								break;
					}
					
					// packages for "TOGO" customers only have two status: "NOTREADY" and "DONE", waiters could only see the packages for "EATIN" customers
					Packages[MyPack].setStatus(READY); // here set FOOD STATUS, FOOD COULD BE SERVIED OR NOT
					//CustLineLock->Release();
					break;                            //bag one order at each time
			}
		}
        
		// back to take order for customers and reset variables
		MyPack=-1;   
		OTStatus[MyIndex]=0;
		FoodTableLock->Release();

              //  currentThread -> Yield() ;
		

		continue;

	}



	else{
		OTStatus[MyIndex]=0;
	}

          
	
	OrderTakerLock[MyIndex]->Acquire();
	CustLineLock->Release();

	//get order from customer
	OrderTakerCV[MyIndex]->Wait(OrderTakerLock[MyIndex]);

	MyCUST=OTService[MyIndex];
	printf("OrderTaker [%d] is taking order of Customer [%d]\n",MyIndex+1,MyCUST+1);

 
	FoodTableLock->Acquire();
	//When this order is from "EAT_IN" customer, add this to the waiting list of the food to pack. 
	if(Packages[MyCUST].getChoice() == EAT_IN){          
		  Packages[MyCUST].setStatus(NOTREADY); 
		  UnpackedFood++;
		 // printf(" UnpackedFood :==== %d",UnpackedFood);
		  printf("OrderTaker [%d] gives token number [%d] to Customer [%d]\n",MyIndex+1,Packages[MyCUST].getToken()+1,MyCUST+1);
	}
	else{                            
	//When this order is from "TO_GO" customer, Ordertakers will firstly check if customer could get food IMMIDIATELY,IF So, PACK and GIVE to customer	
		int enough = Packages[MyCUST].enoughFood();
		int ch = Packages[MyCUST].getChoice();
		if( enough && ch == TO_GO ){
		Packages[MyCUST].getFood();
		Packages[MyCUST].setOT(MyIndex);
		//printf("customer choooooooooooose toooooooooooooooooooooooooo gooooooooooooooo\n");
		printf("OrderTaker [%d] packed the food for Customer [%d](Customer has chosen to-go the food and the food is ready)\n",MyIndex+1,MyCUST+1);
		Packages[MyCUST].setStatus(DONE);         //send this packed food to "TO-GO" customer             		
		printf("OrderTaker [%d] gives food to Customer [%d](Customer has chosen to-go the food and the food is ready)\n",MyIndex+1,MyCUST+1);
		//send food		
		}
		else{     
		Packages[MyCUST].setStatus(NOTREADY); 
		UnpackedFood++;
		printf("OrderTaker [%d] gives token number [%d] to Customer [%d]\n",MyIndex+1,Packages[MyCUST].getToken()+1,MyCUST+1);

		}
	    
	}
	FoodTableLock->Release();
	
	/*
	int ch = Packages[MyCUST].getChoice();
	int st = Packages[MyCUST].getStatus();
	if( ch ==TO_GO && st ==DONE){
	     ToGoLock->Acquire();
		 ToGoCV->Broadcast(ToGoLock);
            ToGoLock->Release();
	}*/

           

	OrderTakerCV[MyIndex]->Signal(OrderTakerLock[MyIndex]);  //  Let customer know
        //   printf("OT:%d gonna release my ordertaker lock\n ",index);
	OrderTakerLock[MyIndex]->Release();
	
	}
}
