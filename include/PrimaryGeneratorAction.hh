/***************************************************************************

****************************************************************************/

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();    
   ~PrimaryGeneratorAction();

  public:
    void GeneratePrimaries(G4Event*);
    G4ParticleGun* GetParticleGun() { return particleGun;} ;
    G4GeneralParticleSource* GetParticleSource() {return particleSource;} ;       

  private:
    G4ParticleGun*  particleGun;
    G4GeneralParticleSource* particleSource;
    
};

#endif
