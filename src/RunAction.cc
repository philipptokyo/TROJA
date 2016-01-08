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

using namespace G4Root;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(InputInfo* info)
: G4UserRunAction()
{
  // Create analysis manager
  G4AnalysisManager* man = G4AnalysisManager::Instance();

  //man->OpenFile("troja.root");
  //man->OpenFile(info->GetOutfileNameString().c_str());
  man->OpenFile(info->fOutFileNameTroja);
  
  // Create ntuple
  man->CreateNtuple("troja", "sim outputs");
  man->CreateNtupleDColumn("eventNumber");
  man->CreateNtupleDColumn("energyLoss");
  man->CreateNtupleDColumn("energyTotal");
  man->CreateNtupleDColumn("x");
  man->CreateNtupleDColumn("y");
  man->CreateNtupleDColumn("z");
  man->CreateNtupleDColumn("theta");
  man->CreateNtupleDColumn("phi");
  man->FinishNtuple();
  
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
  G4int nofEvents = aRun->GetNumberOfEvent();


  if (nofEvents == 0) return;
  

  // Run conditions
  //
  const G4ParticleGun* particleGun 
    = PrimaryGeneratorAction::Instance()->GetParticleGun();
  G4String particleName 
    = particleGun->GetParticleDefinition()->GetParticleName();                       
  G4double particleEnergy = particleGun->GetParticleEnergy();
  
  // Save histograms
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->Write();
  man->CloseFile();
   
 
  // Print
  //  
  G4cout
     << "\n--------------------End of Run------------------------------\n"
     << " The run consists of " << nofEvents << " "<< particleName << " of "
     <<   G4BestUnit(particleEnergy,"Energy")      
     << "\n------------------------------------------------------------\n"
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
