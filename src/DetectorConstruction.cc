/**********************************************************************************
***********************************************************************************/
#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "G4RotationMatrix.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include <sstream>

DetectorConstruction::DetectorConstruction()
{
	worldSize = 1*m;
	maskHeight = 2*mm;
	detDistToMask = 34*mm;
	maskPixSize  = 1.2*mm;
	detMess = new DetectorMessenger(this); 
 }

DetectorConstruction::~DetectorConstruction()
{
  delete detMess;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

#define GET_MATERIAL G4NistManager::Instance()->FindOrBuildMaterial
  G4double density,natoms;
  G4int ncomponents;
  G4Material* Air = GET_MATERIAL("G4_AIR"); 
  G4Material* Pb = GET_MATERIAL("G4_Pb");
  G4Material* Al = GET_MATERIAL("G4_Al");
  G4Material* Cd = GET_MATERIAL("G4_Cd");
  G4Material* Zn = GET_MATERIAL("G4_Zn");
  G4Material* Te = GET_MATERIAL("G4_Te");
  G4Material* CdZnTe = new G4Material("CdZnTe",density=5.78*g/cm3,ncomponents=3);
  CdZnTe->AddMaterial(Cd,natoms=0.45);
  CdZnTe->AddMaterial(Zn,natoms=0.05);
  CdZnTe->AddMaterial(Te,natoms=0.5);
#undef GET_MATERIAL
 
  G4Box* World_sol = new G4Box("World",worldSize/2,worldSize/2,worldSize/2);		   
  G4LogicalVolume* World_log = new G4LogicalVolume(World_sol,Air,"World");		
  G4VPhysicalVolume* World_phys = new G4PVPlacement(0,G4ThreeVector(0,0,0),World_log,"World",0,false,0);  
  World_log->SetVisAttributes(G4VisAttributes::Invisible);
 
  ConstructMask(Pb,World_log,maskPixSize,maskHeight);
  ConstructDetector(CdZnTe,World_log,detDistToMask);
	
  return World_phys;
}

void DetectorConstruction::ConstructDetector(G4Material* mat,G4LogicalVolume* World_log, G4double mask_dist)
{
	G4VSolid* detect_sol = new G4Box("detect_sol",19.54*0.5*mm,19.54*0.5*mm,5.*0.5*mm);
	G4LogicalVolume* detect_log = new G4LogicalVolume(detect_sol,mat,"detect_log");
  detect_log->SetVisAttributes(G4Color::Yellow());  

	G4VPhysicalVolume* detect_phys = new G4PVPlacement(0,G4ThreeVector((19.54*0.5+1.2-0.34),(19.54*0.5+1.2-0.34),-mask_dist)*mm,detect_log,"detect_phys",World_log,false,0);
  detect_phys = new G4PVPlacement(0,G4ThreeVector(-(19.54*0.5+1.2-0.34),-(19.54*0.5+1.2-0.34),-mask_dist)*mm,detect_log,"detect_phys",World_log,false,0);
	detect_phys = new G4PVPlacement(0,G4ThreeVector(-(19.54*0.5+1.2-0.34),(19.54*0.5+1.2-0.34),-mask_dist)*mm,detect_log,"detect_phys",World_log,false,0);
  detect_phys = new G4PVPlacement(0,G4ThreeVector((19.54*0.5+1.2-0.34),-(19.54*0.5+1.2-0.34),-mask_dist)*mm,detect_log,"detect_phys",World_log,false,0);
}

void DetectorConstruction::ConstructMask(G4Material* mat,G4LogicalVolume* World_log, G4double pixsize,
	G4double height)
{
  G4VSolid* pixel_sol = new G4Box("pixel_sol",pixsize*0.5*mm,pixsize*0.5*mm,height*0.5*mm);
  G4VSolid* strip_sol = new G4Box("strip_sol",(36+0.5)*pixsize*mm,pixsize*0.5*mm,height*mm);
	G4LogicalVolume* pixel_log = new G4LogicalVolume(pixel_sol,mat,"pixel_log");
	G4LogicalVolume* strip_log = new G4LogicalVolume(strip_sol,mat,"strip_log");
  
  bool maskarr[36] = {1,0,1,1,0,0,1,0,1,1,1,1,0,0,0,1,0,0,0,0,1,0,0,0,1,1,1,1,0,1,0,0,1,1,0,1};
	bool maskapf[36][36];
  for (int i=0;i<36;i++)
	{
		for (int j=0;j<36;j++)
		{
      if (maskarr[i] == maskarr[j]) {maskapf[i][j]=1;} else {maskapf[i][j]=0;}
		}
	}

  int xpos[4] = {-36,1,-36,1};
	int ypos[4] = {36,36,-1,-1};
	for (int k=0;k<4;k++)
	{
	 	for (int i=0;i<36;i++)
		{
			for (int j=0;j<36;j++)
			{
 		 		if (maskapf[i][j]==1)
				{
					G4VPhysicalVolume* mask_phys = new G4PVPlacement(0,G4ThreeVector(-(xpos[k]+i),(ypos[k]-j),0)*pixsize*mm,
																					 pixel_log,"mask_phys",World_log,false,0);						
 				}
			}
		}
	}

	G4VPhysicalVolume* mask_phys = new G4PVPlacement(0,G4ThreeVector(0,0,0),strip_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(0,G4ThreeVector(0,37,0)*pixsize*mm,strip_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(0,G4ThreeVector(0,-37,0)*pixsize*mm,strip_log,"mask_phys",World_log,false,0);

	G4RotationMatrix* rotm = new G4RotationMatrix();
	rotm->rotateZ(90*degree);
	mask_phys = new G4PVPlacement(rotm,G4ThreeVector(37,0,0)*pixsize*mm,strip_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(rotm,G4ThreeVector(-37,0,0)*pixsize*mm,strip_log,"mask_phys",World_log,false,0);

	mask_phys = new G4PVPlacement(0,G4ThreeVector(37,37,0)*pixsize*mm,pixel_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(0,G4ThreeVector(-37,-37,0)*pixsize*mm,pixel_log,"mask_phys",World_log,false,0);
	mask_phys = new G4PVPlacement(0,G4ThreeVector(-37,37,0)*pixsize*mm,pixel_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(0,G4ThreeVector(37,-37,0)*pixsize*mm,pixel_log,"mask_phys",World_log,false,0);	
}

void DetectorConstruction::SetDetDistToMask(G4double val)
{
	detDistToMask = val;
}
void DetectorConstruction::SetMaskPixSize(G4double val)
{
	maskPixSize = val;
}

void DetectorConstruction::SetMaskHeight(G4double val)
{
	maskHeight = val;
}

#include "G4RunManager.hh"

void DetectorConstruction::UpdateGeometry()
{
	G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

