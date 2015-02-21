
#include "syscall.h"

/* so weird */
int FREE = 0;
int BUSY = 1;
int STOP = 2;

int BURGER_6 = 1;
int BURGER_3 = 2;
int FRENCH_FRIES = 3;
int VEGGIE_BURGER = 4;

/* Basic Info */
const int max_cust = 20;
const int max_ot = 6;
const int max_cook = 5;
const int max_waiter = 3;

int availableSeat = 4;

int remainingCust;

int cash = 1;

int otIndexGlobal = 0;
int custIndexGlobal = 0;
int waiterIndexGlobal = 0;
int cookIndexGlobal = 0;

int otIndexGlobalLock;
int custIndexGlobalLock;
int waiterIndexGlobalLock;
int cookIndexGlobalLock;


/* Customer - OT */

int custLineCV;
int custLineLength =0;

int custLineLock;

int otStatus[6];
int otServingCust[6];

int orderTakerLock[6];
int orderTakerCV[6];



/* Customer - OT : Make Order */

int custChoice[20];


struct custOrderStruct{
	int burger_6;
	int burger_3;
	int veggieBurger;
	int soda;
	int frenchFries;
}custOrder[20];

/* Cust-OT:After Order */

int custCV[20];
int custLock[20];




/*  OrderQueue EatIn/ToGo Queue */

int orderQueueLength = 0;
int orderQueueIndex = 0; 
int orderQueue[20];

int orderQueueLock;

int eatinTokenQueueLength = 0;

int eatinTokenQueue[20]; 

int eatinTokenQueueLock;



/* Waiter is here */

int waiterLock;
int waiterCV;

/* Manager - Customer */


int managerLock;
int waitingManagerLineLength = 0; 
int managerCV;


int eatinCustStatus[20];

int waitingSeatLineLength = 0; 
int seatLock;
int seatCV;



/* Manager - Cook */

struct foodStruct{
	int burger_6;
	int burger_3;
	int frenchFries;
	int veggieBurger;
}inventory,cookedFood,orderLevel,cookedFoodUpperBound,cookedFoodLowerBound;


int cookFoodType[5]; 
int cookStatus[5]; 


int waitingCookLock;
int waitingCookCV;

int freeCookAmount = 0;  

int cookLock[5];
int cookCV[5];

int randTimeForFoodShowUp;  
int randTimeToCookAFood;

int inventoryLock ;
int inventoryCV;

int cookedFoodLock; 
int cookedFoodCV;


/* funcion defination */

int isFoodReady(struct custOrderStruct custOrder);
int isSodaOnly(struct custOrderStruct custOrder);
int isOrderNothing(struct custOrderStruct custOrder);

/* michael crowley code! */

int myints[20];
int yourints[20];

int myexp ( int count ) {
  int i;

  int val=1;
  for (i=0; i<count; i++ ) {
    val = val * 10;
  }
  return val;
}

void itoa( char arr[], int size, int val ) {
  int i;

  int max, dig, subval, loc;

  for (i=0; i<size; i++ ) {
	arr[i] = '\0';
  }

  for ( i=1; i<=2; i++ ) {
	if (( val / myexp(i) ) == 0 ) {
	  max = i-1;
	  break;
	}
  }

  subval = 0;
  loc = 0;
  for ( i=max; i>=0; i-- ) {
	dig = 48 + ((val-subval) / myexp(i));
	subval = (dig-48) * myexp(max);
	arr[loc] = dig;
	loc++;
  }

  return;
}

void WriteInt(int nachos){
	char buf[6];
	
	itoa( buf, 6, nachos);
	Write( buf, 6, 1 );

}

int returnLen(char *a){

	int n = 0;
    while(*(a++)!=0)
		n++;

	return n;
}


void Customer(){
	int k;
	int i;
	int myOT = -1;
	int custIndex;

	AcquireLock(custIndexGlobalLock);
	custIndex = custIndexGlobal;
	custIndexGlobal++;
	ReleaseLock(custIndexGlobalLock);

	eatinCustStatus[custIndex] = 0; 
	custLock[custIndex] = CreateLock("custLock",8);
	custCV[custIndex] = CreateCondition("custCV",6);


	custOrder[custIndex].burger_6 = 0;
	custOrder[custIndex].burger_3 = 0;
	custOrder[custIndex].veggieBurger = 0;
	
	AcquireLock(custLineLock);

	for(i =0;i<max_ot;i++){

		if(otStatus[i]==0){
			otStatus[i]=1;
			myOT = i;
			break;
		}
	}

	if(myOT==-1){

		custLineLength++;

		WaitCondition(custLineCV,custLineLock);


	
		
		for(i = 0;i<max_ot;i++){
			if(otStatus[i]==2){
				otStatus[i]=1;
				myOT = i;
				break;
			}

		}

	}

	ReleaseLock(custLineLock);

	AcquireLock(orderTakerLock[myOT]);

	otServingCust[myOT] = custIndex;

	SignalCondition(orderTakerCV[myOT],orderTakerLock[myOT]);

    Write( "[Customer -  ", returnLen("[Customer -  "), 1 );
	WriteInt(custIndex);
    Write( "] is giving order to [OrderTaker - ", returnLen("] is giving order to [OrderTaker - "), 1 );
	WriteInt(myOT);
	Write("]\n",3,1);


	do{
		int tempInt = (8>>3)%4;
		if(tempInt == 0)
			custOrder[custIndex].burger_6 = 1;
		else if(tempInt == 1)
			custOrder[custIndex].burger_3 = 1;
		else if(tempInt == 2)



		custOrder[custIndex].soda = (9>>3)%2;
		custOrder[custIndex].frenchFries = (76>>3)%2;
	}while(isOrderNothing(custOrder[custIndex]) == 1);

	custChoice[custIndex] = 1;

	if(custOrder[custIndex].burger_6 == 1){

		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("] is ordering 6-dollar burger",returnLen("] is ordering 6-dollar burger"),1);
		Write("\n",1,1);

	}

	else{
		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("] is not ordering 6-dollar burger",returnLen("] is not ordering 6-dollar burger"),1);
		Write("\n",1,1);
	}

	if(custOrder[custIndex].burger_3 == 1){
		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("] is ordering 6-dollar burger",returnLen("] is ordering 6-dollar burger"),1);
		Write("\n",1,1);
	}
	else{
		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("] is not ordering 3-dollar burger",returnLen("] is not ordering 3-dollar burger"),1);
		Write("\n",1,1);
	}

	if(custOrder[custIndex].veggieBurger == 1){
		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("] is ordering veggie burger",returnLen("] is ordering veggie burger"),1);
		Write("\n",1,1);
	}
	else{
		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("] is not ordering veggie burger",returnLen("] is not ordering veggie burger"),1);
		Write("\n",1,1);
	}

	if(custOrder[custIndex].frenchFries == 1){
		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("] is ordering french fries",returnLen("] is ordering french fries"),1);
		Write("\n",1,1);
	}
	else{
		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("] is not ordering french fries",returnLen("] is not ordering french fries"),1);
		Write("\n",1,1);
	}

	if(custOrder[custIndex].soda == 1){
		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("] is ordering soda",returnLen("] is ordering soda"),1);
		Write("\n",1,1);
	}
	else{
		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("] is not ordering soda",returnLen("] is not ordering soda"),1);
		Write("\n",1,1);
	}

	WaitCondition(orderTakerCV[myOT],orderTakerLock[myOT]);

	SignalCondition(orderTakerCV[myOT],orderTakerLock[myOT]);

    ReleaseLock(orderTakerLock[myOT]);

	if(custChoice[custIndex] == 0){

		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("] chooses to eat-in food",returnLen("] chooses to eat-in food"),1);
		Write("\n",1,1);


		AcquireLock(managerLock);

		waitingManagerLineLength++;
		WaitCondition(managerCV,managerLock);

		ReleaseLock(managerLock);

		AcquireLock(seatLock);
		

		if(availableSeat>0&&waitingSeatLineLength==0){
		  
			Write("[Customer - ",returnLen("[Customer - "),1);
			WriteInt(custIndex);
			Write("] is informed by the Manager the restaurant is not full",returnLen("] is informed by the Manager the restaurant is not full"),1);
			Write("\n",1,1);

			availableSeat--;

		}else{

			Write("[Customer - ",returnLen("[Customer - "),1);
			WriteInt(custIndex);
			Write("] is informed by the Manager the restaurant is full",returnLen("] is informed by the Manager the restaurant is full"),1);
			Write("\n",1,1);

			waitingSeatLineLength++;
			
			WaitCondition(seatCV,seatLock);

			Write("[Customer - ",returnLen("[Customer - "),1);
			WriteInt(custIndex);
			Write("] is seated on the table",returnLen("] is seated on the table"),1);
			Write("\n",1,1);
		}
		eatinCustStatus[custIndex] = 1;

		ReleaseLock(seatLock);

		AcquireLock(custLock[custIndex]);

		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("] is waiting for the waiter to serve the food",returnLen("] is waiting for the waiter to serve the food"),1);
		Write("\n",1,1);

		WaitCondition(custCV[custIndex],custLock[custIndex]);

		ReleaseLock(custLock[custIndex]);
		
	}
	
	else{
		
		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("] chooses to to-go food",returnLen("] chooses to to-go food"),1);
		Write("\n",1,1);

		AcquireLock(custLock[custIndex]);

		Write("[Customer - ",returnLen("[Customer - "),1);
		WriteInt(custIndex);
		Write("]:wait for his food",returnLen("]:wait for his food"),1);
		Write("\n",1,1);
		/*wait for the OT to pack food */
		WaitCondition(custCV[custIndex],custLock[custIndex]);

		ReleaseLock(custLock[custIndex]);


	}


	if(custChoice[custIndex] == 0 ){

		AcquireLock(seatLock);
		
		availableSeat++;

		ReleaseLock(seatLock);

	}

	Write("[Customer - ",returnLen("[Customer - "),1);
	WriteInt(custIndex);
	Write("] is leaving the restaurant after having food",returnLen("] is leaving the restaurant after having food"),1);
	Write("\n",1,1);

	remainingCust--;

	Write("[RemainingCustomers]: ",returnLen("[RemainingCustomers]: "),1);
	WriteInt(remainingCust);
	Write("\n",1,1);

	Exit(0);
	
}

void OrderTaker(){

	int k;
	int i;
	int otIndex;
	int custIndexUsedByOT;
	
	AcquireLock(otIndexGlobalLock);
	otIndex = otIndexGlobal;
	otIndexGlobal++;
	ReleaseLock(otIndexGlobalLock);

	otStatus[otIndex] = 0;

	orderTakerLock[otIndex] = CreateLock("orderTakerLock",14);

	orderTakerCV[otIndex] = CreateCondition("orderTakerCV",12);


	while(1 == 1){
		
		AcquireLock(custLineLock);

		if(custLineLength>0){
			
			SignalCondition(custLineCV,custLineLock); 

			custLineLength--;

			ReleaseLock(custLineLock);
			
			otStatus[otIndex] = 2;
				
		}
	
		else {

			ReleaseLock(custLineLock);
			
			otStatus[otIndex] = 0;

			AcquireLock(orderQueueLock);
			
			while(orderQueueIndex < orderQueueLength){
				
				Write("[OrderTaker - ",returnLen("[OrderTaker - "),1);
				WriteInt(otIndex);
				Write("] tries to bag a food.",returnLen("] tries to bag a food."),1);
				Write("\n",1,1);

				
				custIndexUsedByOT = orderQueue[orderQueueIndex];

				orderQueueIndex++;

				AcquireLock(cookedFoodLock);
				
				while(isFoodReady(custOrder[custIndexUsedByOT]) == 0){
					
					ReleaseLock(orderQueueLock);
					ReleaseLock(cookedFoodLock);
					Yield();
					AcquireLock(cookedFoodLock);
					AcquireLock(orderQueueLock);
					
				}


				Write("[OrderTaker - ",returnLen("[OrderTaker - "),1);
				WriteInt(otIndex);
				Write("]:Customer - ",returnLen("]:Customer - "),1);
				WriteInt(custIndexUsedByOT);
				Write(" 's Food is available to pack",returnLen(" 's Food is available to pack"),1);
				Write("\n",1,1);


				if(custOrder[custIndexUsedByOT].burger_6 == 1){ 
					cookedFood.burger_6--;
				}


				if(custOrder[custIndexUsedByOT].burger_3 == 1){
					cookedFood.burger_3--;

				}
			

				if(custOrder[custIndexUsedByOT].veggieBurger == 1){
					cookedFood.veggieBurger--;

				}
		

				if(custOrder[custIndexUsedByOT].frenchFries == 1){ 
					cookedFood.frenchFries--;

				}
 
				if(custChoice[custIndexUsedByOT]==1){
					AcquireLock(custLock[custIndexUsedByOT]);
					
					Write("[OrderTaker - ",returnLen("[OrderTaker - "),1);
					WriteInt(otIndex);
					Write("]:gives [Customer - ",returnLen("]:gives [Customer - "),1);
					WriteInt(custIndexUsedByOT);
					Write("]->his food",returnLen("]->his food"),1);
					Write("\n",1,1);
		
					SignalCondition(custCV[custIndexUsedByOT],custLock[custIndexUsedByOT]);
					ReleaseLock(custLock[custIndexUsedByOT]);


				}

				else if(custChoice[custIndexUsedByOT]==0){

					Write("[OrderTaker - ",returnLen("[OrderTaker - "),1);
					WriteInt(otIndex);
					Write("]:gives Token number - ",returnLen("]:gives Token number - "),1);
					WriteInt(eatinTokenQueueLength);
					Write(" to Waiter for Customer - ",returnLen(" to Waiter for Customer - "),1);
					WriteInt(custIndexUsedByOT);
					Write("\n",1,1);

					AcquireLock(eatinTokenQueueLock);
					
					eatinTokenQueue[eatinTokenQueueLength] = custIndexUsedByOT;
					eatinTokenQueueLength++;
					ReleaseLock(eatinTokenQueueLock);

				}

				
				ReleaseLock(cookedFoodLock);

				ReleaseLock(orderQueueLock);
					
				Yield();
				
				AcquireLock(orderQueueLock);
			}

			ReleaseLock(orderQueueLock);

		}


		AcquireLock(orderTakerLock[otIndex]);
		
		WaitCondition(orderTakerCV[otIndex],orderTakerLock[otIndex]);
		
		SignalCondition(orderTakerCV[otIndex],orderTakerLock[otIndex]);

		WaitCondition(orderTakerCV[otIndex],orderTakerLock[otIndex]);

		if(isSodaOnly(custOrder[otServingCust[otIndex]]) == 1){

			if(custChoice[otServingCust[otIndex]]==1){

				AcquireLock(custLock[otServingCust[otIndex]]);
				
				Write("[OrderTaker - ",returnLen("[OrderTaker - "),1);
				WriteInt(otIndex);
				Write("]:gives [Customer - ",returnLen("]:gives [Customer - "),1);
				WriteInt(otServingCust[otIndex]);
				Write("]->his food",returnLen("]->his food"),1);
				Write("\n",1,1);
		
				SignalCondition(custCV[otServingCust[otIndex]],custLock[otServingCust[otIndex]]);
				ReleaseLock(custLock[otServingCust[otIndex]]);
			}
			else if(custChoice[otServingCust[otIndex]]==0){

				Write("[OrderTaker - ",returnLen("[OrderTaker - "),1);
				WriteInt(otIndex);
				Write("]:gives Token number - ",returnLen("]:gives Token number - "),1);
				WriteInt(eatinTokenQueueLength);
				Write(" to Waiter for Customer - ",returnLen(" to Waiter for Customer - "),1);
				WriteInt(otServingCust[otIndex]);
				Write("\n",1,1);
				
				AcquireLock(eatinTokenQueueLock);
				eatinTokenQueue[eatinTokenQueueLength] = otServingCust[otIndex];
				eatinTokenQueueLength++;
				ReleaseLock(eatinTokenQueueLock);


			}
		}

		else{
			AcquireLock(cookedFoodLock);

			if(isFoodReady(custOrder[otServingCust[otIndex]]) == 1){
					

				Write("[OrderTaker - ",returnLen("[OrderTaker - "),1);
				WriteInt(otIndex);
				Write("]:Customer - ",returnLen("]:Customer - "),1);
				WriteInt(otServingCust[otIndex]);
				Write(" 's Food is available to pack",returnLen(" 's Food is available to pack"),1);
				Write("\n",1,1);

				if(custOrder[otServingCust[otIndex]].burger_6 == 1){ 
					cookedFood.burger_6--;

				}


				if(custOrder[otServingCust[otIndex]].burger_3 == 1){
					cookedFood.burger_3--;

				}
			

				if(custOrder[otServingCust[otIndex]].veggieBurger == 1){
					cookedFood.veggieBurger--;

				}
		

				if(custOrder[otServingCust[otIndex]].frenchFries == 1){ 
					cookedFood.frenchFries--;

				}


				if(custChoice[otServingCust[otIndex]]==0){

					Write("[OrderTaker - ",returnLen("[OrderTaker - "),1);
					WriteInt(otIndex);
					Write("]:gives Token number - ",returnLen("]:gives Token number - "),1);
					WriteInt(eatinTokenQueueLength);
					Write(" to Waiter for Customer - ",returnLen(" to Waiter for Customer - "),1);
					WriteInt(otServingCust[otIndex]);
					Write("\n",1,1);

					AcquireLock(eatinTokenQueueLock);
					eatinTokenQueue[eatinTokenQueueLength] = otServingCust[otIndex];
					eatinTokenQueueLength++;
					ReleaseLock(eatinTokenQueueLock);

				}

				else if(custChoice[otServingCust[otIndex]]==1){

					AcquireLock(custLock[otServingCust[otIndex]]);
					
					Write("[OrderTaker - ",returnLen("[OrderTaker - "),1);
					WriteInt(otIndex);
					Write("]:gives [Customer - ",returnLen("]:gives [Customer - "),1);
					WriteInt(otServingCust[otIndex]);
					Write("]->his food",returnLen("]->his food"),1);
					Write("\n",1,1);


			
					SignalCondition(custCV[otServingCust[otIndex]],custLock[otServingCust[otIndex]]);
					ReleaseLock(custLock[otServingCust[otIndex]]);

				}

			}

			else{

				AcquireLock(orderQueueLock);

				orderQueue[orderQueueLength] = otServingCust[otIndex];
				orderQueueLength++;

				ReleaseLock(orderQueueLock);

			}

			ReleaseLock(cookedFoodLock);
		}
		
		
		ReleaseLock(orderTakerLock[otIndex]);

		Yield(); 
	
	}
	
	Exit(0);

}




void Waiter(){

	int i;
	int waiterIndex;

	AcquireLock(waiterIndexGlobalLock);
	waiterIndex = waiterIndexGlobal;
	waiterIndexGlobal++;
	ReleaseLock(waiterIndexGlobalLock);
       


	while(1 == 1){

		AcquireLock(waiterLock);


		Write("[Waiter -",returnLen("[Waiter -"),1);
		WriteInt(waiterIndex);
		Write("] is going on break. ",returnLen("] is going on break. "),1);
		Write("\n",1,1);


		WaitCondition(waiterCV,waiterLock);

		Write("[Waiter -",returnLen("[Waiter -"),1);
		WriteInt(waiterIndex);
		Write("] returned from break. ",returnLen("] returned from break. "),1);
		Write("\n",1,1);

		AcquireLock(eatinTokenQueueLock);

		for(i = 0;i < eatinTokenQueueLength;i++){

			if(eatinCustStatus[eatinTokenQueue[i]] == 1){
				eatinCustStatus[eatinTokenQueue[i]] = -1;
				
				Write("[Waiter -",returnLen("[Waiter -"),1);
				WriteInt(waiterIndex);
				Write("] validates the token number for Customer - ",returnLen("] validates the token number for Customer - "),1);
				WriteInt(eatinTokenQueue[i]);
				Write("\n",1,1);

				AcquireLock(custLock[eatinTokenQueue[i]]);
			

				Write("[Waiter -",returnLen("[Waiter -"),1);
				WriteInt(waiterIndex);
				Write("] serves food to Customer - ",returnLen("] serves food to Customer - "),1);
				WriteInt(eatinTokenQueue[i]);
				Write("\n",1,1);

				SignalCondition(custCV[eatinTokenQueue[i]],custLock[eatinTokenQueue[i]]);
				ReleaseLock(custLock[eatinTokenQueue[i]]);
				break;
			}
			

		}

		ReleaseLock(eatinTokenQueueLock);

		ReleaseLock(waiterLock);
	}

	Exit(0);
}



void Cook(){

	int k;
	int i;
	int cookIndex;

	AcquireLock(cookIndexGlobalLock);
	cookIndex = cookIndexGlobal;
	cookIndexGlobal++;
	ReleaseLock(cookIndexGlobalLock);

	cookLock[cookIndex] = CreateLock("cookLock",8);

	cookCV[cookIndex] = CreateCondition("cookCV",6);


	while(1 == 1){
		
		AcquireLock(cookLock[cookIndex]);

		if(cookStatus[cookIndex] == BUSY){   

			switch (cookFoodType[cookIndex]){
				case 1:
					

					AcquireLock(inventoryLock); 
					
					if(inventory.burger_6 > 0){
						
						inventory.burger_6--;
						ReleaseLock(inventoryLock);
						

						for (i = 0; i < randTimeToCookAFood; i++)   
							Yield();
						
						
						AcquireLock(cookedFoodLock);	
						cookedFood.burger_6++;
						ReleaseLock(cookedFoodLock);
						
					}
					else{
						ReleaseLock(inventoryLock);

						AcquireLock(waitingCookLock);

						Write("[Cook - ",returnLen("[Cook - "),1);
						WriteInt(cookIndex);
						Write("] is going on break",returnLen("] is going on break"),1);
						Write("\n",1,1);


						freeCookAmount++;
						ReleaseLock(waitingCookLock);
						
						cookStatus[cookIndex] = FREE; 
						WaitCondition(cookCV[cookIndex],cookLock[cookIndex]);

						Write("[Cook - ",returnLen("[Cook - "),1);
						WriteInt(cookIndex);
						Write("] returned from break",returnLen("] returned from break"),1);
						Write("\n",1,1);
					}
					break;

				case 2:


					AcquireLock(inventoryLock); 
					if(inventory.burger_3 > 0){
						
						inventory.burger_3--;
						ReleaseLock(inventoryLock);
						
						

						for (i = 0; i < randTimeToCookAFood; i++)    
							Yield();
						

						AcquireLock(cookedFoodLock);	
						cookedFood.burger_3++;
						ReleaseLock(cookedFoodLock);
						
					}
					else{
						ReleaseLock(inventoryLock);

						AcquireLock(waitingCookLock);
						Write("[Cook - ",returnLen("[Cook - "),1);
						WriteInt(cookIndex);
						Write("] is going on break",returnLen("] is going on break"),1);
						Write("\n",1,1);
						freeCookAmount++;
						ReleaseLock(waitingCookLock);
						
						cookStatus[cookIndex] = FREE; 

						WaitCondition(cookCV[cookIndex],cookLock[cookIndex]);
						
						Write("[Cook - ",returnLen("[Cook - "),1);
						WriteInt(cookIndex);
						Write("] returned from break",returnLen("] returned from break"),1);
						Write("\n",1,1);
					}
					break;

				case 3:

					AcquireLock(inventoryLock); 

					if(inventory.veggieBurger > 0){
						
						inventory.veggieBurger--;
						ReleaseLock(inventoryLock);
		
						for (i = 0; i < randTimeToCookAFood; i++)    
							Yield();
						

						AcquireLock(cookedFoodLock);	
						cookedFood.veggieBurger++;
						ReleaseLock(cookedFoodLock);
						
					}
					else{
						ReleaseLock(inventoryLock);

						AcquireLock(waitingCookLock);
						Write("[Cook - ",returnLen("[Cook - "),1);
						WriteInt(cookIndex);
						Write("] is going on break",returnLen("] is going on break"),1);
						Write("\n",1,1);
						freeCookAmount++;
						ReleaseLock(waitingCookLock);
						

						cookStatus[cookIndex] = FREE; 

						WaitCondition(cookCV[cookIndex],cookLock[cookIndex]);
						
						Write("[Cook - ",returnLen("[Cook - "),1);
						WriteInt(cookIndex);
						Write("] returned from break",returnLen("] returned from break"),1);
						Write("\n",1,1);
					}
					break;

					
				case 4:

					AcquireLock(inventoryLock); 

					if(inventory.frenchFries > 0){
						
						inventory.frenchFries--;
						ReleaseLock(inventoryLock);
		
						for (i = 0; i < randTimeToCookAFood; i++)    
							Yield();
						
						AcquireLock(cookedFoodLock);	
						cookedFood.frenchFries++;
						ReleaseLock(cookedFoodLock);
						
					}
					else{
						ReleaseLock(inventoryLock);

						AcquireLock(waitingCookLock);
						Write("[Cook - ",returnLen("[Cook - "),1);
						WriteInt(cookIndex);
						Write("] is going on break",returnLen("] is going on break"),1);
						Write("\n",1,1);
						freeCookAmount++;

						ReleaseLock(waitingCookLock);
						
						cookStatus[cookIndex] = FREE; 

						WaitCondition(cookCV[cookIndex],cookLock[cookIndex]);
						
						Write("[Cook - ",returnLen("[Cook - "),1);
						WriteInt(cookIndex);
						Write("] returned from break",returnLen("] returned from break"),1);
						Write("\n",1,1);
					}
					break;

			
				default:
						Write("Let's Dota!",returnLen("Let's Dota!"),1);
						Write("\n",1,1);


					
			}

		}
		else if(cookStatus[cookIndex] == STOP){   
			cookStatus[cookIndex] = FREE;
			
			Write("[Cook - ",returnLen("[Cook - "),1);
			WriteInt(cookIndex);
			Write("] is informed by Manager to stop for cooking too much same food",returnLen("] is informed by Manager to stop for cooking too much same food"),1);
			Write("\n",1,1);

			Write("[Cook - ",returnLen("[Cook - "),1);
			WriteInt(cookIndex);
			Write("] is going on break",returnLen("] is going on break"),1);
			Write("\n",1,1);

			freeCookAmount++;
			WaitCondition(cookCV[cookIndex],cookLock[cookIndex]);

			Write("[Cook - ",returnLen("[Cook - "),1);
			WriteInt(cookIndex);
			Write("] returned from break",returnLen("] returned from break"),1);
			Write("\n",1,1);

		}
		ReleaseLock(cookLock[cookIndex]);
	}
}

void Manager(){
 
	int k;
	int i;
	int custIndexUsedByManager;
	int isThisFoodShort[4];
	int cookId;

	int myCook = -1;

	for(i=0; i<max_cook; i++){
		cookStatus[i] = -1;
		cookFoodType[i] = -1;
		
	}
	
	for(i = 0; i<4; i++)
		isThisFoodShort[i]=0;

	cookId = 0;    

	while(1 == 1){
	  
		AcquireLock(managerLock);
		while(waitingManagerLineLength>0){
			SignalCondition(managerCV,managerLock);

			waitingManagerLineLength--;	
		}
		ReleaseLock(managerLock);


		AcquireLock(seatLock);
		while(waitingSeatLineLength>0 && availableSeat>0){
			SignalCondition(seatCV,seatLock);
			waitingSeatLineLength-- ;
			availableSeat--;
		}

		ReleaseLock(seatLock);

		AcquireLock(eatinTokenQueueLock);
		

		for(i = 0;i< eatinTokenQueueLength; i++){
			if(eatinCustStatus[eatinTokenQueue[i]] == 1 ){
				AcquireLock(waiterLock);
				SignalCondition(waiterCV,waiterLock);
				ReleaseLock(waiterLock);
			}
		}

		ReleaseLock(eatinTokenQueueLock);

		
		AcquireLock(inventoryLock);

		if((inventory.burger_6 < orderLevel.burger_6) || (inventory.burger_3 < orderLevel.burger_3) || (inventory.veggieBurger < orderLevel.veggieBurger) || (inventory.frenchFries < orderLevel.frenchFries) )
		{

			Write("[Manager] refills the inventory",returnLen("[Manager] refills the inventory"),1);
			Write("\n",1,1);

			if( cash <= 0 ){

				Write("[Manager] goes to bank to withdraw the cash",returnLen("[Manager] goes to bank to withdraw the cash"),1);
			    Write("\n",1,1);
				Yield();
				cash += 12;
			}
			
			if(inventory.burger_6 < orderLevel.burger_6){
				isThisFoodShort[0] = 1;
				cash--;
			}

			if(inventory.burger_3 < orderLevel.burger_3){
				isThisFoodShort[1] = 1;
				cash--;
			}

			if(inventory.veggieBurger < orderLevel.veggieBurger){
				isThisFoodShort[2] = 1;
				cash--;
			}

			if(inventory.frenchFries  < orderLevel.frenchFries){
				isThisFoodShort[3] = 1;
				cash--;
			}

			for (i = randTimeForFoodShowUp; i > 0; i--){    
				ReleaseLock(inventoryLock);
				Yield();
				AcquireLock(inventoryLock);
			}

			
			inventory.burger_6 = (isThisFoodShort[0] == 1) ? inventory.burger_6 + 20 : inventory.burger_6;
			inventory.burger_3 = (isThisFoodShort[1] == 1) ? inventory.burger_3 + 20 : inventory.burger_3;
			inventory.veggieBurger = (isThisFoodShort[2] == 1) ? inventory.veggieBurger + 20 : inventory.veggieBurger;
			inventory.frenchFries = (isThisFoodShort[3] == 1) ? inventory.frenchFries  + 20 : inventory.frenchFries;
			

			Write("[Manager] Inventory is loaded in the restaurant",returnLen("[Manager] Inventory is loaded in the restaurant"),1);
			Write("\n",1,1);

			for(i = 0;i<4;i++)
				isThisFoodShort[i] = 0;
		
		}

		ReleaseLock(inventoryLock);
		
		
		AcquireLock(waitingCookLock);
		AcquireLock(cookedFoodLock);

		if(cookedFood.burger_6 < cookedFoodLowerBound.burger_6){

			if(freeCookAmount > 0)   
			{
				for(k = 0; k < max_cook; k++){   
					if(cookStatus[k] == FREE){

						cookFoodType[k] = BURGER_6;
						cookStatus[k] = BUSY;
						myCook = k;
						break;
					}
				}
				if(myCook != -1){
					

					Write("[Manager] informs [Cook - ",returnLen("[Manager] informs [Cook - "),1);
					WriteInt(myCook);
					Write("] to cook 6-dollar burger",returnLen("] to cook 6-dollar burger"),1);
					Write("\n",1,1);
											
					freeCookAmount--;
					
					AcquireLock(cookLock[myCook]);
					SignalCondition(cookCV[myCook], cookLock[myCook]); 

					Write("[Cook - ",returnLen("[Cook - "),1);
					WriteInt(myCook);
					Write("] is going to cook 6-dollar burger",returnLen("] is going to cook 6-dollar burger"),1);
					Write("\n",1,1);

					ReleaseLock(cookLock[myCook]);
			
					myCook = -1;  
				
				}

				
			}
			else{      
				

				if(cookId<max_cook){

					Write("[Manager] hires a new [Cook - ",returnLen("[Manager] hires a new [Cook - "),1);
					WriteInt(cookId);
					Write("]\n",3,1);

					
					Write("[Manager] informs [Cook - ",returnLen("[Manager] informs [Cook - "),1);
					WriteInt(cookId);
					Write("] to cook 6-dollar burger",returnLen("] to cook 6-dollar burger"),1);
					Write("\n",1,1);



					Fork(Cook);

					cookStatus[cookId] = BUSY; 

					cookFoodType[cookId] = BURGER_6;	
					
					Write("[Cook - ",returnLen("[Cook - "),1);
					WriteInt(cookId);
					Write("] is going to cook 6-dollar burger",returnLen("] is going to cook 6-dollar burger"),1);
					Write("\n",1,1);
					
					cookId++;

					ReleaseLock(cookedFoodLock);
					Yield();
					AcquireLock(cookedFoodLock);
				}
			}

		}

		else if(cookedFood.burger_6 > cookedFoodUpperBound.burger_6){
				

			for(k = 0; k < max_cook; k++)   
			{
				if(cookFoodType[k] == BURGER_6){
					cookStatus[k] = STOP;
				}
			}	
		}	


		if(cookedFood.burger_3 < cookedFoodLowerBound.burger_3){

			if(freeCookAmount > 0)  
			{
				for(k = 0; k < max_cook; k++){   
					if(cookStatus[k] == FREE){

						cookFoodType[k] = BURGER_3;
						cookStatus[k] = BUSY;
						myCook = k;
						break;
					}
				}
				if(myCook != -1){
					
					Write("[Manager] informs [Cook - ",returnLen("[Manager] informs [Cook - "),1);
					WriteInt(myCook);
					Write("] to cook 3-dollar burger",returnLen("] to cook 3-dollar burger"),1);
					Write("\n",1,1);
											
					freeCookAmount--;
					
					AcquireLock(cookLock[myCook]);
					SignalCondition(cookCV[myCook], cookLock[myCook]); 
					
					Write("[Cook - ",returnLen("[Cook - "),1);
					WriteInt(myCook);
					Write("] is going to cook 3-dollar burger",returnLen("] is going to cook 3-dollar burger"),1);
					Write("\n",1,1);

					ReleaseLock(cookLock[myCook]);
			
					myCook = -1; 
				
				}

				
			}
			else{      
				
				if(cookId < max_cook){

					
					Write("[Manager] hires a new [Cook - ",returnLen("[Manager] hires a new [Cook - "),1);
					WriteInt(cookId);
					Write("]\n",3,1);
					
					Write("[Manager] informs [Cook - ",returnLen("[Manager] informs [Cook - "),1);
					WriteInt(cookId);
					Write("] to cook 3-dollar burger",returnLen("] to cook 3-dollar burger"),1);
					Write("\n",1,1);


					Fork(Cook);
					
					cookStatus[cookId] = BUSY; 
					cookFoodType[cookId] = BURGER_3;				
					
					Write("[Cook - ",returnLen("[Cook - "),1);
					WriteInt(cookId);
					Write("] is going to cook 3-dollar burger",returnLen("] is going to cook 3-dollar burger"),1);
					Write("\n",1,1);
					
					cookId++;

					ReleaseLock(cookedFoodLock);
					Yield();
					AcquireLock(cookedFoodLock);
				}
			}
		}

		else if(cookedFood.burger_3 > cookedFoodUpperBound.burger_3){


			for(k = 0; k < max_cook; k++)  
			{

				if(cookFoodType[k] == BURGER_3){
					cookStatus[k] = STOP;
				}
			}	
		}	

		if(cookedFood.veggieBurger < cookedFoodLowerBound.veggieBurger){

			if(freeCookAmount > 0)   
			{
				for(k = 0; k < max_cook; k++){   
					if(cookStatus[k] == FREE){

						cookFoodType[k] = VEGGIE_BURGER;
						cookStatus[k] = BUSY;
						myCook = k;
						break;
					}
				}
				if(myCook != -1){
					
					Write("[Manager] informs [Cook - ",returnLen("[Manager] informs [Cook - "),1);
					WriteInt(myCook);
					Write("] to cook veggie burger",returnLen("] to cook veggie burger"),1);
					Write("\n",1,1);
											
					freeCookAmount--;
					
					AcquireLock(cookLock[myCook]);
					SignalCondition(cookCV[myCook], cookLock[myCook]); 


					Write("[Cook - ",returnLen("[Cook - "),1);
					WriteInt(myCook);
					Write("] is going to cook veggie burger",returnLen("] is going to cook veggie burger"),1);
					Write("\n",1,1);


					ReleaseLock(cookLock[myCook]);
			
					myCook = -1;  
				
				}

				
			}
			else{      
				

				if(cookId < max_cook){

					
					Write("[Manager] hires a new [Cook - ",returnLen("[Manager] hires a new [Cook - "),1);
					WriteInt(cookId);
					Write("]\n",3,1);

					Write("[Manager] informs [Cook - ",returnLen("[Manager] informs [Cook - "),1);
					WriteInt(cookId);
					Write("] to cook veggie burger",returnLen("] to cook veggie burger"),1);
					Write("\n",1,1);


					Fork(Cook);

					cookStatus[cookId] = BUSY;
					cookFoodType[cookId] = VEGGIE_BURGER;	
					
					Write("[Cook - ",returnLen("[Cook - "),1);
					WriteInt(cookId);
					Write("] is going to cook veggie burger",returnLen("] is going to cook veggie burger"),1);
					Write("\n",1,1);
					
					cookId++;

					ReleaseLock(cookedFoodLock);
					Yield();
					AcquireLock(cookedFoodLock);
				}
			}

		}

		else if(cookedFood.veggieBurger > cookedFoodUpperBound.veggieBurger){
			

			for(k = 0; k < max_cook; k++)   
			{
				if(cookFoodType[k] == VEGGIE_BURGER){
					cookStatus[k] = STOP;
					
				}
			}	
		}


		if(cookedFood.frenchFries < cookedFoodLowerBound.frenchFries){
			

			if(freeCookAmount > 0)  
			{
				
				for(k = 0; k < max_cook; k++){  
					
					
					if(cookStatus[k] == FREE){
						
						cookFoodType[k] = FRENCH_FRIES;
						cookStatus[k] = BUSY;
						myCook = k;
						break;
					}
				}
				if(myCook != -1){
					
					Write("[Manager] informs [Cook - ",returnLen("[Manager] informs [Cook - "),1);
					WriteInt(myCook);
					Write("] to cook french fries",returnLen("] to cook french fries"),1);
					Write("\n",1,1);
											
					freeCookAmount--;
					
					AcquireLock(cookLock[myCook]);
					SignalCondition(cookCV[myCook], cookLock[myCook]); 

					Write("[Cook - ",returnLen("[Cook - "),1);
					WriteInt(myCook);
					Write("] is going to cook french fires",returnLen("] is going to cook french fries"),1);
					Write("\n",1,1);

					ReleaseLock(cookLock[myCook]);
			
					myCook = -1; 
				
				}

				
			}
			else{      
				
				if(cookId < max_cook){

					
					Write("[Manager] hires a new [Cook - ",returnLen("[Manager] hires a new [Cook - "),1);
					WriteInt(cookId);
					Write("]\n",3,1);
					
					Write("[Manager] informs [Cook - ",returnLen("[Manager] informs [Cook - "),1);
					WriteInt(cookId);
					Write("] to cook french fries",returnLen("] to cook french fries"),1);
					Write("\n",1,1);

					
					Fork(Cook);

					cookStatus[cookId] = BUSY;
					cookFoodType[cookId] = FRENCH_FRIES;
					
					Write("[Cook - ",returnLen("[Cook - "),1);
					WriteInt(cookId);
					Write("] is going to cook french fires",returnLen("] is going to cook french fries"),1);
					Write("\n",1,1);
					
					cookId++;

					ReleaseLock(cookedFoodLock);
					Yield();
					AcquireLock(cookedFoodLock);
				}
			}

		}

		else if(cookedFood.frenchFries > cookedFoodUpperBound.frenchFries){

			for(k = 0; k < max_cook; k++)  
			{
				if(cookFoodType[k] == FRENCH_FRIES){
					cookStatus[k] = STOP;
					
				}
			}	
		}

		ReleaseLock(cookedFoodLock);
		ReleaseLock(waitingCookLock);
		
		
		AcquireLock(orderQueueLock);
		if(orderQueueIndex < orderQueueLength){
					
			Write("[Manager] tries to bag a food.",returnLen("[Manager] tries to bag a food."),1);
			Write("\n",1,1);

			Write("[RemainningCust] = ",returnLen("[RemainningCust] = "),1);
			WriteInt(remainingCust);
			Write("\n",1,1);

			custIndexUsedByManager = orderQueue[orderQueueIndex];
	
			AcquireLock(cookedFoodLock);
	
			if(isFoodReady(custOrder[custIndexUsedByManager]) == 1){

				orderQueueIndex++;

				Write("[Manager]:Customer - ",returnLen("[Manager]:Customer - "),1);
				WriteInt(custIndexUsedByManager);
				Write(" 's Food is available to pack",returnLen(" 's Food is available to pack"),1);
				Write("\n",1,1);


				if(custOrder[custIndexUsedByManager].burger_6 == 1){ 
					cookedFood.burger_6--;
				}


				if(custOrder[custIndexUsedByManager].burger_3 == 1){
					cookedFood.burger_3--;
				}
			

				if(custOrder[custIndexUsedByManager].veggieBurger == 1){
					cookedFood.veggieBurger--;
				}
		

				if(custOrder[custIndexUsedByManager].frenchFries == 1){ 
					cookedFood.frenchFries--;
				}

				if(custChoice[custIndexUsedByManager]==1){
					AcquireLock(custLock[custIndexUsedByManager]);
				
					Write("[Manager]:gives [Customer - ",returnLen("[Manager]:gives [Customer - "),1);
				    WriteInt(custIndexUsedByManager);
				    Write("]->his food",returnLen("]->his food"),1);
				    Write("\n",1,1);
			
					SignalCondition(custCV[custIndexUsedByManager],custLock[custIndexUsedByManager]);
					ReleaseLock(custLock[custIndexUsedByManager]);

				}


				else if(custChoice[custIndexUsedByManager]==0){

					Write("[Manager]:gives Token number - ",returnLen("[Manager]:gives Token number - "),1);
				    WriteInt(eatinTokenQueueLength);
				    Write(" to Waiter for Customer - ",returnLen(" to Waiter for Customer - "),1);
					WriteInt(custIndexUsedByManager);
				    Write("\n",1,1);

					AcquireLock(eatinTokenQueueLock);
					eatinTokenQueue[eatinTokenQueueLength] = custIndexUsedByManager;
					eatinTokenQueueLength++;
					ReleaseLock(eatinTokenQueueLock);

				}

			}

			ReleaseLock(cookedFoodLock);

		}

		ReleaseLock(orderQueueLock);

		if(remainingCust==0){

			Write("[Manager]:All Customer is Served. Restuarant Closed",returnLen("[Manager]:All Customer is Served. Restuarant Closed"),1);
			Write("\n",1,1);
			

			for(k = 0; k < max_cook; k++) 
			{
				cookStatus[k] = STOP;
			}	
			
			break;
		}

		Yield();
	
	}

	Exit(0);
	
}


int isFoodReady(struct custOrderStruct custOrderPara){

	int flag = 1;
	
	if(custOrderPara.burger_6 == 1 && cookedFood.burger_6 == 0)
			flag = 0;

	if(custOrderPara.burger_3 == 1 && cookedFood.burger_3 == 0)
			flag = 0;

	if(custOrderPara.veggieBurger == 1 && cookedFood.veggieBurger == 0)
			flag = 0;

	if(custOrderPara.frenchFries == 1 && cookedFood.frenchFries == 0)
			flag = 0;

	return flag;

}


int isSodaOnly(struct custOrderStruct custOrderPara){
	int flag = 1;

	if(custOrderPara.burger_6 == 1||custOrderPara.burger_3 == 1 ||custOrderPara.veggieBurger == 1||custOrderPara.frenchFries == 1)
		flag = 0;

	return flag;
}


int isOrderNothing(struct custOrderStruct custOrderPara){
	int flag = 0;

	if(custOrderPara.burger_6 == 0&&custOrderPara.burger_3 == 0 && custOrderPara.veggieBurger == 0 && custOrderPara.frenchFries == 0 && custOrderPara.soda == 0)
		flag = 1;

	return flag;
}




int main()

{
	int i;

	otIndexGlobalLock = CreateLock("otIndexGlobalLock",17);
	custIndexGlobalLock = CreateLock("custIndexGlobalLock",19);
	waiterIndexGlobalLock = CreateLock("waiterIndexGlobalLock",21);
	cookIndexGlobalLock = CreateLock("cookIndexGlobalLock",19);

	
	custLineCV = CreateCondition("custLineCV",10);
	custLineLock  = CreateLock("custLineLock",12);
	Write("CNM ",4,1);
	WriteInt(custLineLock);
	Write("\n",1,1);
	inventoryLock = CreateLock("inventoryLock",13);
	inventoryCV = CreateCondition("inventoryCV",11);

	cookedFoodLock = CreateLock("cookedFoodLock",14); 
	cookedFoodCV = CreateCondition("cookedFoodCV",12);

	waitingCookLock = CreateLock("waitingCookLock",15);
	waitingCookCV = CreateCondition("waitingCookCV",13);

	waiterLock = CreateLock("waiterLock",10);
	waiterCV = CreateCondition("waiterCV",8);

	managerLock = CreateLock("managerLock",11);
	managerCV = CreateCondition("managerCV",9);


	seatLock = CreateLock("seatLock",8);
	seatCV = CreateCondition("seatCV",6);

	orderQueueLock = CreateLock("orderQueueLock",14);
	eatinTokenQueueLock = CreateLock("eatinTokenQueueLock",19);

	randTimeForFoodShowUp = 27 % 2 + 10;  
	randTimeToCookAFood = 29 % 2 + 1;

	remainingCust = max_cust;


	inventory.burger_3 = 0;
	inventory.burger_6 = 0;
	inventory.veggieBurger = 0;
	inventory.frenchFries = 0;

	cookedFood.burger_3 = 20;
	cookedFood.burger_6 = 20;
	cookedFood.veggieBurger = 20;
	cookedFood.frenchFries = 20;

	orderLevel.burger_3 = 5;
	orderLevel.burger_6 = 5;
	orderLevel.veggieBurger = 5;
	orderLevel.frenchFries = 5;

	cookedFoodUpperBound.burger_3 = 5;
	cookedFoodUpperBound.burger_6 = 5;
	cookedFoodUpperBound.veggieBurger = 5;
	cookedFoodUpperBound.frenchFries = 5;

	cookedFoodLowerBound.burger_3 = 3;
	cookedFoodLowerBound.burger_6 = 3;
	cookedFoodLowerBound.veggieBurger = 3;
	cookedFoodLowerBound.frenchFries = 3;


	Write("# Number of OrderTakers = ",returnLen("# Number of OrderTakers = "),1);
	WriteInt(max_ot);
	Write("\n",1,1);

	Write("# Number of Waiters = ",returnLen("# Number of Waiters = "),1);
	WriteInt(max_waiter);
	Write("\n",1,1);

	Write("# Number of Cooks = ",returnLen("# Number of Cooks = "),1);
	WriteInt(max_cook);
	Write("\n",1,1);

	Write("# Number of Customers = ",returnLen("# Number of Customers = "),1);
	WriteInt(max_cust);
	Write("\n",1,1);

	Write("* Total Number of tables in the Restaurant = ",returnLen("* Total Number of tables in the Restaurant = "),1);
	WriteInt(availableSeat);
	Write("\n",1,1);

	Write("* Minimum number of cooked 6-dollar burger = ",returnLen("* Minimum number of cooked 6-dollar burger = "),1);
	WriteInt(cookedFoodLowerBound.burger_6);
	Write("\n",1,1);

	Write("* Minimum number of cooked 3-dollar burger = ",returnLen("* Minimum number of cooked 3-dollar burger = "),1);
	WriteInt(cookedFoodLowerBound.burger_3);
	Write("\n",1,1);

	Write("* Minimum number of cooked veggie burger = ",returnLen("* Minimum number of cooked veggie burger = "),1);
	WriteInt(cookedFoodLowerBound.veggieBurger);
	Write("\n",1,1);

	Write("* Minimum number of cooked french fries = ",returnLen("* Minimum number of cooked french fries = "),1);
	WriteInt(cookedFoodLowerBound.frenchFries);
	Write("\n",1,1);


	/* hardcode the amount of each variable for part 3, project 2.*/
	
	for(i = 0;i<max_ot;i++){
		otStatus[i] = -1;
	}
	
	Fork(Manager);

	for(i =0;i<max_waiter;i++){
		Fork(Waiter);
	}
	
	for(i =0;i<max_ot;i++){
		Fork(OrderTaker);
	}

	for(i =0;i<max_cust;i++){
		Fork(Customer);
	}
}

