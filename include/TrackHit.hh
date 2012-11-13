#ifndef TrackHit_h
#define TrackHit_h 1

#include "G4VHit.hh" 
#include "G4THitsCollection.hh"

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

	void SetID(G4double val) {ID=val;}
	G4double GetID() {return ID;}
	
private :	
	G4double ID;

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
