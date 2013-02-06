/**************************************************************************
* Author  : Yigit Dallilar
* Date    : 21.01.2013
* Project : Sabanci University coded mask simulation
* File    : include/DetectorConstruction.hh
* - header file for src/DetectorConstruction 
***************************************************************************/

/**************************************************************************
  NOTES :
  (1) It is better to define some variables global as like materials.
**************************************************************************/

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "Messenger.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

class SensitiveDetector;
class G4Element;

/*************************************************************************/

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
    DetectorConstruction();
   ~DetectorConstruction();
    static G4bool binaryOutput;
    static G4bool textOutput;
    static G4ThreeVector sourcePos;         
    static G4ThreeVector sourceRot;
    static G4ThreeVector posToRef;
    
    G4VPhysicalVolume* Construct();                 
		
  private:
  
    void ConstructSpectrumMode();
		void ConstructAlBoxCover();
		void ConstructInclinedBox();
		void ConstructMask();   
		void ConstructDetector(); 
		void ConstructCollimator(G4int);
		void ConstructSourceHolder(G4int);
		void DefineMaterials();
		
		G4double worldSize;    
		G4double detDistToMask;
		G4double maskPixSize;
		G4double maskHeight;
		G4bool spectrumOn;   
		G4bool maskOn;
		G4bool detectorOn;
		G4bool inclboxOn;
		G4bool AlBoxCoverOn;
		G4int collimatorType;
		G4int sourceHolderType;
		G4ThreeVector sourceHolderPos;
		G4ThreeVector sourceHolderRot;
		G4double shiftCollimator;
		G4RotationMatrix rotm;
		
  	G4Material* Air; G4Material* Pb;
  	G4Material* W;
  	G4Material* Al;  G4Element* Cd;
  	G4Element* Zn;  G4Element* Te;
  	G4Material* CdZnTe;

		G4LogicalVolume* detect_log;

		G4VPhysicalVolume* mask_phys; 
		G4VPhysicalVolume* detect_phys; 
    G4VPhysicalVolume* inclbox_phys;
    G4VPhysicalVolume* AlBoxCover_phys;
    G4VPhysicalVolume* AlCs137HolderPhys;
    G4VPhysicalVolume* LeadInsideCs137Phys;
    G4VPhysicalVolume* sourceHolderCoverCs137Phys;
    G4VPhysicalVolume* collAluminumCoverPhys;
    G4VPhysicalVolume* collTungstenPhys;
    G4VPhysicalVolume* collLeadPhys;  
    G4VPhysicalVolume* collAluminumBigPhys;
    G4VPhysicalVolume* collAluminumSmallPhys;
    G4VSolid* World_sol;						G4LogicalVolume* World_log;		
    G4VPhysicalVolume* World_phys;  
    G4LogicalVolume* blank1_log;
    G4LogicalVolume* blank2_log;

    SensitiveDetector* sensDet;
};

#endif

/********************************************************************************/
