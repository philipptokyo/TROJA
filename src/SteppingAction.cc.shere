//
// $Id$
//
/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class

#include "SteppingAction.hh"

#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

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
  fVolume(0),
  fEnergy1(0.),
  fEnergy2(0.),
  fX1(0.),
  fY1(0.),
  fZ1(0.)
{ 
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



  // collect energy and track length step by step
  // first Si
    //printf("volume name is %s\n",volume->GetName().c_str());
  if (std::strcmp(volume->GetName(), "Shape1")==0 ){
    //printf("in volume 1\n");
    G4double edep = step->GetTotalEnergyDeposit();
    fEnergy1 += edep;
    //printf("fEnergy1 = %f\n", fEnergy1);
    
    // check if the particle just entered Shape1
    // if so, get the position of the first interaction point
    if (point1->GetStepStatus() == fGeomBoundary) {
      //printf("FI ");
      G4ThreeVector pos1 = point1->GetPosition();
      fX1=pos1.getX();
      fY1=pos1.getY();
      fZ1=pos1.getZ();
      //printf("x=%f y=%f z=%f\n", fX1, fY1, fZ1);
    }
  }
  
  
  if (std::strcmp(volume->GetName(), "Shape2")==0 ){
    //printf("in volume 2\n");
    G4double edep = step->GetTotalEnergyDeposit();
    fEnergy2 += edep;
    //printf("fEnergy2 = %f\n", fEnergy2);
    
  }
   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::Reset()
{
  fEnergy1 = 0.;
  fEnergy2 = 0.;
  fX1 = 0.;
  fY1 = 0.;
  fZ1 = 0.;
}

