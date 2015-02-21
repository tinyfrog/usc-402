#ifndef _RESTRAURANT_H
#define	_RESTRAURANT_H

#include "synch.h"
#include "package.h"

#define NOTREADY 0      //Package Status
#define READY 1
#define DONE 2
#define SERVE 3


//#define BUSY 1     
//#define FREE 0

#define TO_GO 0    //Customer Choices
#define EAT_IN 1

#define ORDER_LEVEL 2    //the number under which cook should do their jobs
#define TOP_LEVEL 5      //cook stops cooking when FOODCOUNT is beyond this level
#define INVEN_ORDER_LEVEL 5 // if amount of any inventory is below this LEVEL, then manager go and buy it
#define PRE_ORDERING -1
#define WAITING_FOR_SEAT 0
#define SEATED 1

#define MAX_OTS     5       //set initial counts for all members involved in this project
#define MAX_CUSTS   30
#define MAX_COOKS   5
#define MAX_MANAGERS 1
#define MAX_WAITERS  5
#define MAX_SEATS     20      //seat

#define NUM_OF_FOOD 5
#define NUM_OF_COOK_FOOD 4 // num of food which need keep track

// for all ±‰¡ø used in Restaurant

extern int NUM_OF_OTS;             //set numbers for all members involved in this project
extern int NUM_OF_CUSTS; 
extern int NUM_OF_COOKS;
extern int NUM_OF_MANAGERS;
extern int NUM_OF_WAITERS;
extern int NUM_OF_SEATS; //seat
extern int NUM_OF_INITFOOD;       
extern int NUM_OF_INITINVEN;

#define MAX_FOOD_ON_TABLE   100
#define ALL_INVEN_PRICE 20   // the cash manager needs for once purchase 

/////////////////////////////////////////////
//set Variables for Restaurant
/////////////////////////////////////////////      
extern int RestaurantSeat;                                    //s????????????????????????????????????eat and table?
extern Lock* SeatLock;
extern Condition* SeatCV;
extern int tables[MAX_SEATS];                                         //???????????????????????????????????????????????????????????
extern int Inventory[NUM_OF_COOK_FOOD];       //we dont keep track of soda~ 
extern package* Packages;
extern Lock* InventoryLock;
extern char** FoodName;// 6-dollar burger/3-dollar burger/veggie burger/french fries/soda




extern int cash;
extern int FoodPrice[NUM_OF_COOK_FOOD]; 
extern int CustomerSeat[MAX_CUSTS];
extern int CustTableNum[MAX_CUSTS];          //inital to -1

/////////////////////////////////////////////////////////////
// set Variables for Customers-Ordertakers interaction
////////////////////////////////////////////////////////////
extern Lock* CustLineLock;    //lock for customers waiting line
extern Lock** OrderTakerLock;   
extern Condition* CustWaitingCV;   //point to CustWaitingCV[]
extern Condition** OrderTakerCV;
extern int CustLineLength;
extern int OTStatus[MAX_OTS];

extern int UnpackedFood;//Num of unpacked Foods 

//extern int* FoodStatus;//property of food
extern int CustStatus[MAX_CUSTS];


////////////////////////////////////////////////////////////
//set Variables for Ordertakers -- Waiters iteraction
/////////////////////////////////////////////////////////////////
extern Lock* PackageLock;



//extern condition *PackageCV;                                               //???????????????????????????????????????????????????????????

//extern Lock** WaiterLock;
//extern Condition** WaiterCV;


//for track what is the manager doing
extern int ManagerStatus;


//////////////////////////////////////////////////////////
//set Variables for Cook-OrderTaker iteraction
/////////////////////////////////////////////////////////////
extern Lock* FoodTableLock;
extern int FoodCount[NUM_OF_COOK_FOOD];

extern int OTService[MAX_OTS];


///////////////////////////////////////////////////////////////
//set Variables for customer Manager interaction
////////////////////////////////////////////////////////////
extern Lock* CustManagerLock;
extern Condition* CustManagerCV;
extern Lock** SeatCustLock;
extern Condition** SeatCustCV;
extern int  CustomerStatus[MAX_CUSTS];
extern int ManagerCustNum;
////////////////////////////////////////////////////////////
//set Variables for customer-Waiter interaction
///////////////////////////////////////////////////////////////
//extern Lock* CustWaiterLock;
extern Condition** CustWaiterCV;
extern int WaiterService[MAX_CUSTS];

//for to-go customer that wait for their food(first check not ready)
extern Lock* ToGoLock;
extern Condition* ToGoCV;

//for eat-in customer that wait for waiter to send food
extern Lock* EatInLock;
extern Condition* EatInCV;                                             //   change  ??????????????????????????????????????????????????????? EatInCondition

//////////////////////////////////////////////////////////////////
//setVariable for Waiters- Manager
/////////////////////////////////////////////////////////////////////
extern int WaiterStatus[MAX_WAITERS];
extern Condition* WaiterWakeCV;

extern int WorkingWaiterCount; 



//////////////////////////////////////////////////////////////////
//setVariable for Cooks- Manager
/////////////////////////////////////////////////////////////////////
extern int CookDoing[MAX_COOKS];
extern int CookRequest[MAX_COOKS];
extern int CookStatus[MAX_COOKS];

extern Lock* CookRequestLock;
extern Condition** CookWaitCV;



void initRestaurant(int seat, int cust, int ot, int cook, int waiter, int icash, int food, int inventory);

#endif
