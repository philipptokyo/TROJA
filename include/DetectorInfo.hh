#ifndef DetectorInfo_h
#define DetectorInfo_h 1

#include "globals.hh"

//#define maxDetectors 10
#include "DetectorGlobals.hh"


typedef struct _geo
{

  G4String name;
  G4String type;

  G4double center[3];
  G4double rotation[3]; // rotation angle 
  G4double size[5]; 
  // size for box: x, y, z direction (detector coordinate system)
  // size for tube: rmin, rmax, dz, phistart, phiD

  G4int noStrips[2]; // in x and y

} geo;


typedef struct _dat
{
  G4int eventNumber;

  G4double fIX, fIY, fIZ; // first interaction point, x, y, z

  //G4int noOfDet[maxDetectors];
  G4int fIDetID; // ID of detector with first interaction point

  G4int haveHit[maxDetectors]; // flag if detector was hit by particle
  G4int haveHitID[maxDetectors]; // aux for quicker debugging... 
  
  G4double energy[maxDetectors];
  G4int stripX[maxDetectors];
  G4int stripY[maxDetectors];

} dat;




class DetectorInfo
{
  public:
    DetectorInfo();
    virtual ~DetectorInfo();

    void Parse(G4String filename);    
    void CheckInput();

    void ResetData();
    void ClearGeometry();
    
    void CalcStripNumbers(G4int detID, G4double hx, G4double hy, G4double hz, G4int &stripx, G4int &stripy);
        

    // getter
   G4int GetMaxNoDetectors() const { return (G4int)maxDetectors; }
    G4int GetNoOfDetectors() const { return fNoOfDet; }
    
      G4double GetCenterX(G4int d) const { return det[d].center[0] ;}
      G4double GetCenterY(G4int d) const { return det[d].center[1] ;}
      G4double GetCenterZ(G4int d) const { return det[d].center[2] ;}

    G4double GetRotationX(G4int d) const { return det[d].rotation[0] ;}
    G4double GetRotationY(G4int d) const { return det[d].rotation[1] ;}
    G4double GetRotationZ(G4int d) const { return det[d].rotation[2] ;}

        G4double GetSize0(G4int d) const { return det[d].size[0] ;}
        G4double GetSize1(G4int d) const { return det[d].size[1] ;}
        G4double GetSize2(G4int d) const { return det[d].size[2] ;}
        G4double GetSize3(G4int d) const { return det[d].size[3] ;}
        G4double GetSize4(G4int d) const { return det[d].size[4] ;}

       G4int GetNoStripsX(G4int d) const { return det[d].noStrips[0]; }
       G4int GetNoStripsY(G4int d) const { return det[d].noStrips[1]; }
       
//           G4bool HaveHit(G4int d) const { return det[d].haveHit; }

         G4String GetName(G4int d) { return det[d].name; }
         G4String GetType(G4int d) { return det[d].type; }



    // setter
    
    void SetNoOfDetectors(G4int n) { fNoOfDet=n; }
    
      void SetCenterX(G4int d, G4double c) { det[d].center[0]=c ;}
      void SetCenterY(G4int d, G4double c) { det[d].center[1]=c ;}
      void SetCenterZ(G4int d, G4double c) { det[d].center[2]=c ;}

    void SetRotationX(G4int d, G4double r) { det[d].rotation[0]=r;}
    void SetRotationY(G4int d, G4double r) { det[d].rotation[1]=r;}
    void SetRotationZ(G4int d, G4double r) { det[d].rotation[2]=r;}

        void SetSize0(G4int d, G4double s) { det[d].size[0]=s ;}
        void SetSize1(G4int d, G4double s) { det[d].size[1]=s ;}
        void SetSize2(G4int d, G4double s) { det[d].size[2]=s ;}
        void SetSize3(G4int d, G4double s) { det[d].size[3]=s ;}
        void SetSize4(G4int d, G4double s) { det[d].size[4]=s ;}

    void SetNoStripsX(G4int d, G4int n)    { det[d].noStrips[0]=n; }
    void SetNoStripsY(G4int d, G4int n)    { det[d].noStrips[1]=n; }

//      void SetHaveHit(G4int d, G4bool h)   { det[d].haveHit=h; }

         void SetName(G4int d, G4String n) { det[d].name=n; }
         void SetType(G4int d, G4String n) { det[d].type=n; }
    
    dat detData; // todo: this should be private, ne


  private:

    geo det[maxDetectors];

    G4int fNoOfDet; // number of detectors


};



#endif
