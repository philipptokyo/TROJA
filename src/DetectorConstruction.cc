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
  G4double world_sizeXY = 100*cm;
  G4double world_sizeZ  = 100*cm;
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
 
  

                    
//  //     
//  // first layer of thin Si for dE / x,y,z measurements 
//  //
//  
//  G4Material* shape1_mat = nist->FindOrBuildMaterial("G4_Si");
//  G4ThreeVector pos1 = G4ThreeVector(0.0*cm, 0.0*cm, 0.0*cm);
//
//  // Sphere shape
//  G4double shape1_rmin = 10.0*cm, shape1_rmax = 10.01*cm;  //100 mum full-size Si???
//  G4double shape1_thetamin = 0.*deg, shape1_thetamax =  180.*deg;    
//  G4double shape1_phimin = 0.*deg, shape1_phimax =  360.*deg;    
//  G4Sphere* solidShape1 =    
//    new G4Sphere("Shape1",                  //its name
//        shape1_rmin, shape1_rmax,                //its size
//        shape1_phimin, shape1_phimax,            //phi angle
//        shape1_thetamin, shape1_thetamax);       //theta angle
//     
//                      
//  G4LogicalVolume* logicShape1 =                         
//    new G4LogicalVolume(solidShape1,         //its solid
//                        shape1_mat,          //its material
//                        "Shape1");           //its name
//               
//  new G4PVPlacement(0,                       //no rotation
//                    pos1,                    //at position
//                    logicShape1,             //its logical volume
//                    "Shape1",                //its name
//                    logicWorld,                //its mother  volume
//                    false,                   //no boolean operation
//                    0,                       //copy number
//                    checkOverlaps);          //overlaps checking
//
//
//


//  //     
//  // second layer of thick Si for E measurement 
//  //
//  
//  G4Material* shape2_mat = nist->FindOrBuildMaterial("G4_Si");
//  G4ThreeVector pos2 = G4ThreeVector(0.0*cm, 0.0*cm, 0.0*cm);
//
//  // Sphere shape
////  G4double shape2_rmin = 10.50*cm, shape2_rmax = 10.6*cm;  //1 mm full-size Si??
//  G4double shape2_rmin = 15.50*cm, shape2_rmax = 17.5*cm;  //10 mm full-size Si??
//  G4double shape2_thetamin = 0.*deg, shape2_thetamax =  180.*deg;    
//  G4double shape2_phimin = 0.*deg, shape2_phimax =  360.*deg;    
//  G4Sphere* solidShape2 =    
//    new G4Sphere("Shape2",                  //its name
//        shape2_rmin, shape2_rmax,                //its size
//        shape2_phimin, shape2_phimax,            //phi angle
//        shape2_thetamin, shape2_thetamax);       //theta angle
//     
//                      
//  G4LogicalVolume* logicShape2 =                         
//    new G4LogicalVolume(solidShape2,         //its solid
//                        shape2_mat,          //its material
//                        "Shape2");           //its name
//               
//  new G4PVPlacement(0,                       //no rotation
//                    pos2,                    //at position
//                    logicShape2,             //its logical volume
//                    "Shape2",                //its name
//                    logicWorld,              //its mother  volume
//                    false,                   //no boolean operation
//                    0,                       //copy number
//                    checkOverlaps);          //overlaps checking











  //G4Material* fLead    = nist->FindOrBuildMaterial("G4_Pb"); 
  //G4Material* fIron    = nist->FindOrBuildMaterial("G4_Fe");
  G4Material* fCarbon    = nist->FindOrBuildMaterial("G4_C");



  //------------------- Target-----------------------------
  
  G4ThreeVector fTargetPos(0,0,0);

  G4Box* solidTarget = new G4Box("tgt_box",40*0.5*mm,40*0.5*mm,0.00100*0.5*mm);  //1um

  G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget, fCarbon, "target_log");

  new G4PVPlacement(0, fTargetPos, logicTarget, "Target", logicWorld, false, 0);

  logicTarget->SetVisAttributes(new G4VisAttributes(G4Colour::Red()));
  





  //----------- Silicon Strip Detectors -------------------


  G4Material* fSilicon = nist->FindOrBuildMaterial("G4_Si"); 
  

  G4int noOfDet = fDetInfo->GetNoOfDetectors();
  
  G4ThreeVector pos[maxDetectors]; // from detector globals
  G4RotationMatrix* rotMat[maxDetectors];  
  G4double size[maxDetectors][5] = {{0.0}};
  G4Box* box[maxDetectors];
  G4Tubs* tube[maxDetectors];
  G4LogicalVolume* logical[maxDetectors];
  
  
  for(G4int d=0; d<noOfDet; d++){
    pos[d].set(fDetInfo->GetCenterX(d), fDetInfo->GetCenterY(d), fDetInfo->GetCenterZ(d));
    
    rotMat[d] = new G4RotationMatrix();
    rotMat[d]->rotateX(fDetInfo->GetRotationX(d));
    rotMat[d]->rotateY(fDetInfo->GetRotationY(d));
    rotMat[d]->rotateZ(fDetInfo->GetRotationZ(d));

    size[d][0]     = fDetInfo->GetSize0(d)*0.5; // half width
    size[d][1]     = fDetInfo->GetSize1(d)*0.5; // half length 
    size[d][2]     = fDetInfo->GetSize2(d)*0.5; // half thickness
    size[d][3]     = fDetInfo->GetSize3(d); // phi_start
    size[d][4]     = fDetInfo->GetSize4(d); // phi_detector

    char tmpName[50];

    //sprintf(tmpName, "box%02d", d);
    //box[d] = new G4Box(tmpName, size[d][0], size[d][1], size[d][2]);
    //sprintf(tmpName, "tube%02d", d);
    //tube[d] = new G4Tubs(tmpName, size[d][0], size[d][1], size[d][2], size[d][3], size[d][4]);

    if( strcmp(fDetInfo->GetType(d).data(), "DSSDbox")==0 ){
      
      sprintf(tmpName, "box%02d", d);
      box[d] = new G4Box(tmpName, size[d][0], size[d][1], size[d][2]);
      
      sprintf(tmpName, "logical%02d", d);
      logical[d] = new G4LogicalVolume(box[d], fSilicon, tmpName);

    } else if( strcmp(fDetInfo->GetType(d).data(), "DSSDtube")==0 ){
      
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
  //steppingAction->SetVolume(logicShape1);
  //steppingAction->SetVolume(logicShape2);
  
  for(G4int d=0; d<noOfDet; d++){
    steppingAction->SetVolume(logical[d]);
  }

  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
