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
        detID2 = d;
        
        if(fDetInfo->IsPosDet(d)){
          fFI = 1;
        }
        
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
  
  //if(edep>0){
  //  printf("\nEvent %d, edep %lf\n", eID, edep);
  //}

  char tmpName[50];
  for(G4int d=0; d<fDetInfo->GetNoOfDetectors(); d++){
    sprintf(tmpName, "logical%02d", d);
    //printf("looking for %s\n", tmpName);
    if(std::strcmp(volume->GetName(), tmpName)==0){
      fDetInfo->detData.energyNotSmeared[d] += edep;
      fDetInfo->detData.haveHit[d] = 1;
      fDetInfo->detData.haveHitID[d] = d; // aux
      //printf("EnergyDeposit in %s is %f\n", tmpName, edep);
    }

  }

  if(std::strcmp(volume->GetName(), "target_log")==0){
    fDetInfo->detData.targetEnergyLoss += edep;
  }
  
  
  
  
  // ************************************************************************
  // ****************************** GRAPE ***********************************
  // ************************************************************************
  

  if(fDetInfo->IncludeGrape()){
//  //if( (parentID==0 && pdgCode==22) || (fDetInfo->detData.grapeEnergy>0 && parentID>0 && pdgCode<=22) ){ // gammas and it's secondaries
//    
//    G4Track * theTrack = step->GetTrack();
//    int trackVolume = theTrack->GetVolume()->GetCopyNo();
//
//    //if(trackVolume>0) {
//    //  printf("trackVolume %d\n", trackVolume);
//    //}
//    
//    //if(trackVolume >=200100 && trackVolume <= 202000)
//    if(trackVolume >=200100 && trackVolume <= 202000 && edep>0.000)
//        {
//          //if(TMath::IsNaN(fDetInfo->detData.grapeEnergy)){
//          //  fDetInfo->detData.grapeEnergy=0.0;
//          //}
//          
//          //the copy number is 2xxxyz, with xxx=detector number(0-17),y=crystal(0-1),z=segment(0-8)
//          // for all other material belonging to the detector, y>=20 
//          // reminder of the variables:
//          // grapeCrystalFlag[20][2][20];
//          //  grapeCrystalEnergyGamma[20][2][20];
//          if(trackVolume%100<20)
//          {
//            //G4cout <<"TrackVolume: "<<trackVolume<<G4endl;
//            int seg = trackVolume%10;
//            int cry = ((trackVolume-seg)/10)%10;
//            int det = (trackVolume-10*cry-seg-200000)/100 -1;
//            //G4cout <<"det: "<<det<<G4endl;
//            //G4cout <<"cry: "<<cry<<G4endl;
//    //        fDetector->fGrapeArray->SetCrystalFlagTrue(det,cry,0);  // The entire crystal
//    //        fDetector->fGrapeArray->AddCrystalEnergy(det,cry,0,edep);
//    //
//    //        fDetector->fGrapeArray->SetCrystalFlagTrue(det,cry,seg+1);  // The individual segments
//    //        fDetector->fGrapeArray->AddCrystalEnergy(det,cry,seg+1,edep);
//
//            //printf("Grape: seg %d, cry %d, det %d, energy dep %f\n", seg, cry, det, edep);
//
//            ////fDetInfo->detData.grapeEnergy += edep;
//            //if(fDetInfo->detData.grapeDet==-1){
//            //  fDetInfo->detData.grapeDet=det;
//            //  fDetInfo->detData.grapeCrystal=cry;
//            //  fDetInfo->detData.grapeSegment=seg;
//            //  fDetInfo->detData.grapeEnergy += edep;
//            //}else if(fDetInfo->detData.grapeDet==det){
//            //  fDetInfo->detData.grapeEnergy += edep;
//            //}
//
//            if(TMath::IsNaN(fDetInfo->detData.grapeDetEnergy[det])){
//              fDetInfo->detData.grapeDetEnergy[det]=0.0;
//              fDetInfo->detData.grapeDetMul++;
//            }
//            if(TMath::IsNaN(fDetInfo->detData.grapeCryEnergy[det][cry])){
//              fDetInfo->detData.grapeCryEnergy[det][cry]=0.0;
//              fDetInfo->detData.grapeCryMul[det]++;
//            }
//            if(TMath::IsNaN(fDetInfo->detData.grapeSegEnergy[det][cry][seg])){
//              fDetInfo->detData.grapeSegEnergy[det][cry][seg]=0.0;
//              fDetInfo->detData.grapeSegMul[det][cry]++;
//            }
//
//            fDetInfo->detData.grapeDetEnergy[det] += edep;
//            fDetInfo->detData.grapeCryEnergy[det][cry] += edep;
//            fDetInfo->detData.grapeSegEnergy[det][cry][seg] += edep;
//
//            //G4ParticleDefinition* thisDef = step->GetTrack()->GetDefinition(); 
//            //G4int pdgCode = thisDef->GetPDGEncoding();
//            //G4int parentID = step->GetTrack()->GetParentID();
//            //printf("event %d: parent id is %d, pdg code is %d\n", eID, parentID, pdgCode);
//            //printf("Grape: seg %d, cry %d, det %d, energy %f\n", seg, cry, det, edep);
//            //printf("egam det %f \n", fDetInfo->detData.grapeDetEnergy[0]);
//            //printf("mul det %d, cry %d\n", fDetInfo->detData.grapeDetMul, fDetInfo->detData.grapeCryMul[0]);
//    
//          }
//        }

    // checking energy deposit
    //G4double energyDep   = step->GetTotalEnergyDeposit();
    //G4double deltaEnergy = step->GetDeltaEnergy();
    //G4double stepl       = step->GetStepLength(); 
 
    // If delta-energy is equal to zero, skip this process.
    // The events where energy deposit is zero are reserved 
    // becase in the compton scattering event the energy deposit is zero 
    // but the momentum of gamma ray is changed.
    //   if (deltaEnergy == 0.) return false;
    //   if (energyDep/keV == 0.) return false;
 
    // using post step information
    G4StepPoint        *preStepPoint  = step->GetPreStepPoint();
    G4StepPoint        *postStepPoint = step->GetPostStepPoint();
    //G4VPhysicalVolume  *preStepVolume;
    //G4VPhysicalVolume  *postStepVolume;
 
    G4TouchableHistory *preStepTouchable;
    G4TouchableHistory *postStepTouchable;
    //G4VProcess         *preStepProcess;
    //G4VProcess         *postStepProcess;
    G4ThreeVector globalPostStepPosition,globalPreStepPosition;
    G4ThreeVector localPostStepPosition, localPreStepPosition;
    G4ThreeVector globalPosition, localPosition;
    G4int seg;
    G4int cry;
    G4int det;
    //G4int id;
    //G4double mass = preStepPoint->GetMass();
 
    // pre step information 
    //preStepProcess = (G4VProcess*) preStepPoint->GetProcessDefinedStep();
    preStepTouchable = (G4TouchableHistory*)(preStepPoint->GetTouchable());
    //preStepVolume = preStepTouchable->GetVolume();
    globalPreStepPosition = preStepPoint->GetPosition();
    localPreStepPosition  = 
      preStepTouchable->GetHistory()
      ->GetTopTransform().TransformPoint(globalPreStepPosition);
    
    // post step information 
    //postStepProcess = (G4VProcess*) postStepPoint->GetProcessDefinedStep();
    postStepTouchable = (G4TouchableHistory*)(postStepPoint->GetTouchable());
    //postStepVolume = postStepTouchable->GetVolume();
    globalPostStepPosition = postStepPoint->GetPosition();
    localPostStepPosition  = 
      postStepTouchable->GetHistory()
      ->GetTopTransform().TransformPoint(globalPostStepPosition);
 
    //if(preStepProcess &&
    //    (preStepProcess->GetProcessName() == G4String("LowEnCompton") ||
    //     preStepProcess->GetProcessName() == G4String("LowEnPhotoElec") ||
    //     preStepProcess->GetProcessName() == G4String("LowEnConversion") ||
    //     preStepProcess->GetProcessName() == G4String("LowEnBrem"))) {
    //  globalPosition = globalPreStepPosition;
    //  localPosition  = localPreStepPosition;
    //}else{
    //  globalPosition = (globalPreStepPosition + globalPostStepPosition)/2.;
    //  localPosition  = (localPreStepPosition + localPostStepPosition)/2.;
    //}      
    
    //det = preStepTouchable->GetReplicaNumber(3);
    det = preStepTouchable->GetReplicaNumber(2);
    seg = preStepTouchable->GetReplicaNumber(0);
    if(seg>9){
      cry=1;
      seg-=11;
    }else{
      cry=0;
      seg-=1; // start counting from 0
    }
    //id = segmentID + detectorID * 20;
     
    string materialname = volume->GetName();
    //if(materialname=="World") cout << endl;
    //cout << materialname << endl;
    size_t contains = materialname.find("lSeg");
    
    if( ((Int_t)contains > -1) && (edep>0.000)){
    //if( ((Int_t)contains > -1) && (edep>0.15)){ // step limit 
      
      //printf("\npreStepTouchable->GetReplicaNumber %d %d %d %d\n", preStepTouchable->GetReplicaNumber(0), preStepTouchable->GetReplicaNumber(1), preStepTouchable->GetReplicaNumber(2), preStepTouchable->GetReplicaNumber(3));
      //printf("track in volume %s, edep, %f, det %d, cry %d, seg %d\n", volume->GetName().c_str(), edep, det, cry, seg);
 
            
      G4ParticleDefinition* thisDef = step->GetTrack()->GetDefinition(); 
      G4int pdgCode = thisDef->GetPDGEncoding();
      G4int parentID = step->GetTrack()->GetParentID();
      
      Bool_t newCrystal=false;
      if(fDetInfo->detData.grapeCryMul[det]>0 && TMath::IsNaN(fDetInfo->detData.grapeCryEnergy[det][cry])){newCrystal=true;}
      
      //if( !( (newCrystal && parentID>0) && (TMath::Abs(pdgCode)==11) ) ){ // exclude e- going from one crystal to the other
      if( !( (newCrystal && parentID==1) && (TMath::Abs(pdgCode)==11) ) ){ // exclude e- going from one crystal to the other
      
        if(TMath::IsNaN(fDetInfo->detData.grapeDetEnergy[det])){
          fDetInfo->detData.grapeDetEnergy[det]=0.0;
          fDetInfo->detData.grapeDetMul++;
        }
        if(TMath::IsNaN(fDetInfo->detData.grapeCryEnergy[det][cry])){
          fDetInfo->detData.grapeCryEnergy[det][cry]=0.0;
          fDetInfo->detData.grapeCryMul[det]++;
        }
        if(TMath::IsNaN(fDetInfo->detData.grapeSegEnergy[det][cry][seg])){
          fDetInfo->detData.grapeSegEnergy[det][cry][seg]=0.0;
          fDetInfo->detData.grapeSegMul[det][cry]++;
        }
        
        //fDetInfo->detData.grapeDetEnergy[det] += edep;
        //fDetInfo->detData.grapeCryEnergy[det][cry] += edep;
        fDetInfo->detData.grapeSegEnergy[det][cry][seg] += edep;

        //if(fDetInfo->detData.grapeSegEnergy[det][cry][seg]>0.1){
        //  for(Int_t c=0; c<2; c++){
        //  for(Int_t s=0; s<9; s++){
        //    

        //  }
        //  }
        //}
        
        //G4ParticleDefinition* thisDef = step->GetTrack()->GetDefinition(); 
        //G4int pdgCode = thisDef->GetPDGEncoding();
        //G4int parentID = step->GetTrack()->GetParentID();
        //
        ////if(fDetInfo->detData.grapeDetEnergy[0]>1.0 && fDetInfo->detData.grapeCryMul[det]>1) {
        //  printf("event %d: parent id is %d, pdg code is %d\n", eID, parentID, pdgCode);
        //  //printf("Grape: seg %d, cry %d, det %d, energy %f\n", seg, cry, det, edep);
        //  printf("egam det %f \n", fDetInfo->detData.grapeDetEnergy[0]);
        //  printf("mul det %d, cry %d\n", fDetInfo->detData.grapeDetMul, fDetInfo->detData.grapeCryMul[0]);
        ////}
      
      
      }



    }

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

