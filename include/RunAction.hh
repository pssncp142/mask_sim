#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4RunManager.hh"
#include "globals.hh"

class G4Run;

class RunAction : public G4UserRunAction
{

public:
  RunAction();
  ~RunAction();

	G4Run* GenerateRun();
  void   BeginOfRunAction(const G4Run*);
  void   EndOfRunAction(const G4Run*);
    
private:
	//G4RunManager* runManager;
	G4int NofRuns;
	std::vector<G4String> SDName; 

};

#endif

