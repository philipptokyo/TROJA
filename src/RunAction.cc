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
//using namespace DetectorGlobals;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(InputInfo* info, DetectorInfo* detInfo)
: G4UserRunAction()
{
  
  fDetInfo = detInfo;

  // Create analysis manager
  //G4AnalysisManager* man = G4AnalysisManager::Instance();

  //man->OpenFile("troja.root");
  //man->OpenFile(info->GetOutfileNameString().c_str());
  //man->OpenFile(info->fOutFileNameTroja);

  char tmpName[50];

  fOutFile = new TFile(info->fOutFileNameTroja, "recreate");
  fOutTree = new TTree("troja", "Transfer at OEDO in Japan, Geant4 simulation file");
  
  fOutTree->Branch("eventNumber", &(fDetInfo->detData.eventNumber), "eventNumber/I");
  fOutTree->Branch("FIx", &(fDetInfo->detData.fIX), "FIx/D");
  fOutTree->Branch("FIy", &(fDetInfo->detData.fIY), "FIy/D");
  fOutTree->Branch("FIz", &(fDetInfo->detData.fIZ), "FIz/D");
  

  fOutTree->Branch("detID", &(fDetInfo->detData.detID), "detID/I");

  sprintf(tmpName, "energy[%i]/D", maxDetectors);
  fOutTree->Branch("energy", (fDetInfo->detData.energy), tmpName);

  sprintf(tmpName, "stripX[%i]/I", maxDetectors);
  fOutTree->Branch("stripX", (fDetInfo->detData.stripX), tmpName);

  sprintf(tmpName, "stripY[%i]/I", maxDetectors);
  fOutTree->Branch("stripY", (fDetInfo->detData.stripY), tmpName);



  // Create ntuple
//  man->CreateNtuple("troja", "sim outputs");
//  man->CreateNtupleIColumn("eventNumber");
//  man->CreateNtupleDColumn("energyLoss");
//  man->CreateNtupleDColumn("energyTotal");
//  man->CreateNtupleDColumn("x");
//  man->CreateNtupleDColumn("y");
//  man->CreateNtupleDColumn("z");
//  man->CreateNtupleDColumn("theta");
//  man->CreateNtupleDColumn("phi");
//  man->CreateNtupleIColumn("stripX");
//  man->CreateNtupleIColumn("stripY");
//  man->FinishNtuple();
  
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
  //G4int nofEvents = aRun->GetNumberOfEvent();
  fDetInfo->detData.eventNumber = aRun->GetNumberOfEvent();


  //if (nofEvents == 0) return;
  if (fDetInfo->detData.eventNumber == 0) return;
  

  // Run conditions
  //
  const G4ParticleGun* particleGun 
    = PrimaryGeneratorAction::Instance()->GetParticleGun();
  G4String particleName 
    = particleGun->GetParticleDefinition()->GetParticleName();                       
  G4double particleEnergy = particleGun->GetParticleEnergy();
  
  // Save histograms
//  G4AnalysisManager* man = G4AnalysisManager::Instance();
//  man->Write();
//  man->CloseFile();

  fOutTree->Write("troja");   
  fOutFile->Close();
 
  // Print
  //  
  G4cout
     << "\n--------------------End of Run------------------------------\n"
     //<< " The run consists of " << nofEvents << " "<< particleName << " of "
     << " The run consists of " << fDetInfo->detData.eventNumber << " "<< particleName << " of "
     <<   G4BestUnit(particleEnergy,"Energy")      
     << "\n------------------------------------------------------------\n"
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
