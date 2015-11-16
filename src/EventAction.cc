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
  fPrintModulo(10000),
  fEnergy1(0.),
  fEnergy2(0.),
  fX1(0.),
  fY1(0.),
  fZ1(0.)
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

void EventAction::EndOfEventAction(const G4Event* /*event*/)
{
  // get energy and position information
  fEnergy1  = SteppingAction::Instance()->GetEnergy1();

  fEnergy2  = SteppingAction::Instance()->GetEnergy2();
  
  fX1  = SteppingAction::Instance()->GetX1();
  fY1  = SteppingAction::Instance()->GetY1();
  fZ1  = SteppingAction::Instance()->GetZ1();
  
  //calculate theta and phi
  //at this point the target position needs to be included, 
  //if the particles do not start at origin
  G4ThreeVector v(fX1, fY1, fZ1); 
  G4double th = v.getTheta(); 
  G4double ph = v.getPhi(); 
 
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->FillNtupleDColumn(0, fEnergy1);
  man->FillNtupleDColumn(1, fEnergy2);
  man->FillNtupleDColumn(2, fX1);
  man->FillNtupleDColumn(3, fY1);
  man->FillNtupleDColumn(4, fZ1);
  man->FillNtupleDColumn(5, th/3.1415926535*180.0); //in deg
  man->FillNtupleDColumn(6, ph);

  man->AddNtupleRow();  
  
  //printf("Got: Eloss=%f Etot=%f x=%f y=%f z=%f\n", fEnergy1, fEnergy2, fX1, fY1, fZ1);  
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::Reset()
{
  //reset cumulative quantities
  //
  fEnergy1 = 0.;
  fEnergy2 = 0.;
  fX1 = 0.;
  fY1 = 0.;
  fZ1 = 0.;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
