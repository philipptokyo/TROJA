//
// $Id$
//
/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class

#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "DetectorGlobals.hh"
#include "DetectorInfo.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

#include "TMath.h"
//#include <iostream>
#include <cmath>
//#include <ctgmath>
//#include <cstdint>
//#include <cstring>

//using namespace DetectorGlobals;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction* SteppingAction::fgInstance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction* SteppingAction::Instance()
{
// Static acces function via G4RunManager 

  return fgInstance;
}      

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction()
: G4UserSteppingAction(),
  fVolume(0)
//  fEnergy1(0.),
//  fEnergy2(0.),
//  fX1(0.),
//  fY1(0.),
//  fZ1(0.),
//  fStripX(-1),
//  fStripY(-1),
//  fFI(0)
{ 
  fgInstance = this;
}

SteppingAction::SteppingAction(DetectorInfo* detInfo)
: G4UserSteppingAction(),
  fVolume(0)
//  fEnergy1(0.),
//  fEnergy2(0.),
//  fX1(0.),
//  fY1(0.),
//  fZ1(0.),
//  fStripX(-1),
//  fStripY(-1),
//  fFI(0)
{ 
  fDetInfo=detInfo;
  fgInstance = this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{ 
  fgInstance = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  // get volume of the current step
  G4LogicalVolume* volume 
    = step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();
      
  // 
  G4StepPoint* point1 = step->GetPreStepPoint();

  G4int eID = 0;
  const G4Event* evt = G4RunManager::GetRunManager()->GetCurrentEvent();
  if(evt) eID = evt->GetEventID();

  fDetInfo->detData.eventNumber = eID;

  // ************************************************************************
  // ************************* Silicon Spheres ******************************
  // ************************************************************************
  
  // collect energy and track length step by step
  // first Si
    //printf("volume name is %s\n",volume->GetName().c_str());
//  if (std::strcmp(volume->GetName(), "Shape1")==0 ){
//    //printf("in volume 1\n");
//    G4double edep = step->GetTotalEnergyDeposit();
//    fEnergy1 += edep;
//    //printf("fEnergy1 = %f\n", fEnergy1);
//    
//    // check if the particle just entered Shape1
//    // if so, get the position of the first interaction point
//    if (point1->GetStepStatus() == fGeomBoundary) {
//      //printf("FI ");
//      G4ThreeVector pos1 = point1->GetPosition();
//      fX1=pos1.getX();
//      fY1=pos1.getY();
//      fZ1=pos1.getZ();
//      //printf("x=%f y=%f z=%f\n", fX1, fY1, fZ1);
//    }
//  }
//  
//  
//  if (std::strcmp(volume->GetName(), "Shape2")==0 ){
//    //printf("in volume 2\n");
//    if(TMath::IsNaN(fEnergy2)){
//      fEnergy2 = 0.0;
//    }
//    G4double edep = step->GetTotalEnergyDeposit();
//    fEnergy2 += edep;
//    //printf("fEnergy2 = %f\n", fEnergy2);
//    
//  }
  
  
  
  
  
  
  // ************************************************************************
  // *************************** Silicon SSD ********************************
  // ************************************************************************
  
  // check if it is the first interaction point in a Silicon detector
  if ((point1->GetStepStatus() == fGeomBoundary) && !(fFI) ) {

//    fFI = 1;

    G4ThreeVector pos1 = point1->GetPosition();
//    fX1=pos1.getX();
//    fY1=pos1.getY();
//    fZ1=pos1.getZ();
//    fDetInfo->detData.fIX=pos1.getX();
//    fDetInfo->detData.fIY=pos1.getY();
//    fDetInfo->detData.fIZ=pos1.getZ();

    G4int detID1 = atoi(point1->GetPhysicalVolume()->GetName().data());
    G4int detID2 = -1; // for cross checking
    
    // check which Si detector was hit
    for(G4int d=0; d<fDetInfo->GetNoOfDetectors(); d++){
      char tmpName[50];
      sprintf(tmpName, "logical%02d", d);
      //printf("looking for %s\n", tmpName);
      if(std::strcmp(volume->GetName(), tmpName)==0){
        fFI = 1;
        detID2 = d;
        
        // save FI for later cross checking
        fDetInfo->detData.fIX=pos1.getX();
        fDetInfo->detData.fIY=pos1.getY();
        fDetInfo->detData.fIZ=pos1.getZ();
        
        // cross check detector IDs
        if(detID1 != detID2){
          printf("Missmatch in obtained detector geometry ID: %d != %d\n", detID1, detID2);
          abort();
        }
        
        //fDetInfo->detData.detID = detID1;
         
        // get strip number of FI
        // for later position determination in the analysis
        //fDetInfo->CalcStripNumbers(detID2, fX1, fY1, fZ1, fStripX, fStripY);
        fDetInfo->CalcStripNumbers(detID1, 
                                   fDetInfo->detData.fIX, 
                                   fDetInfo->detData.fIY, 
                                   fDetInfo->detData.fIZ, 
                                   fDetInfo->detData.stripX[detID1], 
                                   fDetInfo->detData.stripY[detID1] );
        
        // to sum up energies in each detector, set them from NAN to 0.0
        for(G4int dd=0; dd<fDetInfo->GetNoOfDetectors(); dd++){
          fDetInfo->detData.energy[dd]=0.0; // was NAN 
        }

//        printf("Event %d, Found first interaction point\n", eID);
//        printf("Is in %s (detID1 %d, detID2 %d)\n", point1->GetPhysicalVolume()->GetName().data(), detID1, detID2);
//        printf("Is in %s\n", volume->GetName().data());
//        //printf("Position x %f, y %f, z %f\n", X1, fY1, fZ1);
//        printf("Position x %f, y %f, z %f\n", fDetInfo->detData.fIX, fDetInfo->detData.fIY, fDetInfo->detData.fIZ);
//        printf("Detector ID %d, strip x %d, strip y %d\n", detID1, fStripX, fStripY);
//        printf("\n");
        
        // leave the FI loop
        break;
      }
 
 
 
    }
  } // end of first interaction point

  
  // now, sum up the energies in each detector

  G4double edep = step->GetTotalEnergyDeposit(); // HERE IS A PROBLEM!!!!!!!!!!!!!!! todo

  for(G4int d=0; d<fDetInfo->GetNoOfDetectors(); d++){
      char tmpName[50];
      sprintf(tmpName, "logical%02d", d);
      //printf("looking for %s\n", tmpName);
      if(std::strcmp(volume->GetName(), tmpName)==0){
        fDetInfo->detData.energy[d] += edep;
      }

  }
  
//  if (std::strcmp(volume->GetName(), "logical00")==0 ){
//    //printf("in SSD1\n");
//    if(TMath::IsNaN(fEnergy1)){ // default is nan, set to zero in case of hit
//      fEnergy1 = 0.0;
//    }
//    G4double edep = step->GetTotalEnergyDeposit();
//    fEnergy1 += edep;
//    //printf("fEnergy1 = %f\n", fEnergy1);
//
//
//      
//    //if (point1->GetStepStatus() == fGeomBoundary) { // first interaction point?
//    if ((point1->GetStepStatus() == fGeomBoundary) && (fStripX == -1) ) { // first interaction point?
//      //printf("FI ");
//
//      fDetInfo->CalcStripNumbers(0, fX1, fY1, fZ1, fStripX, fStripY);
//      //printf("x=%f y=%f z=%f\n", fX1, fY1, fZ1);
//    }
//
//    //printf("Got: fEnergy1 = %f, fStripNo1 %i, fX1 %f, fY1 %f, fZ1 %f\n", fEnergy1, fStripNo1, fX1, fY1, fZ1);
//
//
//  }
  
  
  
  
  
  
  
     
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::Reset()
{
//  fEnergy1 = NAN;
//  fEnergy2 = NAN;
//  fX1 = NAN;
//  fY1 = NAN;
//  fZ1 = NAN;
//  fStripX = -1;
//  fStripY = -1;

  fDetInfo->detData.detID = -1;

  fDetInfo->detData.fIX = NAN;
  fDetInfo->detData.fIY = NAN;
  fDetInfo->detData.fIZ = NAN;
  
  for(G4int d=0; d<maxDetectors; d++){
    fDetInfo->detData.stripX[d] = -1;
    fDetInfo->detData.stripY[d] = -1;
    fDetInfo->detData.energy[d] = NAN;
  }
  
  fFI = 0;

}

