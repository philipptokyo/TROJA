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

#include "G4IntersectionSolid.hh"

#include "G4Polyhedra.hh"
#include "G4Trd.hh"
#include "G4Polycone.hh"




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
  G4double world_sizeXY = 2000*mm;
  G4double world_sizeZ  = 2000*mm;
  //G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
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
 
  

  
  G4Material* fTarget;
    
  G4Element* C = new G4Element("Carbon", "C", 6., 12.01*g/mole);
  G4Element* D = new G4Element("Deuterium", "D", 1., 2.01*g/mole);
  G4Material* fCD2 = new G4Material("CD2", 0.88*g/cm3, 2);
  fCD2->AddElement(C, .333);
  fCD2->AddElement(D, .667);

  G4Element* Ti = new G4Element("Titanium", "Ti", 22., 47.867*g/mole);
  G4Material* fTiD2 = new G4Material("TiD2", 3.75*g/cm3, 2);
  fTiD2->AddElement(Ti, .333);
  fTiD2->AddElement(D, .667);

  printf("Building %s target\n", fDetInfo->GetTargetMaterial().c_str());
  
  if( strcmp(fDetInfo->GetTargetMaterial().c_str(), "CD2")==0 ){

    fTarget = fCD2;

  }else{

    //G4Material* fLead    = nist->FindOrBuildMaterial("G4_Pb"); 
    //G4Material* fIron    = nist->FindOrBuildMaterial("G4_Fe");
    //G4Material* fCarbon    = nist->FindOrBuildMaterial("G4_C");
    //G4Material* fPE    = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
    //G4Material* fTarget    = nist->FindOrBuildMaterial(fDetInfo->GetTargetMaterial().c_str());
    
    fTarget    = nist->FindOrBuildMaterial(fDetInfo->GetTargetMaterial().c_str());

  }
  
  G4Material* fCarbon    = nist->FindOrBuildMaterial("G4_C");

  //------------------- Target-----------------------------
  
  //G4ThreeVector fTargetPos(0,0,0);
  G4ThreeVector fTargetPos(fDetInfo->GetTargetPosition(0), fDetInfo->GetTargetPosition(1), fDetInfo->GetTargetPosition(2));

  //G4Box* solidTarget = new G4Box("tgt_box", 40*0.5*mm, 40*0.5*mm, 0.00100*0.5*mm);  //1um
  //G4Box* solidTarget = new G4Box("tgt_box", 40*0.5*mm, 40*0.5*mm, 0.00100*2.65*mm);  //5.3um
  //G4Box* solidTarget = new G4Box("tgt_box", 60*0.5*mm, 60*0.5*mm, 0.00100*2.65*mm);  //5.3um
  //G4Box* solidTarget = new G4Box("tgt_box", 60*0.5*mm, 60*0.5*mm, 0.00100*0.1*0.5*mm);  //0.1um
  G4Box* solidTarget = new G4Box("tgt_box", fDetInfo->GetTargetSize(0)*0.5*mm, fDetInfo->GetTargetSize(1)*0.5*mm, fDetInfo->GetTargetSize(2)*0.5*mm); 

  printf("Target material is %s\n", fTarget->GetName().c_str());

    //G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget, fCarbon, "target_log");
    //G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget, fPE, "target_log");
    G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget, fTarget, "target_log");

    new G4PVPlacement(0, fTargetPos, logicTarget, "Target", logicWorld, false, 0, checkOverlaps);

    logicTarget->SetVisAttributes(new G4VisAttributes(G4Colour::Red()));
    





    //----------- Silicon Strip and CsI Detectors -------------------

    char tmpName[50];


    G4Material* fSilicon = nist->FindOrBuildMaterial("G4_Si"); 
    printf("Material G4_Si has density %f g/cm3\n", fSilicon->GetDensity()/g*cm3);

    G4Element* Cs = new G4Element("Cesium", "Cs", 55., 132.9*g/mole);
    G4Element* I = new G4Element("Iodine", "I", 53., 126.9*g/mole);
    G4Material* fCsI = new G4Material("CsI", 4.51*g/cm3, 2);
    fCsI->AddElement(I, .5);
    fCsI->AddElement(Cs, .5);
    
    G4Material* fAl = nist->FindOrBuildMaterial("G4_Al");

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

      } else if( (strcmp(fDetInfo->GetType(d).data(), "DSSDtube")==0) ||
                 (strcmp(fDetInfo->GetType(d).data(), "CsItube")==0) ||
                 (strcmp(fDetInfo->GetType(d).data(), "Altube")==0) 
               ){

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
        if(strcmp(fDetInfo->GetType(d).data(), "DSSDtube")==0){
          logical[d] = new G4LogicalVolume(tube[d], fSilicon, tmpName);
        }else if(strcmp(fDetInfo->GetType(d).data(), "CsItube")==0){
          logical[d] = new G4LogicalVolume(tube[d], fCsI, tmpName);
        }else if(strcmp(fDetInfo->GetType(d).data(), "Altube")==0){
          logical[d] = new G4LogicalVolume(tube[d], fAl, tmpName);
        }else{
          printf("Error in detector construction. But this should not happen.\n");
          abort();
        }

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
    


    




    if(fDetInfo->IncludeBeamPipe()){
      
      //const G4int beamPipeSegments=1;
      const G4int beamPipeSegments=3;
      G4double sizeBeamPipe[beamPipeSegments][3]={{0.0}};
      G4double posBeamPipe[beamPipeSegments][3]={{0.0}};
      
      //sizeBeamPipe[0][0] = 158.0;
      //sizeBeamPipe[0][1] = 162.0;
      //sizeBeamPipe[0][2] = 600.0;
      //posBeamPipe[0][2] = -145.0;

      
      // option 1 files _34
      //sizeBeamPipe[0][0] = 158.0;
      sizeBeamPipe[0][0] = 156.0;
      sizeBeamPipe[0][1] = 162.0;
      sizeBeamPipe[0][2] = 310.0;
      posBeamPipe[0][2] = -150.0;

      sizeBeamPipe[1][0] = 49.0;
      sizeBeamPipe[1][1] = sizeBeamPipe[0][1];
      sizeBeamPipe[1][2] = 5.0;
      posBeamPipe[1][2] = 7.5;
      
      sizeBeamPipe[2][0] = sizeBeamPipe[1][0];
      sizeBeamPipe[2][1] = 51.0;
      sizeBeamPipe[2][2] = 300.0;

      posBeamPipe[2][2] = 160.0;
      
      //// option 2,3 with reduced pipe
      //sizeBeamPipe[0][0] = 130.0;
      //sizeBeamPipe[0][1] = 136.6;
      //sizeBeamPipe[0][2] = 310.0;
      //posBeamPipe[0][2] = -150.0;

      //sizeBeamPipe[1][0] = 49.0;
      //sizeBeamPipe[1][1] = sizeBeamPipe[0][1];
      //sizeBeamPipe[1][2] = 5.0;
      //posBeamPipe[1][2] = 7.5;
      //
      //sizeBeamPipe[2][0] = sizeBeamPipe[1][0];
      //sizeBeamPipe[2][1] = 51.0;
      //sizeBeamPipe[2][2] = 300.0;
      //posBeamPipe[2][2] = 160.0;
      
      
      //// option 2,3 with straight tube
      //sizeBeamPipe[0][0] = 130.0;
      //sizeBeamPipe[0][1] = 136.6;
      //sizeBeamPipe[0][2] = 600.0;
      //posBeamPipe[0][2] = -145.0;

      


      G4Tubs* sBeamPipeTubs[beamPipeSegments];
      G4LogicalVolume* lBeamPipeTubs[beamPipeSegments];
      
//      G4ThreeVector vBeamPipe[beamPipeSegments];
      G4RotationMatrix* rotBeamPipe[beamPipeSegments];
      G4VisAttributes* visAttBeamPipe = new G4VisAttributes(G4Colour(2.0,2.0,2.0));
      
      for(G4int b=0; b<beamPipeSegments; b++){
      //for(G4int b=0; b<1; b++){
        
        rotBeamPipe[b] = new G4RotationMatrix();

        //printf("placing beam pipe to %lf %lf %lf\n", posBeamPipe[b][0], posBeamPipe[b][1], posBeamPipe[b][2]);

        sprintf(tmpName, "sBeamPipeTubs%d", b);
        sBeamPipeTubs[b]= new G4Tubs(tmpName, sizeBeamPipe[b][0]*mm, sizeBeamPipe[b][1]*mm, sizeBeamPipe[b][2]/2.0*mm, 0*deg, 360*deg);
        
        sprintf(tmpName, "lBeamPipeTubs%d", b);
        lBeamPipeTubs[b] = new G4LogicalVolume(sBeamPipeTubs[b],nist->FindOrBuildMaterial("G4_Al"), tmpName);
        lBeamPipeTubs[0]->SetVisAttributes(visAttBeamPipe);

//        vBeamPipe[b].set(posBeamPipe[b][0], posBeamPipe[b][1], posBeamPipe[b][2]);
        //rotBeamPipe[0].rotateX(90.0*deg);
        
        G4ThreeVector vBP(posBeamPipe[b][0], posBeamPipe[b][1], posBeamPipe[b][2]);
        
        //printf("beam pipe is at vector %lf %lf %lf\n", vBP.x(), vBP.y(), vBP.z());
        
        sprintf(tmpName, "pBeamPipeTubs%d", b);
        new G4PVPlacement(rotBeamPipe[0], 
//                          vBeamPipe[b],
                          vBP,
                          lBeamPipeTubs[b],
                          tmpName,
                          logicWorld, 
                          false, 
                          0, 
                          checkOverlaps
                          );

      }

    }





//    if(fDetInfo->IncludeGrape()){
//      char fTempname[100];
//  
//  
//      Float_t fPosX[20][2][10];
//      Float_t fPosY[20][2][10];
//      Float_t fPosZ[20][2][10];
//  
//      for(int i=0;i<20;i++)  {
//        for(int j=0;j<2;j++)  {
//          for(int k=0;k<10;k++)  {
//            fPosX[i][j][k] = fPosY[i][j][k] = fPosZ[i][j][k] = 0.0;
//          }
//        }
//      }
//      // So far only the crystal without any housing
//      G4ThreeVector segmentPos;
//      G4RotationMatrix segmentRot3D;
//      segmentRot3D.set(0, 0, 0);
//  
//      G4IntersectionSolid* sGrapeSegment[10];
//      G4LogicalVolume* lGrapeSegment[10];
//  
//  
//      //Starting with a tube:
//      G4Tubs* sGrapeCrystalTube = new G4Tubs("sGrapeCrystalTube",0.0*mm,60.0*mm/2.0,20.0*mm/2.0,0*deg, 360*deg);
//      //Making the boxes, the intersections with the tube will correspond to the segments.
//      G4Box* sGrapeCrystalBox = new G4Box("sGrapeCrystalBox",20*mm/2.0,20*mm/2.0,20*mm/2.0);
//      //Making the nine crystals:
//      for(int i=0;i<9;i++)  {
//        sprintf(fTempname,"sGrapeSegment[%i]",i);
//        G4double xPos = ((i%3)-1)*20.0*mm;
//        G4double yPos = ((int)(i/3)-1)*20.0*mm;
//  
//        segmentPos = G4ThreeVector(xPos, yPos, 0.0);
//        sGrapeSegment[i] = new G4IntersectionSolid(fTempname,sGrapeCrystalTube,sGrapeCrystalBox,&segmentRot3D,segmentPos);
//        // Making a logic volume out og the nine segments:
//        sprintf(fTempname,"lGrapeSegment[%i]",i);
//        //lGrapeSegment[i] = new G4LogicalVolume(sGrapeSegment[i],fMaterialList->GetMaterial("Ge"),fTempname,0,0,0);
//        lGrapeSegment[i] = new G4LogicalVolume(sGrapeSegment[i],nist->FindOrBuildMaterial("G4_Ge"),fTempname,0,0,0);
//  
//      }
//      //_______________________________________________________________________________________________
//      //------------------------------
//      //Grape Dewar  
//      G4Tubs* sGrapeDewar = new G4Tubs("sGrapeDewar",0.0*mm,210.0*mm/2.0,280.0*mm/2.0,0.0*deg,360.0*deg);
//      //lGrapeDewar = new G4LogicalVolume(sGrapeDewar,fMaterialList->GetMaterial("Air"),"lGrapeDewar",0,0,0);
//      G4LogicalVolume* lGrapeDewar = new G4LogicalVolume(sGrapeDewar,nist->FindOrBuildMaterial("G4_Galactic"),"lGrapeDewar",0,0,0);
//      //------------------------------
//      //Grape AlBar  
//      G4Tubs* sGrapeAlBar = new G4Tubs("sGrapeAlBar",0.0*mm,35.0*mm/2.0,130.0*mm/2.0,0.0*deg,360.0*deg);
//      //lGrapeAlBar = new G4LogicalVolume(sGrapeAlBar,fMaterialList->GetMaterial("Air"),"lGrapeAlBar",0,0,0);
//      G4LogicalVolume* lGrapeAlBar = new G4LogicalVolume(sGrapeAlBar,nist->FindOrBuildMaterial("G4_Galactic"),"lGrapeAlBar",0,0,0);
//  
//      G4VisAttributes* visAttDewar = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
//      G4VisAttributes* visAttAl    = new G4VisAttributes(G4Colour(0.8,0.8,0.8));
//      G4VisAttributes* visAttGe    = new G4VisAttributes(G4Colour(1.0,0.5,0.6));
//  
//      int i3=0;
//      for(i3=0;i3<3;i3++)  {
//        lGrapeSegment[i3]->SetVisAttributes(visAttGe);
//        lGrapeSegment[i3+3]->SetVisAttributes(visAttGe);
//        lGrapeSegment[i3+6]->SetVisAttributes(visAttGe);
//      }
//      lGrapeDewar->SetVisAttributes(visAttDewar);
//      lGrapeAlBar->SetVisAttributes(visAttAl);
//  
//  
//  
//  
//  
//  
//      float x,y,z,psi,theta,phi;
//      int id;
//      string nameList = "ABCDEFGHIJKLMNOPQR";
//  
//      FILE* fFileIn = fopen("/home/philipp/sim/troja/grape_geometry_in.txt","r");
//      FILE* fFileOut = fopen("/home/philipp/sim/troja/grape_geometry_out.txt","w");
//  
//      Int_t NUMBEROFGRAPEDETECTORS=fDetInfo->GetNumberOfGrapeDetectors();
//  
//      int i = 0;
//      while (!feof(fFileIn)&&i<NUMBEROFGRAPEDETECTORS)  {//Maximum 18 Detectors  
//        int fres = fscanf(fFileIn,"%f %f %f %f %f %f",&x,&y,&z,&psi,&theta,&phi);
//        if(fres>0){}
//  
//        //The first digit is for the array type, digits 2-4 are used for the detector number
//        //The last two digits are for the different physical materials of one detector
//        id =  200000 +(i+1)*100;
//        //The angle values for phi and theta are 90 degrees less than "expected" for a right-handed system
//        //due to the intrinsic orientation of the detector!
//        // OK the values are corrected in the Grape creator now!
//        // Also the theta angle is defined as 0 for antiparallel to the beam direction (crystal looking upstream) 
//        //and 180 parallel to the beam direction (crystal looking downstream)
//  
//        int grapeDet = (id-200000)/100-1;
//  
//        G4RotationMatrix Rot3D;
//        G4ThreeVector centerPos(x*cm,y*cm,z*cm); //Position of the center
//  
//        G4ThreeVector crystalPos;  //Position of the cylindars centers 
//  
//        Rot3D.set(0, 0, 0);
//        //Rot3D.rotateY(90.*degree); 
//        Rot3D.rotateX(psi*degree);
//        Rot3D.rotateY(theta*degree+90*degree);
//        Rot3D.rotateZ(phi*degree);
//  
//        //G4PVPlacement* pGrapeSegment[20][2][10];
//        //G4PVPlacement* pGrapeAlBar[20];
//        //G4PVPlacement* pGrapeDewar[20];
//  
//  
//        // The two crystals:
//        for(int iii=0;iii<2;iii++)  {
//          // All the segments:
//          for(int j=0;j<9;j++)  {
//            float  zShift = 0.;
//  
//            if(iii==0) zShift = -1.0*cm;
//            if(iii==1) zShift = +1.0*cm;
//  
//            crystalPos = centerPos + Rot3D(G4ThreeVector(0.,0.,zShift));
//  
//            sprintf(fTempname,"pGrapeSegment[%i][%i][%i]",grapeDet,iii,j);
//            //pGrapeSegment[grapeDet][iii][j] = new G4PVPlacement(G4Transform3D(Rot3D,crystalPos),
//            new G4PVPlacement(G4Transform3D(Rot3D,crystalPos),
//                                               lGrapeSegment[j],fTempname,logicWorld,false,(id+10*iii+j), checkOverlaps);
//  
//            //cout<<"id_start+10*iii+j = "<< (id_start+10*iii+j)<<endl;
//  
//            G4double xPos = ((j%3)-1)*20.0*mm;
//            G4double yPos = ((int)(j/3)-1)*20.0*mm;
//  
//            G4RotationMatrix segmentRotation;
//  
//            segmentPos = crystalPos + Rot3D(G4ThreeVector(xPos, yPos, 0));
//  
//            // Assigning the positions:
//            fPosX[grapeDet][iii][j] = segmentPos.getX();
//            fPosY[grapeDet][iii][j] = segmentPos.getY();
//            fPosZ[grapeDet][iii][j] = segmentPos.getZ();
//            //-----------------------------------------
//            //cout------------------------------------------------
//            cout << id+10*iii+j << " " << fPosX[grapeDet][iii][j]/cm
//                                << " " << fPosY[grapeDet][iii][j]/cm
//                                << " " << fPosZ[grapeDet][iii][j]/cm << " " << endl;
//            //----------------------------------------------------
//  
//            float dis_this = sqrt(fPosX[grapeDet][iii][j]/mm
//                                 *fPosX[grapeDet][iii][j]/mm
//                                + fPosY[grapeDet][iii][j]/mm
//                                 *fPosY[grapeDet][iii][j]/mm
//                                + fPosZ[grapeDet][iii][j]/mm
//                                 *fPosZ[grapeDet][iii][j]/mm);
//  
//            float thetaThis = acos(fPosZ[grapeDet][iii][j]/mm/dis_this);
//            float phiThis = acos(fPosX[grapeDet][iii][j]/mm/dis_this/sin(thetaThis));
//  
//            if(fPosY[grapeDet][iii][j]/mm < 0.0) phiThis = 2.0*3.14159-phiThis;
//            thetaThis = thetaThis/3.14159*180.0;
//            phiThis = phiThis/3.14159*180.0;
//  
//            if(abs(fPosY[grapeDet][iii][j]/mm)<1.0 && fPosX[grapeDet][iii][j]/mm>=0.0) phiThis = 0.0;
//            if(abs(fPosY[grapeDet][iii][j]/mm)<1.0 && fPosX[grapeDet][iii][j]/mm<0.0) phiThis = 180.0;
//  
//            fprintf(fFileOut,"%i %i %i %f %f %f \n",i , iii, j, thetaThis, phiThis, dis_this);
//          }
//        }
//        //Placing the dewar:
//  
//        G4ThreeVector dewarPos;
//        G4ThreeVector AlBarPos;
//        G4RotationMatrix dummyMatrix;
//        dummyMatrix.set(0, 0, 0);
//        dummyMatrix.rotateX(psi*degree);
//        dummyMatrix.rotateY(theta*degree);
//        dummyMatrix.rotateZ(phi*degree);
//        dewarPos = centerPos + dummyMatrix(G4ThreeVector(0.,0.,30*cm));
//        AlBarPos = centerPos + dummyMatrix(G4ThreeVector(0.,0.,9.5*cm));
//  
//        sprintf(fTempname,"pGrapeAlBar[%i]",grapeDet);
//        //pGrapeAlBar[grapeDet] = new G4PVPlacement(G4Transform3D(dummyMatrix,AlBarPos),
//        new G4PVPlacement(G4Transform3D(dummyMatrix,AlBarPos),
//                               lGrapeAlBar,fTempname,logicWorld,false,(id+31), checkOverlaps);
//  
//        sprintf(fTempname,"pGrapeDewar[%i]",grapeDet);
//        //pGrapeDewar[grapeDet] = new G4PVPlacement(G4Transform3D(dummyMatrix,dewarPos),
//        new G4PVPlacement(G4Transform3D(dummyMatrix,dewarPos),
//                               lGrapeDewar,fTempname,logicWorld,false,(id+30), checkOverlaps);
//        i++;
//      }
//      fclose(fFileOut);
//      fclose(fFileIn);
//
//
//    } // include grape




    if(fDetInfo->IncludeGrape()){

      G4Material *Al  = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
      G4Material *Cu  = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
      G4Material *G4Ge  = G4NistManager::Instance()->FindOrBuildMaterial("G4_Ge");
      //G4Material *In  = G4NistManager::Instance()->FindOrBuildMaterial("G4_In");
      G4Material *Vacuum = 
         G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
      
      G4Isotope *Ge70 = new G4Isotope("Ge70",32,38,69.9242504*g/mole);
      G4Isotope *Ge72 = new G4Isotope("Ge72",32,40,71.9220762*g/mole);
      G4Isotope *Ge73 = new G4Isotope("Ge73",32,41,72.9234594*g/mole);
      G4Isotope *Ge74 = new G4Isotope("Ge74",32,42,73.9211782*g/mole);
      G4Isotope *Ge76 = new G4Isotope("Ge76",32,44,75.9214027*g/mole);
      G4Element *elGe = new G4Element("elGe","elGe",5);
      elGe->AddIsotope(Ge70,20.84*perCent);
      elGe->AddIsotope(Ge72,27.54*perCent);
      elGe->AddIsotope(Ge73, 7.73*perCent);
      elGe->AddIsotope(Ge74,36.28*perCent);
      elGe->AddIsotope(Ge76, 7.61*perCent);
      G4Material *Ge = new G4Material("Ge",5.323*g/cm3,1);
      Ge->AddElement(elGe,1.);
   
   
      G4cout << "Ge:" << Ge->GetRadlen()/cm << G4endl;
      G4cout << Ge->GetIonisation()->GetMeanExcitationEnergy() << G4endl;
      G4cout << "G4Ge:" << G4Ge->GetRadlen()/cm << G4endl;
      G4cout << "Ionisation" << G4endl;
      G4cout << "Al: " << Al->GetIonisation()->GetMeanExcitationEnergy()/eV << G4endl;
      G4cout << "Cu: " << Cu->GetIonisation()->GetMeanExcitationEnergy()/eV << G4endl;
   
      // hexagonal hausing
      G4VSolid *sHausingHexBase1; // outer
      {
        const G4int nz = 2;
        const G4int nss = 6;
        G4double z[nz]   = {-32.5*mm, 32.5*mm};
        G4double rin[nz] = {0.*mm, 0.*mm};
        G4double rout[nz] = {50.*mm, 50.*mm};
        sHausingHexBase1 = new G4Polyhedra("sHausingHexBase1", 30.*deg, 360.*deg,
                                       nss, nz, z, rin, rout);
      }
//      G4VSolid *sHausingHexBase2; // inner, -2 mm???
//      {
//        const G4int nz = 2;
//        const G4int nss = 6;
//        G4double z[nz]   = {-30.5*mm, 30.5*mm};
//        G4double rin[nz] = {0.*mm, 0.*mm};
//        G4double rout[nz] = {48.*mm, 48.*mm};
//        sHausingHexBase2 = new G4Polyhedra("sHausingHexBase2", 30.*deg, 360.*deg,
//                                       nss, nz, z, rin, rout);
//      }
//      G4VSolid *sHausingHexBase = new G4SubtractionSolid("sHausingHexBase", sHausingHexBase1, sHausingHexBase2);

      // trd hausing
      G4VSolid *sHausingTrd =
         new G4Trd("sHausingTrd",28.*mm,28.*mm,
                   50.*2/sqrt(3.)*mm, 50.*2/sqrt(3.)*40./100.,30.*mm);
      // hausing hex 
      G4RotationMatrix rmY90;
      rmY90.rotateY(90.*deg);

      G4VSolid *sHausingHex =
         //new G4UnionSolid("sHausingHex",sHausingHexBase,sHausingTrd,
         new G4UnionSolid("sHausingHex",sHausingHexBase1,sHausingTrd,
                          G4Transform3D(rmY90,G4ThreeVector(30.*mm,0.,0.)));
      // hausing pre-amp.
      G4VSolid *sPreAmpHausing;
      {
        const G4int nz = 15;
        G4double z[nz] = 
           { 50.*mm, 60.*mm, 60.*mm, 62.*mm, 62.*mm,
             100.*mm, 150.*mm, 150.*mm, 290.*mm, 290.*mm, 
             334.*mm,334.*mm,340.*mm,340.*mm,607.*mm };
        G4double rin[nz] = 
           { 0.*mm,0.*mm,0.*mm,0.*mm,0.*mm,
             0.*mm,0.*mm,0.*mm,0.*mm,0.*mm,
             0.*mm,0.*mm,0.*mm,0.*mm,0.*mm };
        G4double rout[nz] = 
           { 20.*mm, 20.*mm, 23.09*mm, 23.09*mm, 20.*mm,
             20.*mm, 50.*mm, 70.*mm, 70.*mm, 27.5*mm, 
             27.5*mm, 30.*mm, 30.*mm, 111.*mm, 111.*mm };
        sPreAmpHausing = new G4Polycone("sPreAmpHausing",0.*deg, 360.*deg,
                                        nz, z, rin, rout);
      }
      G4VSolid *sHausing =
         new G4UnionSolid("sHausing",sHausingHex,sPreAmpHausing, // orig
//         new G4UnionSolid("sHausing",sHausingHexBase1,sPreAmpHausing,
//         new G4UnionSolid("sHausing",sHausingTrd,sPreAmpHausing,
                          G4Transform3D(rmY90,G4ThreeVector()));
   
      // vacuum
//      G4VSolid *sHeadVacuumBase;
//      {
//        const G4int nz = 10 ;
//        G4double z[nz] = 
//           { -31.5*mm, -26.5*mm, -26.5*mm, -21.5*mm, -21.5*mm,
//             21.5*mm, 21.5*mm, 26.5*mm,26.5*mm,31.5*mm };
//        G4double rin[nz] = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. };
//        G4double rout[nz] = 
//           { 44.5*mm, 44.5*mm, 45.5*mm, 45.5*mm, 48.5*mm,
//             48.5*mm, 45.5*mm, 45.5*mm, 44.5*mm, 44.5*mm };
//        sHeadVacuumBase = new G4Polycone("sHeadVacuumBase",0.*deg, 360.*deg,
//                                     nz, z, rin, rout);
//      }
//      G4VSolid *sHeadVacuumMid;
//      {
//        const G4int nz = 7;
//        G4double z[nz] = { 0.*mm, 100.*mm, 150.*mm, 150.*mm, 345.*mm, 
//                           345.*mm, 602.*mm};
//        G4double rin[nz] = {0., 0., 0., 0., 0., 0., 0.,};
//        G4double rout[nz] = { 17.*mm, 17.*mm, 46.25*mm, 17.*mm, 17.*mm,
//                              106.*mm, 106.*mm};
//        sHeadVacuumMid = new G4Polycone("sHeadVacuumMid",0.*deg, 360.*deg,
//                                        nz, z, rin, rout);
//      }
//      G4VSolid *sHeadVacuum =
//        new G4UnionSolid("sHeadVacuum",sHeadVacuumBase, sHeadVacuumMid, // orig
//        //new G4UnionSolid("sHeadVacuum",sHeadVacuumBase, sHeadVacuumMid,
//                         G4Transform3D(rmY90,G4ThreeVector()));
      
      
      G4VSolid *sHeadVacuum; // same as sHauseingHexBase2 
      {
        const G4int nz = 2;
        const G4int nss = 6;
        G4double z[nz]   = {-30.5*mm, 30.5*mm};
        G4double rin[nz] = {0.*mm, 0.*mm};
        G4double rout[nz] = {48.*mm, 48.*mm};
        sHeadVacuum = new G4Polyhedra("sHeadVacuum", 30.*deg, 360.*deg,
                                       nss, nz, z, rin, rout);
      }
      
      
      
      
      
      
      
      
      G4VSolid *sPreAmpVacuum;
      {
        const G4int nz = 10;
        G4double z[nz] =
           { 157.*mm, 158.*mm, 158.*mm, 166.*mm, 166.*mm,
             245.*mm, 245.*mm, 252.*mm, 252.*mm, 285.*mm };
        G4double rin[nz] = 
           { 14.5*mm, 14.5*mm, 14.5*mm, 14.5*mm, 14.5*mm,
             14.5*mm, 14.5*mm, 14.5*mm, 14.5*mm, 14.5*mm };
        //      G4double rin[nz] = 
        //         { 14.5*mm, 14.5*mm, 50.*mm, 50.*mm, 14.5*mm,
        //           14.5*mm, 50.*mm, 50.*mm, 14.5*mm, 14.5*mm };
        G4double rout[nz] =
           { 68.*mm, 68.*mm, 68.*mm, 68.*mm, 68.*mm, 
             68.*mm, 68.*mm, 68.*mm, 65.*mm, 65.*mm } ;
        sPreAmpVacuum = 
           new G4Polycone("sPreAmpVacuum",0.*deg,360.*deg,nz,z,rin,rout);
      }
   
      G4VSolid *sDewarColor = new G4Tubs("sDewarColor",106.*mm,111.*mm,133.5*mm,
                                        0.*deg,360.*deg);
      
      // inner dewar
      G4VSolid *sInnerDewar = new G4Tubs("sInnerDewar",0.,75.*mm,105.*mm,
                                         0.*deg,360.*deg);
      // liquid or vacuum
      G4VSolid *sLiquid = new G4Tubs("sLiquid",0.,70.*mm,100.*mm,0.*deg,360.*deg);
      
      // cold finger (Cu
      G4VSolid *sColdFinger;
      {
         const G4int nz=2;
         G4double z[nz] = { 50.*mm, 368.5*mm };
         G4double rin[nz] = { 0., 0. };
         G4double rout[nz] = { 9.*mm, 9.*mm };
         sColdFinger = new G4Polycone("sColdFinger",0.*deg,360.*deg,
                                      nz,z,rin,rout);
      }
   
      // cold case
      G4VSolid *sColdCase = new G4Tubs("sColdCase",35.*mm,40.*mm,20.*mm,
                                       0.*deg,360.*deg);
   
      // Ge Grystal
      G4VSolid *sGeCrystal = new G4Tubs("sGeCrystal",0.*mm,35.*mm,23.*mm,
                                        0.*deg, 360.*deg);
      // Ge segment
      //G4double rmaxSD = 32.*mm; // orig
      G4double rmaxSD = 32.*mm;
      //G4VSolid *sGeSensitive = new G4Tubs("sGeCrystal",0.*mm,rmaxSD,20.*mm, // orig
      G4VSolid *sGeSensitive = new G4Tubs("sGeCrystal",0.*mm,rmaxSD,20.*mm,
                                        0.*deg, 360.*deg);
      // cathode? 
      // todo: what is the material and thickness of the cathode?
      // is there other material between the crystals?
      G4VSolid *sGeCathode = new G4Tubs("sGeCathode",0.*mm, rmaxSD,0.1*mm,
                                        0.*deg, 360.*deg);
   
      //// Indium shield
      //G4VSolid *sInShield = new G4Tubs("sInShield",32.*mm, 40.*mm,0.5*mm,
      //                                 0.*deg,360.*deg);
                                                  
         
      // logical volumes
      G4LogicalVolume *lHausing =
         new G4LogicalVolume(sHausing,Al,"lHausing",0,0,0);
         //new G4LogicalVolume(sHausingHexBase1,Al,"lHausing",0,0,0);
      
      G4VisAttributes *hausingVisAtt = new G4VisAttributes();
      hausingVisAtt->SetForceWireframe(true);
      
      
//      lHausing->SetVisAttributes(hausingVisAtt);

      G4LogicalVolume *lHeadVacuum = 
         new G4LogicalVolume(sHeadVacuum,Vacuum,"lHeadVacuum",0,0,0);
      lHeadVacuum->SetVisAttributes(hausingVisAtt);
      G4LogicalVolume *lPreAmpVacuum =
         new G4LogicalVolume(sPreAmpVacuum,Vacuum,"lPreAmpVacuum",0,0,0);
      G4LogicalVolume *lInnerDewar =
         new G4LogicalVolume(sInnerDewar,Al,"lInnerDewar",0,0,0);
      G4LogicalVolume *lDewarColor =
         new G4LogicalVolume(sDewarColor,Al,"lDewarColor",0,0,0);
      G4LogicalVolume *lLiquid =
         new G4LogicalVolume(sLiquid,Vacuum,"lLiquid",0,0,0);
      G4LogicalVolume *lColdFinger =
         new G4LogicalVolume(sColdFinger,Cu,"lColdFinger",0,0,0);
      G4LogicalVolume *lColdCase =
         new G4LogicalVolume(sColdCase,Al,"lColdCase",0,0,0);
      lColdCase->SetVisAttributes(hausingVisAtt);

      G4LogicalVolume *lGeCrystal =
         //new G4LogicalVolume(sGeCrystal,Ge,"lGeCrystal",0,0,0);
         new G4LogicalVolume(sGeCrystal,Vacuum,"lGeCrystal",0,0,0);
      //lGeCrystal->SetVisAttributes(hausingVisAtt);
      G4LogicalVolume *lGeCathode =
         new G4LogicalVolume(sGeCathode,fCarbon,"lGeCathode",0,0,0);
      //G4LogicalVolume *lInShield =
      //   new G4LogicalVolume(sInShield,In,"lInShield",0,0,0);
      new G4PVPlacement(0,G4ThreeVector(0,0,0),lGeCathode, "pCathode",
                        lGeCrystal,false,0, checkOverlaps);
                        //logicWorld,false,0, checkOverlaps);
                        //lHausing,false,0, checkOverlaps);
      
      // Segment location
      G4double offs = 20.*mm + (rmaxSD-30.*mm)/2.;
      G4ThreeVector *segPos = new G4ThreeVector[18];
      segPos[0]  = G4ThreeVector( offs*mm,-offs*mm,-12.50*mm);
      segPos[1]  = G4ThreeVector(  0.0*mm,-offs*mm,-12.50*mm);
      segPos[2]  = G4ThreeVector(-offs*mm,-offs*mm,-12.50*mm);
      segPos[3]  = G4ThreeVector( offs*mm,  0.0*mm,-12.50*mm);
      segPos[4]  = G4ThreeVector(  0.0*mm,  0.0*mm,-12.50*mm);
      segPos[5]  = G4ThreeVector(-offs*mm,  0.0*mm,-12.50*mm);
      segPos[6]  = G4ThreeVector( offs*mm, offs*mm,-12.50*mm);
      segPos[7]  = G4ThreeVector(  0.0*mm, offs*mm,-12.50*mm);
      segPos[8]  = G4ThreeVector(-offs*mm, offs*mm,-12.50*mm);
      segPos[9]  = G4ThreeVector( offs*mm, offs*mm, 12.50*mm);
      segPos[10] = G4ThreeVector(  0.0*mm, offs*mm, 12.50*mm);
      segPos[11] = G4ThreeVector(-offs*mm, offs*mm, 12.50*mm);
      segPos[12] = G4ThreeVector( offs*mm,  0.0*mm, 12.50*mm);
      segPos[13] = G4ThreeVector(  0.0*mm,  0.0*mm, 12.50*mm);
      segPos[14] = G4ThreeVector(-offs*mm,  0.0*mm, 12.50*mm);
      segPos[15] = G4ThreeVector( offs*mm,-offs*mm, 12.50*mm);
      segPos[16] = G4ThreeVector(  0.0*mm,-offs*mm, 12.50*mm);
      segPos[17] = G4ThreeVector(-offs*mm,-offs*mm, 12.50*mm);
      G4double longLength = 10.*mm + (rmaxSD - 30.*mm)/2.;
      G4double defaultLength = 10.*mm;
      for (G4int i=0; i<18; i++) {
        G4VSolid *sSegBox;
        switch (i%9) {
        case 0:
        case 2:
        case 6:
        case 8:
          sSegBox = new G4Box("sSegBox",longLength,longLength,defaultLength);
          break;
        case 1:
        case 7:
          sSegBox = new G4Box("sSegBox",defaultLength,longLength,defaultLength);
          break;
        case 3:
        case 5:
          sSegBox = new G4Box("sSegBox",longLength,defaultLength,defaultLength);
          break;
        default:
          sSegBox = new G4Box("sSegBox",defaultLength,defaultLength,defaultLength);
        }
   
        //if(i%2==0){
        char sName[20],lName[20],pName[20];
        sprintf(sName,"sSeg%02d",(i<9?i+1:i+2));
        sprintf(lName,"lSeg%02d",(i<9?i+1:i+2));
        sprintf(pName,"pSeg%02d",(i<9?i+1:i+2));
        G4VSolid *sSeg = 
           new G4IntersectionSolid(sName,sGeSensitive,sSegBox,0,segPos[i]);
        G4LogicalVolume *lSeg =new G4LogicalVolume(sSeg,Ge,lName,0,0,0);
        //lSeg->SetSensitiveDetector(fGRAPESD);
        new G4PVPlacement(0,G4ThreeVector(),lSeg,pName,
                          lGeCrystal,true,(i<9?i+1:i+2), checkOverlaps);
                          //logicWorld,true,(i<9?i+1:i+2), checkOverlaps);
                          //lHausing,true,(i<9?i+1:i+2), checkOverlaps);
        //}
      }
      //   G4LogicalVolume *lGeSensitive =
      //      new G4LogicalVolume(sGeSensitive,Ge,"lGeSensitive",0,0,0);
      //   new G4PVPlacement(G4Transform3D(),lGeSensitive,"GeSensitive",lGeCrystal,
      //                     true,0);
      //   lGeSensitive->SetSensitiveDetector(fGRAPESD);
   
      // visualize
      lDewarColor->SetVisAttributes(new G4VisAttributes(G4Colour(0.71,0.93,0.71)));
   
      // placements
      new G4PVPlacement(0,G4ThreeVector(),lLiquid,"Liquid",lInnerDewar,false,0, checkOverlaps);
      new G4PVPlacement(G4Transform3D(rmY90,G4ThreeVector(473.5*mm,0.,0.)),
                        //lInnerDewar,"InnerDewar",lHeadVacuum,false,0, checkOverlaps); // orig
                        lInnerDewar,"InnerDewar",lHausing,false,0, checkOverlaps);
    new G4PVPlacement(0,G4ThreeVector(),lGeCrystal,"GeCrystal",
                      lHeadVacuum,false,0,checkOverlaps); // orig
                    //lHausing,false,0,checkOverlaps);
                    //logicWorld,false,0,checkOverlaps);
      new G4PVPlacement(0,G4ThreeVector(),lColdCase,"ColdCase",
                        lHeadVacuum,false,0, checkOverlaps); // orig
                        //lHausing,false,0, checkOverlaps);
      new G4PVPlacement(G4Transform3D(rmY90,G4ThreeVector()),
                        //lColdFinger,"ColdFinger",lHeadVacuum,false,0, checkOverlaps); // orig
                        lColdFinger,"ColdFinger",lHausing,false,0, checkOverlaps);
      new G4PVPlacement(0,G4ThreeVector(),lHeadVacuum,"HeadVacuum",
                        lHausing,false,0, checkOverlaps);
      new G4PVPlacement(G4Transform3D(rmY90,G4ThreeVector()),
                        lPreAmpVacuum,"PreAmpVacuum",lHausing,false,0);
      new G4PVPlacement(G4Transform3D(rmY90,G4ThreeVector(473.5*mm,0.,0.)),
                        lDewarColor,"DewarColor",lHausing,false,0, checkOverlaps);
      //   new G4PVPlacement(0,G4ThreeVector(0.,0.,-20.5*mm),lInShield,
      //                     "InShield",lHausing,false,0, checkOverlaps);
      ifstream fin;
      fin.open("/home/philipp/sim/troja/GeConfig.dat");
      if (!fin) {
        G4cerr << "*** Fatal Error: Cannot open GeConfig.dat ***" << G4endl;
        exit(0);
        return 0;
      }
      G4double Rc, Tc, Pc, Td, Pd;
      G4int    id;
      //G4VPhysicalVolume *aDet;
      G4int nGrape = 0;
      while (fin >> id >> Rc >> Tc >> Pc >> Td >> Pd ) {
        if (fin.eof()) break;
        cout << "Phi center " << Pc << ", phi detector " << Pd << G4endl;
        Rc *= mm;
        Tc *= deg;
        Pc *= deg;
        Td *= deg;
        Pd *= deg;
        // G4ThreeVector    offset(4.24105,-0.645,-4.8061);
        // G4ThreeVector    offset(0.8190,4.9754, -5.590);
   
        G4ThreeVector    ta;
        G4RotationMatrix rm;
        ta[0] = Rc * sin(Tc) * cos(Pc);
        ta[1] = Rc * sin(Tc) * sin(Pc);
        ta[2] = Rc * cos(Tc);
        rm.rotateY(Td);
        rm.rotateZ(Pc+Pd);
        G4cout << "Vector to center " << ta << G4endl;
        //aDet = new G4PVPlacement(G4Transform3D(rm,ta),
        new G4PVPlacement(G4Transform3D(rm,ta),
                         lHausing,"Hausing",logicWorld,false,id, checkOverlaps);
        nGrape++;
      }
      fin.close();
      //fGrapeGeometry = new TArtParameterD<2>("GrapeGeometry",
      //                                        "GRAPE Geometry",
      //                                     gExtent[nGrape][6]);
      //TArtPrmFileCSV prmfile("GeConfig.dat");
      //fGrapeGeometry->Load(prmfile);
      //return aDet;



    }




    //
    //always return the physical World
    //
    return physWorld;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
