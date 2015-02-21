// nettest.cc 
//	Test out message delivery between two "Nachos" machines,
//	using the Post Office to coordinate delivery.
//
//	Two caveats:
//	  1. Two copies of Nachos must be running, with machine ID's 0 and 1:
//		./nachos -m 0 -o 1 &
//		./nachos -m 1 -o 0 &
//
//	  2. You need an implementation of condition variables,
//	     which is *not* provided as part of the baseline threads 
//	     implementation.  The Post Office won't work without
//	     a correct implementation of condition variables.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "string.h"
#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"
#include "table.h"

#include "stdlib.h"
#include <string.h>


//#define BUSY 1
//#define FREE 0

#define CREATE_LOCK      'a'
#define ACQUIRE          'b'
#define RELEASE          'c'
#define DESTROY_LOCK     'd'
#define CREATE_CV        'e'
#define SIGNAL           'f'
#define WAIT             'g'
#define BROADCAST		 'h'
#define DESTROY_CV		 'i'
#define CREATE_MONITOR   'j'
#define RETRIEVE_MONITOR 'k'
#define SET_MONITOR      'l'


#define INIT 'm'
#define INIT_SUM 'n'


typedef   unsigned char   byte; 
using namespace std;


// Test out message delivery, by doing the following:
//	1. send a message to the machine with ID "farAddr", at mail box #0
//	2. wait for the other machine's message to arrive (in our mailbox #0)
//	3. send an acknowledgment for the other machine's message
//	4. wait for an acknowledgement from the other machine to our 
//	    original message
/*
int myexp ( int count ) {
  int i, val=1;
  for (i=0; i<count; i++ ) {
    val = val * 10;
  }
  return val;
}*/

void itob(int u, byte *b){
	//byte b[2];
	b[0] = (byte)(u);
	//b[1] = (byte)(u >> 8);
	//return b;
}

/*
void itoa( char arr[], int size, int val ) {
  int i, max, dig, subval, loc;
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
}*/



void MailTest()
{

  //for project4
  int mIDCounter=0;
  int tIDCounter=0;
  int sumThread=0;


  //maybe need change 
   int ThreadTable[10]={0,0,0,0,0,0,0,0,0,0};
   int machineID[10]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

//-------------------------------

    PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;
	char *ack ="";
    char buffer[MaxMailSize];

	
	char *requestType;
	char *arg1;
	char *arg2; /* may be used when Wait or Signal */



	outMailHdr.from = 0; /* it's not the sending mailbox#. This one just tells client which mailbox is used for server to receive */

	/* Wait for the first message from the other machine */
	while(true){

		
		printf("Wait for a request to come...........................................\n");
	    postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);

		//sscanf(buffer,"%2s",);
		requestType = strtok(buffer,",");
		arg1 = strtok(NULL,",");


	    fflush(stdout);

	    outPktHdr.to = inPktHdr.from;
		outMailHdr.to = inMailHdr.from;
	
		printf("FROM MachineID:%d MailBox:%d\n",outPktHdr.to,outMailHdr.to);

		switch(*requestType){

			
			case INIT_SUM:
				if(sumThread!=0) break;
			       printf("get REQUEST\n");
				sumThread=atoi(arg1);
				printf("Total UserPrograms : %d\n", sumThread);
				break;
			case INIT:
				
				
				//printf("get one\n");
				if(sumThread==0){
					printf("Please give the sum of User Programs firstly:\n");
					interrupt->Halt();
					break;
					}

				int newm = 1;
				int find;
				for(int i=0;i<mIDCounter;i++){
					if(outPktHdr.to==machineID[i]){
						newm=0;
						find =i;
						break;			
					}
				}

				if (newm ==1){
					machineID[mIDCounter]=outPktHdr.to;
					//printf("Machine iD: %d\n",outPktHdr.to);

					int mid = atoi(arg1);
					arg2 = strtok(NULL,",");
					int tid =atoi(arg2);
					//sumThread = atoi(arg1);
	
			                
					
					ThreadTable[mIDCounter]++;

					//printf("mIDCoutner :%d\n", mIDCounter);					
					 // printf("THREADTABLE:%d\n",ThreadTable[0]);
					  //printf("THREADTABLE:%d\n",ThreadTable[1]);
						


					mIDCounter++;
				
				}
				else{
					//printf("Machine iD: %d\n",outPktHdr.to);
					ThreadTable[find]++;		
				}
				

				tIDCounter++;
				
				
				

				
				//printf("thread table %d\n",ThreadTable[0]);
				//printf("thread table %d\n",ThreadTable[1]);




				if(tIDCounter==sumThread){// receive msg from all thread, now send simulation	request to all thread;
				
						//printf("All registered successfully! Start Simulation!\n");	
						byte temp[300];
						byte *m=temp;
						for(int i=0;i<300;i++)
							temp[i]='\0';

						//make package
						//int total_pck=  sumThread/12+1;
						
						//totalbyte
						

						for(int i=0;i<10;i++){			
						
								if(ThreadTable[i]==0)
									break;
								//ThreadTable[i]=222;	
						                byte a[3];
						                a[0] = (byte)machineID[i];
								int tt=ThreadTable[i];
								a[1] = (byte)tt>>8;
								a[2] = (byte)tt;
								
								memcpy(m,a,sizeof(byte)*3);
								m=m+3;
									
								

						}

						
						char *data= new char[40];
						for(int i=0;i<40;i++){
							data[i]='\0';
						}

	
							//for(int k=0;k<total_pck;k++){
								int counter=0;
								byte send[12*3];
									 
								//printf("uuuuuuuuuuuu:%d\n",ThreadTable[0]);

									memcpy(send,&temp,(mIDCounter)*3);
								
								
									//int u= (int)(send[1]<<8|send[2]);
									//int v= (int)send[0];	
									//printf("t:%d\n",u);
									//printf("l:%d\n",v);



									
									
									
									//char *sendType="m";
									byte *b=new byte;
									itob(mIDCounter, b);

									int t= (int)b[0];
									//printf("SEND->total:%d\n",strlen((char*)temp));

									//char *a= new char[2];
									//a[0]='f';
									//a[1]='b';
									//char *ab="ab";
									
									char _b=*b;
									char a=',';
									//sprintf(data,"%c,%s",b,send);
									char *_send=(char* )send;
									strncpy(data, &_b , 1);
									strncpy(data+1, &a , 1);
									memcpy(data+2, _send , mIDCounter*3);


									

									//printf("Length: %d\n",strlen(data));
									
									//printf("%s\n",data);
									
									//printf("1 %c\n",data[1]);
									//printf("%c\n",data[3]);	
									
									//printf("%c\n",data[1]);
									byte x=data[2];
									byte o[2];
									o[0]=data[3];
									o[1]=data[4];

									
									t= (int)(o[0]<<8|o[1]);
									//t= (int)o[0];
									//t=(int)data[2];					
									//printf("WHY:::%d\n",t);
									//printf

									counter+=12;
	

						for (int i=0;i<10;i++){
							if(machineID[i]==-1) break;
							for(int j=0;j<ThreadTable[i];j++){

									outMailHdr.length = 3+mIDCounter*3 ;

									//printf("new LENGTH: %d\n", (j+1)*2-1);
									printf("Send to MachineID[%d], MailBox[%d]\n",machineID[i],(j+1)*2-1);
									outMailHdr.to=(j+1)*2-1;
									outPktHdr.to=machineID[i];
									postOffice->Send(outPktHdr, outMailHdr, data);
								}
						}
						interrupt->Halt();
							
				}
				else{
				    printf("Wait for others to register \n");
				
				}
				//monitorValue = atoi(arg2);
				break;

		}


		/* I thought it's necessary to clean the buffer every time server finishes a job */
		buffer[0] = '\0';

	}





}
