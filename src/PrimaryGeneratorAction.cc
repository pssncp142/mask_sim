/**********************************************************************
**********************************************************************/
#include "Messenger.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4Gamma.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4Geantino.hh"
#include "Randomize.hh"
 
PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  //particleGun = new G4ParticleGun();
  particleSource = new G4GeneralParticleSource();
  G4SPSPosDistribution *posDist;
  G4SPSAngDistribution *angDist;
  G4SPSEneDistribution *eneDist;
    
  if (Messenger::spectrumOn)
  { 
    posDist = particleSource->GetCurrentSource()->GetPosDist();
    posDist->SetCentreCoords(G4ThreeVector(0,0,0));	  
  }
  else
  { 
    G4double prob[3] = {8.2,81.9,9.9};
    G4double ener[3] = {14.4*keV,122.1*keV,136.5*keV};
    for (int i = 0; i < 3; i++)
    { 
      if (i == 0)
      {particleSource->SetCurrentSourceIntensity(prob[i]);}
      else
      {particleSource->AddaSource(prob[i]);}
      particleSource->SetParticleDefinition(G4Gamma::GammaDefinition());
      posDist = particleSource->GetCurrentSource()->GetPosDist();
      angDist = particleSource->GetCurrentSource()->GetAngDist();
      eneDist = particleSource->GetCurrentSource()->GetEneDist();
      posDist->SetCentreCoords(Messenger::sourceHolderPos+Messenger::sourceRefPos);	  
      G4ThreeVector rot_vec1 = G4ThreeVector(-Messenger::sourceRefPos.getY()/Messenger::sourceRefPos.getX(),1,0); 
      G4ThreeVector rot_vec2 = G4ThreeVector(-Messenger::sourceRefPos.getZ()/Messenger::sourceRefPos.getX(),0,1);  
      posDist->SetPosDisType("Plane");
      posDist->SetPosDisShape("Circle");
      posDist->SetRadius(2.6*mm);
      posDist->SetPosRot1(rot_vec1);
      posDist->SetPosRot2(rot_vec2);
      angDist->SetAngDistType("iso");
      angDist->DefineAngRefAxes("angref1",G4ThreeVector(-Messenger::sourceRefPos.getY()/Messenger::sourceRefPos.getX(),1,0));
      angDist->DefineAngRefAxes("angref2",G4ThreeVector(-Messenger::sourceRefPos.getZ()/Messenger::sourceRefPos.getX(),0,1));
      angDist->SetMinTheta(0.*degree);
      angDist->SetMaxTheta(0.5*degree);
      eneDist->SetMonoEnergy(ener[i]);
    }
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
