/*********************************************************************************************
* SensitiveDetector.cc  (Yigit Dallilar)
*
* Purpose of this code is to get information on hits...
**********************************************************************************************/

#include "SensitiveDetector.hh"
#include "TrackHit.hh"

#include "G4VSensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"

/**********************************************************************************************/

SensitiveDetector::SensitiveDetector(G4String sname) : G4VSensitiveDetector(sname)
{
	G4String HCname; 
	collectionName.insert(HCname="HitCollection");
}

/**********************************************************************************************/

SensitiveDetector::~SensitiveDetector()
{

}

/**********************************************************************************************/

void SensitiveDetector::Initialize(G4HCofThisEvent* hitsColl)
{
  HitCollection = new TrackHitCollection(SensitiveDetectorName,collectionName[0]);
  G4int HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hitsColl->AddHitsCollection(HCID,HitCollection); 
}

/**********************************************************************************************/

G4bool SensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory* THist)
{
	aTrack = aStep->GetTrack();
	preStep = aStep->GetPreStepPoint();
	postStep = aStep->GetPostStepPoint();	
  
	// track information
	G4String parName = aTrack->GetParticleDefinition()->GetParticleName(); 
	G4double gTime = aTrack->GetGlobalTime(); 
	G4ThreeVector pos = aTrack->GetPosition();
	//G4ThreeVector vertex_pos = preStep->GetPosition();
	//G4ThreeVector vertex_pos = aTrack->GetVertexPosition();
	G4double xx = pos.x();
	G4double yy = pos.y();
	G4double zz = pos.z();
	G4double edep = aStep->GetTotalEnergyDeposit();
  G4double ener = preStep->GetTotalEnergy();
  //G4double parentID = aTrack->GetParentID();
  //G4double trackID = aTrack->GetTrackID();
	//G4cout << parName;
	TrackHit* thisHit = new TrackHit();

	//Information to be stored step by step
	//*****************************************************************
	thisHit->SetParName(parName);
	thisHit->SetX(xx);
	thisHit->SetY(yy);
	thisHit->SetZ(zz);
	thisHit->SetEdep(edep);
	thisHit->SetGlobalTime(gTime);
	thisHit->SetTotalEnergy(ener);
	//thisHit->SetParentID(parentID);
	//thisHit->SetTrackID(trackID);
	//thisHit->SetVertexPos(vertex_pos);
	//*****************************************************************
	HitCollection->insert(thisHit);
	return false;
}

/**********************************************************************************************/

void SensitiveDetector::EndOfEvent(G4HCofThisEvent* hitsColl)
{
	//G4cout << "End of event.." << G4endl;
	TrackHit* thisHit = new TrackHit();

	//Total information about all event
	//*****************************************************************

	//*****************************************************************

	//G4cout << a << G4endl;
	HitCollection->insert( thisHit );
}

/**********************************************************************************************/
