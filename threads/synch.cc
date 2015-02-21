// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(char* debugName) {
   name=debugName;
   lockerOwner=NULL;
   waitQueue=new List();
   status=FREE;
}
Lock::~Lock() {
  delete waitQueue;
}
bool Lock::isHeldByCurrentThread(){
  return (currentThread==lockerOwner);
}
void Lock::Acquire() 
{
  IntStatus old = interrupt->SetLevel(IntOff);		// D.I.
  if( isHeldByCurrentThread() )				// Checking if I'm the lock owner 
    {
      interrupt->SetLevel(old);				// R.I. & Return
      return;
    } 
  
  if(status==FREE){                                        //if a lock is not in use
    status=BUSY;                                             //set the status to busy
    lockerOwner=currentThread;                               //set the current thread to be the owner
  }
  
  else{
    waitQueue->Append((void *)currentThread);
    currentThread->Sleep();
  }
  interrupt->SetLevel(old);				// R.I. & Return
  return;
  
}
void Lock::Release() {
  IntStatus old = interrupt->SetLevel(IntOff);		         // D.I.
  if(!isHeldByCurrentThread())                                   // if the current thread is not the 
  {
       printf("non owner tries release lock, return");
       interrupt->SetLevel(old);				// R.I. & Return
       return;
  }
  if(!waitQueue->IsEmpty()){                                  //if someone is in the waiting Q
       Thread *nextThread =(Thread *)waitQueue->Remove();         
       scheduler->ReadyToRun ( nextThread );
       lockerOwner=nextThread;
  }
  else{
       status=FREE;
       lockerOwner=NULL;
  }
       interrupt->SetLevel(old);				// R.I. & Return
       return;
}



bool Lock:: isQueueEmpty(){
	if(waitQueue->IsEmpty())
                    return 1;
        else return 0;
 
}



Condition::Condition(char* debugName) { 
       name=debugName;
       waitQueue=new List();
       waitLock=NULL;
}
Condition::~Condition() { 
   delete waitQueue;
}
void Condition::Wait(Lock* conditionLock) {
        IntStatus old = interrupt->SetLevel(IntOff);		// D.I.
        if(waitLock==NULL){                                     //no one is waiting
	waitLock=conditionLock;                                   
	}
        if(waitLock==NULL){                                     // at this point , the waitlock should not be null so an error is generated
	printf("error, the waitLock can not be null at this point");
        interrupt->SetLevel(old);				// R.I. & Return
        return;
	}
	if(waitLock!=conditionLock){                            //see if they matches, if not ,print error
	printf("ERROR! the waitLock and condtionLock does not match");
	interrupt->SetLevel(old);				// R.I. & Return
        return;
	}
	waitQueue->Append((void *) currentThread);
	conditionLock->Release();
	currentThread->Sleep();
        conditionLock->Acquire();
	interrupt->SetLevel(old);					// R.I.
}
void Condition::Signal(Lock* conditionLock) {
        IntStatus old=interrupt->SetLevel(IntOff);
	if(waitQueue->IsEmpty()){                     // if nobody is waiting we signal no one and return
	   interrupt->SetLevel(old);
	   return;
	}
	if(conditionLock!=waitLock)
	{
	  printf("ERROR! locks do not match");
	  interrupt->SetLevel(old);
	  return;
	}
	//wake up the first one in the waiting queue
       Thread *nextThread=(Thread *)waitQueue->Remove();         
       scheduler->ReadyToRun ( nextThread );
     //  waitLock.lockerOwner=nextThread;


       if(waitQueue->IsEmpty())
       {
          waitLock=NULL;
       }
       interrupt->SetLevel(old);
}
void Condition::Broadcast(Lock* conditionLock) {
  while(!waitQueue->IsEmpty()){
    Signal(conditionLock);
  }
}


bool Condition:: isQueueEmpty(){
	if(waitQueue->IsEmpty())
                    return 1;
        else return 0;
 
}
