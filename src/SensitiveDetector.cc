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
	G4String HCname; 
	collectionName.insert(HCname="HitCollection");
	a=2;
}

SensitiveDetector::~SensitiveDetector()
{

}

void SensitiveDetector::Initialize(G4HCofThisEvent* hitsColl)
{
	G4cout << SensitiveDetectorName << G4endl;
	HitCollection = new TrackHitCollection(SensitiveDetectorName,collectionName[0]);	
  G4int HCID =  G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	hitsColl->AddHitsCollection(HCID,HitCollection); 
	a=3;
}

G4bool SensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory* THist)
{
	HitCollection = GetInfo();
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

	G4cout << "step point.." << G4endl;

	thisHit = new TrackHit();

	//Information to be stored step by step
	//*****************************************************************
	//a=10;
	thisHit->SetID(5);
	G4cout << postMom << " " << a << G4endl; 
	G4cout << HitCollection << G4endl;
	//*****************************************************************
	HitCollection->insert( thisHit );

	G4cout << "process event.." << G4endl;

	return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent* hitsColl)
{
	//G4cout << thisHit->GetID() << G4endl;
	G4cout << "end of event.."  << a << G4endl;
	thisHit = new TrackHit();
	//Total information about all event
	//*****************************************************************


	//*****************************************************************
	HitCollection->insert(thisHit);
}

