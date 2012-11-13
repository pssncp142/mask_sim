#ifndef SensitiveDetector_h
#define SensitiveDetector_h 1

#include "TrackHit.hh"

#include "G4VSensitiveDetector.hh"

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;
class G4Track;
class G4StepPoint; 

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
	TrackHit* thisHit;
};

#endif
