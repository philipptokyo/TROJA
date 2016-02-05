//
// $Id$
//
/// \file SteppingAction.hh
/// \brief Definition of the SteppingAction class

#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "DetectorInfo.hh"

class G4LogicalVolume;

/// Stepping action class
/// 
/// It holds data member fEnergy for accumulating the energy deposit
/// in a selected volume step by step.
/// The selected volume is set from  the detector construction via the  
/// SetVolume() function. The accumulated energy deposit is reset for each 
/// new event via the Reset() function from the event action.

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction();
    SteppingAction(DetectorInfo* detInfo);
    virtual ~SteppingAction();

    // static access method
    static SteppingAction* Instance();

    // method from the base class
    virtual void UserSteppingAction(const G4Step*);

    // reset accumulated energy
    void Reset();

    // set methods
    void SetVolume(G4LogicalVolume* volume) { fVolume = volume; }
  
    // get methods
    G4LogicalVolume* GetVolume() const { return fVolume; }
    G4double GetEnergy1() const { return fEnergy1; }
    G4double GetEnergy2() const { return fEnergy2; }
    G4double GetX1() const { return fX1; }
    G4double GetY1() const { return fY1; }
    G4double GetZ1() const { return fZ1; }
    G4int    GetStripX() const { return fStripX; }
    G4int    GetStripY() const { return fStripY; }
   
  private:
    static SteppingAction* fgInstance;  
  
    G4LogicalVolume* fVolume;
    G4double  fEnergy1; //first Si
    G4double  fEnergy2; //second Si
    G4double  fX1; //first Si
    G4double  fY1; //first Si
    G4double  fZ1; //first Si
    G4int     fStripX; // SSD 1
    G4int     fStripY; // SSD 1

    DetectorInfo* fDetInfo;

    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
