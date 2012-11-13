/**********************************************************************************

***********************************************************************************/
#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"

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

#include "G4SDManager.hh"

DetectorConstruction::DetectorConstruction()
{
	worldSize = 1*m;
	detDistToMask = 34*mm;
	maskPixSize  = 1.2*mm;
	maskHeight = 2*mm;
	
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

	DefineMaterials();

  World_sol = new G4Box("World",worldSize/2,worldSize/2,worldSize/2);		   
  World_log = new G4LogicalVolume(World_sol,Air,"World");		
  World_phys = new G4PVPlacement(0,G4ThreeVector(0,0,0),World_log,"World",0,false,0);  
  World_log->SetVisAttributes(G4VisAttributes::Invisible);

	ConstructDetector();
  ConstructMask();
  
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
  sensDet = new SensitiveDetector("/SensDetector");
  SDman->AddNewDetector(sensDet);
  detect_log->SetSensitiveDetector(sensDet);
	
  return World_phys;
} 

void DetectorConstruction::ConstructDetector()
{
	detect_sol = new G4Box("detect_sol",19.54*0.5*mm,19.54*0.5*mm,5.*0.5*mm);
	detect_log = new G4LogicalVolume(detect_sol,CdZnTe,"detect_log");
  detect_log->SetVisAttributes(G4Color::Yellow());  

	detect_phys = new G4PVPlacement(0,G4ThreeVector((19.54*0.5+1.2-0.34),(19.54*0.5+1.2-0.34),-detDistToMask)*mm,detect_log,"detect_phys",World_log,false,0);
  detect_phys = new G4PVPlacement(0,G4ThreeVector(-(19.54*0.5+1.2-0.34),-(19.54*0.5+1.2-0.34),-detDistToMask)*mm,detect_log,"detect_phys",World_log,false,0);
	detect_phys = new G4PVPlacement(0,G4ThreeVector(-(19.54*0.5+1.2-0.34),(19.54*0.5+1.2-0.34),-detDistToMask)*mm,detect_log,"detect_phys",World_log,false,0);
  detect_phys = new G4PVPlacement(0,G4ThreeVector((19.54*0.5+1.2-0.34),-(19.54*0.5+1.2-0.34),-detDistToMask)*mm,detect_log,"detect_phys",World_log,false,0);
}

void DetectorConstruction::ConstructMask()
{
  pixel_sol = new G4Box("pixel_sol",maskPixSize*0.5*mm,maskPixSize*0.5*mm,maskHeight*0.5*mm);
  strip_sol = new G4Box("strip_sol",(36+0.5)*maskPixSize*mm,maskPixSize*0.5*mm,maskHeight*mm);
	pixel_log = new G4LogicalVolume(pixel_sol,Pb,"pixel_log");
	strip_log = new G4LogicalVolume(strip_sol,Pb,"strip_log");
  
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
					mask_phys = new G4PVPlacement(0,G4ThreeVector(-(xpos[k]+i),(ypos[k]-j),0)*maskPixSize*mm,
																					 pixel_log,"mask_phys",World_log,false,0);						
 				}
			}
		}
	}

	mask_phys = new G4PVPlacement(0,G4ThreeVector(0,0,0),strip_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(0,G4ThreeVector(0,37,0)*maskPixSize*mm,strip_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(0,G4ThreeVector(0,-37,0)*maskPixSize*mm,strip_log,"mask_phys",World_log,false,0);

	rotm = new G4RotationMatrix();
	rotm->rotateZ(90*degree);
	mask_phys = new G4PVPlacement(rotm,G4ThreeVector(37,0,0)*maskPixSize*mm,strip_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(rotm,G4ThreeVector(-37,0,0)*maskPixSize*mm,strip_log,"mask_phys",World_log,false,0);

	mask_phys = new G4PVPlacement(0,G4ThreeVector(37,37,0)*maskPixSize*mm,pixel_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(0,G4ThreeVector(-37,-37,0)*maskPixSize*mm,pixel_log,"mask_phys",World_log,false,0);
	mask_phys = new G4PVPlacement(0,G4ThreeVector(-37,37,0)*maskPixSize*mm,pixel_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(0,G4ThreeVector(37,-37,0)*maskPixSize*mm,pixel_log,"mask_phys",World_log,false,0);	
}

void DetectorConstruction::DefineMaterials()
{
	G4String symbol;
	G4double a,density;
	G4int z,natoms,ncomponents;
	#define GET_MATERIAL G4NistManager::Instance()->FindOrBuildMaterial
  Air = GET_MATERIAL("G4_AIR"); 
  Pb = GET_MATERIAL("G4_Pb");
  Al = GET_MATERIAL("G4_Al");
	G4Element* Cd = new G4Element("Cadmium", symbol="Cd", z=48, a=112.411*g/mole);
  G4Element* Zn = new G4Element("Zinc", symbol="Zn", z=30, a=65.39*g/mole);
  G4Element* Te = new G4Element("Tellurium", symbol="Te", z=52, a=127.6*g/mole);
	CdZnTe = new G4Material("CdZnTe",density=5.78*g/cm3,ncomponents=3);
  CdZnTe->AddElement(Cd,natoms=9);
  CdZnTe->AddElement(Zn,natoms=1);
  CdZnTe->AddElement(Te,natoms=10);
	#undef GET_MATERIAL
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
