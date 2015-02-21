class Thread;



#ifndef PROCESS_H_
#define PROCESS_H_
#ifdef USER_PROGRAM
#include "copyright.h"
#include "../userprog/table.h"
#include "../threads/thread.h"


enum ProcessStatus { PROCESS_JUST_CREATED, PROCESS_RUNNING, PROCESS_READY, PROCESS_BLOCKED, PROCESS_ZOMBIE };
class Process{
	private:
		int ProcessID;
		int end;
	
		AddrSpace* addrSpace;
	
		//Process *ParentProcess;
		//	Thread  *MainThread;
		ProcessStatus status;
		
		// Associate with each process
		//int ChildProcessCounter; // number of child
		//int exitCode;
		//Table* ChildProcess;
		//List *waitqueue;

		//Associate with each thread





public:
		char* PName;
	//Process(Thread *myExecutor,Process *myParent);
		//Process *getParent() { return ParentProcess;}
		Thread* MainThread;

		Table* ChildThread;
		int ChildThreadCounter;

		Process(AddrSpace* as, char* buf){
					PName=buf;
					addrSpace = as;
					ChildThread = new Table(100);
					ChildThreadCounter=0;
		       		
				}

	    //Thread *getMainThread() {return MainThread;}
		
	    //int ChildrenProcessNUM(){return  ChildProcessCounter;}
		ProcessStatus getStatus() {return status;}
		void setStatus(ProcessStatus st) { status = st; }
	
		void setAddrSpace(AddrSpace* as){addrSpace= as;}
		 AddrSpace* getAddrSpace(){return addrSpace;};
		
		//void addChildProcess(Process *myChild);
		
		int getPID() {return ProcessID;}
		void setPID(int id) {ProcessID=id; }
		 

		void ExitProcess(int ex);
		//void deathOfChild(Process *p);

		int ifEnd(){return end;}
		//void dumpChildInfo();
		~Process();

		int removeThread(int pid){ 
						ChildThread->Remove(pid);
						ChildThreadCounter--;
						return ChildThreadCounter;
						} 





};

class IPT {
  public:
    int virtualPage;  	// The page number in virtual memory.
    int physicalPage;  	// The page number in real memory (relative to the
			//  start of "mainMemory"
    bool valid;         // If this bit is set, the translation is ignored.
			// (In other words, the entry hasn't been initialized.)
    bool readOnly;	// If this bit is set, the user program is not allowed
			// to modify the contents of the page.
    bool use;           // This bit is set by the hardware every time the
			// page is referenced or modified.
    bool dirty;         // This bit is set by the hardware every time the
			// page is modified.

	int pid;

};


class PageEntry {
  public:
    int virtualPage;  	// The page number in virtual memory.
    int physicalPage;  	// The page number in real memory (relative to the
			//  start of "mainMemory"
    bool valid;         // If this bit is set, the translation is ignored.
			// (In other words, the entry hasn't been initialized.)
    bool readOnly;	// If this bit is set, the user program is not allowed
			// to modify the contents of the page.
    bool use;           // This bit is set by the hardware every time the
			// page is referenced or modified.
    bool dirty;         // This bit is set by the hardware every time the
			// page is modified.

	int pid;
	int status; //0-executable, 1-swapfile, 2-neither 
	unsigned int diskLocation;
};

#endif





#endif /*PROCESS_H_*/
