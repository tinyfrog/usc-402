#ifndef   _PACKAGE_H_
#define  _PACKAGE_H_


class package{
public:
		 package();
		void addPackage(int SIXDOLLARBURGER,int THREEDOLLARBURGER,int VIGGIEBURGER,int FRYS,int SODA,int EATIN_TOGO,int t);

	    bool enoughFood();   //check if FoodTable has enough to pack for this order
		void getFood();      // do pack 

		int  getToken();
		int  getStatus();
		int getChoice();
		int  getOT();

		void setToken(int t);
		void setChoice(int ch);
        void setStatus(int st);
		void setOT(int ot);
		
		int getPrice();    //return Price 
private:
		int food[5];   //save the ordered food 
		int choice; // choice between "EAT_IN" and "TO_GO"
		int token;   //the token of one package, it's unique
		int status;  //Status include: NOTREADY,READY,DONE 
		int ot;      //Ordertaker who packs this food
};
#endif

