/**********************************************************************
**********************************************************************/
#include "Messenger.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "G4ThreeVector.hh"
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
  
  particleSource = new G4GeneralParticleSource();
  G4SPSPosDistribution *posDist;
  G4SPSAngDistribution *angDist;
  G4SPSEneDistribution *eneDist;
    
  if (Messenger::spectrumOn)
  { 
    posDist = particleSource->GetCurrentSource()->GetPosDist();
    posDist->SetCentreCoords(G4ThreeVector(0,0,0));	  
  }
  else if (true)
  {
     //Building SU text source
    G4double posx[22] = {-1.,-2.,-3.,-3.,-3.,-2.,-1.,-1.,-1.,-2.,-3.,1.,1.,1.,1.,1.,2.,3.,3.,3.,3.,3.};
    G4double posy[22] = {2.,2.,2.,1.,0.,0.,0.,-1.,-2.,-2.,-2.,2.,1.,0.,-1.,-2.,-2.,-2.,-1.,0.,1.,2.};
    for(int i = 0; i < 22; i++)
    {
      if (i != 0)
      {particleSource->AddaSource(1.);}
      particleSource->SetParticleDefinition(G4Gamma::GammaDefinition());
      posDist = particleSource->GetCurrentSource()->GetPosDist();
      angDist = particleSource->GetCurrentSource()->GetAngDist();
      eneDist = particleSource->GetCurrentSource()->GetEneDist();
      posDist->SetCentreCoords(G4ThreeVector(posx[i]*m,posy[i]*m,30.*m));	  
      G4ThreeVector rot_vec1,rot_vec2;
      G4ThreeVector pos_vec = G4ThreeVector (0,0,Messenger::detDistToMask) + G4ThreeVector(posx[i]*m,posy[i]*m,30.*m);
      rot_vec1 = G4ThreeVector(1,0,-pos_vec.getX()/pos_vec.getZ());
      rot_vec2 = G4ThreeVector(0,1,-pos_vec.getY()/pos_vec.getZ());
      posDist->SetPosRot1(rot_vec1);
      posDist->SetPosRot2(rot_vec2);
      angDist->DefineAngRefAxes("angref1",rot_vec1);
      angDist->DefineAngRefAxes("angref2",rot_vec2);
      posDist->SetPosDisType("Plane");
      posDist->SetPosDisShape("Circle");
      posDist->SetRadius(2.*mm);
      angDist->SetAngDistType("iso");
      angDist->SetMinTheta(0.*deg);
      angDist->SetMaxTheta(0.2*deg);
      eneDist->SetMonoEnergy(122.*keV);    
    }   
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
      
      G4ThreeVector rot_vec1,rot_vec2;
      if (Messenger::lookDedector)
      {
        G4ThreeVector pos_vec = G4ThreeVector (0,0,Messenger::detDistToMask) + Messenger::sourceHolderPos + Messenger::sourceRefPos;
        rot_vec1 = G4ThreeVector(1,0,-pos_vec.getX()/pos_vec.getZ());
        rot_vec2 = G4ThreeVector(0,1,-pos_vec.getY()/pos_vec.getZ());
      }
      else
      {
        if (Messenger::sourceRefPos.getX() == 0)
        {
          rot_vec1 = G4ThreeVector(1,0,0); 
          rot_vec2 = G4ThreeVector(0,1,0);
        }
        else
        {
          rot_vec1 = G4ThreeVector(-Messenger::sourceRefPos.getY()/Messenger::sourceRefPos.getX(),1,0); 
          rot_vec2 = G4ThreeVector(-Messenger::sourceRefPos.getZ()/Messenger::sourceRefPos.getX(),0,1);
        }
      }
      
      posDist->SetPosRot1(rot_vec1);
      posDist->SetPosRot2(rot_vec2);
      angDist->DefineAngRefAxes("angref1",rot_vec1);
      angDist->DefineAngRefAxes("angref2",rot_vec2);
      posDist->SetPosDisType("Plane");
      posDist->SetPosDisShape("Circle");
      posDist->SetRadius(2.6*mm);
      angDist->SetAngDistType("iso");
      angDist->SetMinTheta(0.*deg);
      angDist->SetMaxTheta(0.08*deg);
      eneDist->SetMonoEnergy(ener[i]);
    }
  }  
}   

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete particleSource;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
  if (Messenger::sourceProfile)
  {
    G4ThreeVector pos = particleSource->GetParticlePosition();
    G4ThreeVector dir = particleSource->GetParticleMomentumDirection();
    G4double ener = particleSource->GetParticleEnergy();
    std::ofstream ofs;
    G4double px = pos.getX(),py = pos.getY(),pz = pos.getZ();
    G4double dx = dir.getX(),dy = dir.getY(),dz = dir.getZ();
    ofs.open("output/sourceprofile.bin",std::iostream::app);
    ofs.write((char*)(&px),sizeof(double));
    ofs.write((char*)(&py),sizeof(double));
    ofs.write((char*)(&pz),sizeof(double));
    ofs.write((char*)(&dx),sizeof(double));
    ofs.write((char*)(&dy),sizeof(double));
    ofs.write((char*)(&dz),sizeof(double));
    ofs.write((char*)(&ener),sizeof(double));
    ofs.close();
  }
  particleSource->GeneratePrimaryVertex(anEvent);

}
