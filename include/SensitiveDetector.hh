#ifndef SensitiveDetector_h
#define SensitiveDetector_h 1

#include "TrackHit.hh"

#include "G4VSensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh" 

class SensitiveDetector : public G4VSensitiveDetector
{
	public : 
	SensitiveDetector(G4String);
	SensitiveDetector(const G4VSensitiveDetector &right);
	~SensitiveDetector();

	void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);

	private :

	TrackHitCollection* GetInfo() {return HitCollection;};
	
	G4int a;
	G4Track* aTrack;
	G4StepPoint* preStep;
	G4StepPoint* postStep;
	TrackHitCollection* HitCollection;
	//TrackHit* thisHit;
	G4String parName;
};

#endif
