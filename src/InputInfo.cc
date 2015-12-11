
#include "InputInfo.hh"

using namespace std;

InputInfo::InputInfo(){
	
	fNumberEvents=100;
	
}

InputInfo::~InputInfo(){
	
}

void InputInfo::parse(char filename[100]){
	
	cout << endl;
	cout << "Parsing of input file" << endl;
	
	
	//check if in/out file exists
	ifstream fin;
	ifstream FileTest(filename);
	if(!FileTest){
		cout << "Can not open file '" << filename << "'! Does it exist?" << endl;
		abort();
	}else{
		cout << "Opening file '" << filename << "'" << endl;
		fin.open(filename);
	}
	
	
	//parse input file line-wise
	string line;
	Int_t counter=0;
	const Int_t stopper=1000; 
	
	const Int_t maxArg=2;
	char temp[maxArg][200];
	
	
	
	while(fin.good())  {
		
		
		//reset values
		for(Int_t i=0; i< maxArg; i++){temp[i][0]='\0';}
		
		
		getline(fin,line);
		//printf("Parsing line '%s'\n",line.c_str());
		
		
		//skip comments
		line.copy(temp[0],2,0); //first two characters
		temp[0][2]='\0';
		//printf("temp1=%s\n",temp1);
		if(strcmp(temp[0],"//")==0){continue;}
		
		//parse the line
		//write each word to a char
		//delimiter is whitespace
		std::istringstream iss(line);
		for(Int_t i=0; i<maxArg; i++){iss >> temp[i];}
		
		//skip empty lines
		if(strcmp(temp[0],"")==0){continue;}
		
		
		
		//get input parameter:
		
		
		if(strcmp(temp[0],"input_from_reaction")==0)  {
// 			sprintf(infilenameFromDicebox,temp[1]);
			strcpy(fInfileName,temp[1]);
			cout << "Input file with events is '" << fInfileName << "'" << endl;
		}
		else if(strcmp(temp[0],"output_rootfile")==0){
// 			sprintf(rootfilenameDicebox,temp[1]);
			strcpy(fOutfileName,temp[1]);
			cout << "Output root file is '" << fOutfileName << "'" << endl;
		}
		else if(strcmp(temp[0],"number_events")==0){
			fNumberEvents=atoi(temp[1]);
			cout << "Number of events to generate is set to " << fNumberEvents << endl;
		}
		else {
			cout<<"InputInfo.cc: Could not read your input keyword '" << temp[0] << "'. Aborting program."<<endl; 
			abort();
		}
		
		
		//count lines
		counter++;
		if(counter>stopper){
			cout << "Reached " << counter << " lines in file '" << filename << "'! Stopping!" << endl;
			abort();
		}
	} // end of reading input file
	
	cout << endl;
	
	
}
