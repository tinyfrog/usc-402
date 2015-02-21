//---------------------------------
//  Cook.cc   Author:Pei   date: 9.10
//--------------------------
///////////////////////////////////////////////////
//  
////////////////////////////////////////////////////
#include "synch.h"
#include "restaurant.h"
#include "copyright.h"
#include "cook.h"
#include "system.h"
using namespace std;


void SimulateCook(int CookIndex){
     //    printf("HELLCOOOOOOOOOOOOOOOOOOOOOOOO\n");
	 int MyIndex=CookIndex;
	/* CookDoing[CookIndex] = -1; //store which food i'm cooking now, if not working, set to -1
	 
	 CookStatus[MyIndex]=FREE;
	 
	 CookRequest[MAX_COOKS] = -1;*/
	
 
	 while (true){
     
		 //check commands from manager
	 CookRequestLock->Acquire();     // cook checks his own request
	 
	 if(CookRequest[MyIndex] == -1){      
	        CookStatus[MyIndex]=FREE;
		CookDoing[MyIndex] = -1;
		//printf("COOK %d: ^^^^^^^^^^^^-1\n", MyIndex);
	 }	 
	 else{
	    CookDoing[MyIndex]=CookRequest[MyIndex];
	        CookStatus[MyIndex]=BUSY;
		printf("Cook [%d] is going to cook [%s]\n",MyIndex+1,FoodName[CookRequest[MyIndex]]);
	 }
	  
	 //if nothing to do, have a break
	 if(CookStatus[MyIndex]==FREE){
		CookDoing[MyIndex] = -1;
		CookRequest[MyIndex] = -1;
		printf("Cook [%d] is going to break  %d  \n",MyIndex+1,CookStatus[MyIndex]==FREE);
	    CookWaitCV[MyIndex]->Wait(CookRequestLock);
		printf("Cook [%d] returned from break\n",MyIndex+1);
	    CookRequestLock->Release();
		continue;
	 }

	 CookRequestLock->Release();



	
	 /*  //neednot
	 CookLock[MyIndex]->Acquire();
	 CookCV[MyIndex]->Wait(CookLock[MyIndex]);// get Request from manager	 
	 CookStatus[MyIndex] = BUSY;*/

	 //cook 


	 InventoryLock->Acquire();
	// printf("INVENTORY---%d  %d  %d  %d-----\n",Inventory[0],Inventory[1],Inventory[2],Inventory[3]);
	 int i=CookDoing[CookIndex];
	 if(Inventory[i]>0){
		 Inventory[i]--;
	 }
	 else{
	     // if no inventory, stop to have a break
		 InventoryLock->Release();
		 
		 CookRequestLock->Acquire();
		 CookStatus[MyIndex] =FREE;
		 CookDoing[MyIndex] = -1;
		 CookRequest[MyIndex] = -1;
		 printf("Cook [%d] is going to break^^^%d^^^^\n",MyIndex+1,CookStatus[MyIndex]==FREE);
		 CookWaitCV[MyIndex]->Wait(CookRequestLock);
	     printf("Cook [%d] returned from break\n",MyIndex+1); //come back and go to check next work
	     CookRequestLock->Release();
		 continue;
	 }

	 InventoryLock->Release();

	

        int  cookingTime = abs (rand() % 2 ) ;   // make assumption of cooking time
        for (int i=0 ; i< cookingTime ; i++) {
            currentThread -> Yield() ;
        }
	



	 FoodTableLock->Acquire();  //add cooked food to FoodTable
         //printf("Cook [%d] COOKED [%s]\n",MyIndex+1,FoodName[CookDoing[MyIndex]]);
	 FoodCount[CookDoing[MyIndex]]++;// add one new cooked food
         //printf("[%s] COUNT: %d\n",FoodName[CookDoing[MyIndex]],FoodCount[CookDoing[MyIndex]]);
	 FoodTableLock->Release();
	 
        	 
	 
	 
	 } 


}