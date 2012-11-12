#include "RunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"

RunAction::RunAction()
{
	NofRuns = 0;
}

RunAction::~RunAction()
{}

/*G4Run* GenerateRun()
{
		G4cout << "Run Started...!!" << G4endl;
}*/

void RunAction::BeginOfRunAction(const G4Run* myRun)
{
	if (NofRuns != 0)
	G4cout << G4endl <<
	"***************************************************************************************" <<
	G4endl <<
	"                                 Run " << NofRuns << " started" << 
	G4endl <<
	"---------------------------------------------------------------------------------------" << 
	G4endl << G4endl;
	//G4RunManager()::GeometryHasBeenModified();
}

void RunAction::EndOfRunAction(const G4Run* myRun)
{
	if (NofRuns != 0)
	G4cout <<
	G4endl <<
	"---------------------------------------------------------------------------------------" << 
	G4endl <<
	"                                 Run " << NofRuns << " finished" << 
	G4endl << 
	"***************************************************************************************" <<	
	G4endl;
	
	NofRuns++;
}
