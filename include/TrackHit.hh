#ifndef TrackHit_h
#define TrackHit_h 1

#include "G4VHit.hh" 
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

class TrackHit : public G4VHit
{
	 TrackHit();
	 ~TrackHit();
	 TrackHit(const TrackHit &right);

	 const TrackHit& operator=(const TrackHit &right);
   G4int operator==(const TrackHit &right) const;

   inline void * operator new(size_t);
   inline void operator delete(void *aHit);

   void Draw();
   void Print();
};

typedef G4THitsCollection<TrackHit> TrackHitsCollection;

extern G4Allocator<TrackHit> TrackHitAllocator;

inline void* TrackHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) TrackHitAllocator.MallocSingle();
  return aHit;
}

inline void TrackHit::operator delete(void *aHit)
{
  TrackHitAllocator.FreeSingle((TrackHit*) aHit);
}


#endif
