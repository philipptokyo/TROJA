//
// $Id$
//
/// \file EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

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

    // get methods
    G4double GetEnergy1() const { return fEnergy1; }
    G4double GetEnergy2() const { return fEnergy2; }
    G4double GetX1() const { return fX1; }
    G4double GetY1() const { return fY1; }
    G4double GetZ1() const { return fZ1; }
     
     
  private:
    static EventAction* fgInstance;  

    G4int     fPrintModulo;
    G4double  fEnergy1;
    G4double  fEnergy2;
    G4double  fX1;
    G4double  fY1;
    G4double  fZ1;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
