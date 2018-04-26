//
// $Id$
//
/// \file EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "DetectorGlobals.hh"
#include "DetectorInfo.hh"

#include "TRandom3.h"

class SteppingAction;

/// Event action class
///

class EventAction : public G4UserEventAction
{
  public:
    EventAction();
    virtual ~EventAction();
    
    // static access method
    static EventAction* Instance();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void Reset();
    void ResetDali(DetectorInfo* detInfo);

     
  private:
    static EventAction* fgInstance;  

    G4int     fPrintModulo;
    TRandom3 *fRandomizer;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
