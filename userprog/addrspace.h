// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
class PageEntry;

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "table.h"
#include "machine.h"

#define UserStackSize 1024 	// increase this as necessary!

#define MaxOpenFiles 256
#define MaxChildSpaces 256

class AddrSpace {
  public:
	OpenFile *exec;


    AddrSpace(OpenFile *executable);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch
    Table fileTable;			// Table of openfiles
    int getFirstThreadStartVPN();

	//defined by SSB
	int IncrementStackSpace(int pid);
	//define end

	void DecreaseStackSpace(int numStackPage);// decrease physaddr allocated to this process(one thread leaves)
    void DeleteSpace(); //delete space of this process

	void addTLB(int ppn);
    void setPIDToIPT(int _pid);
	//OpenFile* getExecutable(){return exec; };
	void updateIPT(unsigned int vaddr, int pid);
	void writeOneToSwapFile();
     void pageToSwapFile(int vpn, int loc );


 private:
    //TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!

	PageEntry *pageTable;

    unsigned int numPages;		// Number of pages in the virtual 
					// address space

	//define by SSB
	unsigned int stackPages;
	//define end
	int firstThreadStartVPN;
    
	
	unsigned int codeandinitNumPages;
	unsigned int codeandinitDataSize;
};

#endif // ADDRSPACE_H
