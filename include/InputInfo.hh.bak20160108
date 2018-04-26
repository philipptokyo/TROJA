#ifndef InputInfo_hh
#define InputInfo_hh

#include "../LibPerso.h"

class InputInfo
{
	public:

        //getter and setter
        /// todo: not complete, yet	
        std::string GetInfileNameString(){return (std::string) fInfileName;};
        std::string GetOutfileNameString(){return (std::string) fOutfileName;};
        
        void SetNumberOfEvents(Int_t number){fNumberEvents = number;};
        Int_t GetNumberOfEvents(){return fNumberEvents;};
        	
	void parse(char filename[100]);
	
	InputInfo();
	~InputInfo();

        
        private:
        
	char fInfileName[200]; //root file
	
	char fOutfileName[200];//root file
	
	Int_t fNumberEvents;
        	
	
	
};

#endif
