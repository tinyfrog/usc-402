#include "copyright.h"
#include "string.h"
#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"
#include "table.h"

#include "stdlib.h"
#include "synch.h"

#include <ctime>

#define TOTALNUM 5

List *multiReceivedMsgQueue[5];

int sizeofList = 0;

PacketHeader outPktHdrEC, inPktHdrEC;
MailHeader outMailHdrEC, inMailHdrEC;

time_t currentTime, lastExecutionTime;

char* ReceiveBuffer = new char[MaxMailSize];
//char ReceiveBuffer[MaxMailSize];
//char buffer[MaxMailSize];
char* sentMsg ;
char* ackMsg = new char[MaxMailSize];

struct SendEntry{
	int seqNum;
	char *msg;
	int machineID;
	int mailboxNum;
};

struct ReceiveEntry{
	int seqNum;
	int machineID;
	int mailboxNum;
};

SendEntry *tempSendEntry;
ReceiveEntry *tempReceiveEntry;
ReceiveEntry *tempDupReceiveEntry;
ReceiveEntry *re;
			

List *sentMessagesQueue = new List;
List *receivedMsgQueue = new List;

char* msgType;
char* seqNumChar;

Lock *listLock = new Lock("listLock");

using namespace std;

void Resend(){

	//printf("I'm Resend() funciton,BLABLABLA^^\n");

	listLock->Acquire();

	if(!sentMessagesQueue->IsEmpty()){

		

		ListElement *tempResendElement = sentMessagesQueue->first;		while(tempResendElement!=NULL)		{			tempSendEntry = (SendEntry*)tempResendElement->item;			outPktHdrEC.to = tempSendEntry->machineID;
			outMailHdrEC.to = tempSendEntry->mailboxNum;			outMailHdrEC.length = strlen(tempSendEntry->msg) + 1;			//printf("seqNum is %d\n",tempSendEntry->seqNum);			//printf("outPktHdrEC.to = %d\n",outPktHdrEC.to);			//printf("outMailHdrEC.to = %d\n",outMailHdrEC.to);			printf("Resend %s to Machine %d\n",tempSendEntry->msg,outPktHdrEC.to);			postOffice->Send( outPktHdrEC, outMailHdrEC,tempSendEntry->msg);						tempResendElement=tempResendElement->next;		}		//delete tempResendElement;	}	listLock->Release();	
	//printf("BLABLABLA.....\n");
}


void TimerFucntion(int interval){

	currentTime = time(0);

	/* subtract current time from last execution time */

	if( (currentTime - lastExecutionTime) > interval ){
		printf("Time to RESEND!!!\n");
		Thread *t = new Thread("resendFunc");
		lastExecutionTime = currentTime;
		t->Fork((VoidFunctionPtr)Resend,0);
	/* let the thread do the job , because it cannot be interrupted(in network thread, the interrupt would occur)*/

	}

}

void Sender(char* destMachineID){

	int seqNum = 0; 

	Timer *t = new Timer(TimerFucntion,1,false);
	//printf("Timer Function starts!\n");

	//printf("This is the Sender function\n");

	outPktHdrEC.to = atoi(destMachineID);
	outMailHdrEC.to = 0;
	outMailHdrEC.from = 0;

	//printf("outPktHdrEC.from = %d\n",netname);

	printf("Send to machineID is %d, destMailboxNum is %d.\n",outPktHdrEC.to,outMailHdrEC.to);

	SendEntry *newEntry;

	for(int i =0;i<3;i++){
		sentMsg = new char[MaxMailSize];

		sprintf(sentMsg,"%s,%d","msg",seqNum);

		/* define the machine and mailbox id of the receiver */

		newEntry = new SendEntry;	
		newEntry->seqNum = seqNum;
		
		newEntry->msg = sentMsg;
		newEntry->machineID = outPktHdrEC.to;
		newEntry->mailboxNum =  outMailHdrEC.to;

		outMailHdrEC.length = strlen(sentMsg) + 1;
		postOffice->Send(outPktHdrEC, outMailHdrEC, sentMsg);

		listLock->Acquire();

		/*Crowley said there need a lock*/
		sentMessagesQueue->Append(newEntry);

		sizeofList++;
		listLock->Release();
		
		seqNum++;

	}
	
	/* Do the receive! */
	//printf("Receiver starts to work\n");

	while(true){

		postOffice->Receive(0, &inPktHdrEC, &inMailHdrEC, ReceiveBuffer);

		//printf("Receive %s from machine %d, mailbox %d\n",ReceiveBuffer,inPktHdrEC.from, inMailHdrEC.from);
		outPktHdrEC.to = inPktHdrEC.from;
		outMailHdrEC.to = inMailHdrEC.from;
		

		msgType = strtok(ReceiveBuffer,",");
		seqNumChar = strtok(NULL,",");

		/* received an ack */
		/* remove the specific element corresponding to the seq num */
		if(strcmp(msgType,"ack")==0 ){
			
			/* remove items from the sending list */
						ListElement *tempElement;			listLock->Acquire();			for(int i = 0;i<sizeofList;i++){				tempElement = sentMessagesQueue->first;				tempSendEntry = (SendEntry*)tempElement->item;				//printf("atoi(seqNumChar) = %d\n",atoi(seqNumChar));
				//printf("Interesting loop:\n");
				//printf("tempSendEntry->seqNum = %d\n",tempSendEntry->seqNum);
				if(tempSendEntry->seqNum == atoi(seqNumChar)){					printf("Receive ACK from Machine %d, Mailbox %d, with the sequence %d!\n",inPktHdrEC.from,inMailHdrEC.from,atoi(seqNumChar));					sentMessagesQueue->Remove();					sizeofList--;							break;				}				else{					sentMessagesQueue->Append( (SendEntry*)sentMessagesQueue->Remove() );				}							}			listLock->Release();			//delete tempElement;

		}
		/* it's a msg */
		else if(strcmp(msgType,"msg") == 0) {

			if(!receivedMsgQueue->IsEmpty()){

				tempReceiveEntry = (ReceiveEntry*)receivedMsgQueue->last->item;

				/* If sequence is right,send back ack */
				if( (tempReceiveEntry->seqNum + 1) == atoi(seqNumChar) ){

					sprintf(ackMsg,"%s,%d","ack",atoi(seqNumChar));

					printf("Receive message from Machine %d, Mailbox %d, with the sequence %d!\n",inPktHdrEC.from,inMailHdrEC.from,atoi(seqNumChar));
					//printf("Store it & Send back ACK!!\n");
					/* new an entry to store */
					re = new ReceiveEntry();
					re->seqNum = atoi(seqNumChar);
					re->machineID = inPktHdrEC.from;
					re->mailboxNum = inMailHdrEC.from;

					receivedMsgQueue -> Append(re);

					outMailHdrEC.length = strlen(ackMsg) + 1;
					postOffice->Send(outPktHdrEC,outMailHdrEC,ackMsg);


				}
				else{

					ListElement *tempDuplicatedElement = receivedMsgQueue->first;

					while(tempDuplicatedElement != NULL){

						tempDupReceiveEntry = (ReceiveEntry*)tempDuplicatedElement->item;
						//printf("tempDupReceiveEntry->seqNum is %d\n",tempDupReceiveEntry->seqNum);

						if(tempDupReceiveEntry->seqNum == atoi(seqNumChar)){

							//printf("Receive duplicate msg with seq# %d! Send ack Again!\n", atoi(seqNumChar));
							sprintf(ackMsg,"%s,%d","ack",atoi(seqNumChar));
					
							outMailHdrEC.length = strlen(ackMsg) + 1;
							postOffice->Send(outPktHdrEC,outMailHdrEC,ackMsg);
							break;
						}

						tempDuplicatedElement = tempDuplicatedElement->next;

					}

					//if(tempDuplicatedElement == NULL)
						//printf("Receive out of order msg! Drop It!\n");

					//delete tempDuplicatedElement;
				}
			}
			else{
				if(atoi(seqNumChar) == 0){
					re = new ReceiveEntry();
					re->seqNum = atoi(seqNumChar);
					re->machineID = inPktHdrEC.from;
					re->mailboxNum = inMailHdrEC.from;

					printf("Receive message from Machine %d, Mailbox %d, with the sequence %d!\n",inPktHdrEC.from,inMailHdrEC.from,atoi(seqNumChar));

					receivedMsgQueue -> Append(re);
					sprintf(ackMsg,"%s,%d","ack",atoi(seqNumChar));
				
					outMailHdrEC.length = strlen(ackMsg) + 1;
					postOffice->Send(outPktHdrEC,outMailHdrEC,ackMsg);

				}
					//printf("Receive out of order msg! Drop It!\n");

			}
				



		}

	}

}


void MultiSender(){

	Timer *t = new Timer(TimerFucntion,1,false);
	//printf("Timer Function starts!\n");

	for(int k = 0;k<5;k++){

		multiReceivedMsgQueue[k] = new List;

	}

	//printf("This is the Sender function\n");
	
	outMailHdrEC.to = 0;
	outMailHdrEC.from = 0;	

	//printf("Send to machineID is %d, destMailboxNum is %d.\n",outPktHdrEC.to,outMailHdrEC.to);

	SendEntry *newEntry;

	//printf("size of the Sending List is %d\n",sizeof sentMessagesQueue);

	/* different from the sender */
	for (int m = 0 ; m<TOTALNUM; m++){
		if(m != netname){
			outPktHdrEC.to = m;
			printf("Send to machineID is %d, destMailboxNum is %d.\n",outPktHdrEC.to,outMailHdrEC.to);
			for(int i =0;i<3;i++){
				sentMsg = new char[MaxMailSize];

				sprintf(sentMsg,"%s,%d","msg",i);

				/* define the machine and mailbox id of the receiver */

				newEntry = new SendEntry;	
				newEntry->seqNum = i;
				
				newEntry->msg = sentMsg;
				newEntry->machineID = outPktHdrEC.to;
				newEntry->mailboxNum =  outMailHdrEC.to;

				outMailHdrEC.length = strlen(sentMsg) + 1;
				postOffice->Send(outPktHdrEC, outMailHdrEC, sentMsg);

				listLock->Acquire();
				/*Crowley said there need a lock*/
				sentMessagesQueue->Append(newEntry);
				sizeofList++;

				listLock->Release();
				
			}
		}
	}
	
	//printf("size of the Sending List is %d\n",sizeof sentMessagesQueue);

	/* Do the receive! */
	//printf("Receiver starts to work\n");

	while(true){

		postOffice->Receive(0, &inPktHdrEC, &inMailHdrEC, ReceiveBuffer);

		//printf("Receive %s from machine %d, mailbox %d\n",ReceiveBuffer,inPktHdrEC.from, inMailHdrEC.from);
		outPktHdrEC.to = inPktHdrEC.from;
		outMailHdrEC.to = inMailHdrEC.from;
		

		msgType = strtok(ReceiveBuffer,",");
		seqNumChar = strtok(NULL,",");

		/* received an ack */
		/* remove the specific element corresponding to the seq num */
		if(strcmp(msgType,"ack")==0 ){
			
			/* remove items from the sending list */
						ListElement *tempElement;			listLock->Acquire();			for(int i=0;i<sizeofList;i++){				tempElement = sentMessagesQueue->first;				tempSendEntry = (SendEntry*)tempElement->item;				//printf("atoi(seqNumChar) = %d\n",atoi(seqNumChar));
				//printf("Interesting loop:\n");
				//printf("tempSendEntry->seqNum = %d\n",tempSendEntry->seqNum);
				//printf("tempSendEntry->machineID = %d\n",tempSendEntry->machineID);
				//printf("inPktHdrEC.from = %d\n",inPktHdrEC.from);
				if(tempSendEntry->seqNum == atoi(seqNumChar)&& tempSendEntry->machineID == inPktHdrEC.from ){					printf("Receive ACK from Machine %d, Mailbox %d, with the sequence %d!\n",inPktHdrEC.from,inMailHdrEC.from,atoi(seqNumChar));					sentMessagesQueue->Remove();					sizeofList--;					break;				}				else{										sentMessagesQueue->Append( (SendEntry*)sentMessagesQueue->Remove() );							}							}			listLock->Release();			//delete tempElement;

		}
		/* it's a msg */
		else if(strcmp(msgType,"msg") == 0) {

			if(!multiReceivedMsgQueue[inPktHdrEC.from]->IsEmpty()){

				tempReceiveEntry = (ReceiveEntry*)multiReceivedMsgQueue[inPktHdrEC.from]->last->item;

				/* If sequence is right,send back ack */
				if( (tempReceiveEntry->seqNum + 1) == atoi(seqNumChar) ){

					sprintf(ackMsg,"%s,%d","ack",atoi(seqNumChar));

					printf("Receive message from Machine %d, Mailbox %d, with the sequence %d!\n",inPktHdrEC.from,inMailHdrEC.from,atoi(seqNumChar));
					//printf("Store it & Send back ACK!!\n");

					/* new an entry to store */
					re = new ReceiveEntry();
					re->seqNum = atoi(seqNumChar);
					re->machineID = inPktHdrEC.from;
					re->mailboxNum = inMailHdrEC.from;

					multiReceivedMsgQueue[inPktHdrEC.from] -> Append(re);

					outMailHdrEC.length = strlen(ackMsg) + 1;
					postOffice->Send(outPktHdrEC,outMailHdrEC,ackMsg);


				}
				else{

					ListElement *tempDuplicatedElement = multiReceivedMsgQueue[inPktHdrEC.from]->first;

					while(tempDuplicatedElement != NULL){

						tempDupReceiveEntry = (ReceiveEntry*)tempDuplicatedElement->item;
						//printf("tempDupReceiveEntry->seqNum is %d\n",tempDupReceiveEntry->seqNum);

						if(tempDupReceiveEntry->seqNum == atoi(seqNumChar)){

							//printf("Receive duplicate msg with seq# %d! Send ack Again!\n", atoi(seqNumChar));
							sprintf(ackMsg,"%s,%d","ack",atoi(seqNumChar));
					
							outMailHdrEC.length = strlen(ackMsg) + 1;
							postOffice->Send(outPktHdrEC,outMailHdrEC,ackMsg);
							break;
						}

						tempDuplicatedElement = tempDuplicatedElement->next;

					}

					//if(tempDuplicatedElement == NULL)
						//printf("Receive out of order msg! Drop It!\n");

					//delete tempDuplicatedElement;
				}
			}
			else{
				if(atoi(seqNumChar) == 0){
					re = new ReceiveEntry();
					re->seqNum = atoi(seqNumChar);
					re->machineID = inPktHdrEC.from;
					re->mailboxNum = inMailHdrEC.from;

					printf("Receive message from Machine %d, Mailbox %d, with the sequence %d!\n",inPktHdrEC.from,inMailHdrEC.from,atoi(seqNumChar));

					multiReceivedMsgQueue[inPktHdrEC.from] -> Append(re);
					sprintf(ackMsg,"%s,%d","ack",atoi(seqNumChar));
				
					outMailHdrEC.length = strlen(ackMsg) + 1;
					postOffice->Send(outPktHdrEC,outMailHdrEC,ackMsg);

				}
				//else
					//printf("Receive out of order msg! Drop It!\n");

			}
				



		}

	}

}

