// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include "table.h"
#include "synch.h"

//added by ssb
#include "machine.h"

//added end

extern "C" { int bzero(char *, int); };



Table::Table(int s) : map(s), table(0), lock(0), size(s) {
    table = new void *[size];
    lock = new Lock("TableLock");
}

Table::~Table() {
    if (table) {
	delete table;
	table = 0;
    }
    if (lock) {
	delete lock;
	lock = 0;
    }
}

void *Table::Get(int i) {
    // Return the element associated with the given if, or 0 if
    // there is none.

    return (i >=0 && i < size && map.Test(i)) ? table[i] : 0;
}

int Table::Put(void *f) {
    // Put the element in the table and return the slot it used.  Use a
    // lock so 2 files don't get the same space.
    int i;	// to find the next slot

    lock->Acquire();
    i = map.Find();
    lock->Release();
    if ( i != -1)
	table[i] = f;
    return i;
}

void *Table::Remove(int i) {
    // Remove the element associated with identifier i from the table,
    // and return it.

    void *f =0;

    if ( i >= 0 && i < size ) {
	lock->Acquire();
	if ( map.Test(i) ) {
	    map.Clear(i);
	    f = table[i];
	    table[i] = 0;
	}
	lock->Release();
    }
    return f;
}

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	"executable" is the file containing the object code to load into memory
//
//      It's possible to fail to fully construct the address space for
//      several reasons, including being unable to allocate memory,
//      and being unable to read key parts of the executable.
//      Incompletely consretucted address spaces have the member
//      constructed set to false.
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable) : fileTable(MaxOpenFiles) {
    NoffHeader noffH;
    unsigned int i, size;

	//added by ssb 10.7
	unsigned int codeAndInitDataNumPages;
	unsigned int codeAndInitDataSize;
	
    // Don't allocate the input or output to disk files
    fileTable.Put(0);
    fileTable.Put(0);

//get executable
    exec = executable;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size ;


	//Initialize this two viariables
	codeAndInitDataSize = noffH.code.size + noffH.initData.size;
	codeAndInitDataNumPages = divRoundUp(codeAndInitDataSize, PageSize);


        codeandinitNumPages=codeAndInitDataNumPages;
	codeandinitDataSize=codeAndInitDataSize;

	printf("The codeAndInitDataNumPages is %d\n",codeAndInitDataNumPages);
	printf("The codeAndInitDataSize is %d\n",codeAndInitDataSize);

    numPages = divRoundUp(size, PageSize) + divRoundUp(UserStackSize,PageSize);
    firstThreadStartVPN=divRoundUp(size,PageSize);
    size = numPages * PageSize;

  //  ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);
// first, set up the translation 
 
   
    pageTable = new PageEntry[numPages];

	
    for (i = 0; i < codeAndInitDataNumPages; i++) {

		pageTable[i].virtualPage = i;
		//printf("The bitmap returns the phys addr page is:%d\n",pageTable[i].physicalPage);
		pageTable[i].valid = FALSE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE; 
		pageTable[i].status = 0;
		pageTable[i].diskLocation = noffH.code.inFileAddr + i*PageSize;
	}

	for(;i<numPages;i++){
		pageTable[i].virtualPage = i;
		pageTable[i].valid = FALSE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE; 
		pageTable[i].status = 3;
		//pageTable[i].diskLocation = noffH.code.inFileAddr + i*PageSize;
	}

      


	printf("LAaaaa\n");



  /*  
    pageTable = new TranslationEntry[numPages];

    for (i = 0; i < numPages; i++) {

	pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
	pageTable[i].physicalPage = bm->Find();
	//		printf("The bitmap returns the phys addr page is:%d\n",pageTable[i].physicalPage);
	pageTable[i].valid = TRUE;
	pageTable[i].use = FALSE;
	pageTable[i].dirty = FALSE;
	pageTable[i].readOnly = FALSE; 
	

					// if the code segment was entirely on 
					// a separate page, we could set its 
					// pages to be read-only


	
	//update IPT 
	ipt[pageTable[i].physicalPage].physicalPage = pageTable[i].physicalPage;
	ipt[pageTable[i].physicalPage].virtualPage = i;
	ipt[pageTable[i].physicalPage].valid=TRUE;
	ipt[pageTable[i].physicalPage].use = FALSE;
	ipt[pageTable[i].physicalPage].dirty = FALSE;
    
    }
 
       
    
// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
    	for( int k = 0; k<numPages; k++){

		bzero(&(machine->mainMemory[pageTable[k].physicalPage * PageSize]), PageSize);

	}


    if (noffH.code.size > 0) {
		
		if((codeAndInitDataNumPages * PageSize) >= codeAndInitDataSize){

		//start to copy pages
			for(int j = 0; j < codeAndInitDataNumPages - 1; j++){

				executable->ReadAt(&(machine->mainMemory[pageTable[j].physicalPage * PageSize]),
					PageSize, (noffH.code.inFileAddr + j*PageSize));
			}
			
			//printf("pageTable[codeAndInitDataNumPages - 1].physicalPage * PageSize = %d\n",pageTable[codeAndInitDataNumPages - 1].physicalPage * PageSize);
			//printf("codeAndInitDataSize-(codeAndInitDataNumPages-1)*PageSize = %d\n",codeAndInitDataSize-(codeAndInitDataNumPages-1)*PageSize);
			//printf("noffH.code.inFileAddr + (codeAndInitDataNumPages-1)*PageSize = %d\n",noffH.code.inFileAddr + (codeAndInitDataNumPages-1)*PageSize);
			//
			executable->ReadAt(&(machine->mainMemory[pageTable[codeAndInitDataNumPages - 1].physicalPage * PageSize]),
				(codeAndInitDataSize-(codeAndInitDataNumPages-1)*PageSize) , (noffH.code.inFileAddr + (codeAndInitDataNumPages-1)*PageSize));

		}
		else
			printf("Something wrong happend\n");

	}
    */
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//
// 	Dealloate an address space.  release pages, page tables, files
// 	and file tables
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    delete pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %x\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    //machine->pageTable = pageTable;
    //machine->pageTableSize = numPages;
   
IntStatus ints = interrupt->SetLevel(IntOff);
   //how about switch among threads in a process...
    for(int i = 0; i<=TLBSize; i++){  //TLBSize
	machine->tlb[i].valid = FALSE;
	}
interrupt -> SetLevel(ints);

}



//defined by SSB
int AddrSpace::IncrementStackSpace(int pid){
	
	PageEntry *pageTable_old = pageTable;
	
	//	printf("The current numPages is %d\n",numPages);
	//printf("The current stack start address is %d\n",numPages*PageSize);

	numPages = numPages + 8;

	/*
	for (int i = 0; i < 3; i++) {
	 	printf("Before Change!!The first 3 vpages are: %d\n",pageTable[i].virtualPage);

	}
	*/
	pageTable = new PageEntry[numPages];


	// copy the old pageTable to the new one
	for (int i = 0; i < numPages-8; i++) {

		pageTable[i].virtualPage = pageTable_old[i].virtualPage;	// for now, virtual page # = phys page #
		pageTable[i].physicalPage = pageTable_old[i].physicalPage;
		pageTable[i].valid = pageTable_old[i].valid;
		pageTable[i].use = pageTable_old[i].use;
		pageTable[i].dirty = pageTable_old[i].dirty;
		pageTable[i].readOnly = pageTable_old[i].readOnly;  
		pageTable[i].status = pageTable_old[i].status;
		pageTable[i].diskLocation = pageTable_old[i].diskLocation;
    }

	for (int i = numPages - 8; i < numPages; i++) {
	
		pageTable[i].virtualPage = i;
		pageTable[i].valid = FALSE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE; 
		pageTable[i].status = 3;
		//pageTable[i].diskLocation = noffH.code.inFileAddr + i*PageSize;

		
		
		/*step2
		pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
		pageTable[i].physicalPage = bm->Find();  //it should be bm->find()
		//	printf("New Stack!!The bitmap returns the phys addr page is:%d\n",pageTable[i].physicalPage);
		if(bm->Find()==-1)
		   printf("bitmap returns -1, halt the system!\n");
		pageTable[i].valid = TRUE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE;  

		
		//update IPT
		ipt[pageTable[i].physicalPage].physicalPage = pageTable[i].physicalPage;
		ipt[pageTable[i].physicalPage].virtualPage = i;
		ipt[pageTable[i].physicalPage].valid=TRUE;
		ipt[pageTable[i].physicalPage].use = FALSE;
		ipt[pageTable[i].physicalPage].dirty = FALSE;
		ipt[pageTable[i].physicalPage].pid = pid;
		*/


    }

	//delete the old, now pageTable has
	delete pageTable_old;
	/*
	for (int i = 0; i < 3; i++) {
		printf("After Change!!The first 3 vpages are: %d\n",pageTable[i].virtualPage);

	}
	*/
	//	printf("The after numPages is %d\n",numPages);
	//	printf("The after stack start address is %d\n",numPages*PageSize);


	int stackStartVirtualAddress = numPages * PageSize - 16;	

	return stackStartVirtualAddress;
}
//define end


//add by pei

void AddrSpace::DecreaseStackSpace(int numStackPage){
	
    // acquire lock
	//physLock->Acquire();
	
	printf("im in the DECREASESTAKESPACE function,this is to kill a thread\n");
       
	
	for (int i = numStackPage; i < 8+numStackPage; i++) {
	        
		// delete this occupy of bitmap like
	  if(pageTable[i].valid == TRUE)
		bm->Clear(pageTable[i].physicalPage);    //important

		//printf("return the phys addr page :%d,  virtual ADDR: %d\n",pageTable[i].physicalPage, i);
		pageTable[i].valid = FALSE; 

    }

	//physLock->Release();

}
//define end


void AddrSpace::DeleteSpace(){	
    // acquire lock
	//physLock->Acquire();

	PageEntry *pageTable_old = pageTable;//??????????????????????????????????????????????????????
		printf("im in the DELETESPACE function, this is to kill a process\n");
      

	
	for (int i = 0; i < numPages; i++) {
	  if(pageTable[i].valid== true){
		// delete this occupy of bitmap like
		bm->Clear(pageTable[i].physicalPage);    //important

		//       	printf("return the phys addr page :%d\n",pageTable[i].physicalPage);
	  } 
    }

	//physLock->Release();

}



int AddrSpace::getFirstThreadStartVPN(){
  return firstThreadStartVPN;

}

void AddrSpace::addTLB(int ppn){
	
	//TranslationEntry *pt = &(pageTable[vpn]);
	TranslationEntry *pt= new TranslationEntry;
         
	IntStatus old = interrupt->SetLevel(IntOff);
	
	// find entry from ipt


			pt->virtualPage = ipt[ppn].virtualPage;	
			pt->physicalPage = ppn;
			pt->valid = ipt[ppn].valid;
			pt->use = ipt[ppn].use;  
			pt->dirty = ipt[ppn].dirty;
			pt->readOnly = ipt[ppn].readOnly;  
       			     

	if(pt!=NULL){
	  
		machine->tlb[TLBCounter%TLBSize] = *pt;
		TLBCounter++;
		if(machine->tlb[(TLBCounter-1)%TLBSize].virtualPage==pt->virtualPage){ }
	if(machine->tlb[(TLBCounter-1)%TLBSize].physicalPage==pt->physicalPage){ }
		}
	interrupt->SetLevel(old);

}


void AddrSpace::pageToSwapFile(int vpn, int loc ){
	pageTable[vpn].status=1;
	pageTable[vpn].diskLocation = loc * PageSize;
	pageTable[vpn].valid = FALSE;
	//printf("We did that!!!!!!~~vpn %d~~~\n",vpn);
}



void AddrSpace::writeOneToSwapFile() {
               
	if ( ChoiceOfReplacementPolicy == RAND){
		  int phy = rand()%32;
	       printf("Random evict page: %d\n",phy);
	       IntStatus old = interrupt->SetLevel(IntOff);
	       for(int i = 0; i<=TLBSize; i++){  //TLBSize
		 if(machine->tlb[i].physicalPage == phy){
		   machine->tlb[i].valid = FALSE;
		  // break;
		 }
	       } 
	       interrupt->SetLevel(old);


	       //phy = test % NumPhysPages;
			
		int locNum = sfBM->Find();  //find a place in the disk Location
		swapFile->WriteAt(&(machine->mainMemory[phy * PageSize]),
							PageSize, locNum * PageSize );
		printf("Write Physical Page [%d] to SwapFile!\n", phy);					
		Process* p = (Process*)ProcessTable->Get(ipt[phy].pid);
		AddrSpace* as = p->getAddrSpace(); 
		as->pageToSwapFile(ipt[phy].virtualPage,locNum );
		
		//fifoQueue.pop_front();
		
	
	       	bzero(&(machine->mainMemory[phy * PageSize]), PageSize);
		bm->Clear(phy);

	}


	else if (ChoiceOfReplacementPolicy == FIFO){
		//PageEntry* pe = (PageEntry* )fifoQueue->Remove();
		int phy = fifoQueue.front();  //get one physical page to be swapped

		printf("FIFO- evict page : %d\n",phy);
	       IntStatus old = interrupt->SetLevel(IntOff);
	       for(int i = 0; i<=TLBSize; i++){  //TLBSize
		 if(machine->tlb[i].physicalPage == phy){
		   machine->tlb[i].valid = FALSE;
		  // break;
		 }
	       } 
	       interrupt->SetLevel(old);


	       //phy = test % NumPhysPages;
			
		int locNum = sfBM->Find();  //find a place in the disk Location
		swapFile->WriteAt(&(machine->mainMemory[phy * PageSize]),
							PageSize, locNum * PageSize );
		printf("Write Physical Page [%d] to SwapFile!\n", phy);
							
		Process* p = (Process*)ProcessTable->Get(ipt[phy].pid);
		AddrSpace* as = p->getAddrSpace(); 
		as->pageToSwapFile(ipt[phy].virtualPage,locNum );
		
		fifoQueue.pop_front();
		
	
	       	bzero(&(machine->mainMemory[phy * PageSize]), PageSize);
		bm->Clear(phy);
		   
		//fifoQueue->Append(pe);
	}
	
}


//IPT by pei
void AddrSpace::updateIPT(unsigned int vaddr, int pid){
      IPTLock->Acquire();

	int vpn = vaddr / PageSize;
	

	if ( vpn >= numPages ){
		printf("ERROR!!!!!numPages%d!!!!!!VPN:%d!\n",vpn,numPages);
		return;	
	}


      if(pageTable[vpn].status != 1){    //Not in SwapFile

      	     pageTable[vpn].physicalPage=bm->Find();
	     if( pageTable[vpn].physicalPage == -1 ){//MainMemory is FULL
			writeOneToSwapFile();
			pageTable[vpn].physicalPage=bm->Find();

	     }	
	     if(vpn <  codeandinitNumPages ){

	       //printf("HERE---vpn %d\n",vpn);

			//printf("what?pageTable[vpn].physicalPage %d\n",pageTable[vpn].physicalPage);
			//printf("HERE$$$\n");
			if( vpn ==  codeandinitNumPages-1 ){
				 bzero(&(machine->mainMemory[pageTable[vpn].physicalPage * PageSize]), PageSize);									
				//printf("pageTable[codeAndInitDataNumPages - 1].physicalPage * PageSize = %d\n",pageTable[codeAndInitDataNumPages - 1].physicalPage * PageSize);
				//printf("codeAndInitDataSize-(codeAndInitDataNumPages-1)*PageSize = %d\n",codeAndInitDataSize-(codeAndInitDataNumPages-1)*PageSize);
				//printf("noffH.code.inFileAddr + (codeAndInitDataNumPages-1)*PageSize = %d\n",noffH.code.inFileAddr + (codeAndInitDataNumPages-1)*PageSize);
				//
				exec->ReadAt(&(machine->mainMemory[pageTable[vpn].physicalPage * PageSize]),
					(codeandinitDataSize-( codeandinitNumPages-1)*PageSize) , pageTable[vpn].diskLocation);

			}
			else{
				bzero(&(machine->mainMemory[pageTable[vpn].physicalPage * PageSize]), PageSize);
				exec->ReadAt(&(machine->mainMemory[pageTable[vpn].physicalPage * PageSize]),
							PageSize, pageTable[vpn].diskLocation);
							

			}
	
		//printf("HEREEEEEE\n");

			ipt[pageTable[vpn].physicalPage].physicalPage = pageTable[vpn].physicalPage;
			ipt[pageTable[vpn].physicalPage].virtualPage = vpn;
			ipt[pageTable[vpn].physicalPage].valid = TRUE;
			ipt[pageTable[vpn].physicalPage].use = pageTable[vpn].use;
			ipt[pageTable[vpn].physicalPage].dirty = pageTable[vpn].dirty;
			ipt[pageTable[vpn].physicalPage].pid = pid;

		
			if (ChoiceOfReplacementPolicy == FIFO){
			
				//IPT * pe =  ipt + pageTable[vpn].physicalPage;
				fifoQueue.push_back(pageTable[vpn].physicalPage);//fifo	
			}
                        
			pageTable[vpn].valid = TRUE;
			
		
		
		}
	
		else{
		
			if( vpn < numPages ){
		//	printf("HERE_____2222\n");

		//pageTable[vpn].physicalPage=bm->Find();
		
		ipt[pageTable[vpn].physicalPage].physicalPage = pageTable[vpn].physicalPage;
		ipt[pageTable[vpn].physicalPage].virtualPage = vpn;
		ipt[pageTable[vpn].physicalPage].valid = TRUE;
		ipt[pageTable[vpn].physicalPage].use = pageTable[vpn].use;
		ipt[pageTable[vpn].physicalPage].dirty = pageTable[vpn].dirty;
		ipt[pageTable[vpn].physicalPage].pid = pid;
		pageTable[vpn].valid = TRUE;

		// bzero(&(machine->mainMemory[pageTable[vpn].physicalPage * PageSize]), PageSize);
		
		if (ChoiceOfReplacementPolicy == FIFO){
			
				//IPT * pe =  ipt + pageTable[vpn].physicalPage;
				fifoQueue.push_back(pageTable[vpn].physicalPage);
				
			
			}

		}	
	   }

     }// status=1
	else{
	  	
     	 pageTable[vpn].physicalPage = bm->Find();
	     if( pageTable[vpn].physicalPage == -1 ){//MainMemory is FULL
			writeOneToSwapFile();
			pageTable[vpn].physicalPage=bm->Find();
	     }
		int diskloc = pageTable[vpn].diskLocation; //location in SwapFile
		 bzero(&(machine->mainMemory[pageTable[vpn].physicalPage * PageSize]), PageSize);
		swapFile->ReadAt(&(machine->mainMemory[pageTable[vpn].physicalPage * PageSize]),
				                        PageSize , diskloc);
		printf("Load one page from SwapFile to Physical Page [%d] !~\n",pageTable[vpn].physicalPage);
		sfBM->Clear( diskloc/PageSize );
		ipt[pageTable[vpn].physicalPage].physicalPage = pageTable[vpn].physicalPage;
		ipt[pageTable[vpn].physicalPage].virtualPage = vpn;
		ipt[pageTable[vpn].physicalPage].valid = TRUE;
		ipt[pageTable[vpn].physicalPage].use = pageTable[vpn].use;
		ipt[pageTable[vpn].physicalPage].dirty = pageTable[vpn].dirty;
		ipt[pageTable[vpn].physicalPage].pid = pid;
		pageTable[vpn].valid = TRUE;
		if (ChoiceOfReplacementPolicy == FIFO){
			
				//IPT * pe =  ipt + pageTable[vpn].physicalPage;
				fifoQueue.push_back(pageTable[vpn].physicalPage);
			}		
		//	printf("move in VPN: %d \n",vpn) ;
	}

 IPTLock->Release();

}







//no use now
void AddrSpace::setPIDToIPT(int _pid){
	for (int i = 0; i < numPages; i++) {
	//update IPT 
	ipt[pageTable[i].physicalPage].pid = _pid;
    }

}
