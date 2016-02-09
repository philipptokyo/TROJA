#include "DetectorInfo.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4Material.hh"
#include "G4ThreeVector.hh"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

//#include        <stdio.h>
//#include        <cstdio>
//#include        <cstdlib>




DetectorInfo::DetectorInfo()
{ 
  

  for(G4int d=0; d<GetMaxNoDetectors(); d++){
      SetCenterX(d, 0.0);
      SetCenterY(d, 0.0);
      SetCenterZ(d, 0.0);
    
    SetRotationX(d, 0.0); 
    SetRotationY(d, 0.0); 
    SetRotationZ(d, 0.0); 
    
        SetSizeX(d, 0.0); 
        SetSizeY(d, 0.0); 
        SetSizeZ(d, 0.0); 
           
    SetNoStripsX(d, 0);  
    SetNoStripsY(d, 0);  
  }
  
}

DetectorInfo::~DetectorInfo()
{ }


void DetectorInfo::CalcStripNumbers(G4int detID, G4double hx, G4double hy, G4double hz, G4int &stripx, G4int &stripy)
{
  //printf("inside: calculating strip no x\n");

  G4ThreeVector vhit(hx, hy, hz); // vector from origin to FI
  G4ThreeVector vcen(GetCenterX(detID), GetCenterY(detID), GetCenterZ(detID)); // vector from origin to center detector
  G4ThreeVector vori(GetSizeX(detID)/2.0, GetSizeY(detID)/2.0, 0.0); // vector to move origin to center of lower left corner of detector
  
  G4ThreeVector vdet=vhit-vcen;

  vdet.rotateX(GetRotationX(detID));
  vdet.rotateY(GetRotationY(detID));
  vdet.rotateZ(GetRotationZ(detID));
  
  G4ThreeVector vpix=vdet+vori;
  

  stripx = (G4int)(vpix.getX()/(GetSizeX(detID)/GetNoStripsX(detID)));
  stripy = (G4int)(vpix.getY()/(GetSizeY(detID)/GetNoStripsY(detID)));
  
}




void DetectorInfo::Parse(G4String filename)
{
  
  printf("\n\nParsing of detector geometry file\n");


  //check if in/out file exists
  std::ifstream fin;
  std::ifstream FileTest(filename);
  if(!FileTest){
    printf("Can not open file '%s'! Does it exist?\n", filename.data());
    abort();
  }else{
    printf("Opening file '%s'\n", filename.data());
    fin.open(filename);
  }


  //parse input file line-wise
  std::string line;
  G4int counter=0;
  const G4int stopper=10000;

  const G4int maxArg=5;
  char temp[maxArg][500];



  while(fin.good())  {

    //reset values
    for(G4int i=0; i< maxArg; i++){temp[i][0]='\0';}


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
    for(G4int i=0; i<maxArg; i++){iss >> temp[i];}

    //skip empty lines
    if(strcmp(temp[0],"")==0){continue;}



    //get input parameter:

    // file names   
    if(strcmp(temp[0],"number_of_detectors")==0)  {
      fNoOfDet=atoi(temp[1]);
      printf("Number of detectors %d\n", fNoOfDet);
    }
    else if(strcmp(temp[0],"position")==0){
      G4int index = atoi(temp[1]);
      SetCenterX(index, atof(temp[2])*cm);
      SetCenterY(index, atof(temp[3])*cm);
      SetCenterZ(index, atof(temp[4])*cm);
      printf("Got position of detector %d: x = %f, y = %f, z = %f\n", index, GetCenterX(index), GetCenterY(index), GetCenterZ(index));
    }
    else if(strcmp(temp[0],"rotation")==0){
      G4int index = atoi(temp[1]);
      SetRotationX(index, atof(temp[2])*deg);
      SetRotationY(index, atof(temp[3])*deg);
      SetRotationZ(index, atof(temp[4])*deg);
      printf("Got rotation of detector %d: x = %f, y = %f, z = %f\n", index, GetRotationX(index), GetRotationY(index), GetRotationZ(index));
    }
    else if(strcmp(temp[0],"size")==0){
      G4int index = atoi(temp[1]);
      SetSizeX(index, atof(temp[2])*cm);
      SetSizeY(index, atof(temp[3])*cm);
      SetSizeZ(index, atof(temp[4])*cm);
      printf("Got full size of detector %d: x = %f, y = %f, z = %f\n", index, GetSizeX(index), GetSizeY(index), GetSizeZ(index));
    }
    else if(strcmp(temp[0],"number_of_stripes")==0){
      G4int index = atoi(temp[1]);
      SetNoStripsX(index, atoi(temp[2]));
      SetNoStripsY(index, atoi(temp[3]));
      printf("Got number of strips of detector %d: x = %d, y = %d\n", index, GetNoStripsX(index), GetNoStripsY(index));
    }
    else if(strcmp(temp[0],"name")==0){
      printf("Info: 'name' not yet implemented.\n");
    }
    else if(strcmp(temp[0],"type")==0){
      printf("Info: 'type' not yet implemented.\n");
    }
    else {
      printf("Could not read your input keyword '%s'. Aborting program.\n", temp[0]);
      abort();
    }


    //count lines
    counter++;
    if(counter>stopper){
      printf("Reached %d lines in file '%s'! Stopping!\n", counter, filename.data());
      abort();
    }
  } // end of reading input file

  printf("\n");



}



