#ifndef DetectorInfo_h
#define DetectorInfo_h 1

#include "globals.hh"

#define maxDetectors 2

typedef struct _geo
{

  G4String name;
  G4String type;

  G4double center[3];

  G4double rotation[3]; // rotation angle 

  G4double size[3]; // size in x, y, z direction (detector coordinate system)

  G4int noStrips[2]; // in x and y

} geo;


class DetectorInfo
{
  public:
    DetectorInfo();
    virtual ~DetectorInfo();

    void Parse(G4String filename);    
    
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

        G4double GetSizeX(G4int d) const { return det[d].size[0] ;}
        G4double GetSizeY(G4int d) const { return det[d].size[1] ;}
        G4double GetSizeZ(G4int d) const { return det[d].size[2] ;}

       G4int GetNoStripsX(G4int d) const { return det[d].noStrips[0]; }
       G4int GetNoStripsY(G4int d) const { return det[d].noStrips[1]; }

         G4String GetName(G4int d) { return det[d].name; }
         G4String GetType(G4int d) { return det[d].type; }



    // setter
    
      void SetCenterX(G4int d, G4double c) { det[d].center[0]=c ;}
      void SetCenterY(G4int d, G4double c) { det[d].center[1]=c ;}
      void SetCenterZ(G4int d, G4double c) { det[d].center[2]=c ;}

    void SetRotationX(G4int d, G4double r) { det[d].rotation[0]=r;}
    void SetRotationY(G4int d, G4double r) { det[d].rotation[1]=r;}
    void SetRotationZ(G4int d, G4double r) { det[d].rotation[2]=r;}

        void SetSizeX(G4int d, G4double s) { det[d].size[0]=s ;}
        void SetSizeY(G4int d, G4double s) { det[d].size[1]=s ;}
        void SetSizeZ(G4int d, G4double s) { det[d].size[2]=s ;}

    void SetNoStripsX(G4int d, G4int n)    { det[d].noStrips[0]=n; }
    void SetNoStripsY(G4int d, G4int n)    { det[d].noStrips[1]=n; }

  private:

    geo det[maxDetectors];
    G4int fNoOfDet; // number of detectors


};



#endif
