//
// $Id$
//

#include "EventAction.hh"

#include "RunAction.hh"
#include "SteppingAction.hh"
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

  outTree->Fill();



}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::Reset()
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
