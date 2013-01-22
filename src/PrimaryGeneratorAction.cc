/**********************************************************************
**********************************************************************/
#include "Messenger.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

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
  G4SPSPosDistribution *posDist = particleSource->GetCurrentSource()->GetPosDist();
  posDist->SetCentreCoords(Messenger::sourceHolderPos+Messenger::sourceRefPos);	
}   

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  //delete particleGun;
  delete particleSource;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //creates gun controls
  //particleSource->SetParticlePosition(G4ThreeVector(0*mm,0*mm,200*mm));
  particleSource->GeneratePrimaryVertex(anEvent);
}
