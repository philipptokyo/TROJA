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
    TTree* GetHeaderTree() {return fHeaderTree;}

    DetectorInfo* GetDetInfo() {return fDetInfo;} 


  private:
    
    DetectorInfo* fDetInfo;

    TFile* fOutFile;
    TTree* fOutTree;
    TTree* fHeaderTree;


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

