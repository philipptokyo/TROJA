#ifndef Dali2_h
#define Dali2_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Sphere.hh"
#include "G4Polyhedra.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4UserLimits.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4NistManager.hh"

//#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "Randomize.hh"

#include "G4ios.hh"
#include <iostream>
#include "TMath.h"

#include "MaterialList.hh"
#include "DaliGlobals.hh"

#include "DetectorInfo.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class Dali2  {
public:
  
  //Dali2(MaterialList *ptMaterialList,G4LogicalVolume *ptWorld);
  Dali2(G4NistManager *ptMaterialList,G4LogicalVolume *ptWorld, DetectorInfo* di);
  ~Dali2();

  void   AddCrystalEnergy(int a,float b){fCrystalEnergy[a] = fCrystalEnergy[a] + b;}
  void   CreateArrayXYZPsiThetaPhi();
  void   DetermineCrystalMult(){for(int i=0;i<NUMBEROFDALI2CRYSTALS;i++){fCrystalMult = fCrystalMult + fCrystalFlag[i];}} 
         //pschrock: this detemines the number of crystals that fired
         
  float  GetCrystalEnergy(int a){return fCrystalEnergy[a];}
  bool   GetCrystalFlag(int a){return fCrystalFlag[a];}
  float  GetCrystalMeasuredEnergy(int a);
  float  GetCrystalMeasuredTime(int a);
  int    GetCrystalMult(){return fCrystalMult;}
  float  GetCrystalTime(int a){return fCrystalTime[a];}
  int    GetFIPointOption(){return fFIPointOption;}
  double GetFITime(int a){return fFITime[a];}
  float  GetFIX(int a){return fFIX[a];}
  float  GetFIY(int a){return fFIY[a];}
  float  GetFIZ(int a){return fFIZ[a];} 
  double GetPosXCrystal(int i){return fPosX[i];}
  double GetPosYCrystal(int i){return fPosY[i];}
  double GetPosZCrystal(int i){return fPosZ[i];}
  void   ResetValues();

  //void   SetAlAbsorberThickness(double thicknessAl){fAbsorberThicknessAl = 10.*thicknessAl*mm;}
  void   SetCrystalFlagTrue(int a){fCrystalFlag[a] = true;}
  void   SetCrystalTime(int a,float b){fCrystalTime[a] = b;}
  void   SetEnergyResolution(int a,float b,float c){fTypeOfEnergyResolution = a; fEnergyResolution[0] = b;fEnergyResolution[1] = c;}
  void   SetEnergyResolutionInd(int a);
  void   SetFIPointOption(int a){fFIPointOption = a;}
  void   SetFITime(int a, double b){fFITime[a] = b;}
  void   SetFIX(int a, float b){fFIX[a] = b;}
  void   SetFIY(int a, float b){fFIY[a] = b;}
  void   SetFIZ(int a, float b){fFIZ[a] = b;}
  //void   SetPbAbsorberThickness(double thicknessPb){fAbsorberThicknessPb = 10.*thicknessPb*mm;}
  //void   SetSnAbsorberThickness(double thicknessSn){fAbsorberThicknessSn = 10.*thicknessSn*mm;}
  void   SetTimeResolution(float a,float b){fTimeResolution[0] = a;fTimeResolution[1] = b;}
  void   SetZPosShift(float a){fZPosShift = a;}
private:

  DetectorInfo* fDetInfo;

  G4LogicalVolume*    lWorld;                    // Need a pointer to the logic world volume
  //MaterialList*       fMaterialList;             // Pointer to the material list
  G4NistManager*       fMaterialList;             // Pointer to the material list

  G4Box*              sDali2AlHouseOutSG;        // Saint-Gobain
  G4Box*              sDali2AlHouseInSG;
  G4SubtractionSolid* sDali2AlHouseSG;
  G4LogicalVolume*    lDali2AlHouseSG;

  G4Box*              sDali2MgOCoatOutSG;       
  G4Box*              sDali2MgOCoatInSG;        
  G4SubtractionSolid* sDali2MgOCoatSG;
  G4LogicalVolume*    lDali2MgOCoatSG;

  G4Box*              sDali2CrystalSG; 
  G4LogicalVolume*    lDali2CrystalSG;         
    
  G4Box*              sDali2AlHouseOutSC;        // Scionix
  G4Box*              sDali2AlHouseInSC;      
  G4SubtractionSolid* sDali2AlHouseSC;
  G4LogicalVolume*    lDali2AlHouseSC;

  G4Box*              sDali2MgOCoatOutSC;       
  G4Box*              sDali2MgOCoatInSC;        
  G4SubtractionSolid* sDali2MgOCoatSC;
  G4LogicalVolume*    lDali2MgOCoatSC;

  G4Box*              sDali2CrystalSC; 
  G4LogicalVolume*    lDali2CrystalSC;  
   
  G4VPhysicalVolume*  pDali2AlHouse[NUMBEROFDALI2CRYSTALS];
  G4VPhysicalVolume*  pDali2MgOCoat[NUMBEROFDALI2CRYSTALS];
  G4VPhysicalVolume*  pDali2Crystal[NUMBEROFDALI2CRYSTALS];

  // Including the PMT for optics               
  G4Tubs*             sPMT; 
  G4LogicalVolume*    lPMT; 
  G4VPhysicalVolume*  pPMT[NUMBEROFDALI2CRYSTALS]; 

  // In the new Dali2 array also some old dali1 detectors are used:
  G4Box*              sDali1AlHouseOut;       
  G4Box*              sDali1AlHouseIn;
  G4SubtractionSolid* sDali1AlHouse;
  G4LogicalVolume*    lDali1AlHouse;

  G4Box*              sDali1Crystal; 
  G4LogicalVolume*    lDali1Crystal;  
     
  //G4VPhysicalVolume*  pDali1AlHouse[12];
  //G4VPhysicalVolume*  pDali1MgOCoat[12];
  //G4VPhysicalVolume*  pDali1Crystal[12];

  //double fAbsorberThicknessAl; 
  //double fAbsorberThicknessPb; 
  //double fAbsorberThicknessSn;
  bool   fCrystalFlag[NUMBEROFDALI2CRYSTALS];
  float  fCrystalEnergy[NUMBEROFDALI2CRYSTALS];
  int    fCrystalMult;
  Float_t  fCrystalTime[NUMBEROFDALI2CRYSTALS];
  Float_t  fEnergyResolution[2];
  Float_t  fEnergyResolutionInd[2][NUMBEROFDALI2CRYSTALS];   // individuall resolution for every detector
  int    fIndEnergyResOpt;
  int    fFIPointOption;
  double fFITime[NUMBEROFDALI2CRYSTALS];
  float  fFIX[NUMBEROFDALI2CRYSTALS];
  float  fFIY[NUMBEROFDALI2CRYSTALS];
  float  fFIZ[NUMBEROFDALI2CRYSTALS];
  FILE*  fFileIn;
  FILE*  fFileOut;
  Double_t fPosX[NUMBEROFDALI2CRYSTALS];
  Double_t fPosY[NUMBEROFDALI2CRYSTALS];
  Double_t fPosZ[NUMBEROFDALI2CRYSTALS];
  char   fTempname[200];
  Int_t    fTypeOfEnergyResolution;
  Float_t  fTimeResolution[2];
  float  fZPosShift;
};
#endif
