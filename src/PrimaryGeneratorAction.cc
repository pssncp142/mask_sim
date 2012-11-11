/**********************************************************************
**********************************************************************/

#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Geantino.hh"
#include "Randomize.hh"
 
PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  //particleGun = new G4ParticleGun();
  particleSource = new G4GeneralParticleSource();	
}   

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  //delete particleGun;
  delete particleSource;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //creates gun controls !!!!!!!!!!!!!!!!!!!
  //particleGun->GeneratePrimaryVertex(anEvent);
  particleSource->GeneratePrimaryVertex(anEvent);

}
