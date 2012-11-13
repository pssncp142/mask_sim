#include "TrackHit.hh"

#include "G4VHit.hh"

G4Allocator<TrackHit> TrackHitAllocator;

TrackHit::TrackHit()
{
	G4cout << "track hit" << G4endl;
}

TrackHit::TrackHit(const TrackHit& right) : G4VHit()
{
}

TrackHit::~TrackHit()
{
	
}

const TrackHit& TrackHit::operator=(const TrackHit& right)
{
	return *this;
}

G4int TrackHit::operator==(const TrackHit& right) const
{
  return (this==&right) ? 1 : 0;
}

void TrackHit::Print()
{

}

void TrackHit::Draw()
{

}
