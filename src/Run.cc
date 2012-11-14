#include "Run.hh"
#include "TrackHit.hh"

#include "G4Run.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"

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
	 
  TrackHitCollection* HC = (TrackHitCollection*)(HCE->GetHC(collectionID));
	G4cout << HC->GetSDname() << "   " << HC->GetName() << G4endl;

	G4int NbHits = HC->entries()-1;  
  
	for (int i=0;i<NbHits;i++)
	{
		TrackHit* thisHit = (*HC)[i];	
		G4cout << thisHit->GetParName() << " " << G4BestUnit(thisHit->GetPreTotEn(),"Energy") << " " << G4endl;
		//G4BestUnit(thisHit->GetPrePVName(),"Energy") << G4endl;
	}

}
