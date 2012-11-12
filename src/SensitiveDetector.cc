#include "SensitiveDetector.hh"

#include "G4VSensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"

SensitiveDetector::SensitiveDetector(G4String sname) : G4VSensitiveDetector(sname)
{

}

SensitiveDetector::SensitiveDetector(const G4VSensitiveDetector &right) : G4VSensitiveDetector(right)
{

}

SensitiveDetector::~SensitiveDetector()
{

}

void SensitiveDetector::Initialize(G4HCofThisEvent* hitsColl)
{

}

void SensitiveDetector::EndOfThisEvent(G4HCofThisEvent* hitsColl)
{

}

G4bool SensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory* THist)
{

}

