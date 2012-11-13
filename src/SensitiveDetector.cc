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
	G4cout << collectionName[0] << " tralla " << G4endl;
}

SensitiveDetector::~SensitiveDetector()
{
	//delete HitCollection;
	G4cout << "Hitcollection is deleted" << G4endl;
}

void SensitiveDetector::Initialize(G4HCofThisEvent* hitsColl)
{
	HitCollection = new TrackHitCollection(SensitiveDetectorName,collectionName[0]);
  G4int HCID =  G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	hitsColl->AddHitsCollection(HCID,HitCollection); 
	G4cout << "initialise " << a << G4endl;
}

G4bool SensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory* THist)
{
	//HitCollection = GetInfo();
	aTrack = aStep->GetTrack();
	preStep = aStep->GetPreStepPoint();
	postStep = aStep->GetPostStepPoint();
	//G4double totEn = aStep->GetTotalEnergyDeposit();	

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
	G4cout << "step point.." << G4endl;

	thisHit = new TrackHit();

	//Information to be stored step by step
	//*****************************************************************
	thisHit parName
	//*****************************************************************
	HitCollection->insert( thisHit );

	G4cout << "process event.." << G4endl;

	return false;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent* hitsColl)
{
	//G4cout << thisHit->GetID() << G4endl;
	G4cout << a << "  end of event.."  << HitCollection <<G4endl;
	thisHit = new TrackHit();
	//Total information about all event
	//*****************************************************************


	//*****************************************************************
	HitCollection->insert( thisHit );
}

