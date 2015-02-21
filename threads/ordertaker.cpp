//---------------------------------------------
// Ordertakers, by pei
//--------------------------------------------


#include "system.h"
#include "synch.h"
#include "restaurant.h"
#include "ordertaker.h"

ordertaker::ordertaker(int index){
	 int MyIndex=index;       //
	 OTStatus[MyIndex]=FREE;  
	 int MyCUST=-1;          //indicate the customer who receives Ordertaker's service
	 int MyPack=-1;          //indicate which package would be packed at this time.

	while(true){	

	CustLineLock->Acquire();

	if(CustLineLength>0){
	    CustWaitingCV->Signal(CustLineLock);
		OTStatus[MyIndex]=2;  //wait for the customer to come and take an order.
	    CustLineLength--;
	}

	else if(UnpackedFood>0){   //have food to pack or not
		OTStatus[MyIndex]=BUSY;
		CustLineLock->Release();
		FoodTableLock->Acquire();

		//find a order which is unpacked
	    for(int i=0;i<NUM_OF_CUSTS;i++){              
			//when current counts of food could satisfy the request of one order, then odertaker packs this
			if(((Packages[i].getStatus())==NOTREADY) && Packages[i].enoughFood()){	        				
					MyPack=i;
					Packages[MyPack].getFood();
					Packages[MyPack].setStatus(READY); // here set FOOD STATUS, FOOD COULD BE SERVIED OR NOT
					UnpackedFood--;
				    //if this package is for "To Go" customers, ordertaker directly asks customer to take the food
					if(Packages[MyPack].getChoice() == TO_GO ){                        
								ToGoLock->Acquire();
								Packages[MyPack].setStatus(DONE);
								ToGoCV->Broadcast(ToGoLock);			   
								ToGoLock->Release();
					}
					break;                            //bag one order at each time
			}
		}
        
		// back to take order for customers and reset variables
		MyPack=-1;   
		OTStatus[MyIndex]=FREE;
		FoodTableLock->Release();
		continue;
	}

	else{
		OTStatus[MyIndex]=FREE;
	}


	
	OrdertakerLock[MyIndex]->Acquire();
	CustLineLock->Release();

	//get order from customer
	OrdertakerCV[MyIndex]->Wait(OrdertakerLock[MyIndex]);

	
	
	MyCUST=OTService[MyIndex];

	FoodTableLock->Acquire();
	//When this order is from "EAT_IN" customer, add this to the waiting list of the food to pack. 
	if(Packages[MyCUST].getChoice() == EAT_IN){          
		  Packages[MyCUST].setStatus(NOTREADY); 
		  UnpackedFood++;
	}
	else{                            
	//When this order is from "TO_GO" customer, Ordertakers will firstly check if customer could get food IMMIDIATELY,IF So, PACK and GIVE to customer	
		if(Packages[MyCUST].enoughFood() && Packages[MyCUST].getChoice == TO_GO ){
		Packages[MyCUST].getFood();
		Packages[MyCUST].setStatus(DONE);         //send this packed food to "TO-GO" customer             		
		//send food		
		}
		else{     
		Packages[MyCUST].setStatus(NOTREADY); 
		UnpackedFood++;
		}
	    
	}
	FoodTableLock->Release();
	
	/*if((Packages[MyCUST].getChoice()==TO_GO)&&(Packages[MyCUST].getStatus()==DONE)){
	     ToGoLock->Acquire();
		 ToGoCV->Broadcast(ToGoLock);
	
	}*/



	OrdertakerCV[MyIndex]->Signal(OrdertakerLock[MyIndex]);  //  Let customer know

	OrdertakerLock[MyIndex]->Release();



	}
}