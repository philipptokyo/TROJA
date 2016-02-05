//
// $Id$
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "DetectorInfo.hh"
#include "SteppingAction.hh"
   // use of stepping action to set the accounting volume

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
//#include "G4Cons.hh"
//#include "G4Orb.hh"
#include "G4Sphere.hh"
//#include "G4Trd.hh"
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
  
  // Envelope parameters
  //
//  G4double env_sizeXY = 30*cm, env_sizeZ = 30*cm;
//  G4Material* env_mat = nist->FindOrBuildMaterial("G4_Galactic");
   
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //     
  // World
  //
//  G4double world_sizeXY = 1.2*env_sizeXY;
  G4double world_sizeXY = 100*cm;
//  G4double world_sizeZ  = 1.2*env_sizeZ;
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



  //=========================  Beam Line =======================================


  //------------------- Target-----------------------------
  
  G4ThreeVector fTargetPos(0,0,0);

  G4Box* solidTarget = new G4Box("tgt_box",40*0.5*mm,40*0.5*mm,0.00100*0.5*mm);  //1um

  G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget, fCarbon, "target_log");

  //G4VPhysicalVolume* target_phys = new G4PVPlacement(0,fTargetPos,
  new G4PVPlacement(0,fTargetPos,
                logicTarget,"Target", logicWorld,false,0);

  logicTarget->SetVisAttributes(new G4VisAttributes(G4Colour::Red()));

  //----------- Silicon Strip Detectors -------------------


  G4Material* fSilicon = nist->FindOrBuildMaterial("G4_Si"); 
  
  const G4int maxDet = 2;
  
  G4ThreeVector pos[maxDet];
  pos[0].set(fDetInfo->GetCenterX(), fDetInfo->GetCenterY(), fDetInfo->GetCenterZ());
  pos[1].set(fDetInfo->GetCenterX()+5*cm, fDetInfo->GetCenterY()+5*cm, fDetInfo->GetCenterZ());

  G4RotationMatrix* rotMat[maxDet];  
  rotMat[0] = new G4RotationMatrix();
  rotMat[1] = new G4RotationMatrix();


  rotMat[0]->rotateX(fDetInfo->GetRotationX());
  rotMat[0]->rotateY(fDetInfo->GetRotationY());
  rotMat[0]->rotateZ(fDetInfo->GetRotationZ());

  rotMat[1]->rotateX(fDetInfo->GetRotationX());
  rotMat[1]->rotateY(fDetInfo->GetRotationY());
  rotMat[1]->rotateZ(fDetInfo->GetRotationZ());
  

  G4double size[maxDet][3] = {{0.0}};
  
  size[0][0]     = fDetInfo->GetSizeX()*0.5; // half width
  size[0][1]     = fDetInfo->GetSizeY()*0.5; // half length 
  size[0][2]     = fDetInfo->GetSizeZ()*0.5; // half thickness

  size[1][0]     = fDetInfo->GetSizeX()*0.5; // half width
  size[1][1]     = fDetInfo->GetSizeY()*0.5; // half length 
  size[1][2]     = fDetInfo->GetSizeZ()*0.5; // half thickness
  

  G4Box* box[maxDet];
  box[0] = new G4Box("box0", size[0][0], size[0][1], size[0][2]);
  box[1] = new G4Box("box1", size[1][0], size[1][1], size[1][2]);

  G4LogicalVolume* logical[maxDet];
  logical[0] = new G4LogicalVolume(box[0], fSilicon, "logical0"); 
  logical[1] = new G4LogicalVolume(box[1], fSilicon, "logical1"); 


  new G4PVPlacement(rotMat[0], 
                    pos[0],
                    logical[0],
                    "ssd0",
                    logicWorld,
                    false,
                    0,
                    checkOverlaps);

  new G4PVPlacement(rotMat[1], 
                    pos[1],
                    logical[1],
                    "ssd1",
                    logicWorld,
                    false,
                    0,
                    checkOverlaps);


//  new G4PVPlacement(ssd_rm, 
//                    ssd_pos,
//                    ssd_log,
//                    "ssd",
//                    logicWorld,
//                    false,
//                    0,
//                    checkOverlaps);










  // Set scoring volume to stepping action 
  // (where we will account energy deposit)
  //
  SteppingAction* steppingAction = SteppingAction::Instance(); 
  //steppingAction->SetVolume(logicShape1);
  //steppingAction->SetVolume(logicShape2);
  //steppingAction->SetVolume(ssd_log);
  
  steppingAction->SetVolume(logical[0]);

  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
