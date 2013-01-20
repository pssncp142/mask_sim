#ifndef Run_h
#define Run_h 1

#include "G4Run.hh"
#include "G4Event.hh"
#include <ctime>

class Run : public G4Run 
{
	public :
	Run(const std::vector<G4String> SDName);
	~Run();
	void RecordEvent(const G4Event*);
	
	private :
  time_t startT,lastT;
	
};

#endif
