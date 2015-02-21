//----------------------------------------
 //   initial global variables    by pei
//--------------------------------------------

#include "restaurant.h"
#include "package.h"


// for all variables used in Restaurant

int NUM_OF_OTS ;            //set numbers for all members involved in this project
int NUM_OF_CUSTS; 
int NUM_OF_COOKS;
int NUM_OF_MANAGERS;
int NUM_OF_WAITERS;
int NUM_OF_SEATS;      //seat
int NUM_OF_INITFOOD;
int NUM_OF_INITINVEN;



// Variables for Restaurant
   
int RestaurantSeat;                                    //s????????????????????????????????????eat and table?
Lock* SeatLock;
Condition* SeatCV;
int tables[MAX_SEATS];                                         //???????????????????????????????????????????????????????????
int Inventory[NUM_OF_COOK_FOOD];       //we dont keep track of soda~ 
//package Packages[MAX_CUSTS];
Lock* InventoryLock;
char** FoodName;// 6-dollar burger/3-dollar burger/veggie burger/french fries/soda

int cash;
int FoodPrice[NUM_OF_COOK_FOOD];
int CustomerSeat[MAX_CUSTS];
int CustTableNum[MAX_CUSTS];


// Variables for Customers-Ordertakers interaction

Lock* CustLineLock;    //lock for customers waiting line
Lock** OrderTakerLock;   
Condition* CustWaitingCV;   //point to CustWaitingCV[]
Condition** OrderTakerCV;
int CustLineLength;
int OTStatus[MAX_OTS];

int UnpackedFood;//Num of unpacked Foods 
int CustStatus[MAX_CUSTS];
int OTService[MAX_OTS];

package* Packages;



//?????????????????????????????????????????????????????????????????????????????????????????condition *PackageCV;



//Lock* WaiterLock;                            //--------??????????????????????????????????????????????????????
//Condition** WaiterCV;                       //?????????????????????????????????????????????????????????????????????????


//for track what is the manager doing
int ManagerStatus;


//////////////////////////////////////////////////////////
//set Variables for Cook-OrderTaker iteraction
/////////////////////////////////////////////////////////////
Lock* FoodTableLock;
int FoodCount[NUM_OF_COOK_FOOD];



///////////////////////////////////////////////////////////////
//set Variables for customer Manager interaction
////////////////////////////////////////////////////////////
Lock* CustManagerLock;
Condition* CustManagerCV;
Lock** SeatCustLock;
Condition** SeatCustCV;
int  CustomerStatus[MAX_CUSTS];                              //??????????????????????????????????????????????
int ManagerCustNum;
////////////////////////////////////////////////////////////
//set Variables for customer-Waiter interaction
///////////////////////////////////////////////////////////////
//Lock* CustWaiterLock;                                        /////////////????????????????????????????????
Condition** CustWaiterCV;                                                //?????????????????????????????????????????
int WaiterService[MAX_CUSTS];


//for to-go customer that wait for their food(first check not ready)
Lock* ToGoLock;
Condition* ToGoCV;

//for eat-in customer that wait for waiter to send food
Lock* EatInLock;
Condition* EatInCV;                                             //   change  ??????????????????????????????????????????????????????? EatInCondition


///////////////////////////////////////////////////////////////
//set Variables for Ordertaker--Waiter interaction
////////////////////////////////////////////////////////////
Lock* WaiterLock;
Lock* PackageLock;  // Waiters check the food 




//////////////////////////////////////////////////////////////////
//setVariable for Waiters- Manager
/////////////////////////////////////////////////////////////////////
int WaiterStatus[MAX_WAITERS];
Condition* WaiterWakeCV;

int WorkingWaiterCount; 



//////////////////////////////////////////////////////////////////
//setVariable for Cooks- Manager
/////////////////////////////////////////////////////////////////////
int CookDoing[MAX_COOKS];
int CookRequest[MAX_COOKS];
int CookStatus[MAX_COOKS];

Lock* CookRequestLock;
Condition** CookWaitCV;







void initRestaurant(int seat, int cust, int ot, int cook, int waiter, int icash, int food, int inventory){
	//-------------------------------------
	//initiate Variables 
	//-------------------------------------
	NUM_OF_OTS = ot;            //set numbers for all members involved in this project
	NUM_OF_CUSTS = cust;
	NUM_OF_COOKS = cook;
	NUM_OF_MANAGERS = 1;
	NUM_OF_WAITERS = waiter;
	//NUM_OF_WAITERS = 2;
	NUM_OF_SEATS = seat;           //seat
	NUM_OF_INITINVEN = inventory; 
	//NUM_OF_INITINVEN=0;
	NUM_OF_INITFOOD = food; 
	//NUM_OF_INITFOOD= 4;
        cash = icash ;


	RestaurantSeat =0 ;                                    //s????????????????????????????????????eat and table?
    SeatLock = new Lock("SeatLock");
    SeatCV = new Condition("SeatCV");
   // SeatCustLock= new Lock("SeatCustLock");
  //  SeatCustCV = new Condition("SeatCustCV");
	
	//tables[];      //???????????????????????????????????????????????????????????
       	for(int i=0;i<RestaurantSeat;i++){
	tables[i]=0;
	}
	 

	 for(int i=0;i<NUM_OF_COOK_FOOD;i++){
	    Inventory[i] =  NUM_OF_INITINVEN  ;    //we dont keep track of soda~ 	 
	    FoodCount[i] =  NUM_OF_INITFOOD;
	    
	    
	    FoodPrice[i] = 3;                        ///////////////////////////////////for TEST

	 }
	 

	 Packages= new package[MAX_CUSTS];
	   
//	 int o=Packages[0].getChoice();
//             printf("pack %d",o);
	// for(int i=0;i<MAX_CUSTS;i++)
	//	Packages[i].setChoice(-1);

		InventoryLock = new Lock("InventoryLock");


		FoodName = new char*[NUM_OF_FOOD];// 6-dollar burger/3-dollar burger/veggie burger/french fries/soda
		FoodName[0] = "6-dollar burger" ;
		FoodName[1] = "3-dollar burger" ;
		FoodName[2] = "veggie burger" ;
		FoodName[3] = "french fries" ;
		FoodName[4] = "soda" ;

	 
        //SeatedCustomer = 0;
	
	    RestaurantSeat=MAX_SEATS;
	    CustLineLength = 0;

 
	     
	CustLineLock= new Lock("CustLineLock");
	CustWaitingCV= new Condition("CustWaitingCV");  
	//-------------------------------------------------
	//Variables related to Ordertakers
   //-------------------------------------------------------
	OrderTakerLock= new Lock*[MAX_OTS];   
	OrderTakerCV= new Condition*[MAX_OTS];  
	SeatCustLock= new Lock*[MAX_CUSTS];
	SeatCustCV = new Condition*[MAX_CUSTS];

	   for(int i=0;i<MAX_OTS;i++){
	   OrderTakerLock[i] = new Lock("OrderTakerLock"+(i+1)); 
	   OrderTakerCV[i] = new Condition("OrderTakerCV"+(i+1));
	   OTStatus[i] =1;
	   OTService[i]=-1;
	   }
           for(int i=0;i<MAX_CUSTS;i++){
	   SeatCustLock[i] = new Lock("SeatCustLock"+(i+1));
           SeatCustCV[i] = new Condition("SeatCustLock"+(i+1));
		CustomerSeat[i] = -1;

	   }
	   UnpackedFood=0;
//-------------------------------------------------
	//Variables related to Customers
	   //-----------------------------------------
	   
	   for(int i=0;i<MAX_CUSTS;i++){
		   CustStatus[i]= 0       ; 
		   WaiterService[i] = -1; 
	   }





	
//set Variables for Cook-OrderTaker iteraction
//
	FoodTableLock = new Lock("FoodTableLock");



	


	//for to-go customer that wait for their food(first check not ready)
	 ToGoLock = new Lock("ToGoLock");
	ToGoCV  =new Condition("ToGoCV");

	//for eat-in customer that wait for waiter to send food
	EatInLock = new Lock("EatInLock");
	EatInCV = new Condition("EatInCV");



	/*
	///////////////////////////////////////////////////////////////
//set Variables for customer Manager interaction
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//set Variables for customer-Waiter interaction
///////////////////////////////////////////////////////////////
Lock* CustWaiterLock;                                        /////////////????????????????????????????????
Condition* CustWaiterCV;   


	*/
	WaiterLock = new Lock("WaiterLock");
    PackageLock = new Lock("PackageLock");
CustManagerLock = new Lock("CustManagerLock");
CustManagerCV    = new Condition("CustManagerCV");
	CustWaiterCV = new Condition*[MAX_WAITERS];
	for(int i=0;i<MAX_WAITERS;i++){
	        CustWaiterCV[i]=new Condition("CustWaiterCV"+(i+1));
		WaiterStatus[i] =FREE;	
		
	}

	 WaiterWakeCV = new Condition("WaiterWakeCV");

     WorkingWaiterCount = waiter; 


	 CookRequestLock= new Lock("CookRequestLock");
     CookWaitCV = new Condition*[MAX_COOKS];
	 for(int i=0;i<MAX_COOKS;i++){
		CookWaitCV[i] = new Condition("CookWakeCV");
		CookDoing[i]=-1;
		CookRequest[i]=-1;
		CookStatus[i]=FREE;
	 }
	for(int i=0;i<MAX_CUSTS;i++){
	CustTableNum[i]=-1;
	}

	for(int i=0;i<MAX_CUSTS;i++){
	CustomerStatus[i]=PRE_ORDERING;
	}

	 	 InventoryLock = new Lock("InventoryLock");



    printf("finished");

}