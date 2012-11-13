#include "Run.hh"
#include "TrackHit.hh"

#include "G4Run.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"

Run::Run(const std::vector<G4String> SDName) : G4Run()
{

}

Run::~Run()
{

}

void Run::RecordEvent(const G4Event* aEvent)
{
	G4HCofThisEvent* HCE = aEvent->GetHCofThisEvent();
  if (!HCE) return;
  
  G4SDManager* fSDM = G4SDManager::GetSDMpointer();
  G4int collectionID = fSDM->GetCollectionID("HitCollection");
  TrackHitCollection * HC = (TrackHitCollection*)(HCE->GetHC(collectionID));

	G4int NbHits = HC->entries()-1;  
  TrackHit* thisHit = (*HC)[NbHits];	

	
}
