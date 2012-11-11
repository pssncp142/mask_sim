/******************************************************************************
Yigit Dallilar
*******************************************************************************/

#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "globals.hh"

class PhysicsList: public G4VUserPhysicsList
{
  public:
    PhysicsList();
   ~PhysicsList();

  protected:
    // Construct particle and physics
    void ConstructParticle();
    void ConstructProcess(); 
		void RadioactiveDecay();
		void GeneralPhysics();
    void SetCuts();   
};

#endif



