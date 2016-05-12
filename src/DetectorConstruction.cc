//
// $Id$
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "DetectorInfo.hh"
#include "SteppingAction.hh"
   // use of stepping action to set the accounting volume

#include "DetectorGlobals.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"


#include "G4MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4TransportationManager.hh"
#include "G4PropagatorInField.hh"
#include "G4UniformMagField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4ClassicalRK4.hh"




#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trap.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Polyhedron.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4UserLimits.hh"
#include "G4NistManager.hh"
//#include "globals.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RunManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"



// using namespace CLHEP

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction()
{ 
  fDetInfo = new DetectorInfo();  
  printf("WARNING: Empty detector information in DetectorConstruction.cc!!!\n");
}

DetectorConstruction::DetectorConstruction(DetectorInfo* detInfo)
: G4VUserDetectorConstruction()
{ 
  fDetInfo=detInfo;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  
   
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //     
  // World
  //
  G4double world_sizeXY = 1000*mm;
  G4double world_sizeZ  = 1000*mm;
//  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_Galactic");
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name
                                   
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
 
  

                    

  //G4Material* fLead    = nist->FindOrBuildMaterial("G4_Pb"); 
  //G4Material* fIron    = nist->FindOrBuildMaterial("G4_Fe");
  G4Material* fCarbon    = nist->FindOrBuildMaterial("G4_C");



  //------------------- Target-----------------------------
  
  G4ThreeVector fTargetPos(0,0,0);

  G4Box* solidTarget = new G4Box("tgt_box",40*0.5*mm,40*0.5*mm,0.00100*0.5*mm);  //1um

  G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget, fCarbon, "target_log");

//  new G4PVPlacement(0, fTargetPos, logicTarget, "Target", logicWorld, false, 0);

  logicTarget->SetVisAttributes(new G4VisAttributes(G4Colour::Red()));
  





  //----------- Silicon Strip and CsI Detectors -------------------


  G4Material* fSilicon = nist->FindOrBuildMaterial("G4_Si"); 
  printf("Material G4_Si has density %f g/cm3\n", fSilicon->GetDensity()/g*cm3);

  G4Element* Cs = new G4Element("Cesium", "Cs", 55., 132.9*g/mole);
  G4Element* I = new G4Element("Iodine", "I", 53., 126.9*g/mole);
  G4Material* fCsI = new G4Material("CsI", 4.51*g/cm3, 2);
  fCsI->AddElement(I, .5);
  fCsI->AddElement(Cs, .5);
  

  G4int noOfDet = fDetInfo->GetNoOfDetectors();
  
  G4ThreeVector pos[maxDetectors]; // from detector globals
  G4RotationMatrix* rotMat[maxDetectors];  
  G4double size[maxDetectors][5] = {{0.0}};
  G4Box* box[maxDetectors];
  G4Tubs* tube[maxDetectors];
  G4LogicalVolume* logical[maxDetectors];
  
  
  for(G4int d=0; d<noOfDet; d++){
    pos[d].set(fDetInfo->GetCenterX(d), fDetInfo->GetCenterY(d), fDetInfo->GetCenterZ(d));
    
    size[d][0]     = fDetInfo->GetSize0(d)*0.5; // half width
    size[d][1]     = fDetInfo->GetSize1(d)*0.5; // half length 
    size[d][2]     = fDetInfo->GetSize2(d)*0.5; // half thickness
    size[d][3]     = fDetInfo->GetSize3(d); // phi_start
    size[d][4]     = fDetInfo->GetSize4(d); // phi_detector

    rotMat[d] = new G4RotationMatrix();


    char tmpName[50];

    //sprintf(tmpName, "box%02d", d);
    //box[d] = new G4Box(tmpName, size[d][0], size[d][1], size[d][2]);
    //sprintf(tmpName, "tube%02d", d);
    //tube[d] = new G4Tubs(tmpName, size[d][0], size[d][1], size[d][2], size[d][3], size[d][4]);

    if( (strcmp(fDetInfo->GetType(d).data(), "DSSDbox")==0) || (strcmp(fDetInfo->GetType(d).data(), "CsIbox")==0) ){

      rotMat[d]->rotateX(fDetInfo->GetRotationX(d));
      rotMat[d]->rotateY(fDetInfo->GetRotationY(d));
      rotMat[d]->rotateZ(fDetInfo->GetRotationZ(d));
      //rotMat[d]->rotate(fDetInfo->GetRotationX(d), G4ThreeVector(1.0, 0.0, 0.0));
      //rotMat[d]->rotate(fDetInfo->GetRotationY(d), G4ThreeVector(0.0, 1.0, 0.0));
      //rotMat[d]->rotate(fDetInfo->GetRotationZ(d), G4ThreeVector(0.0, 0.0, 1.0));

      sprintf(tmpName, "box%02d", d);
      box[d] = new G4Box(tmpName, size[d][0], size[d][1], size[d][2]);
      
      sprintf(tmpName, "logical%02d", d);
      if((strcmp(fDetInfo->GetType(d).data(), "DSSDbox")==0)){
        logical[d] = new G4LogicalVolume(box[d], fSilicon, tmpName);
      }else if(strcmp(fDetInfo->GetType(d).data(), "CsIbox")==0){ // this if is redundant, ne
        logical[d] = new G4LogicalVolume(box[d], fCsI, tmpName);
      }

    } else if( strcmp(fDetInfo->GetType(d).data(), "DSSDtube")==0 ){

  //    G4ThreeVector vCen(1.0, 0.0, 0.0); // vector through center of detector
      G4ThreeVector vRot(1.0, 0.0, 0.0); // rotation vector
  
      vRot.rotateZ(size[d][3] + size[d][4]/2.0);
      vRot.rotateZ(-fDetInfo->GetRotationY(d));
      vRot.rotateZ(90*deg);
  
      //vCen.rotate(-fDetInfo->GetRotationX(d), vRot);
      //vCen.rotateY(-fDetInfo->GetRotationX(d));
      //vCen.rotateZ(fDetInfo->GetRotationY(d));
      //vCen.rotateZ( (size[d][3] + size[d][4]/2.0));
  
      
      rotMat[d]->rotateZ(size[d][3] + size[d][4]/2.0);
      rotMat[d]->rotate(fDetInfo->GetRotationX(d), vRot); // theta rotation
      rotMat[d]->rotateZ(fDetInfo->GetRotationY(d));
      //rotMat[d]->rotate(fDetInfo->GetRotationY(d), G4ThreeVector(0.0, 0.0, 1.0));      // phi rotation
      //rotMat[d]->rotate(fDetInfo->GetRotationX(d), vRot); // theta rotation
      //rotMat[d]->rotate(fDetInfo->GetRotationY(d), G4ThreeVector(0.0, 0.0, 1.0));      // phi rotation
      //rotMat[d]->rotate(fDetInfo->GetRotationZ(d), vCen); // around detectors axis
      
      size[d][0] *= 2.0; // radii are given in full length
      size[d][1] *= 2.0;       
      sprintf(tmpName, "tube%02d", d);
      tube[d] = new G4Tubs(tmpName, size[d][0], size[d][1], size[d][2], size[d][3], size[d][4]);
      
      sprintf(tmpName, "logical%02d", d);
      logical[d] = new G4LogicalVolume(tube[d], fSilicon, tmpName);

    } else {
      printf("Error in detector construction: unknown type '%s' (id %d)\n", fDetInfo->GetType(d).data(), d);
      abort();
    }

    sprintf(tmpName, "%02d%s", d, fDetInfo->GetName(d).data());
    new G4PVPlacement(rotMat[d], 
                      pos[d],
                      logical[d],
                      tmpName,
                      logicWorld,
                      false,
                      0,
                      checkOverlaps);
  }





  // Set scoring volume to stepping action 
  // (where we will account energy deposit)
  //
  SteppingAction* steppingAction = SteppingAction::Instance(); 
  
  for(G4int d=0; d<noOfDet; d++){
    steppingAction->SetVolume(logical[d]);
  }

  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
