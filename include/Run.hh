#ifndef Run_h
#define Run_h 1

#include "G4Run.hh"

#include <vector>

class G4Step;

class Run : public G4Run 
{
	Run(const std::vector<G4String> mfdName);
	~Run();
	void RecordEvent(G4Step*);
};

#endif
