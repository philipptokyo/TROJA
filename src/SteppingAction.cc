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

  G4ParticleDefinition* thisDef = step->GetTrack()->GetDefinition(); 
  G4int pdgCode = thisDef->GetPDGEncoding();
  G4int parentID = step->GetTrack()->GetParentID();
  //printf("event %d: parent id is %d, pdg code is %d\n", eID, parentID, pdgCode);
  
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
        fDetInfo->detData.fITheta=pos1.getTheta();
        
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
        
        // CalHitPosition should be done in analysis
        //fDetInfo->CalcHitPosition(detID1, fDetInfo->detData.stripX[detID1], fDetInfo->detData.stripY[detID1]);

        //printf("Found FI x = %f, y = %f, z = %f, stripx = %d, stripy = %d\n", fDetInfo->detData.fIX, fDetInfo->detData.fIY, fDetInfo->detData.fIZ, fDetInfo->detData.stripX[detID1], fDetInfo->detData.stripY[detID1]);
        
        // to sum up energies in each detector, set them from NAN to 0.0
        for(G4int dd=0; dd<fDetInfo->GetNoOfDetectors(); dd++){
          fDetInfo->detData.energyNotSmeared[dd]=0.0; // was NAN 
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
        fDetInfo->detData.energyNotSmeared[d] += edep;
        fDetInfo->detData.haveHit[d] = 1;
        fDetInfo->detData.haveHitID[d] = d; // aux
        //printf("EnergyDeposit in %s is %f\n", tmpName, edep);
      }

  }
  
  
  
  
  // ************************************************************************
  // ****************************** GRAPE ***********************************
  // ************************************************************************
  

  if(fDetInfo->IncludeGrape()){
  //if( (parentID==0 && pdgCode==22) || (fDetInfo->detData.grapeEnergy>0 && parentID>0 && pdgCode<=22) ){ // gammas and it's secondaries
    
    if(TMath::IsNaN(fDetInfo->detData.grapeEnergy)){
      fDetInfo->detData.grapeEnergy=0.0;
    }
    G4Track * theTrack = step->GetTrack();
    int trackVolume = theTrack->GetVolume()->GetCopyNo();
    
    if(trackVolume >=200100 && trackVolume <= 202000)
        {
          //the copy number is 2xxxyz, with xxx=detector number(0-17),y=crystal(0-1),z=segment(0-8)
          // for all other material belonging to the detector, y>=20 
          // reminder of the variables:
          // grapeCrystalFlag[20][2][20];
          //  grapeCrystalEnergyGamma[20][2][20];
          if(trackVolume%100<20)
          {
            //G4cout <<"TrackVolume: "<<trackVolume<<G4endl;
            int seg = trackVolume%10;
            int cry = ((trackVolume-seg)/10)%10;
            int det = (trackVolume-10*cry-seg-200000)/100 -1;
            //G4cout <<"det: "<<det<<G4endl;
            //G4cout <<"cry: "<<cry<<G4endl;
    //        fDetector->fGrapeArray->SetCrystalFlagTrue(det,cry,0);  // The entire crystal
    //        fDetector->fGrapeArray->AddCrystalEnergy(det,cry,0,edep);
    //
    //        fDetector->fGrapeArray->SetCrystalFlagTrue(det,cry,seg+1);  // The individual segments
    //        fDetector->fGrapeArray->AddCrystalEnergy(det,cry,seg+1,edep);

            //printf("Grape: seg %d, cry %d, det %d, energy %f\n", seg, cry, det, edep);

            fDetInfo->detData.grapeEnergy += edep;
    
          }
        }


  //} // cut on primary gammas
  } // IncludeGrape
  
     
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

