//
// $Id$
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
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

// using namespace CLHEP

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction()
{ }

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
  G4double world_sizeXY = 30*cm;
//  G4double world_sizeZ  = 1.2*env_sizeZ;
  G4double world_sizeZ  = 30*cm;
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
 
  

                    
  //     
  // first layer of thin Si for dE / x,y,z measurements 
  //
  
  G4Material* shape1_mat = nist->FindOrBuildMaterial("G4_Si");
  G4ThreeVector pos1 = G4ThreeVector(0.0*cm, 0.0*cm, 0.0*cm);

  // Sphere shape
  G4double shape1_rmin = 10.0*cm, shape1_rmax = 10.01*cm;  //100 mum full-size Si???
  G4double shape1_thetamin = 0.*deg, shape1_thetamax =  180.*deg;    
  G4double shape1_phimin = 0.*deg, shape1_phimax =  360.*deg;    
  G4Sphere* solidShape1 =    
    new G4Sphere("Shape1",                  //its name
        shape1_rmin, shape1_rmax,                //its size
        shape1_phimin, shape1_phimax,            //phi angle
        shape1_thetamin, shape1_thetamax);       //theta angle
     
                      
  G4LogicalVolume* logicShape1 =                         
    new G4LogicalVolume(solidShape1,         //its solid
                        shape1_mat,          //its material
                        "Shape1");           //its name
               
  new G4PVPlacement(0,                       //no rotation
                    pos1,                    //at position
                    logicShape1,             //its logical volume
                    "Shape1",                //its name
                    logicWorld,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking



  //     
  // second layer of thick Si for E measurement 
  //
  
  G4Material* shape2_mat = nist->FindOrBuildMaterial("G4_Si");
  G4ThreeVector pos2 = G4ThreeVector(0.0*cm, 0.0*cm, 0.0*cm);

  // Sphere shape
//  G4double shape2_rmin = 10.50*cm, shape2_rmax = 10.6*cm;  //1 mm full-size Si??
  G4double shape2_rmin = 10.50*cm, shape2_rmax = 11.5*cm;  //10 mm full-size Si??
  G4double shape2_thetamin = 0.*deg, shape2_thetamax =  180.*deg;    
  G4double shape2_phimin = 0.*deg, shape2_phimax =  360.*deg;    
  G4Sphere* solidShape2 =    
    new G4Sphere("Shape2",                  //its name
        shape2_rmin, shape2_rmax,                //its size
        shape2_phimin, shape2_phimax,            //phi angle
        shape2_thetamin, shape2_thetamax);       //theta angle
     
                      
  G4LogicalVolume* logicShape2 =                         
    new G4LogicalVolume(solidShape2,         //its solid
                        shape2_mat,          //its material
                        "Shape2");           //its name
               
  new G4PVPlacement(0,                       //no rotation
                    pos2,                    //at position
                    logicShape2,             //its logical volume
                    "Shape2",                //its name
                    logicWorld,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking






  // Set scoring volume to stepping action 
  // (where we will account energy deposit)
  //
  SteppingAction* steppingAction = SteppingAction::Instance(); 
  steppingAction->SetVolume(logicShape1);
  steppingAction->SetVolume(logicShape2);


  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
