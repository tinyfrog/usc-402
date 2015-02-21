//---------------------------------
//  Manager.cc      date: 9.10
//--------------------------
///////////////////////////////////////////////////
//  
////////////////////////////////////////////////////
#include "synch.h"
#include "restaurant.h"
#include "manager.h"
#include <stdlib.h>
#include "system.h"

using namespace std;

void SimulateManager(int m){
      	 
	int  Time = abs (rand() % 5 ) ;   // wait for some time to start working
        for (int i=0 ; i< Time ; i++) {
            currentThread -> Yield() ;
        }
      // printf("HELLO----Manager!!!!!!!\n");

   while(true){
       
// we set the first priority of the manager is manager the inventory because no other people can get a hand on this
       for(int i=0;i<NUM_OF_COOK_FOOD;i++){
             if(Inventory[i]<=ORDER_LEVEL) {                //check if manager need to buy anything
	     ManagerStatus=0;                          //to buy things
	       printf("\n manager refills inventory!\n");
		if(cash<ALL_INVEN_PRICE){                            //if cash is not enough
		printf("\n####Manager goes to bank to withdraw the cash \n");
		for(int a=0;a<5;a++){
		currentThread->Yield();
		}
		cash=cash+ALL_INVEN_PRICE*10;
		}

		currentThread->Yield();
		cash -= ALL_INVEN_PRICE;
		InventoryLock->Acquire();
		  printf("Manager : Inventory is loaded in the restaurant  \n");
		  for(int b=0;b<NUM_OF_COOK_FOOD;b++){
		  Inventory[b] = Inventory[b]+10;   //buy 10 per time
		  }
                InventoryLock->Release();
		ManagerStatus=1;
		break;
	     }//if need to buy so the else below means no need to buy food
	}//for loop check food 

 


 
    //Check if there are customers waiting to be seated.
     SeatLock->Acquire();
    for(int i=0;i<NUM_OF_CUSTS;i++){
    if(CustomerStatus[i]==WAITING_FOR_SEAT && CustTableNum[i]==-1){
          SeatCustLock[i]->Acquire();
	  for(int t=0;t<NUM_OF_SEATS;t++){
	  if(tables[t]==0){                                            //zero means the table is free
	      CustTableNum[i]=t;
	      CustomerStatus[i]==SEATED;
	      SeatCustCV[i]->Signal(SeatCustLock[i]);
	      CustomerSeat[i]=t;
	      tables[t]=1;
	     // printf("\n\n\n\n manager is assign table %d to customer %d \n",t+1,i+1);
              break;   
		}
	  }
	 SeatCustLock[i]->Release();
	  }//search table
    }//loop search for customer waiting for seat
	SeatLock->Release();




  // WAITER
        // printf("CHECKCHECK~~~~~~~~~~WorkingWaiterCount~~~~~~~~~~~~~~~%d\n",WorkingWaiterCount);
	//Check the status of WAITERS, if all WAITERS are ON BREAK and There are packed food required to send to CUSTOMERS,then wake WAITERS up.
	PackageLock->Acquire();
	int PFood = 0;
	int c=0;
	for(int i=0;i<NUM_OF_CUSTS;i++){
			if((Packages[i].getStatus())==READY){
                PFood++;	        				
			}		
		}
	for(int i=0;i<NUM_OF_WAITERS;i++){
                           if(WaiterStatus[i]==BUSY)
			       c++;
			       }

	//printf("PFood:_______________________________>>>%d\n",PFood);
	if(PFood>0 && c==0){
		//printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Manager calls back all Waiters from break\n");
		 WaiterWakeCV->Broadcast(PackageLock);
	}
    PackageLock->Release();
	//------------------------------------------------




/*

  //serving as an ordertaker to take order and pack food
  
    CustLineLock->Acquire();
	 int MyCUST=-1;          //indicate the customer who receives manager service
	 int MyPack=-1;          //indicate which package would be packed at this time.
    	if(CustLineLength>2){
	
	    CustWaitingCV->Signal(CustLineLock);
	    CustLineLength--;
	}
        
	else if(UnpackedFood>2){   //have food to pack or not
		CustLineLock->Release();
		FoodTableLock->Acquire();
		//find a order which is unpacked
	    for(int i=0;i<NUM_OF_CUSTS;i++){              
			//when current counts of food could satisfy the request of one order, then manager packs this
			int st= Packages[i].getStatus();
			int b=  Packages[i].enoughFood();
			if( (st==0) && (b==1) ){	        				
					MyPack=i;
					Packages[MyPack].getFood();
					Packages[MyPack].setStatus(READY); // here set FOOD STATUS, FOOD COULD BE SERVIED OR NOT
					UnpackedFood--;


				    //if this package is for "To Go" customers, manager directly asks customer to take the food
					if(Packages[MyPack].getChoice() == TO_GO ){                        
								ToGoLock->Acquire();           
								ToGoCV->Broadcast(ToGoLock);	
								Packages[MyPack].setStatus(DONE);
								ToGoLock->Release();
					}
					break;                            //bag one order at each time
			}
		}
		continue;
		// back to take order for customers and reset variables
		FoodTableLock->Release();
	}//bracket for if(custlinelength>0)
	
	
	CustManagerLock->Acquire();
	CustLineLock->Release();

	//get order from customer
	//printf("Manager is taking order of Customer %d", MyPack+1);
	CustManagerCV->Wait(CustManagerLock);

	
	
	MyCUST=ManagerCustNum;

	FoodTableLock->Acquire();
	//When this order is from "EAT_IN" customer, add this to the waiting list of the food to pack. 
	if(Packages[MyCUST].getChoice() == EAT_IN){          
		  Packages[MyCUST].setStatus(NOTREADY); 
		  printf("Manager gives token number %d to Customer %d", MyCUST,MyCUST);
		  UnpackedFood++;
	}
	else{                            
	//When this order is from "TO_GO" customer, Ordertakers will firstly check if customer could get food IMMIDIATELY,IF So, PACK and GIVE to customer	
		if(Packages[MyCUST].enoughFood() && Packages[MyCUST].getChoice() == TO_GO ){
		Packages[MyCUST].getFood();
		Packages[MyCUST].setStatus(DONE); 
                
		//send this packed food to "TO-GO" customer             		
		//send food		
		}
		else{     
		Packages[MyCUST].setStatus(NOTREADY); 
		printf("Manager gives token number %d to Customer %d", MyCUST,MyCUST);
		UnpackedFood++;
		}
	    
	}
	FoodTableLock->Release();

	CustManagerCV->Signal(CustManagerLock);  //  Let customer know

	CustManagerLock->Release();

	 //--------end of serving as OT

*/





//---------------------------------------------------------
	//  manager- cook  added by pei
	//-------------------------------------------------------
	  int  breakTime = abs (rand() % 2 )  ;
        for (int i=0 ; i< breakTime ; i++) {
            currentThread -> Yield() ;
        }

	FoodTableLock->Acquire(); 
	int request[NUM_OF_COOK_FOOD]={0,0,0,0}; //indicate which food need to cook(=1) or stop(=-1)
	for(int i=0;i<NUM_OF_COOK_FOOD;i++){
		if(FoodCount[i]<ORDER_LEVEL)
			request[i]=1;
	    else if(FoodCount[i]>=TOP_LEVEL)
			request[i]=-1;		   
	
	}
	//printf("____NEED___%d  %d  %d  %d_______\n",request[0],request[1],request[2],request[3]);
	FoodTableLock->Release();

	CookRequestLock->Acquire();
	    int one[NUM_OF_COOK_FOOD]={0,0,0,0};   // if there is a cook making the requested food
//printf("BEFORE------%d----%d----OPOPOPOPO\n",CookStatus[0],CookStatus[1]);
	    
		for(int i=0;i<NUM_OF_COOK_FOOD;i++){  //about the food which should be stopped
				for(int j=0;j<NUM_OF_COOKS;j++){
				 if( (request[i]== -1)&& (CookRequest[j]==i) ) {     //if need not to cook anymore, manager tell cook to stop cooking
					   CookRequest[j]=-1;
					   CookStatus[j] = FREE;
					 //  printf("LET COOK [%d] know he should stop\n",j+1);
				 }
				 if( (request[i]== 1)&& (CookRequest[j]==i) ){  // find if there is a cook to do the requested food
				      one[i] =1;
				    //  printf("____ONE___%d  %d  %d  %d_______\n",one[0],one[1],one[2],one[3]);
				   }
				}
		}
               
        


		for(int i=0;i<NUM_OF_COOK_FOOD;i++){ //about the food which should be cooked
		                if ((one[i] == 1) && (request[i] == 1) ){}   //if still have a cook to make the requested food, then manager does nothing
							//continue;
						else if((one[i] == 0) && (request[i]== 1) ){
						      printf("__last__ONE___%d  %d  %d  %d_______\n",one[0],one[1],one[2],one[3]);
							for(int j=0;j<NUM_OF_COOKS;j++){    //manager tell one cook to do the requested food
							      //  printf("Cook %d is going to cook veggie burger \n",j+1,CookStatus[j],CookRequest[j]);
								if(CookStatus[j] == FREE && CookRequest[j]== -1){
								//printf("Manager:Signallllllllllllllllllllllllllllll\n");
									CookRequest[j] = i;
									one[i]=1;
									CookWaitCV[j]->Signal(CookRequestLock);// wake up one cook to do the work
									//printf("\nManager: wake up one Cook[%d]\n",j+1);
									//CookStatus[j] = BUSY;
									break;
								}                       
							}
						}
		}
		
     		//printf("Manager:ENNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNND\n");
     CookRequestLock->Release();
		//-------------------------------------------------------
		//-------------------------------------------------------

   /*

	InventoryLock->Acquire();
	   
	for()

	
	InventoryLock->Release();
   */

      
	if(CustLineLength==0 && UnpackedFood==0 && PFood==0){
	      CustLineLock->Acquire();
	      CustWaitingCV->Wait(CustLineLock);
	      CustLineLock->Release();
	}


	}
}