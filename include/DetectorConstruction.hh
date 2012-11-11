/**************************************************************************

***************************************************************************/
#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "DetectorMessenger.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "globals.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
    DetectorConstruction();
   ~DetectorConstruction();
     
    G4VPhysicalVolume* Construct();                 
		void SetMaskHeight(G4double);
		void SetMaskPixSize(G4double);
		void SetDetDistToMask(G4double);
		void UpdateGeometry();

		G4double GetWorldSize() {return worldSize;}; 
		G4double GetmaskHeight() {return maskHeight;}; 
		G4double GetmaskPixSize() {return maskPixSize;}; 
		G4double GetdetDistToMask() {return detDistToMask;}; 

  private:
  
		void ConstructMask(G4Material* mat,G4LogicalVolume* World_log,G4double pixsize,G4double height);   
		void ConstructDetector(G4Material* mat,G4LogicalVolume* World_log,G4double mask_dist); 

		G4double detDistToMask;
		G4double maskPixSize;
		G4double maskHeight;   
    G4double worldSize;
		DetectorMessenger* detMess;

};

#endif

