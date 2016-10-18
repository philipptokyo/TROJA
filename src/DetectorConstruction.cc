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
  //G4Material* fCarbon    = nist->FindOrBuildMaterial("G4_C");
  //G4Material* fPE    = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
  G4Material* fTarget    = nist->FindOrBuildMaterial(fDetInfo->GetTargetMaterial().c_str());



  //------------------- Target-----------------------------
  
  //G4ThreeVector fTargetPos(0,0,0);
  G4ThreeVector fTargetPos(fDetInfo->GetTargetPosition(0), fDetInfo->GetTargetPosition(1), fDetInfo->GetTargetPosition(2));

  //G4Box* solidTarget = new G4Box("tgt_box", 40*0.5*mm, 40*0.5*mm, 0.00100*0.5*mm);  //1um
  //G4Box* solidTarget = new G4Box("tgt_box", 40*0.5*mm, 40*0.5*mm, 0.00100*2.65*mm);  //5.3um
  //G4Box* solidTarget = new G4Box("tgt_box", 60*0.5*mm, 60*0.5*mm, 0.00100*2.65*mm);  //5.3um
  //G4Box* solidTarget = new G4Box("tgt_box", 60*0.5*mm, 60*0.5*mm, 0.00100*0.1*0.5*mm);  //0.1um
  G4Box* solidTarget = new G4Box("tgt_box", fDetInfo->GetTargetSize(0)*0.5*mm, fDetInfo->GetTargetSize(1)*0.5*mm, fDetInfo->GetTargetSize(2)*0.5*mm); 

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
    


    




    if(fDetInfo->IncludeBeamPipe()){
      
      const G4int beamPipeSegments=3;
      G4double sizeBeamPipe[beamPipeSegments][3]={{0.0}};
      G4double posBeamPipe[beamPipeSegments][3]={{0.0}};
      
      sizeBeamPipe[0][0] = 150.0;
      sizeBeamPipe[0][1] = 155.0;
      sizeBeamPipe[0][2] = 300.0;
      posBeamPipe[0][2] = -145.0;

      sizeBeamPipe[1][0] = 75.0;
      sizeBeamPipe[1][1] = sizeBeamPipe[0][1];
      sizeBeamPipe[1][2] = 5.0;
      posBeamPipe[1][2] = 7.5;
      
      sizeBeamPipe[2][0] = sizeBeamPipe[1][0];
      sizeBeamPipe[2][1] = 85.0;
      sizeBeamPipe[2][2] = 300.0;

      posBeamPipe[2][2] = 160.0;


      G4Tubs* sBeamPipeTubs[beamPipeSegments];
      G4LogicalVolume* lBeamPipeTubs[beamPipeSegments];
      
//      G4ThreeVector vBeamPipe[beamPipeSegments];
      G4RotationMatrix* rotBeamPipe[beamPipeSegments];
      //G4VisAttributes* visAttBeamPipe = new G4VisAttributes(G4Colour(2.0,2.0,2.0));
      
      for(G4int b=0; b<beamPipeSegments; b++){

        //printf("placing beam pipe to %lf %lf %lf\n", posBeamPipe[b][0], posBeamPipe[b][1], posBeamPipe[b][2]);

        sBeamPipeTubs[b]= new G4Tubs(Form("sBeamPipeTubs%d", b), sizeBeamPipe[b][0]*mm, sizeBeamPipe[b][1]*mm, sizeBeamPipe[b][2]/2.0*mm, 0*deg, 360*deg);
        lBeamPipeTubs[b] = new G4LogicalVolume(sBeamPipeTubs[b],nist->FindOrBuildMaterial("G4_Al"), Form("lBeamPipeTubs%d", b));
        //lBeamPipeTubs[0]->SetVisAttributes(visAttBeamPipe);

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
                          1, 
                          checkOverlaps
                          );

      }

    }





















    if(fDetInfo->IncludeGrape()){
      char fTempname[100];
  
  
      Float_t fPosX[20][2][10];
      Float_t fPosY[20][2][10];
      Float_t fPosZ[20][2][10];
  
      for(int i=0;i<20;i++)  {
        for(int j=0;j<2;j++)  {
          for(int k=0;k<10;k++)  {
            fPosX[i][j][k] = fPosY[i][j][k] = fPosZ[i][j][k] = 0.0;
          }
        }
      }
      // So far only the crystal without any housing
      G4ThreeVector segmentPos;
      G4RotationMatrix segmentRot3D;
      segmentRot3D.set(0, 0, 0);
  
      G4IntersectionSolid* sGrapeSegment[10];
      G4LogicalVolume* lGrapeSegment[10];
  
  
      //Starting with a tube:
      G4Tubs* sGrapeCrystalTube = new G4Tubs("sGrapeCrystalTube",0.0*mm,60.0*mm/2.0,20.0*mm/2.0,0*deg, 360*deg);
      //Making the boxes, the intersections with the tube will correspond to the segments.
      G4Box* sGrapeCrystalBox = new G4Box("sGrapeCrystalBox",20*mm/2.0,20*mm/2.0,20*mm/2.0);
      //Making the nine crystals:
      for(int i=0;i<9;i++)  {
        sprintf(fTempname,"sGrapeSegment[%i]",i);
        G4double xPos = ((i%3)-1)*20.0*mm;
        G4double yPos = ((int)(i/3)-1)*20.0*mm;
  
        segmentPos = G4ThreeVector(xPos, yPos, 0.0);
        sGrapeSegment[i] = new G4IntersectionSolid(fTempname,sGrapeCrystalTube,sGrapeCrystalBox,&segmentRot3D,segmentPos);
        // Making a logic volume out og the nine segments:
        sprintf(fTempname,"lGrapeSegment[%i]",i);
        //lGrapeSegment[i] = new G4LogicalVolume(sGrapeSegment[i],fMaterialList->GetMaterial("Ge"),fTempname,0,0,0);
        lGrapeSegment[i] = new G4LogicalVolume(sGrapeSegment[i],nist->FindOrBuildMaterial("G4_Ge"),fTempname,0,0,0);
  
      }
      //_______________________________________________________________________________________________
      //------------------------------
      //Grape Dewar  
      G4Tubs* sGrapeDewar = new G4Tubs("sGrapeDewar",0.0*mm,210.0*mm/2.0,280.0*mm/2.0,0.0*deg,360.0*deg);
      //lGrapeDewar = new G4LogicalVolume(sGrapeDewar,fMaterialList->GetMaterial("Air"),"lGrapeDewar",0,0,0);
      G4LogicalVolume* lGrapeDewar = new G4LogicalVolume(sGrapeDewar,nist->FindOrBuildMaterial("G4_Galactic"),"lGrapeDewar",0,0,0);
      //------------------------------
      //Grape AlBar  
      G4Tubs* sGrapeAlBar = new G4Tubs("sGrapeAlBar",0.0*mm,35.0*mm/2.0,130.0*mm/2.0,0.0*deg,360.0*deg);
      //lGrapeAlBar = new G4LogicalVolume(sGrapeAlBar,fMaterialList->GetMaterial("Air"),"lGrapeAlBar",0,0,0);
      G4LogicalVolume* lGrapeAlBar = new G4LogicalVolume(sGrapeAlBar,nist->FindOrBuildMaterial("G4_Galactic"),"lGrapeAlBar",0,0,0);
  
      G4VisAttributes* visAttDewar = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
      G4VisAttributes* visAttAl    = new G4VisAttributes(G4Colour(0.8,0.8,0.8));
      G4VisAttributes* visAttGe    = new G4VisAttributes(G4Colour(1.0,0.5,0.6));
  
      int i3=0;
      for(i3=0;i3<3;i3++)  {
        lGrapeSegment[i3]->SetVisAttributes(visAttGe);
        lGrapeSegment[i3+3]->SetVisAttributes(visAttGe);
        lGrapeSegment[i3+6]->SetVisAttributes(visAttGe);
      }
      lGrapeDewar->SetVisAttributes(visAttDewar);
      lGrapeAlBar->SetVisAttributes(visAttAl);
  
  
  
  
  
  
      float x,y,z,psi,theta,phi;
      int id;
      string nameList = "ABCDEFGHIJKLMNOPQR";
  
      FILE* fFileIn = fopen("/home/philipp/sim/troja/grape_geometry_in.txt","r");
      FILE* fFileOut = fopen("/home/philipp/sim/troja/grape_geometry_out.txt","w");
  
      Int_t NUMBEROFGRAPEDETECTORS=fDetInfo->GetNumberOfGrapeDetectors();
  
      int i = 0;
      while (!feof(fFileIn)&&i<NUMBEROFGRAPEDETECTORS)  {//Maximum 18 Detectors  
        int fres = fscanf(fFileIn,"%f %f %f %f %f %f",&x,&y,&z,&psi,&theta,&phi);
        if(fres>0){}
  
        //The first digit is for the array type, digits 2-4 are used for the detector number
        //The last two digits are for the different physical materials of one detector
        id =  200000 +(i+1)*100;
        //The angle values for phi and theta are 90 degrees less than "expected" for a right-handed system
        //due to the intrinsic orientation of the detector!
        // OK the values are corrected in the Grape creator now!
        // Also the theta angle is defined as 0 for antiparallel to the beam direction (crystal looking upstream) 
        //and 180 parallel to the beam direction (crystal looking downstream)
  
        int grapeDet = (id-200000)/100-1;
  
        G4RotationMatrix Rot3D;
        G4ThreeVector centerPos(x*cm,y*cm,z*cm); //Position of the center
  
        G4ThreeVector crystalPos;  //Position of the cylindars centers 
  
        Rot3D.set(0, 0, 0);
        //Rot3D.rotateY(90.*degree); 
        Rot3D.rotateX(psi*degree);
        Rot3D.rotateY(theta*degree+90*degree);
        Rot3D.rotateZ(phi*degree);
  
        //G4PVPlacement* pGrapeSegment[20][2][10];
        //G4PVPlacement* pGrapeAlBar[20];
        //G4PVPlacement* pGrapeDewar[20];
  
  
        // The two crystals:
        for(int iii=0;iii<2;iii++)  {
          // All the segments:
          for(int j=0;j<9;j++)  {
            float  zShift = 0.;
  
            if(iii==0) zShift = -1.0*cm;
            if(iii==1) zShift = +1.0*cm;
  
            crystalPos = centerPos + Rot3D(G4ThreeVector(0.,0.,zShift));
  
            sprintf(fTempname,"pGrapeSegment[%i][%i][%i]",grapeDet,iii,j);
            //pGrapeSegment[grapeDet][iii][j] = new G4PVPlacement(G4Transform3D(Rot3D,crystalPos),
            new G4PVPlacement(G4Transform3D(Rot3D,crystalPos),
                                               lGrapeSegment[j],fTempname,logicWorld,false,(id+10*iii+j), checkOverlaps);
  
            //cout<<"id_start+10*iii+j = "<< (id_start+10*iii+j)<<endl;
  
            G4double xPos = ((j%3)-1)*20.0*mm;
            G4double yPos = ((int)(j/3)-1)*20.0*mm;
  
            G4RotationMatrix segmentRotation;
  
            segmentPos = crystalPos + Rot3D(G4ThreeVector(xPos, yPos, 0));
  
            // Assigning the positions:
            fPosX[grapeDet][iii][j] = segmentPos.getX();
            fPosY[grapeDet][iii][j] = segmentPos.getY();
            fPosZ[grapeDet][iii][j] = segmentPos.getZ();
            //-----------------------------------------
            //cout------------------------------------------------
            cout << id+10*iii+j << " " << fPosX[grapeDet][iii][j]/cm
                                << " " << fPosY[grapeDet][iii][j]/cm
                                << " " << fPosZ[grapeDet][iii][j]/cm << " " << endl;
            //----------------------------------------------------
  
            float dis_this = sqrt(fPosX[grapeDet][iii][j]/mm
                                 *fPosX[grapeDet][iii][j]/mm
                                + fPosY[grapeDet][iii][j]/mm
                                 *fPosY[grapeDet][iii][j]/mm
                                + fPosZ[grapeDet][iii][j]/mm
                                 *fPosZ[grapeDet][iii][j]/mm);
  
            float thetaThis = acos(fPosZ[grapeDet][iii][j]/mm/dis_this);
            float phiThis = acos(fPosX[grapeDet][iii][j]/mm/dis_this/sin(thetaThis));
  
            if(fPosY[grapeDet][iii][j]/mm < 0.0) phiThis = 2.0*3.14159-phiThis;
            thetaThis = thetaThis/3.14159*180.0;
            phiThis = phiThis/3.14159*180.0;
  
            if(abs(fPosY[grapeDet][iii][j]/mm)<1.0 && fPosX[grapeDet][iii][j]/mm>=0.0) phiThis = 0.0;
            if(abs(fPosY[grapeDet][iii][j]/mm)<1.0 && fPosX[grapeDet][iii][j]/mm<0.0) phiThis = 180.0;
  
            fprintf(fFileOut,"%i %i %i %f %f %f \n",i , iii, j, thetaThis, phiThis, dis_this);
          }
        }
        //Placing the dewar:
  
        G4ThreeVector dewarPos;
        G4ThreeVector AlBarPos;
        G4RotationMatrix dummyMatrix;
        dummyMatrix.set(0, 0, 0);
        dummyMatrix.rotateX(psi*degree);
        dummyMatrix.rotateY(theta*degree);
        dummyMatrix.rotateZ(phi*degree);
        dewarPos = centerPos + dummyMatrix(G4ThreeVector(0.,0.,30*cm));
        AlBarPos = centerPos + dummyMatrix(G4ThreeVector(0.,0.,9.5*cm));
  
        sprintf(fTempname,"pGrapeAlBar[%i]",grapeDet);
        //pGrapeAlBar[grapeDet] = new G4PVPlacement(G4Transform3D(dummyMatrix,AlBarPos),
        new G4PVPlacement(G4Transform3D(dummyMatrix,AlBarPos),
                               lGrapeAlBar,fTempname,logicWorld,false,(id+31), checkOverlaps);
  
        sprintf(fTempname,"pGrapeDewar[%i]",grapeDet);
        //pGrapeDewar[grapeDet] = new G4PVPlacement(G4Transform3D(dummyMatrix,dewarPos),
        new G4PVPlacement(G4Transform3D(dummyMatrix,dewarPos),
                               lGrapeDewar,fTempname,logicWorld,false,(id+30), checkOverlaps);
        i++;
      }
      fclose(fFileOut);
      fclose(fFileIn);


    } // include grape















    //
    //always return the physical World
    //
    return physWorld;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
