#include "TrackHit.hh"

#include "G4VHit.hh"

G4Allocator<TrackHit> TrackHitAllocator;

TrackHit::TrackHit()
{
	
}

TrackHit::TrackHit(const TrackHit& right) : G4VHit()
{
	prePVName = right.prePVName;
	parName = right.parName;
	preTotEn = right.preTotEn;
}

TrackHit::~TrackHit()
{
	
}

const TrackHit& TrackHit::operator=(const TrackHit& right)
{
	preTotEn = right.preTotEn;
	prePVName = right.prePVName;
	parName = right.parName;
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
