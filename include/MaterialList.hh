#ifndef MaterialList_h
#define MaterialList_h 1

#include "G4ios.hh"
#include <iostream>
#include "G4Material.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class MaterialList  {
 public:
  
   MaterialList();
  ~MaterialList();

   G4Material *GetMaterial(G4String materialName);
 private:
  // All defined elements:
  G4Element* elH,*elHe,*elBe,*elC,*elN,*elO,*elF,*elNa,*elMg,*elFe,*elNi,*elBr,*elI,*elCs,*elBa,*elLa,*elW;
    
  //All defined materials:
  G4Material* H2O,*Air,*vacuum,*He,*LHe,*Be,*C,*Al,*Si,*Fe,*Ge,*Zr,*Sn,*Au,*Pb,*Sci,*MgO,*BaF2,*NaI,*CsI,*LaBr,*LH2, *Delrin, *DENSIMET, *CH2;

 public: 
  //Set the material composition of the collimator block:
  void SetCollimatorMaterial(float composition[],float density);

};
#endif
