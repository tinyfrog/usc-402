//---------------------------------------------
// Waiters, by pei
//--------------------------------------------


#include "system.h"
#include "synch.h"
#include "restaurant.h"
#include "waiter.h"


void SimulateWaiter(int index){
	//WaiterStatus[index] = FREE;
	int MyIndex=index;
	//WorkingWaiterCount
	//Package* pack;
	int MyCust = -1;   // the package waiter takes
	while(true){
               // printf("HellooooooooooooooooooooooWAITER!\n");

		PackageLock->Acquire();  //PackageLock monitors PackageList
		MyCust == -1;
		//find packed food
		for(int i=0;i<NUM_OF_CUSTS;i++){
			if((Packages[i].getStatus())==READY && CustTableNum[i]!=-1 ){   //until customer have found place to seat
            MyCust=i;
            printf("OrderTaker [%d] gives Token number [%d] to Waiter [%d] for Customer [%d]\n",Packages[MyCust].getOT()+1,Packages[MyCust].getToken()+1,MyIndex+1,MyCust+1);
		   // WorkingWaiterCount++;
			//WaiterStatus[index]=BUSY ;  //changWaiterStatus to BUSY
			if(Packages[i].getOT()== -2)  //image  OT==-1 //////////////////////////////////////////
				printf("Waiter [%d] got token number [%d] for Customer [%d] from Manager\n",MyIndex+1,Packages[MyCust].getToken()+1,MyCust+1);
			else 
			   printf("Waiter [%d] got token number [%d] for Customer [%d] from OrderTaker [%d]\n",MyIndex+1,Packages[MyCust].getToken()+1,MyCust+1,Packages[MyCust].getOT()+1);
			break;	        				
			}		
		}


		//there is no packed food, waiter turns to be on break
		if((MyCust == -1) || (Packages[MyCust].getStatus() == SERVE)){
			
			WaiterStatus[index] = FREE; 
			//PackageFoodLock->Release();
			//WaiterManagerCV->wait(WaiterManagerLock);
			//wait manager to wake up
			printf("waiter [%d] is going on break\n",MyIndex+1);
			WorkingWaiterCount--;
			
			WaiterWakeCV->Wait(PackageLock);
			printf("Waiter [%d] returned from break\n",MyIndex+1);
			WorkingWaiterCount++;
			WaiterStatus[index] = BUSY; 
			//wake up, turn back to work
			PackageLock->Release();
			//WaiterStatus[index] = BUSY ;
			//WaiterManagerLock->Release();

			continue;
		}
		
		Packages[MyCust].setStatus(DONE);
		PackageLock->Release();
		
		printf("Waiter [%d] validates the token number for Customer [%d]\n",MyIndex+1,MyCust+1);
		//waiter take the food and search for the customer("Eat_In") with this token
		EatInLock->Acquire();
		Packages[MyCust].setStatus(SERVE);
		printf("Waiter [%d] serves food to Customer [%d]\n",MyIndex+1,MyCust+1);
		EatInCV->Broadcast(EatInLock);
		
		//wait for the customer take the food 

               WaiterService[MyCust] = index;
               CustWaiterCV[index]->Wait(EatInLock);




	    //return to do next work
		EatInLock->Release();
		
	}
               
}

// take one package food -> find customer with the token -> next work
//used Lock:
// PackageFoodLock
//WaiterManagerLock
//CustomerLock[*pack.token]                    and CV for each
//

//class package