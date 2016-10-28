//
// $Id$
//

#include "EventAction.hh"

#include "RunAction.hh"
#include "SteppingAction.hh"
#include "DetectorInfo.hh"
// use of stepping action to get and reset accumulated energy  

#include "PrimaryGeneratorAction.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"

// for root file writing
#include "g4analysis_defs.hh"

#include "DetectorGlobals.hh"

#include "TMath.h"

using namespace G4Root;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction* EventAction::fgInstance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction* EventAction::Instance()
{
// Static acces function via G4RunManager 

  return fgInstance;
}      

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
: G4UserEventAction(),
  fPrintModulo(100000)
{ 
  fgInstance = this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{ 
  fgInstance = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* event)
{  
  G4int eventNb = event->GetEventID();
  if (eventNb%fPrintModulo == 0) { 
    G4cout << "\n---> Begin of event: " << eventNb << G4endl;
  }
 
  // Reset accounted energy in stepping action
  SteppingAction::Instance()->Reset();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
  // get energy and position information
//  fEnergy1  = SteppingAction::Instance()->GetEnergy1();
//
//  fEnergy2  = SteppingAction::Instance()->GetEnergy2();
//  
//  fX1  = SteppingAction::Instance()->GetX1();
//  fY1  = SteppingAction::Instance()->GetY1();
//  fZ1  = SteppingAction::Instance()->GetZ1();
//  
//  fStripX = SteppingAction::Instance()->GetStripX();
//  fStripY = SteppingAction::Instance()->GetStripY();
  
  //calculate theta and phi
  //at this point the target position needs to be included, 
  //if the particles do not start at origin
//  G4ThreeVector v(fX1, fY1, fZ1); 
//  G4double fTh = v.getTheta(); 
//  G4double fPh = v.getPhi();
//  fEvntNr = event->GetEventID(); 
  
  
  G4int evntNoGen = PrimaryGeneratorAction::Instance()->GetEventNumber();
  G4int evntNoSim = event->GetEventID(); 
  
  if(evntNoGen != evntNoSim){
    printf("ERROR: Mismatch in event numbers! Generator %d, Simulation %d! Please check (EventAction)...!\n", evntNoGen, evntNoSim);
    abort();
  }
  //printf("Got: Eloss=%f Etot=%f x=%f y=%f z=%f\n", fEnergy1, fEnergy2, fX1, fY1, fZ1);  

  RunAction *runact = (RunAction*)G4RunManager::GetRunManager()->GetUserRunAction(); 
  TTree* outTree = (TTree*)runact->GetOutTree();
  
  DetectorInfo* detInfo = runact->GetDetInfo();
  for(G4int d=0; d<maxDetectors; d++){
    detInfo->detData.energy[d] = detInfo->SmearEnergy(d, detInfo->detData.energyNotSmeared[d]);
  }
  





  //Bool_t printRes=false;
  //if(detInfo->detData.grapeDetMul>0){
  //  printf("\nHave before cuts:\n");
  //  for(Int_t d=0; d<1; d++){
  //    printf("Det mul %d, energy %f, ", detInfo->detData.grapeDetMul, detInfo->detData.grapeDetEnergy[d]);  
  //    printf("Cry mul %d:\n", detInfo->detData.grapeCryMul[d]);
  //    for(Int_t c=0; c<2; c++){
  //      printf("Cry energy %f, seg mul %d: \n", detInfo->detData.grapeCryEnergy[d][c], detInfo->detData.grapeSegMul[d][c]);
  //      for(Int_t s=0; s<9; s++){
  //        printf("seg %d en %f, ", s, detInfo->detData.grapeSegEnergy[d][c][s]);
  //      }
  //      printf("\n");
  //    }
  //  }
  //  printRes=true;
  //}

  Double_t threshold[18][2][9]={{{0.0}}};

  Double_t th0[9]={0.084, 0.092, 0.090, 
                   0.091, 0.090, 0.092, 
                   0.092, 0.087, 0.092};
  Double_t th1[9]={0.064, 0.068, 0.063,
                   0.070, 0.062, 0.068,
                   0.069, 0.065, 0.066};

  for(Int_t s=0; s<9; s++){
    threshold[0][0][s]=th0[s];
    threshold[0][1][s]=th1[s];
  }


  for(Int_t d=0; d<1; d++){
    if(detInfo->detData.grapeDetMul>0){
      Double_t sumEnDet=0.0;
      for(Int_t c=0; c<2; c++){
        if(detInfo->detData.grapeCryMul[d]>0){
          Double_t sumEnCry=0.0;
          for(Int_t s=0; s<9; s++){
            if(detInfo->detData.grapeSegEnergy[d][c][s]>threshold[d][c][s]){ // apply threshold
              sumEnCry+=detInfo->detData.grapeSegEnergy[d][c][s];
            }else{
              if(!TMath::IsNaN(detInfo->detData.grapeSegEnergy[d][c][s])){
                detInfo->detData.grapeSegEnergy[d][c][s]=0.0;
                detInfo->detData.grapeSegMul[d][c]--;
              }
            }
          }
          if(sumEnCry>0.0){
            detInfo->detData.grapeCryEnergy[d][c]=sumEnCry;
            sumEnDet+=sumEnCry;
          }else{
            if(!TMath::IsNaN(detInfo->detData.grapeCryEnergy[d][c])){
              detInfo->detData.grapeCryMul[d]--;
            }
          }
        }
      }
      if(sumEnDet>0.0){
        detInfo->detData.grapeDetEnergy[d]=sumEnDet;
      }else{
        if(!TMath::IsNaN(detInfo->detData.grapeDetEnergy[d])){
          detInfo->detData.grapeDetMul--;
        }
      }
    }
  }

  //if(printRes){
  //  printf("Have after cuts:\n");
  //  for(Int_t d=0; d<1; d++){
  //    printf("Det mul %d, energy %f, ", detInfo->detData.grapeDetMul, detInfo->detData.grapeDetEnergy[d]);  
  //    printf("Cry mul %d:\n", detInfo->detData.grapeCryMul[d]);
  //    for(Int_t c=0; c<2; c++){
  //      printf("Cry energy %f, seg mul %d: \n", detInfo->detData.grapeCryEnergy[d][c], detInfo->detData.grapeSegMul[d][c]);
  //      for(Int_t s=0; s<9; s++){
  //        printf("seg %d en %f, ", s, detInfo->detData.grapeSegEnergy[d][c][s]);
  //      }
  //      printf("\n");
  //    }
  //  }
  //}

  outTree->Fill();



}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::Reset()
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
