#include "SensitiveDetector.hh"
#include "TrackHit.hh"

#include "G4VSensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"

SensitiveDetector::SensitiveDetector(G4String sname) : G4VSensitiveDetector(sname)
{
	a=0;
	G4String HCname; 
	collectionName.insert(HCname="HitCollection");
}

SensitiveDetector::~SensitiveDetector()
{

}

void SensitiveDetector::Initialize(G4HCofThisEvent* hitsColl)
{
	HitCollection = new TrackHitCollection(SensitiveDetectorName,collectionName[0]);
  G4int HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	G4cout << "HCID  : " << HCID << G4endl;
	hitsColl->AddHitsCollection(HCID,HitCollection); 
	//G4cout << "Start of Event" << G4endl;
}

G4bool SensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory* THist)
{
	aTrack = aStep->GetTrack();
	preStep = aStep->GetPreStepPoint();
	postStep = aStep->GetPostStepPoint();	
	// step information

	// previous point information
	G4VPhysicalVolume * prePV  			 =  preStep->GetPhysicalVolume();
  G4String            prePVName    =  prePV->GetName();
  G4ThreeVector 			prePos       =  preStep->GetPosition();
  G4ThreeVector 			preMomDir    =  preStep->GetMomentumDirection();
  G4Material *  			preMat       =  preStep->GetMaterial();
  G4String      			preMatname   =  preMat->GetName();
  G4double      			preTotEn     =  preStep->GetTotalEnergy();
  G4ThreeVector 			preMom       =  preStep->GetMomentum();
  G4double      			preKinEn     =  preStep->GetKineticEnergy();

	// post point information
	G4VPhysicalVolume * postPV  		 =  postStep->GetPhysicalVolume();
  G4String            postPVName   =  postPV->GetName();
  G4ThreeVector 			postPos      =  postStep->GetPosition();
  G4ThreeVector 			postMomDir   =  postStep->GetMomentumDirection();
  G4Material *  			postMat      =  postStep->GetMaterial();
  G4String      			postMatname  =  postMat->GetName();
  G4double      			postTotEn    =  postStep->GetTotalEnergy();
  G4ThreeVector 			postMom      =  postStep->GetMomentum();
  G4double      			postKinEn    =  postStep->GetKineticEnergy();

	// track information
	G4int 							trackID      =  aTrack->GetTrackID();
	G4int 							parentID     =  aTrack->GetParentID();
	G4String            parName      =  aTrack->GetParticleDefinition()->GetParticleName(); 

	TrackHit* thisHit = new TrackHit();

	//Information to be stored step by step
	//*****************************************************************
	thisHit->SetParName(parName);
	thisHit->SetPreTotEn(postTotEn);
	//*****************************************************************

	a = HitCollection->insert( thisHit );
	
	return false;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent* hitsColl)
{
	//G4cout << "End of event.." << G4endl;
	TrackHit* thisHit = new TrackHit();

	//Total information about all event
	//*****************************************************************
	thisHit->SetPrePVName("cücük");
	//*****************************************************************

	//G4cout << a << G4endl;
	HitCollection->insert( thisHit );
}

