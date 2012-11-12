#ifndef SensitiveDetector_h
#define SensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

class SensitiveDetector : public G4VSensitiveDetector
{
	public : 
	SensitiveDetector(G4String);
	SensitiveDetector(const G4VSensitiveDetector &right);
	~SensitiveDetector();

	void Initialize(G4HCofThisEvent*);
	void EndOfThisEvent(G4HCofThisEvent*);

	private :

	G4bool ProcessHits(G4Step*,G4TouchableHistory*);
};

#endif
