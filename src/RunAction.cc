#include "RunAction.hh"
#include "Run.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"

RunAction::RunAction()
{
	SDName.push_back("SensDetector");	
	NofRuns = 0;
}

RunAction::~RunAction()
{}

G4Run* RunAction::GenerateRun()
{
	
	G4cout << "Run Started...!!" << G4endl;
	//G4cout << SDName[0] << G4endl;
	Run* userRun = new Run(SDName);
  //userRun->SetRunID(m_nRunID);
  return userRun;
}

void RunAction::BeginOfRunAction(const G4Run* myRun)
{
	if (NofRuns >= 0)
	G4cout << G4endl <<
	"***************************************************************************************" <<
	G4endl <<
	"                                 Run " << ++NofRuns << " started" << 
	G4endl <<
	"---------------------------------------------------------------------------------------" << 
	G4endl << G4endl;
	//G4RunManager()::GeometryHasBeenModified();
}

void RunAction::EndOfRunAction(const G4Run* myRun)
{
	Run* aRun = (Run*) myRun;

	if (NofRuns != 0)
	G4cout <<
	G4endl <<
	"---------------------------------------------------------------------------------------" << 
	G4endl <<
	"                                 Run " << NofRuns << " finished" << 
	G4endl << 
	"***************************************************************************************" <<	
	G4endl;
}
