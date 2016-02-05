//
// $Id$
//

#include "EventAction.hh"

#include "RunAction.hh"
#include "SteppingAction.hh"
  // use of stepping action to get and reset accumulated energy  

#include "G4RunManager.hh"
#include "G4Event.hh"

// for root file writing
#include "g4analysis_defs.hh"

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
  fPrintModulo(100000),
  fEvntNr(0),
  fEnergy1(0.),
  fEnergy2(0.),
  fX1(0.),
  fY1(0.),
  fZ1(0.),
  fStripX(-1),
  fStripY(-1)
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
  fEnergy1  = SteppingAction::Instance()->GetEnergy1();

  fEnergy2  = SteppingAction::Instance()->GetEnergy2();
  
  fX1  = SteppingAction::Instance()->GetX1();
  fY1  = SteppingAction::Instance()->GetY1();
  fZ1  = SteppingAction::Instance()->GetZ1();
  
  fStripX = SteppingAction::Instance()->GetStripX();
  fStripY = SteppingAction::Instance()->GetStripY();
  
  //calculate theta and phi
  //at this point the target position needs to be included, 
  //if the particles do not start at origin
  G4ThreeVector v(fX1, fY1, fZ1); 
  G4double fTh = v.getTheta(); 
  G4double fPh = v.getPhi();
  fEvntNr = event->GetEventID(); 
 
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->FillNtupleIColumn(0, fEvntNr);
  man->FillNtupleDColumn(1, fEnergy1);
  man->FillNtupleDColumn(2, fEnergy2);
  man->FillNtupleDColumn(3, fX1);
  man->FillNtupleDColumn(4, fY1);
  man->FillNtupleDColumn(5, fZ1);
  //man->FillNtupleDColumn(6, fTh); //in deg
  man->FillNtupleDColumn(6, fTh/TMath::Pi()*180.0); //in deg
  man->FillNtupleDColumn(7, fPh);
  man->FillNtupleIColumn(8, fStripX);
  man->FillNtupleIColumn(9, fStripY);

  man->AddNtupleRow();  
  
  //printf("Got: Eloss=%f Etot=%f x=%f y=%f z=%f\n", fEnergy1, fEnergy2, fX1, fY1, fZ1);  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::Reset()
{
  //reset cumulative quantities
  //
  fEnergy1 = NAN;
  fEnergy2 = NAN;
  fX1 = NAN;
  fY1 = NAN;
  fZ1 = NAN;
  fStripX = -1;
  fStripY = -1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
