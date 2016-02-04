//
// $Id$
//
/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "DetectorInfo.hh"

class G4VPhysicalVolume;

/// Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    DetectorConstruction(DetectorInfo* detInfo);
    virtual ~DetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();
    G4VPhysicalVolume* Construct(DetectorInfo* detInfo);

    DetectorInfo* GetDetectorInfo() {return fDetInfo;}

  private:
    DetectorInfo* fDetInfo;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

