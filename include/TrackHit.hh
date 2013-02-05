#ifndef TrackHit_h
#define TrackHit_h 1

#include "G4VHit.hh" 
#include "G4THitsCollection.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4VPhysicalVolume.hh"

class TrackHit : public G4VHit
{

public :
  TrackHit();
  ~TrackHit();
  TrackHit(const TrackHit &right);
  
  const TrackHit& operator=(const TrackHit&);
  G4int operator==(const TrackHit&) const;
  
  inline void * operator new(size_t);
  inline void operator delete(void *aHit);

  void Draw();
  void Print();
  
  /*************************************************************************************/	
  //GETS
  G4String GetParName() {return parName;}
  G4double GetX() {return xx;}
  G4double GetY() {return yy;}
  G4double GetZ() {return zz;}
  G4double GetEdep() {return edep;}
  G4double GetGlobalTime() {return gTime;}
  G4double GetTotalEnergy() {return ener;}
  G4double GetParentID() {return parentID;}
  G4double GetTrackID() {return trackID;}
  G4ThreeVector GetVertexPos() {return vertexPos;}
  /*************************************************************************************/
  //SETS
  void SetParName(G4String val)	{parName=val;};
  void SetX(G4double val) {xx=val;};
  void SetY(G4double val) {yy=val;};
  void SetZ(G4double val) {zz=val;};
  void SetEdep(G4double val) {edep=val;};
  void SetGlobalTime(G4double val) {gTime=val;};
  void SetTotalEnergy(G4double val) {ener=val;};
  void SetParentID(G4double val) {parentID=val;};
  void SetTrackID(G4double val) {trackID=val;};
  void SetVertexPos(G4ThreeVector val) {vertexPos=val;};
  /*************************************************************************************/	
private :	
  /*************************************************************************************/
  //VARIABLES
  G4String parName;
  G4double xx;
  G4double yy;
  G4double zz;
  G4double edep; 
  G4double gTime;			
  G4double ener;
  G4double parentID;
  G4double trackID;
  G4ThreeVector vertexPos;
  /*************************************************************************************/

};

typedef G4THitsCollection<TrackHit> TrackHitCollection;

extern G4Allocator<TrackHit> TrackHitAllocator;

void* TrackHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) TrackHitAllocator.MallocSingle();
  return aHit;
}

void TrackHit::operator delete(void *aHit)
{
  TrackHitAllocator.FreeSingle((TrackHit*) aHit);
}

#endif
