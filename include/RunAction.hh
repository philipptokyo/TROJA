//
// $Id$
//
/// \file RunAction.hh
/// \brief Definition of the RunAction class

#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

#include "InputInfo.hh"
#include "DetectorInfo.hh"
#include "DetectorGlobals.hh"

//#include "TFile.h"
//#include "TTree.h"
#include "../LibPerso.h"

class G4Run;

/// Run action class
///

class RunAction : public G4UserRunAction
{
  public:
    // RunAction();
    RunAction(InputInfo* info, DetectorInfo* detInfo);
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

    TFile* GetOutFile() {return fOutFile;}
    TTree* GetOutTree() {return fOutTree;}

    DetectorInfo* GetDetInfo() {return fDetInfo;} 


  private:
    
//    G4int fEventNumber;
//    G4double fFIX, fFIY, fFIZ; // first interaction point, x, y, z
//
//    G4int fNoOfDet[maxDetectors];
//    G4int fDetID[maxDetectors];
//
//    G4double fEnergy[maxDetectors];
//    G4int fStripX[maxDetectors];
//    G4int fStripY[maxDetectors];

    DetectorInfo* fDetInfo;

    TFile* fOutFile;
    TTree* fOutTree;


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

