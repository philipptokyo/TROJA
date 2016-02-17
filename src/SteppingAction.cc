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
#include "G4SystemOfUnits.hh"

#include "TMath.h"
#include <cmath>


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
{ 
  fgInstance = this;
}

SteppingAction::SteppingAction(DetectorInfo* detInfo)
: G4UserSteppingAction(),
  fVolume(0)
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

    G4int detID1 = atoi(point1->GetPhysicalVolume()->GetName().data());
    G4int detID2 = -1; // for cross checking
    
    // check which Si detector was hit
    for(G4int d=0; d<fDetInfo->GetNoOfDetectors(); d++){
      char tmpName[50];
      sprintf(tmpName, "logical%02d", d);
      //printf("looking for %s, found %s\n", tmpName, volume->GetName().data());
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
        
        fDetInfo->detData.fIDetID = detID1;
         
        // get strip number of FI
        // for later position determination in the analysis
        fDetInfo->CalcStripNumbers(detID1, 
                                   fDetInfo->detData.fIX, 
                                   fDetInfo->detData.fIY, 
                                   fDetInfo->detData.fIZ, 
                                   fDetInfo->detData.stripX[detID1], 
                                   fDetInfo->detData.stripY[detID1] );

        //printf("Found FI x = %f, y = %f, z = %f, stripx = %d, stripy = %d\n", fDetInfo->detData.fIX, fDetInfo->detData.fIY, fDetInfo->detData.fIZ, fDetInfo->detData.stripX[detID1], fDetInfo->detData.stripY[detID1]);
        
        // to sum up energies in each detector, set them from NAN to 0.0
        for(G4int dd=0; dd<fDetInfo->GetNoOfDetectors(); dd++){
          fDetInfo->detData.energy[dd]=0.0; // was NAN 
        }

        
        // leave the FI loop
        break;
      }
 
 
 
    }
  } // end of first interaction point

  
  // now, sum up the energies in each detector

  G4double edep = step->GetTotalEnergyDeposit()/MeV; 
  
  //printf("\nEvent %d\n", eID);
  for(G4int d=0; d<fDetInfo->GetNoOfDetectors(); d++){
      char tmpName[50];
      sprintf(tmpName, "logical%02d", d);
      //printf("looking for %s\n", tmpName);
      if(std::strcmp(volume->GetName(), tmpName)==0){
        fDetInfo->detData.energy[d] += edep;
        fDetInfo->detData.haveHit[d] = 1;
        fDetInfo->detData.haveHitID[d] = d; // aux
        //printf("EnergyDeposit in %s is %f\n", tmpName, edep);
      }

  }
  
  
  
  
  
     
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

//  fDetInfo->detData.detID = -1;
//
//  fDetInfo->detData.fIX = NAN;
//  fDetInfo->detData.fIY = NAN;
//  fDetInfo->detData.fIZ = NAN;
//  
//  for(G4int d=0; d<maxDetectors; d++){
//    fDetInfo->detData.stripX[d] = -1;
//    fDetInfo->detData.stripY[d] = -1;
//    fDetInfo->detData.energy[d] = NAN;
//  }

  fDetInfo->ResetData();  
  fFI = 0;

}

