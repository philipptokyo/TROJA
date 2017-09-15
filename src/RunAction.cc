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

  fHeaderTree = new TTree("header","some detector globals, for instance");


  sprintf(tmpName, "daliPosX[%d]/D", NUMBEROFDALI2CRYSTALS);
  fHeaderTree->Branch("daliPosX", (fDetInfo->daliHead.fDaliPosX), tmpName);
  sprintf(tmpName, "daliPosY[%d]/D", NUMBEROFDALI2CRYSTALS);
  fHeaderTree->Branch("daliPosY", (fDetInfo->daliHead.fDaliPosY), tmpName);
  sprintf(tmpName, "daliPosZ[%d]/D", NUMBEROFDALI2CRYSTALS);
  fHeaderTree->Branch("daliPosZ", (fDetInfo->daliHead.fDaliPosZ), tmpName);
  sprintf(tmpName, "daliTheta[%d]/D", NUMBEROFDALI2CRYSTALS);
  fHeaderTree->Branch("daliTheta", (fDetInfo->daliHead.fDaliTheta), tmpName);
  sprintf(tmpName, "daliPhi[%d]/D", NUMBEROFDALI2CRYSTALS);
  fHeaderTree->Branch("daliPhi", (fDetInfo->daliHead.fDaliPhi), tmpName);
  sprintf(tmpName, "daliDistance[%d]/D", NUMBEROFDALI2CRYSTALS);
  fHeaderTree->Branch("daliDistance", (fDetInfo->daliHead.fDaliDistance), tmpName);
  sprintf(tmpName, "daliTimeResolution[%d]/F",2);
  fHeaderTree->Branch("daliTimeResolution", (fDetInfo->daliHead.fDaliTimeResolution), tmpName);
  sprintf(tmpName, "daliEnergyResolutionInd[%d][%d]/F", 2, NUMBEROFDALI2CRYSTALS);
  fHeaderTree->Branch("daliEnergyResolutionInd", (fDetInfo->daliHead.fDaliEnergyResolutionInd), tmpName);



  fOutTree = new TTree("troja", "Transfer at OEDO in Japan, Geant4 simulation file");
  
  fOutTree->Branch("eventNumber", &(fDetInfo->detData.eventNumber), "eventNumber/I");
  fOutTree->Branch("FIx", &(fDetInfo->detData.fIX), "FIx/D");
  fOutTree->Branch("FIy", &(fDetInfo->detData.fIY), "FIy/D");
  fOutTree->Branch("FIz", &(fDetInfo->detData.fIZ), "FIz/D");
  fOutTree->Branch("FItheta", &(fDetInfo->detData.fITheta), "FItheta/D");
  

  fOutTree->Branch("FIdetID", &(fDetInfo->detData.fIDetID), "FIdetID/I"); // ID of the detector with the first interaction point
  
  sprintf(tmpName, "detHit[%i]/I", maxDetectors);
  fOutTree->Branch("detHit", &(fDetInfo->detData.haveHit), tmpName); // flag which detectors were hit

  sprintf(tmpName, "detHitID[%i]/I", maxDetectors);
  fOutTree->Branch("detHitID", &(fDetInfo->detData.haveHitID), tmpName); // ID which detectors were hit, aux

  sprintf(tmpName, "energy[%i]/D", maxDetectors);
  fOutTree->Branch("energy", (fDetInfo->detData.energy), tmpName);

  sprintf(tmpName, "energyNotSmeared[%i]/D", maxDetectors);
  fOutTree->Branch("energyNotSmeared", (fDetInfo->detData.energyNotSmeared), tmpName);

  sprintf(tmpName, "stripX[%i]/I", maxDetectors);
  fOutTree->Branch("stripX", (fDetInfo->detData.stripX), tmpName);

  sprintf(tmpName, "stripY[%i]/I", maxDetectors);
  fOutTree->Branch("stripY", (fDetInfo->detData.stripY), tmpName);

//  sprintf(tmpName, "recoPosX[%i]/D", maxDetectors);
//  fOutTree->Branch("recoPosX", (fDetInfo->detData.hitPositionX), tmpName);
//
//  sprintf(tmpName, "recoPosY[%i]/D", maxDetectors);
//  fOutTree->Branch("recoPosY", (fDetInfo->detData.hitPositionY), tmpName);
//
//  sprintf(tmpName, "recoPosZ[%i]/D", maxDetectors);
//  fOutTree->Branch("recoPosZ", (fDetInfo->detData.hitPositionZ), tmpName);

  //fOutTree->Branch("grapeEnergy", &(fDetInfo->detData.grapeEnergy), "grapeEnergy/D"); // ID of the detector with the first interaction point
  //fOutTree->Branch("grapeDetector", &(fDetInfo->detData.grapeDet), "grapeDetector/I"); // ID of the detector with the first interaction point
  //fOutTree->Branch("grapeCrystal", &(fDetInfo->detData.grapeCrystal), "grapeCrystal/I"); // ID of the detector with the first interaction point
  //fOutTree->Branch("grapeSegment", &(fDetInfo->detData.grapeSegment), "grapeSegment/I"); // ID of the detector with the first interaction point

  if(detInfo->IncludeGrape()){
    fOutTree->Branch("grapeDetMul", &(fDetInfo->detData.grapeDetMul), "grapeDetMul/I"); // ID of the detector with the first interaction point

    sprintf(tmpName, "grapeCryMul[%d]/I", grapeMaxDet);
    fOutTree->Branch("grapeCryMul", (fDetInfo->detData.grapeCryMul), tmpName); // ID of the detector with the first interaction point
    sprintf(tmpName, "grapeSegMul[%d][%d]/I", grapeMaxDet, grapeMaxCry);
    fOutTree->Branch("grapeSegMul", (fDetInfo->detData.grapeSegMul), tmpName); // ID of the detector with the first interaction point
    
    sprintf(tmpName, "grapeDetEnergy[%d]/D", grapeMaxDet);
    fOutTree->Branch("grapeDetEnergy", (fDetInfo->detData.grapeDetEnergy), tmpName); // ID of the detector with the first interaction point
    sprintf(tmpName, "grapeCryEnergy[%d][%d]/D", grapeMaxDet, grapeMaxCry);
    fOutTree->Branch("grapeCryEnergy", (fDetInfo->detData.grapeCryEnergy), tmpName); // ID of the detector with the first interaction point
    sprintf(tmpName, "grapeSegEnergy[%d][%d][%d]/D", grapeMaxDet, grapeMaxCry, grapeMaxSeg);
    fOutTree->Branch("grapeSegEnergy", (fDetInfo->detData.grapeSegEnergy), tmpName); // ID of the detector with the first interaction point
  }

  if(detInfo->IncludeDali()){

    sprintf(tmpName, "DALI2Flag[%d]/O", NUMBEROFDALI2CRYSTALS);
    fOutTree->Branch("DALI2Flag", (fDetInfo->detData.fDaliCrystalFlag), tmpName);
    sprintf(tmpName, "DALI2EnergyNotCor[%d]/F", NUMBEROFDALI2CRYSTALS);
    fOutTree->Branch("DALI2EnergyNotCor", (fDetInfo->detData.fDaliCrystalEnergy), tmpName);
    sprintf(tmpName, "DALI2Mult/I");
    fOutTree->Branch("DALI2Mult", &(fDetInfo->detData.fDaliCrystalMult), tmpName);
    sprintf(tmpName, "DALI2Time[%d]/F", NUMBEROFDALI2CRYSTALS);
    fOutTree->Branch("DALI2Time", (fDetInfo->detData.fDaliCrystalTime), tmpName);
    sprintf(tmpName, "DALI2FITime[%d]/D", NUMBEROFDALI2CRYSTALS);
    fOutTree->Branch("DALI2FITime", (fDetInfo->detData.fDaliFITime), tmpName);
    sprintf(tmpName, "DALI2FIX[%d]/F", NUMBEROFDALI2CRYSTALS);
    fOutTree->Branch("DALI2FIX", (fDetInfo->detData.fDaliFIX), tmpName);
    sprintf(tmpName, "DALI2FIY[%d]/F", NUMBEROFDALI2CRYSTALS);
    fOutTree->Branch("DALI2FIY", (fDetInfo->detData.fDaliFIY), tmpName);
    sprintf(tmpName, "DALI2FIZ[%d]/F", NUMBEROFDALI2CRYSTALS);
    fOutTree->Branch("DALI2FIZ", (fDetInfo->detData.fDaliFIZ), tmpName);

  }

  fOutTree->Branch("targetEnergyLoss", &(fDetInfo->detData.targetEnergyLoss), "targetEnergyLoss/D"); 

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
  
  fHeaderTree->Fill();
  fHeaderTree->Write("header");

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
