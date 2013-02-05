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

  if (Messenger::spectrumOn)
  {
    posDist->SetCentreCoords(G4ThreeVector(0,0,0));	  
  }
  else
  { 
    posDist->SetCentreCoords(Messenger::sourceHolderPos+Messenger::sourceRefPos);	  
    G4ThreeVector rot_vec1 = G4ThreeVector(-Messenger::sourceRefPos.getY()/Messenger::sourceRefPos.getX(),1,0); 
    G4ThreeVector rot_vec2 = G4ThreeVector(-Messenger::sourceRefPos.getZ()/Messenger::sourceRefPos.getX(),0,1);  
    posDist->SetPosDisType("Plane");
    posDist->SetPosDisShape("Circle");
    posDist->SetRadius(1*mm);
    posDist->SetPosRot1(rot_vec1);
    posDist->SetPosRot2(rot_vec2);
  }

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
