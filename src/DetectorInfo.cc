#include "DetectorInfo.hh"
#include "/home/philipp/sim/troja/include/DetectorGlobals.hh"

//#include "G4PhysicalConstants.hh"
//#include "G4SystemOfUnits.hh"
//
//#include "G4Material.hh"
//#include "TVector3.hh"

//#include "TMath.h"
//
//#include <iostream>
//#include <sstream>
//#include <fstream>
//#include <string>

#include "LibPerso.h"

const Double_t mm = 1.0;
const Double_t deg = TMath::Pi()/180.0;

const Int_t _geo::resNoPars[2] = {0, 1};

DetectorInfo::DetectorInfo()
{ 
  
  ClearGeometry();
  ResetData();

  //SetTargetMaterial("G4_Galactic");
  //SetTargetPosition(0.0, 0.0, 0.0);
  //SetTargetSize(60, 60, 0.001);

  fRandomizer = new TRandom3();
  fRandomizer->SetSeed(0);

  fIncludeDali=false;
  
  fIncludeGrape=false;
  fNumberOfGrapeDetectors=6;
  fIncludeBeamPipe=false;

  for(Int_t d=0; d<GetMaxNoDetectors(); d++){
    fPosDet[d] = false;
    fShield[d] = false;
  }

  fShieldSimple=false;
  
}

DetectorInfo::~DetectorInfo()
{ }


void DetectorInfo::ClearGeometry()
{
  
  for(Int_t d=0; d<GetMaxNoDetectors(); d++){
      SetCenterX(d, NAN);
      SetCenterY(d, NAN);
      SetCenterZ(d, NAN);

    SetRotationX(d, NAN);
    SetRotationY(d, NAN);
    SetRotationZ(d, NAN);

        SetSize0(d, NAN);
        SetSize1(d, NAN);
        SetSize2(d, NAN);
        SetSize3(d, NAN);
        SetSize4(d, NAN);

    SetNoStripsX(d, 0);
    SetNoStripsY(d, 0);

         SetName(d, "");
         SetType(d, "");

//    for(Int_t p=0; p<GetResNoPars(d); p++){
//       SetResPar(d, p, 0.0);
//    }
      SetResType(d, 0);
  }

  fShieldSimpleThickness=0.0;
  fShieldSimpleTheta=TMath::Pi()/2.0;
  fShieldSimpleDensity=0.0;

}

void DetectorInfo::ResetData()
{

  // keep first interaction point information
  // for debugging...
  detData.fIDetID = -1; // first interaction point detector ID

  detData.fIX = NAN; // first interaction point, x coordinate (origin: center of target)
  detData.fIY = NAN;
  detData.fIZ = NAN;
  
  for(Int_t d=0; d<maxDetectors; d++){
    detData.haveHit[d]= 0;
    detData.haveHitID[d]= -1;
    detData.stripX[d] = -1;
    detData.stripY[d] = -1;
    detData.energy[d] = NAN;
    detData.energyNotSmeared[d] = NAN;

    detData.hitPositionX[d]=NAN;
    detData.hitPositionY[d]=NAN;
    detData.hitPositionZ[d]=NAN;
  }
  

  detData.grapeDetMul=0;
  //detData.grapeCryMul=0;
  //detData.grapeSegMul=0;
  for(Int_t d=0; d<grapeMaxDet; d++){
    detData.grapeCryMul[d]=0;
    detData.grapeDetEnergy[d]=NAN;
    for(Int_t c=0; c<grapeMaxCry; c++){
      detData.grapeSegMul[d][c]=0;
      detData.grapeCryEnergy[d][c]=NAN;
      for(Int_t s=0; s<grapeMaxSeg; s++){
        detData.grapeSegEnergy[d][c][s]=NAN;
      }
    }
  }
  //detData.grapeEnergy=NAN;  
  //detData.grapeDet=-1;
  //detData.grapeCrystal=-1;
  //detData.grapeSegment=-1;

  for(Int_t d=0; d<NUMBEROFDALI2CRYSTALS; d++){
    detData.fDaliCrystalFlag[d] = false;
    detData.fDaliCrystalEnergy[d] = NAN;
    detData.fDaliCrystalMult = 0;
    detData.fDaliCrystalTime[d] = 0.0;
    detData.fDaliFITime[d] = 0.0;
    detData.fDaliFIX[d] = NAN;
    detData.fDaliFIY[d] = NAN;
    detData.fDaliFIZ[d] = NAN;

  }

  detData.targetEnergyLoss=0.0;

}





Bool_t DetectorInfo::IsInFront(Int_t id1, Int_t id2)
{
  // this routine checks if detector 1 is closer to the center of target than detector 2
  // if both detectors have the same distance to target, than false is returned
  
  if(id2<0 && id1>-1){
    return true;
  }
  if(id1>maxDetectors){
    return false;
  }
  if(id2>maxDetectors){
    return true;
  }

  TVector3 detCenter1, detCenter2;

  detCenter1.SetXYZ(GetCenterX(id1), GetCenterY(id1), GetCenterZ(id1));
  detCenter2.SetXYZ(GetCenterX(id2), GetCenterY(id2), GetCenterZ(id2));

  if(detCenter2.Mag() > detCenter1.Mag()){
    return true;
  }else{
    return false;
  }

}











void DetectorInfo::CalcStripNumbers(Int_t detID, Double_t hx, Double_t hy, Double_t hz, Int_t &stripx, Int_t &stripy)
{
  //printf("inside: calculating strip no x\n");
  TVector3 vhit(hx, hy, hz); // vector from origin to FI
  TVector3 vcen(GetCenterX(detID), GetCenterY(detID), GetCenterZ(detID)); // vector from origin to center detector

  TVector3 vdet=vhit-vcen; // hit with respect to center of detector
  

  if( (strcmp(detGeo[detID].type.c_str(), "DSSDbox")==0) || (strcmp(detGeo[detID].type.c_str(), "CsIbox")==0) ){

    vdet.RotateX(GetRotationX(detID));
    vdet.RotateY(GetRotationY(detID));
    vdet.RotateZ(GetRotationZ(detID));
    
    TVector3 vori(GetSize0(detID)/2.0, GetSize1(detID)/2.0, 0.0); // vector to move origin to center of lower left corner of detector
  
    TVector3 vpix=vdet+vori;

    stripx = (Int_t)(vpix.X()/(GetSize0(detID)/GetNoStripsX(detID)));
    stripy = (Int_t)(vpix.Y()/(GetSize1(detID)/GetNoStripsY(detID)));

  } else if( (strcmp(detGeo[detID].type.c_str(), "DSSDtube")==0) ||
             (strcmp(detGeo[detID].type.c_str(), "CsItube")==0) ||
             (strcmp(detGeo[detID].type.c_str(), "Altube")==0) 
           ){

    TVector3 vRot(1.0, 0.0, 0.0); // rotation vector

    vRot.RotateZ(GetSize3(detID) + GetSize4(detID)/2.0);
    vRot.RotateZ(-GetRotationY(detID));
    vRot.RotateZ(90*deg);

    vdet.RotateZ((GetSize3(detID) + GetSize4(detID)/2.0));
    vdet.Rotate(GetRotationX(detID), vRot); // theta rotation
    vdet.RotateZ(GetRotationY(detID));

    
    Double_t phips = GetSize4(detID)/((Double_t)GetNoStripsX(detID));  // angle coverage phi per x strip
    Double_t radps = (GetSize1(detID)-GetSize0(detID))/((Double_t)GetNoStripsY(detID)); // radius coverage per y strip
    
    Double_t hitPhi = vdet.Phi(); // vector: phi = -pi ... pi
    if(hitPhi<0.0){ // detector: phi = 0 ... 2pi
      hitPhi+=2.0*TMath::Pi();
    }
    Double_t hitRad = vdet.Mag();
    
    stripx = (Int_t)((hitPhi-GetSize3(detID))/phips);
    stripy = (Int_t)((hitRad-GetSize0(detID))/radps);
    
    //printf("FIglob: %f %f %f\n", hx, hy, hz);
    //printf("FIdet:  %f %f %f\n", vdet.x(), vdet.y(), vdet.z());
    //printf("hitPhi %f, phi_s %f, phips %f;      hitRad %f, radMin %f, radps %f\n", hitPhi, GetSize3(detID), phips, hitRad, GetSize0(detID), radps);
    //printf("CalcStrip: got strip x = %d, strip y = %d\n\n", stripx, stripy);

  } else {
    stripx = -1;
    stripy = -1;
  }
  
}






void DetectorInfo::CalcHitPosition(Int_t detID, Int_t stripx, Int_t stripy, Double_t &hx, Double_t &hy, Double_t &hz)
{

  TVector3 vcen(GetCenterX(detID), GetCenterY(detID), GetCenterZ(detID)); // vector from origin to center detector
  TVector3 vdet(0.0, 1.0, 0.0);

  if( (strcmp(detGeo[detID].type.c_str(), "DSSDbox")==0) || (strcmp(detGeo[detID].type.c_str(), "CsIbox")==0) ){

    Double_t pixx = (Double_t)stripx * (GetSize0(detID)/GetNoStripsX(detID)) + (GetSize0(detID)/GetNoStripsX(detID))/2.0;
    Double_t pixy = (Double_t)stripy * (GetSize1(detID)/GetNoStripsY(detID)) + (GetSize1(detID)/GetNoStripsY(detID))/2.0;

    TVector3 vpix(pixx, pixy, 0.0);

    TVector3 vori(GetSize0(detID)/2.0, GetSize1(detID)/2.0, 0.0);

    vdet = vpix-vori;

    vdet.RotateZ(-GetRotationZ(detID));
    vdet.RotateY(-GetRotationY(detID));
    vdet.RotateX(-GetRotationX(detID));


  } else if( (strcmp(detGeo[detID].type.c_str(), "DSSDtube")==0) || 
             (strcmp(detGeo[detID].type.c_str(), "CsItube")==0) ||
             (strcmp(detGeo[detID].type.c_str(), "Altube")==0) 
           ){

    Double_t phips = GetSize4(detID)/((Double_t)GetNoStripsX(detID));  // angle coverage phi per x strip
    Double_t radps = (GetSize1(detID)-GetSize0(detID))/((Double_t)GetNoStripsY(detID)); // radius coverage per y strip

    Double_t hitPhi = (Double_t)(stripx) * phips + GetSize3(detID) + phips/2.0 ;
    Double_t hitRad = (Double_t)(stripy) * radps + GetSize0(detID) + radps/2.0;

    if(hitPhi>TMath::Pi()){
      hitPhi-=2.0*TMath::Pi();
    }

    // vector from origin of detector to center of strip
    vdet.SetMag(hitRad);
    vdet.SetPhi(hitPhi);
    
    TVector3 vRot(1.0, 0.0, 0.0); // rotation vector

    vRot.RotateZ(GetSize3(detID) + GetSize4(detID)/2.0);
    vRot.RotateZ(-GetRotationY(detID));
    vRot.RotateZ(90*deg);

    vdet.RotateZ(-GetRotationY(detID));
    vdet.Rotate(-GetRotationX(detID), vRot); // theta rotation
    vdet.RotateZ(-(GetSize3(detID) + GetSize4(detID)/2.0));



  } else {
    printf("<DetectorInfo::CalcHitPosition> invalid geometry type '%s'! Cannot determine position!\n", detGeo[detID].type.c_str());
    abort();
  }


  TVector3 vhit=vdet+vcen;

  hx = vhit.X();
  hy = vhit.Y();
  hz = vhit.Z();
}





void DetectorInfo::CalcHitPosition(Int_t detID, Int_t stripx, Int_t stripy)
{
  CalcHitPosition(detID, stripx, stripy, detData.hitPositionX[detID], detData.hitPositionY[detID], detData.hitPositionZ[detID]);
}



Double_t DetectorInfo::SmearEnergy(Int_t detID, Double_t energy)
{
  switch(GetResType(detID)){
    case 0: return energy;
    case 1: // gausian
            return fRandomizer->Gaus(energy, GetResPar(detID, 0));
    default: return energy;
  }
}







void DetectorInfo::Parse(string filename)
{
  
  printf("\n\nParsing of detector geometry file\n");


  //check if in/out file exists
  std::ifstream fin;
  std::ifstream FileTest(filename.c_str());
  if(!FileTest){
    printf("Can not open file '%s'! Does it exist?\n", filename.c_str());
    abort();
  }else{
    printf("Opening file '%s'\n", filename.c_str());
    fin.open(filename.c_str());
  }


  //parse input file line-wise
  std::string line;
  Int_t counter=-1;
  const Int_t stopper=1000000;

  const Int_t maxArg=20;
  char temp[maxArg][200];



  while(fin.good())  {

    counter++;

    //reset values
    for(Int_t i=0; i< maxArg; i++){temp[i][0]='\0';}


    getline(fin,line);
    //printf("Parsing line '%s'\n",line.c_str());



    //skip comments
    line.copy(temp[0],2,0); //first two characters
    temp[0][2]='\0';
    //printf("temp1=%s\n",temp1);
    if(strcmp(temp[0],"//")==0){continue;}

    //parse the line
    //write each word to a char
    //delimiter is whitespace
    std::istringstream iss(line);
    for(Int_t i=0; i<maxArg; i++){iss >> temp[i];}

    //skip empty lines
    if(strcmp(temp[0],"")==0){continue;}



    //get input parameter:

    // file names   
    if(strcmp(temp[0],"number_of_detectors")==0)  {
      fNoOfDet=atoi(temp[1]);
      printf("Number of detectors %d\n", fNoOfDet);
    }
    else if(strcmp(temp[0],"position")==0){
      Int_t index = atoi(temp[1]);
      SetCenterX(index, atof(temp[2])*mm);
      SetCenterY(index, atof(temp[3])*mm);
      SetCenterZ(index, atof(temp[4])*mm);
      printf("Got position of detector %d: x = %f, y = %f, z = %f\n", index, GetCenterX(index), GetCenterY(index), GetCenterZ(index));
    }
    else if(strcmp(temp[0],"rotation")==0){
      Int_t index = atoi(temp[1]);
      SetRotationX(index, atof(temp[2])*deg);
      SetRotationY(index, atof(temp[3])*deg);
      SetRotationZ(index, atof(temp[4])*deg);
      printf("Got rotation of detector %d: x = %f, y = %f, z = %f\n", index, GetRotationX(index), GetRotationY(index), GetRotationZ(index));
    }
    else if(strcmp(temp[0],"size")==0){
      Int_t index = atoi(temp[1]);
      SetSize0(index, atof(temp[2])*mm);
      SetSize1(index, atof(temp[3])*mm);
      SetSize2(index, atof(temp[4])*mm);
      SetSize3(index, atof(temp[5])*deg);
      SetSize4(index, atof(temp[6])*deg);
      printf("Got full size of detector %d: 1st (x or r_min) = %f, 2nd (y or r_max) = %f, 3rd (z) = %f, 4th (void or phi_start) = %f, 5th (void or phi_D) = %f\n", index, GetSize0(index), GetSize1(index), GetSize2(index), GetSize3(index), GetSize4(index));
    }
    else if(strcmp(temp[0],"number_of_stripes")==0){
      Int_t index = atoi(temp[1]);
      SetNoStripsX(index, atoi(temp[2]));
      SetNoStripsY(index, atoi(temp[3]));
      printf("Got number of strips of detector %d: x = %d, y = %d\n", index, GetNoStripsX(index), GetNoStripsY(index));
    }
    else if(strcmp(temp[0],"name")==0){
      printf("Info: 'name' not yet implemented (is 'logical').\n");
    }
    else if(strcmp(temp[0],"type")==0){
      Int_t index = atoi(temp[1]);
      SetType(index, temp[2]);
      printf("Got for detector %d type '%s'.\n", index, GetType(index).c_str());
    }
    else if(strcmp(temp[0],"resolution")==0){
      Int_t index = atoi(temp[1]);
      Int_t type  = atoi(temp[2]);
      
      SetResType(index, type);
      printf("Got for detector %d Resolution type %d, parameter: ", index, GetResType(index));
      
      for(Int_t p=0; p<GetResNoPars(index); p++){
        SetResPar(index, p, atof(temp[3+p]));
        printf("%f ", GetResPar(index, p));
      }
      printf("\n");
    }
    else if(strcmp(temp[0],"position_detectors")==0){
      //Int_t nPosDet=0;
      printf("Setting position sensitive detectors: ");
      for(Int_t d=1; d<maxArg; d++){
        string tmpstr = temp[d];
        if(tmpstr.size()>0){
          Int_t pdi=atoi(tmpstr.c_str());
          printf("%d ", pdi);
          SetPosDet(pdi, true);
        }
      }
      printf("\n");
    }
    else if(strcmp(temp[0],"shielding_material")==0){
      printf("Setting passive shielding material: ");
      for(Int_t d=1; d<maxArg; d++){
        string tmpstr = temp[d];
        if(tmpstr.size()>0){
          Int_t pdi=atoi(tmpstr.c_str());
          printf("%d ", pdi);
          SetShieldingID(pdi, true);
        }
      }
      printf("\n");
    }
    else if(strcmp(temp[0],"shielding_simple")==0){
      printf("Setting simplified shielding (for analyzer): ");
      fShieldSimple = true;
      fShieldSimpleMat = temp[1];
      fShieldSimpleThickness = atof(temp[2]);
      fShieldSimpleTheta = atof(temp[3])/180.0*TMath::Pi();
      fShieldSimpleDensity = atof(temp[4]);
      printf("Simplified shielding material %s, thickness %lf mm, theta %lf rad, density %lf mg/cm3\n", fShieldSimpleMat.c_str(), fShieldSimpleThickness, fShieldSimpleTheta, fShieldSimpleDensity);

    }
    else if(strcmp(temp[0],"include_dali")==0){
      fIncludeDali=true;
      printf("Including %d DALI detectors\n", fNumberOfGrapeDetectors);
    }
    else if(strcmp(temp[0],"include_grape")==0){
      fIncludeGrape=true;
      fNumberOfGrapeDetectors=atoi(temp[1]);
      printf("Including %d GRAPE detectors\n", fNumberOfGrapeDetectors);
    }
    else if(strcmp(temp[0],"include_beam_pipe")==0){
      fIncludeBeamPipe=true;
      printf("Inlcuding beam pipe\n");
    }
    //else if(strcmp(temp[0],"target")==0){
    //  fTarget.material = temp[1];
    //  
    //  fTarget.center[0] = atof(temp[2]);
    //  fTarget.center[1] = atof(temp[3]);
    //  fTarget.center[2] = atof(temp[4]);

    //  fTarget.size[0] = atof(temp[5]);
    //  fTarget.size[1] = atof(temp[6]);
    //  fTarget.size[2] = atof(temp[7]);

    //  printf("Got target material '%s', center x %f mm, y %f mm, z %f mm, full size x %f mm, y %f mm, z %f mm\n", fTarget.material.c_str(), fTarget.center[0], fTarget.center[1], fTarget.center[2], fTarget.size[0], fTarget.size[1], fTarget.size[2]);
    //  
    //}
    else {
      printf("Error in DetectorInfo: Cannot read your input keyword '%s' in line %d. Aborting program.\n", temp[0], counter);
      abort();
    }

    // todo: add resolution information

    //count lines
    if(counter>stopper){
      printf("Reached %d lines in file '%s'! Stopping!\n", counter, filename.c_str());
      abort();
    }
  } // end of reading input file

  
  printf("DetectorInfo::Parse done! Checking input...\n");
  CheckInput();


}












void DetectorInfo::CheckInput()
{
  
  if(fNoOfDet>maxDetectors){
    printf("Error: more detectors in input file (%d) than allowed in DetectorGlobals.hh (%d)\n", fNoOfDet, maxDetectors);
    abort();
  }

  if(fNoOfDet==0){
    printf("Info: number of detectors is 0!\n");
  }else if(fNoOfDet<0){
    printf("Error: invalid (negative) number of detectors: %d\n", fNoOfDet);
    abort();
  }

  for(Int_t d=0; d<fNoOfDet; d++){
    Double_t checkSum=0.0;
    
    if( !( (strcmp(detGeo[d].type.c_str(),"DSSDbox")==0) || 
           (strcmp(detGeo[d].type.c_str(),"DSSDtube")==0) || 
           (strcmp(detGeo[d].type.c_str(),"CsItube")==0) || 
           (strcmp(detGeo[d].type.c_str(),"Altube")==0) || 
           (strcmp(detGeo[d].type.c_str(),"CsIbox")==0)
         )){
      printf("Error: invalid detector type '%s' for detector %d!\n", GetType(d).c_str(), d);
      abort();
    }
    
    checkSum = detGeo[d].center[0] * detGeo[d].center[1] * detGeo[d].center[2];
    if(TMath::IsNaN(checkSum)){
      printf("Error: invalid position for detector %d: x = %f, y = %f, z = %f\n", d, detGeo[d].center[0], detGeo[d].center[1], detGeo[d].center[2]);
      abort();
    }

    checkSum = detGeo[d].rotation[0] * detGeo[d].rotation[1] * detGeo[d].rotation[2];
    if(TMath::IsNaN(checkSum)){
      printf("Error: invalid rotation for detector %d: x = %f, y = %f, z = %f\n", d, detGeo[d].rotation[0], detGeo[d].rotation[1], detGeo[d].rotation[2]);
      abort();
    } 

    checkSum = detGeo[d].size[0] * detGeo[d].size[1] * detGeo[d].size[2];
    if(TMath::IsNaN(checkSum) || checkSum==0){
      printf("Error: invalid size for detector %d: 1st = %f, 2nd = %f, 3rd = %f\n", d, detGeo[d].size[0], detGeo[d].size[1], detGeo[d].size[2]);
      abort();
    }
    checkSum = detGeo[d].size[3] * detGeo[d].size[4];
    if( ( (strcmp(detGeo[d].type.c_str(),"DSSDtube")==0) ||
          (strcmp(detGeo[d].type.c_str(),"CsItube")==0) ||
          (strcmp(detGeo[d].type.c_str(),"Altube")==0)
        ) && TMath::IsNaN(checkSum) ){
      printf("Error: invalid angle for detector %d type '~tube': phi_start = %f, phi_D = %f\n", d, detGeo[d].size[3], detGeo[d].size[4]);
      abort();
    }
    
    if( (detGeo[d].noStrips[0] < 0) || (detGeo[d].noStrips[1] < 0) ){
      printf("Error: invalid number of strips in detector %d: %d x, %d y (have to be nonnegative; 0 and 1 is same geometry)\n", d, detGeo[d].noStrips[0], detGeo[d].noStrips[1]);
      abort();
    }

  }

  // todo: check if detector resolution type is valid!!!!!!

  //for testing:
  printf("CheckInput completed sucessfully!\n");
  //abort();
}








TRotation* DetectorInfo::GetRotationMatrix(Int_t d)
{
  
  TVector3 vCen(1.0, 0.0, 0.0); // vector through center of detector
  TVector3 vRot(0.0, 1.0, 0.0); // rotation vector

  vCen.RotateZ(detGeo[d].size[3] + detGeo[d].size[4]/2.0);
  vRot.RotateZ(detGeo[d].size[3] + detGeo[d].size[4]/2.0);

  
  TRotation* rotMat = new TRotation();
  
  rotMat->RotateZ(detGeo[d].size[3] + detGeo[d].size[4]/2.0);
  rotMat->Rotate(detGeo[d].rotation[2], vCen); // around detectors axis
  rotMat->Rotate(detGeo[d].rotation[0], vRot); // theta rotation
  rotMat->RotateZ(detGeo[d].rotation[1]);      // phi rotation


  return rotMat;

}



TRotation DetectorInfo::GetInverseRotationMatrix(Int_t d)
{
  TRotation rotMat = GetRotationMatrix(d)->Inverse();

  return rotMat;
}





