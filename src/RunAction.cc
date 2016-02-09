//
// $Id$
//
//

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
  // use of other actions 
  // - primary generator: to get info for printing about the primary
  // - event action: to get and reset accumulated energy sums
  // - stepping action: to get info about accounting volume 

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

//for root file writing
#include "g4analysis_defs.hh"

#include "InputInfo.hh"

#include "TFile.h"
#include "TTree.h"

#include "DetectorGlobals.hh"
#include "DetectorInfo.hh"

using namespace G4Root;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(InputInfo* info, DetectorInfo* detInfo)
: G4UserRunAction()
{
  
  fDetInfo = detInfo;


  char tmpName[50];

  fOutFile = new TFile(info->fOutFileNameTroja, "recreate");
  fOutTree = new TTree("troja", "Transfer at OEDO in Japan, Geant4 simulation file");
  
  fOutTree->Branch("eventNumber", &(fDetInfo->detData.eventNumber), "eventNumber/I");
  fOutTree->Branch("FIx", &(fDetInfo->detData.fIX), "FIx/D");
  fOutTree->Branch("FIy", &(fDetInfo->detData.fIY), "FIy/D");
  fOutTree->Branch("FIz", &(fDetInfo->detData.fIZ), "FIz/D");
  

  fOutTree->Branch("FIdetID", &(fDetInfo->detData.detID), "FIdetID/I"); // ID of the detector with the first interaction point

  sprintf(tmpName, "energy[%i]/D", maxDetectors);
  fOutTree->Branch("energy", (fDetInfo->detData.energy), tmpName);

  sprintf(tmpName, "stripX[%i]/I", maxDetectors);
  fOutTree->Branch("stripX", (fDetInfo->detData.stripX), tmpName);

  sprintf(tmpName, "stripY[%i]/I", maxDetectors);
  fOutTree->Branch("stripY", (fDetInfo->detData.stripY), tmpName);



}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
  delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* aRun)
{ 
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
    
  //initialize event cumulative quantities
  EventAction::Instance()->Reset();
  
  
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* aRun)
{
  fDetInfo->detData.eventNumber = aRun->GetNumberOfEvent();

  if (fDetInfo->detData.eventNumber == 0) return;
  

  // Run conditions
  //
  const G4ParticleGun* particleGun 
    = PrimaryGeneratorAction::Instance()->GetParticleGun();
  G4String particleName 
    = particleGun->GetParticleDefinition()->GetParticleName();                       
  G4double particleEnergy = particleGun->GetParticleEnergy();
  
  fOutTree->Write("troja");   
  fOutFile->Close();
 
  // Print
  //  
  G4cout
     << "\n--------------------End of Run------------------------------\n"
     << " The run consists of " << fDetInfo->detData.eventNumber << " "<< particleName << " of "
     <<   G4BestUnit(particleEnergy,"Energy")      
     << "\n------------------------------------------------------------\n"
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
