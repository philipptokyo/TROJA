//
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

//geant4.10
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "TFile.h"
#include "TH2F.h"

#include "InputInfo.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction* PrimaryGeneratorAction::fgInstance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const PrimaryGeneratorAction* PrimaryGeneratorAction::Instance()
{
// Static acces function via G4RunManager 

  return fgInstance;
}      

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(InputInfo* info)
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0)
{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);

  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName="proton");
  fParticleGun->SetParticleDefinition(particle);

  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));
  fParticleGun->SetParticleEnergy(10.0*MeV);
  
  fgInstance = this;

  // fRootInputFileName="132Snout.root";
  //fRootInputFileName = info->GetInfileNameString();
  fRootInputFileName = info->fOutFileNameMakeEvents;

  SetRootTreeInput();

}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  
  delete fHistKinematics;
  delete fRootInputFile; 

  delete fParticleGun;
  fgInstance = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void PrimaryGeneratorAction::SetRootTreeInput(){

  printf("Opening file %s\n", fRootInputFileName.c_str() );

  // get histogram from TFile
//  TFile* infile = TFile::Open("/home/philipp/programme/makeEvents/132Snout.root","write");
//  TFile* infile = TFile::Open("132Snout.root","write");
//  fRootInputFile = new TFile("132Snout.root","readonly");
  fRootInputFile = TFile::Open(fRootInputFileName.c_str(),"readonly");

  // obsolete?: events were sampled from histogram
//  fHistKinematics = (TH2F*)fRootInputFile->Get("histAll");
//  if(!fHistKinematics){
//    printf("PrimaryGeneratorAction: Histogram 'histAll' not found!\n");
//    abort();
//  }
//  else{
//    // printf("Found 'histAll' in '132Snout.root'!\n");
//  }
  
  fTree = (TTree*)fRootInputFile->Get("events");
  if(!fTree){
    printf("PrimaryGeneratorAction: Tree 'events' not found!\n");
    abort();
  }
  fTree->SetBranchAddress("eventNumber", &fEventNumber);
  fTree->SetBranchAddress("lightTheta", &fTheta);
  fTree->SetBranchAddress("lightPhi", &fPhi);
  fTree->SetBranchAddress("lightEnergy", &fEnergy);
  fTree->SetBranchAddress("lightPdgId", &fPdgId);
  fTree->SetBranchAddress("beamX", &fX0);
  fTree->SetBranchAddress("beamY", &fY0);
  fTree->SetBranchAddress("beamZ", &fZ0);

}



void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  
//  const double pi = 3.14159265358979323846;

//   //generate random direction
//   G4double x = G4UniformRand()-0.5; 
//   G4double y = G4UniformRand()-0.5; 
//   G4double z = G4UniformRand()-0.5; 
//   //printf("praticle direction x=%f y=%f z=%f\n",x,y,z);

  
  // get energy and theta from histogram
//  G4double en=0.0,th=0.0;

  //printf("\nGetting random event from histogram\n");
//  fHistKinematics->GetRandom2(th,en);
  //printf("Got energy %f, theta %f\n", en, th);
  
//  G4double ph=G4UniformRand()*pi;


  // getting events from root tree
  // printf("G4Event ID = %i,  Int_t = %i\n", anEvent->GetEventID(), (Int_t)anEvent->GetEventID());
  Int_t evntNrMax = fTree->GetEntries();
  Int_t evntNr = -1;
  evntNr = (Int_t)anEvent->GetEventID();
  if(evntNr>evntNrMax || evntNr<0){
    printf("Invalid eventNumber '%i'\n", evntNr);
    printf("Maximum event number in tree is only %i\n", evntNrMax);
    abort();
  }
  else{
    fTree->GetEvent(evntNr);
    //printf("Got EventNumber %i, energy %lf, theta %lf,  phi %lf\n", fEventNumber, fEnergy, fTheta, fPhi);
  }


  G4ThreeVector direction(0.0, 0.0, -1.0);
  direction.setRhoPhiTheta(1.0, fPhi, fTheta);
  //direction.setTheta(fTheta);
  //direction.setPhi(fPhi);

  //G4ThreeVector position(0.0, 0.0, 0.0);
  G4ThreeVector position(fX0*mm, fY0*mm, fZ0*mm);
  //printf("%f %f %f\n", position.getX(), position.getY(), position.getZ());



  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4int particleId=(G4int)fPdgId;
  G4ParticleDefinition* particle = particleTable->FindParticle(particleId);
  fParticleGun->SetParticleDefinition(particle);


//  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(G4UniformRand(),G4UniformRand(),G4UniformRand()));
//  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(x, y, z));
  fParticleGun->SetParticleMomentumDirection(direction);

  fParticleGun->SetParticleEnergy(fEnergy*MeV);
  //fParticleGun->SetParticlePosition(G4ThreeVector(0,0,0));
  fParticleGun->SetParticlePosition(position);

  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

