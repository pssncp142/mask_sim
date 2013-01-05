/**********************************************************************************
* DetectorConstruction.cc (Yigit Dallilar)
* 
* Geometry of the simluation is defined here...
* 		
***********************************************************************************/

#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"

#include "G4NistManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
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
#include "G4RunManager.hh"

/**********************************************************************************************/

DetectorConstruction::DetectorConstruction()
{
	//global options to be used in geometry
  worldSize = 1*m;
  detDistToMask = 34*mm;
  maskPixSize  = 1.2*mm;
  maskHeight = 2*mm;
  maskOn = 1;
  detectorOn = 1;
  inclboxOn = 0;
  AlBoxCoverOn = 0;
  collimatorType = 0;
  sourceHolderType = 0;
  
  detMess = new DetectorMessenger(this); 
}

/**********************************************************************************************/

DetectorConstruction::~DetectorConstruction()
{
  delete detMess;
}

/**********************************************************************************************/

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
  
	G4cout <<  "******************* GEOMETRY OPTIONS ************************" << G4endl << G4endl;
	ConstructCollimator(collimatorType);
	ConstructSourceHolder(sourceHolderType);
	if (AlBoxCoverOn == 1) {ConstructAlBoxCover();} else 
	{G4cout << "- No Aluminum Box Cover" << G4endl;}
	if (inclboxOn == 1) {ConstructInclinedBox();} else
	{G4cout << "- No Inclined Box" << G4endl;}
  if (detectorOn == 1) {ConstructDetector();} else
	{G4cout << "- No Detector" << G4endl;}
  if (maskOn == 1) {ConstructMask();} else 
	{G4cout << "- No Coded Mask" << G4endl;}

	G4cout << G4endl << "**************** GEOMETRY IS INITIALISED ********************" << G4endl << G4endl;
    
  return World_phys;
} 

/**********************************************************************************************/

void DetectorConstruction::ConstructAlBoxCover()
{
//!!! Just little bit changes over the old simulation...

	  // Al box cover dimension.
    G4double AlBox1_x = 9.0*cm/2.0;
    G4double AlBox1_y = 11.5*cm/2.0;
    G4double AlBox1_z = 3.0*mm/2.0;
    
    // Cut out piece.
    G4double AlBox2_x = 3.5*cm/2.0;
    G4double AlBox2_y = 4.0*cm/2.0;
    G4double AlBox2_z = 3.01*mm/2.0;
    
    // Al box cover and cut out piece definition.
    G4VSolid* AlBoxCover1_sol = new G4Box("AlBoxCover1_sol",AlBox1_x,AlBox1_y,AlBox1_z);
    G4VSolid* AlBoxCover2_sol = new G4Box("AlBoxCover2_sol",AlBox2_x,AlBox2_y,AlBox2_z);
    
    // Cut out piece 0.6cm inside. 
    G4ThreeVector zTrans(0,AlBox2_y-AlBox1_y+0.6*cm,0);

    G4SubtractionSolid*  AlBoxCover_sol = new G4SubtractionSolid("AlBoxCover_sol", AlBoxCover1_sol, AlBoxCover2_sol, 0, zTrans);
 
    // Position of Al box cover.
    G4ThreeVector AlBoxCoverPos = G4ThreeVector(0,3.1*cm,300*mm);
    
    // Definition of logical and physical volume.
    G4LogicalVolume* AlBoxCover_log = new G4LogicalVolume(AlBoxCover_sol, Al,"AlBoxCover_log",0,0,0);
    AlBoxCover_phys = new G4PVPlacement(0, AlBoxCoverPos,AlBoxCover_log,"AlBoxCover_phys",World_log,false,0);
    
    // Is volume visible?
    AlBoxCover_log->SetVisAttributes(G4Color::Blue());
		{G4cout << "- Aluminum Box Cover is build..." << G4endl;}
}

/**********************************************************************************************/

void DetectorConstruction::ConstructCollimator(G4int type)
{
	switch (type)
	{
		case 0 :  
			G4cout << "- No Collimator" << G4endl;		
			break;
		case 1 :
			G4cout << "- Collimator Type 1 is build..." << G4endl;
			break;
	}	
}

/**********************************************************************************************/

void DetectorConstruction::ConstructSourceHolder(G4int type)
{
	switch (type)
	{
		case 0 :
			G4cout << "- No Source Holder" << G4endl;  
			break;
		case 1 :
			G4cout << "- Source Holder Type 1 is build..." << G4endl;
			break;
	}
}

/**********************************************************************************************/

void DetectorConstruction::ConstructInclinedBox()
{
	G4double wallThick = 3*mm;
	G4double width1 = 95*0.5*mm;
	G4double width2 = 45*0.5*mm;
	G4VSolid* inclbox1_sol = new G4Trd("inclbox1_sol",width2+wallThick,width1+wallThick,width2+wallThick,width1+wallThick,detDistToMask*0.55);
	G4VSolid* inclbox2_sol = new G4Trd("inclbox2_sol",width2,width1,width2,width1,detDistToMask*0.551);
	G4VSolid* inclbox_sol = new G4SubtractionSolid("inclbox_sol",inclbox1_sol,inclbox2_sol);
	G4LogicalVolume* inclbox_log = new G4LogicalVolume(inclbox_sol,Al,"inclbox_log");
	inclbox_log->SetVisAttributes(G4Color::Red());
	inclbox_phys = new G4PVPlacement(0,G4ThreeVector(0,0,-detDistToMask*0.5)*mm,inclbox_log,"inclbox_phys",World_log,false,0);
	G4cout << "- Inclined Box is build..." << G4endl;
}

/**********************************************************************************************/

void DetectorConstruction::ConstructDetector()
{
  G4VSolid* detect_sol = new G4Box("detect_sol",19.54*0.5*mm,19.54*0.5*mm,5.*0.5*mm);
  detect_log = new G4LogicalVolume(detect_sol,CdZnTe,"detect_log");
  detect_log->SetVisAttributes(G4Color::Yellow());  
  
  detect_phys = new G4PVPlacement(0,G4ThreeVector((19.54*0.5+1.2-0.34),(19.54*0.5+1.2-0.34),-detDistToMask)*mm,detect_log,"detect_phys",World_log,false,0);
  detect_phys = new G4PVPlacement(0,G4ThreeVector(-(19.54*0.5+1.2-0.34),-(19.54*0.5+1.2-0.34),-detDistToMask)*mm,detect_log,"detect_phys",World_log,false,0);
  detect_phys = new G4PVPlacement(0,G4ThreeVector(-(19.54*0.5+1.2-0.34),(19.54*0.5+1.2-0.34),-detDistToMask)*mm,detect_log,"detect_phys",World_log,false,0);
  detect_phys = new G4PVPlacement(0,G4ThreeVector((19.54*0.5+1.2-0.34),-(19.54*0.5+1.2-0.34),-detDistToMask)*mm,detect_log,"detect_phys",World_log,false,0);
	
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
  sensDet = new SensitiveDetector("/SensDetector");
  detect_log->SetSensitiveDetector(sensDet);	
  SDman->AddNewDetector(sensDet);

	G4cout << "- Detectors are build..." << G4endl;
}

/**********************************************************************************************/

void DetectorConstruction::ConstructMask()
{
  G4VSolid* pixel_sol = new G4Box("pixel_sol",maskPixSize*0.5*mm,maskPixSize*0.5*mm,maskHeight*0.5*mm);
  G4VSolid* strip_sol = new G4Box("strip_sol",(36+0.5)*maskPixSize*mm,maskPixSize*0.5*mm,maskHeight*mm);
  G4LogicalVolume* pixel_log = new G4LogicalVolume(pixel_sol,Pb,"pixel_log");
  G4LogicalVolume* strip_log = new G4LogicalVolume(strip_sol,Pb,"strip_log");
  
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

	G4cout << "- Coded Mask is build..." << G4endl;
}

/**********************************************************************************************/

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

/**********************************************************************************************/

void DetectorConstruction::SetDetDistToMask(G4double val){detDistToMask = val;}
void DetectorConstruction::SetMaskPixSize(G4double val){maskPixSize = val;}
void DetectorConstruction::SetMaskHeight(G4double val){maskHeight = val;}
void DetectorConstruction::SetMaskOn(G4bool val){maskOn = val;}
void DetectorConstruction::SetDetectorOn(G4bool val){detectorOn = val;}
void DetectorConstruction::SetInclboxOn(G4bool val){inclboxOn = val;}
void DetectorConstruction::SetAlBoxCoverOn(G4bool val){AlBoxCoverOn = val;}
void DetectorConstruction::SetCollimatorType(G4int val){collimatorType = val;}
void DetectorConstruction::SetSourceHolderType(G4int val){sourceHolderType = val;}

/**********************************************************************************************/

#include "G4RunManager.hh"

void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

/**********************************************************************************************/

