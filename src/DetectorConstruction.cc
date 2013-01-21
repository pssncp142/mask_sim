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
#include "G4Tubs.hh"
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

/**********************************************************************************************
* I used DetectorMessenger file as general messenger so general values are defined here...
**********************************************************************************************/

G4bool DetectorConstruction::binaryOutput=0;
G4bool DetectorConstruction::textOutput=0;
G4ThreeVector DetectorConstruction::sourcePos=G4ThreeVector(0*mm,0*mm,100*mm);
G4ThreeVector DetectorConstruction::sourceRot=G4ThreeVector(0,0,0);

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
  inclboxOn = 1;
  AlBoxCoverOn = 0;
  collimatorType = 0;
  sourceHolderType = 1;
  sourceHolderPos = G4ThreeVector(0*mm,0*mm,100*mm);
  sourceHolderRot = G4ThreeVector(0,0,0);
  
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
  G4RotationMatrix* rotm = new G4RotationMatrix();
  rotm->rotateX(sourceHolderRot.getX()*degree);
  rotm->rotateY(sourceHolderRot.getY()*degree);
  rotm->rotateZ(sourceHolderRot.getZ()*degree);      

  switch (type)
  {  
    case 0 :
    G4cout << "- No Source Holder" << G4endl;
    break;
    //----------------------------------------------------------------------------------------//
    case 1 :
    {            
      // ------------------------------------------------------ //
      // Define dimensions.
      G4double innerRadiusOfTheHolderCs137 = 0.2*cm;
      G4double outerRadiusOfTheHolderCs137 = 10.5*cm/2.0;
      G4double heightOfTheHolderCs137 = 8.4*cm/2.0;
      G4double startAngleOfTheHolder = 0*deg;
      G4double spanningAngleOfTheHolder = 360*deg;

      // Inner cut.
      G4double innerRadiusOfInnerCutCs137 = 0*cm;
      G4double outerRadiusOfInnerCutCs137 = 6.8*cm/2.0;
      G4double heightOfInnerCutCs137 = 7.3*cm/2.0;

      // Outer cut.
      G4double innerRadiusOfOuterCutCs137 = 8.0*cm/2.0;
      G4double outerRadiusOfOuterCutCs137 = 10.6*cm/2.0;
      G4double heightOfOuterCutCs137 = 7.3*cm/2.0;

      G4ThreeVector refFrame1 = G4ThreeVector(0,0,heightOfTheHolderCs137);
      refFrame1.rotateX(-sourceHolderRot.getX()*degree);
      refFrame1.rotateY(sourceHolderRot.getY()*degree);
      refFrame1.rotateZ(sourceHolderRot.getZ()*degree);
      
       
      /*// ShiftHolder. Needed below!.
      // 1.5*cm is the thickness of the collimator for Am, Co and Cd.
      // Shift depends on the existence of collimator.
         if ((CGlobalParameters::enableCollimator == 0) || (CGlobalParameters::enableCollimator == 2))
         {
           shiftCollimator = 1.0*cm;
         } else if (CGlobalParameters::enableCollimator == 1)
         {  
           shiftCollimator = 2.5*cm;
         }*/ 
         
      // Define volumes.
      G4Tubs* InnerCut = new G4Tubs("InnerCut",
             innerRadiusOfInnerCutCs137,
             outerRadiusOfInnerCutCs137,
             heightOfInnerCutCs137,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);
         
      G4Tubs* OuterCut = new G4Tubs("OuterCut",
             innerRadiusOfOuterCutCs137,
             outerRadiusOfOuterCutCs137,
             heightOfOuterCutCs137,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);

      G4Tubs* MainPieceCs = new G4Tubs("MainPieceCs",
             innerRadiusOfTheHolderCs137,
             outerRadiusOfTheHolderCs137,
             heightOfTheHolderCs137,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);
 
        
      // Subtract second lead from the first.
      G4ThreeVector zTransCs1(0,0,+(heightOfTheHolderCs137-heightOfInnerCutCs137));
      G4SubtractionSolid*  firstSubtractAlVolume = new G4SubtractionSolid("firstSubtractAlVolume", MainPieceCs, InnerCut,0, zTransCs1);

      // Subtract third lead from the remaining.
      G4ThreeVector zTransCs2(0,0,+(heightOfTheHolderCs137-heightOfInnerCutCs137));
      G4SubtractionSolid*  AlCs137Holder = new G4SubtractionSolid("AlCs137Holder", firstSubtractAlVolume, OuterCut,0, zTransCs2);
         
      // Position of the Cs 137 holder.
      G4ThreeVector AlCs137HolderPos = sourceHolderPos + refFrame1;

         // Define physical and logical volume.
      G4LogicalVolume* AlCs137HolderLogic = new G4LogicalVolume(AlCs137Holder, Al, "AlCs137HolderLogic",0,0,0);        
      G4VPhysicalVolume* AlCs137HolderPhys = new G4PVPlacement(rotm, AlCs137HolderPos, AlCs137HolderLogic,
             "AlCs137HolderPhys", World_log, false, 0);
         
      // Is volume visible?
      G4VisAttributes* visibleAlCs137Holder = new G4VisAttributes(G4Colour(248,248,248)); 
      AlCs137HolderLogic->SetVisAttributes(visibleAlCs137Holder);
      // ------------------------------------------------------ //
   
   
         // ------------------------------------------------------ //
         /*// If collimator is disabled add extension (small cylindirical volume in front of the source holder).
         if (CGlobalParameters::enableCollimator == 5)
         {
           // Define dimensions.
           G4double innerRadiusOfTheHolderExtensionCs137 = 0.3*cm;
           G4double outerRadiusOfTheHolderExtensionCs137 = 1.5*cm;
           G4double heightOfTheHolderExtensionCs137 = 1.0*cm/2.0;
           
           // Eliminate the shift in position due to the existence of the collimator.
           shiftCollimator = 2.0*heightOfTheHolderExtensionCs137;
           
           // Define volumes.
           G4Tubs* sourceHolderExtensionCs137 = new G4Tubs("sourceHolderExtensionCs137",
               innerRadiusOfTheHolderExtensionCs137,
               outerRadiusOfTheHolderExtensionCs137,
               heightOfTheHolderExtensionCs137,
               startAngleOfTheHolder,
               spanningAngleOfTheHolder);
           
           // Define positions.
           G4ThreeVector sourceHolderExtensionCs137Pos = G4ThreeVector(holderAlX, holderAlY, holderAlZ+shiftCollimator-heightOfTheHolderCs137-heightOfTheHolderExtensionCs137);
           
           // Define physical and logical volume.
           sourceHolderExtensionCs137Logic = new G4LogicalVolume(sourceHolderExtensionCs137, Al, "sourceHolderExtensionCs137",0,0,0);
           sourceHolderExtensionCs137Phys = new G4PVPlacement(0, sourceHolderExtensionCs137Pos, sourceHolderExtensionCs137Logic,
               "sourceHolderExtensionCs137Phys", experimentalHall_log, false, 0);
           
           // Is volume visible?
           G4VisAttributes* visiblesourceHolderExtensionCs137 = new G4VisAttributes(G4Colour(248,248,248)); 
           sourceHolderExtensionCs137Logic->SetVisAttributes(visiblesourceHolderExtensionCs137);
           // SetVisAttributes(G4VisAttributes::Invisible);//
         }*/
         // ------------------------------------------------------ //
      
         
         // ------------------------------------------------------ //
         // Lead inside the source holder Cs 137.
         // Define dimensions.
         G4double innerRadiusOfTheLeadInside1 = 0.0*mm;
         G4double outerRadiusOfTheLeadInside = 6.78*cm/2.0;
         G4double heightOfTheLeadInside1 = 6.0*cm/2.0;
         
         G4double outerRadiusOfTheLeadInside2 = 0.4*cm/2.0;
         G4double heightOfTheLeadInside2 = 2.5*cm/2.0;
         
         G4double outerRadiusOfTheLeadInside3 = 1.25*cm;
         G4double heightOfTheLeadInside3 = 0.5*cm/2.0;
         
         // Define volumes.
         G4Tubs* firstLeadInsideCs137 = new G4Tubs("firstLeadInsideCs137",
             innerRadiusOfTheLeadInside1,
             outerRadiusOfTheLeadInside,
             heightOfTheLeadInside1,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);
         
         G4Tubs* secondLeadInsideCs137 = new G4Tubs("secondLeadInsideCs137",
             innerRadiusOfTheLeadInside1,
             outerRadiusOfTheLeadInside2,
             heightOfTheLeadInside2,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);
         
         G4Tubs* thirdLeadInsideCs137 = new G4Tubs("thirdLeadInsideCs137",
             innerRadiusOfTheLeadInside1,
             outerRadiusOfTheLeadInside3,
             heightOfTheLeadInside3,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);
         
         // Subtract second lead from the first.
         G4ThreeVector zTransLeadCs1371(0,0,-(heightOfTheLeadInside1-heightOfTheLeadInside2)-0.01*mm);
         G4SubtractionSolid*  firstSubtractCs137Volume = new G4SubtractionSolid("firstSubtractCs137Volume", firstLeadInsideCs137, secondLeadInsideCs137, 0, zTransLeadCs1371);
         
         // Subtract third lead from the remaining.
         G4ThreeVector zTransLeadCs1372(0,0,-heightOfTheLeadInside3);
         G4SubtractionSolid*  secondSubtractCs137Volume = new G4SubtractionSolid("secondSubtractCs137Volume", firstSubtractCs137Volume, thirdLeadInsideCs137, 0, zTransLeadCs1372);
         
         // Position of the lead inside the source holder.
         G4ThreeVector LeadInsideCs137Pos = sourceHolderPos + refFrame1;
         
         // Define physical and logical volumes.
         G4LogicalVolume* LeadInsideCs137Logic = new G4LogicalVolume(secondSubtractCs137Volume, Pb, "LeadInsideCs137Logic",0,0,0);
         G4VPhysicalVolume* LeadInsideCs137Phys = new G4PVPlacement(rotm, LeadInsideCs137Pos, LeadInsideCs137Logic,
             "LeadInsideCs137Phys", World_log, false, 0);
         
         // Is volume visible?
         G4VisAttributes* visibleLeadInsideCs137 = new G4VisAttributes(G4Colour(1,0,0)); 
         LeadInsideCs137Logic->SetVisAttributes(visibleLeadInsideCs137);
         // ------------------------------------------------------ //
      
      
         // ------------------------------------------------------ //
         // Cap of the source holder for Cs 137.
         // Define dimensions.
         G4double innerRadiusOfTheSourceHolderCoverCs1371 = 6.8*cm/2.0;
         G4double outerRadiusOfTheSourceHolderCoverCs1371 = 8.02*cm/2.0;
         G4double heightOfTheSourceHolderCoverCs1371 = 2.5*cm/2.0;
         
         G4double innerRadiusOfTheSourceHolderCoverCs1372 = 0.0*mm;
         G4double outerRadiusOfTheSourceHolderCoverCs1372 = 8.0*cm/2.0;
         G4double heightOfTheSourceHolderCoverCs1372 = 3.8*cm/2.0;
         
         // Define volumes.
         G4Tubs* firstSourceHolderCs137Sub = new G4Tubs("firstSourceHolderCs137Sub",
             innerRadiusOfTheSourceHolderCoverCs1371,
             outerRadiusOfTheSourceHolderCoverCs1371,
             heightOfTheSourceHolderCoverCs1371,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);
         
         G4Tubs* firstSourceHolderCs137Cover = new G4Tubs("firstSourceHolderCs137Cover",
             innerRadiusOfTheSourceHolderCoverCs1372,
             outerRadiusOfTheSourceHolderCoverCs1372,
             heightOfTheSourceHolderCoverCs1372,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);
         
         // This is the shift needed to subtract correct part of the volume.        
         G4ThreeVector zTransCoverCs137(0,0,+heightOfTheSourceHolderCoverCs1371-heightOfTheSourceHolderCoverCs1372-0.01*mm);
         // This is the subtracted volume to be used ad a cap for the source holder.
         G4SubtractionSolid* sourceHolderCoverCs137 = new G4SubtractionSolid("sourceHolderCoverCs137", firstSourceHolderCs137Cover,
             firstSourceHolderCs137Sub, 0, zTransCoverCs137);
      
         // Position of the cap.
         G4ThreeVector refFrame2 = G4ThreeVector(0,0,heightOfTheLeadInside1+heightOfTheSourceHolderCoverCs1372+heightOfTheLeadInside3/2.0);
         refFrame2.rotateX(-sourceHolderRot.getX()*degree);
         refFrame2.rotateY(sourceHolderRot.getY()*degree);
         refFrame2.rotateZ(sourceHolderRot.getZ()*degree);
         
         G4ThreeVector sourceHolderCoverCs137Pos = sourceHolderPos + refFrame1 + refFrame2;
        
         // Define physical and logical volume.
         G4LogicalVolume* sourceHolderCoverCs137Logic = new G4LogicalVolume(sourceHolderCoverCs137, Al, "sourceHolderCoverCs137",0,0,0); 
         G4VPhysicalVolume* sourceHolderCoverCs137Phys = new G4PVPlacement(rotm, sourceHolderCoverCs137Pos, sourceHolderCoverCs137Logic,
             "sourceHolderCoverCs137Phys", World_log, false, 0);
       
         // Is volume visible?
         G4VisAttributes* sourceHolderCoverCs137Volume = new G4VisAttributes(G4Colour(248,248,248)); 
         sourceHolderCoverCs137Logic->SetVisAttributes(sourceHolderCoverCs137Volume);
         // SetVisAttributes(G4VisAttributes::Invisible);//
         // ------------------------------------------------------ //
    }
    G4cout << "- Source Holder Type 1 is build..." << G4endl;
    break;
    //--------------------------------------------------------------------------------------------------------------------------------//
    case 2 :
    {
      // Define dimensions.
         G4double innerRadiusOfTheHolderFirst = 0.2*cm;
         G4double outerRadiusOfTheHolder= 22.5*mm;
         G4double heightOfTheHolderFirst = 4.5*cm/2.0;
         G4double startAngleOfTheHolder = 0*deg;
         G4double spanningAngleOfTheHolder = 360*deg;
         
         // This is the part that will be subtracted.
         G4double innerRadiusOfTheHolderSecond = 0.0*mm;
         G4double outerRadiusOfTheHolderSecond = 1.75*cm;
         G4double heightOfTheHolderSecond = 3.0005*cm/2.0;
         
         G4ThreeVector refFrame = G4ThreeVector(0,0,heightOfTheHolderFirst);
         refFrame.rotateX(-sourceHolderRot.getX()*degree);
         refFrame.rotateY(sourceHolderRot.getY()*degree);
         refFrame.rotateZ(sourceHolderRot.getZ()*degree);      

         /*// ShiftHolder. Needed below!.
         // 1.5*cm is the thickness of the collimator for Am, Co and Cd.
         // Shift depends on the existence of collimator.
         if ((CGlobalParameters::enableCollimator == 0) || (CGlobalParameters::enableCollimator == 2)) 
         {
           shiftCollimator = 1.0*cm;
         } else if (CGlobalParameters::enableCollimator == 1)
         {
           shiftCollimator = 2.5*cm;
         } */
         
         // Define volumes.
         G4Tubs* firstUnionVolume = new G4Tubs("firstUnionVolume",
             innerRadiusOfTheHolderFirst,
             outerRadiusOfTheHolder,
             heightOfTheHolderFirst,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);
         
         G4Tubs* secondUnionVolume = new G4Tubs("secondUnionVolume",
             innerRadiusOfTheHolderSecond,
             outerRadiusOfTheHolderSecond,
             heightOfTheHolderSecond,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);
                  
         // This is the shift needed to subtract correct part of the volume.
         G4ThreeVector zTrans(0,0,heightOfTheHolderSecond/2.0);
    
         // This is the remaining volume to be used.
         G4SubtractionSolid*  holderUnionVolume = new G4SubtractionSolid("holderUnionVolume", firstUnionVolume, secondUnionVolume,0, zTrans);
         
         // Position of the holder.
         G4ThreeVector holderUnionVolumePos = sourceHolderPos + refFrame;//shiftcollimator
         
         // Define logical and physical volume.
         G4LogicalVolume* holderUnionVolumeLogic = new G4LogicalVolume(holderUnionVolume, Al, "holderUnionVolume",0,0,0);
         G4VPhysicalVolume* holderUnionVolumePhys = new G4PVPlacement(rotm, holderUnionVolumePos, holderUnionVolumeLogic,
             "holderUnionVolumePhys", World_log, false, 0);
         
         // Is volume visible?            
         G4VisAttributes* visibleholderUnionVolume = new G4VisAttributes(G4Colour(248,248,248)); 
         holderUnionVolumeLogic->SetVisAttributes(visibleholderUnionVolume);
         // holderUnionVolumeLogic->SetVisAttributes(G4VisAttributes::Invisible);//
         // ------------------------------------------------------ //
         
         // ------------------------------------------------------ //
         // Lead inside the source holder.
         // Define dimensions.
         G4double innerRadiusOfTheLeadInside1 = 0.0*mm;
         G4double outerRadiusOfTheLeadInside = 1.75*cm;
         G4double heightOfTheLeadInside1 = 1.984*cm/2.0;

         G4double outerRadiusOfTheLeadInside2 = 0.2*cm;
         G4double heightOfTheLeadInside2 = 0.992*cm/2.0;
         
         G4double outerRadiusOfTheLeadInside3 = 3.96*mm; // (For the old source 1.25 cm, for new source 3.96*mm)
         G4double heightOfTheLeadInside3 = 0.496*cm/2.0; 

         // Define volumes.
         G4Tubs* firstLeadInside = new G4Tubs("firstLeadInside",
             innerRadiusOfTheLeadInside1,
             outerRadiusOfTheLeadInside,
             heightOfTheLeadInside1,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);

         G4Tubs* secondLeadInside = new G4Tubs("secondLeadInside",
             innerRadiusOfTheLeadInside1,
             outerRadiusOfTheLeadInside2,
             heightOfTheLeadInside2,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);

         G4Tubs* thirdLeadInside = new G4Tubs("thirdLeadInside",
             innerRadiusOfTheLeadInside1,
             outerRadiusOfTheLeadInside3,
             heightOfTheLeadInside3,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);
     
         // Subtract second lead from the first.
         G4ThreeVector zTransLead1(0,0,-heightOfTheLeadInside2);
         G4SubtractionSolid*  firstSubtractVolume = new G4SubtractionSolid("firstSubtractVolume", firstLeadInside, secondLeadInside, 0, zTransLead1);

         // Subtract third lead from the remaining.
         G4ThreeVector zTransLead2(0,0,+heightOfTheLeadInside3/2.0);
         G4SubtractionSolid*  secondSubtractVolume = new G4SubtractionSolid("secondSubtractVolume", firstSubtractVolume, thirdLeadInside, 0, zTransLead2);
         
         // Position of the holder.
         G4ThreeVector refFrame1 = G4ThreeVector(0,0,heightOfTheLeadInside3);         
         refFrame1.rotateX(-sourceHolderRot.getX()*degree);
         refFrame1.rotateY(-sourceHolderRot.getY()*degree);
         refFrame1.rotateZ(sourceHolderRot.getZ()*degree);      

         G4ThreeVector firstSubtractPos = sourceHolderPos + refFrame + refFrame1;
         
         // Definition of logical and physical volume.
         G4LogicalVolume* firstSubtractLogic = new G4LogicalVolume(secondSubtractVolume, Pb, "firstSubtractVolume",0,0,0);
         G4VPhysicalVolume* firstSubtractPhys = new G4PVPlacement(rotm, firstSubtractPos, firstSubtractLogic,
             "firstSubtractPhys", World_log, false, 0);
                
         // Is volume visible?          
         G4VisAttributes* visiblefirstSubtract = new G4VisAttributes(G4Colour(144,144,144)); 
         firstSubtractLogic->SetVisAttributes(visiblefirstSubtract);
	//firstSubtractLogic->SetVisAttributes(G4VisAttributes::Invisible);
         // ------------------------------------------------------ //

         
         // ------------------------------------------------------ //
         // Cap of the source holder.
         // Define dimensions.
         G4double innerRadiusOfTheSourceHolderCover1 = 1.75*cm;
         G4double outerRadiusOfTheSourceHolderCover1 = 2.344*cm;
         G4double heightOfTheSourceHolderCover1 = 1.19*cm/2.0;

         G4double innerRadiusOfTheSourceHolderCover2 = 0.0*mm;
         G4double outerRadiusOfTheSourceHolderCover2 = 2.304*cm;
         G4double heightOfTheSourceHolderCover2 = 1.84*cm/2.0;
         
         // Define volumes.
         G4Tubs* firstSourceHolderSub = new G4Tubs("firstSourceHolderSub",
             innerRadiusOfTheSourceHolderCover1,
             outerRadiusOfTheSourceHolderCover1,
             heightOfTheSourceHolderCover1,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);
         
         G4Tubs* firstSourceHolderCover = new G4Tubs("firstSourceHolderCover",
             innerRadiusOfTheSourceHolderCover2,
             outerRadiusOfTheSourceHolderCover2,
             heightOfTheSourceHolderCover2,
             startAngleOfTheHolder,
             spanningAngleOfTheHolder);

         // This is the shift needed to subtract correct part of the volume.
         G4ThreeVector zTransCover(0,0,+heightOfTheSourceHolderCover1-heightOfTheSourceHolderCover2-0.01*mm);
    
         // This is the volume to be used.
         G4SubtractionSolid* sourceHolderCover = new G4SubtractionSolid("sourceHolderCover", firstSourceHolderCover, firstSourceHolderSub,0, zTransCover);
         
         // Define logical volume of the cover.
         G4LogicalVolume* sourceHolderCoverLogic = new G4LogicalVolume(sourceHolderCover, Al, "sourceHolderCover",0,0,0);
                    
         // Position of the lead cover if the source holder is Am 241 or Cd 109.
         refFrame1 = G4ThreeVector(0,0,heightOfTheLeadInside1+heightOfTheSourceHolderCover2+heightOfTheHolderSecond/2.0-2.0*heightOfTheLeadInside3);
         refFrame1.rotateX(-sourceHolderRot.getX()*degree);
         refFrame1.rotateY(-sourceHolderRot.getY()*degree);
         refFrame1.rotateZ(sourceHolderRot.getZ()*degree);      

         G4ThreeVector sourceHolderCoverPos = sourceHolderPos + refFrame + refFrame1;         

         // Physical volume of the cover id the source holder is Am 241 or Cd 109.
         G4VPhysicalVolume* sourceHolderCoverPhys = new G4PVPlacement(rotm, sourceHolderCoverPos, sourceHolderCoverLogic,
             "sourceHolderCoverPhys", World_log, false, 0);
        
         // Is volume visible?
         G4VisAttributes* sourceHolderCoverVolume = new G4VisAttributes(G4Colour(248,248,248)); 
         sourceHolderCoverLogic->SetVisAttributes(sourceHolderCoverVolume);
         // sourceHolderCoverLogic->SetVisAttributes(G4VisAttributes::Invisible);//              
    }
    G4cout << "- Source Holder Type 2 is build..." << G4endl;
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
  G4double width = 19.54*mm;
  G4double height = 5*mm;
  G4double distover2 = 1.2-0.34*mm;
  G4VSolid* detect_sol = new G4Box("detect_sol",width*0.5*mm,width*0.5*mm,height*0.5*mm);
  detect_log = new G4LogicalVolume(detect_sol,CdZnTe,"detect_log");
  detect_log->SetVisAttributes(G4Color::Yellow());  
  
  detect_phys = new G4PVPlacement(0,G4ThreeVector((width*0.5+distover2),(width*0.5+distover2),-detDistToMask+2.5*mm)*mm,detect_log,"detect_phys",World_log,false,0);
  detect_phys = new G4PVPlacement(0,G4ThreeVector(-(width*0.5+distover2),-(width*0.5+distover2),-detDistToMask+2.5*mm)*mm,detect_log,"detect_phys",World_log,false,0);
  detect_phys = new G4PVPlacement(0,G4ThreeVector(-(width*0.5+distover2),(width*0.5+distover2),-detDistToMask+2.5*mm)*mm,detect_log,"detect_phys",World_log,false,0);
  detect_phys = new G4PVPlacement(0,G4ThreeVector((width*0.5+distover2),-(width*0.5+distover2),-detDistToMask+2.5*mm)*mm,detect_log,"detect_phys",World_log,false,0);
	
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
  
  G4RotationMatrix* rotm = new G4RotationMatrix();
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

/*********************************************************************************************/

void DetectorConstruction::SetDetDistToMask(G4double val){detDistToMask = val;}
void DetectorConstruction::SetMaskPixSize(G4double val){maskPixSize = val;}
void DetectorConstruction::SetMaskHeight(G4double val){maskHeight = val;}
void DetectorConstruction::SetMaskOn(G4bool val){maskOn = val;}
void DetectorConstruction::SetDetectorOn(G4bool val){detectorOn = val;}
void DetectorConstruction::SetInclboxOn(G4bool val){inclboxOn = val;}
void DetectorConstruction::SetAlBoxCoverOn(G4bool val){AlBoxCoverOn = val;}
void DetectorConstruction::SetCollimatorType(G4int val){collimatorType = val;}
void DetectorConstruction::SetSourceHolderType(G4int val){sourceHolderType = val;}
void DetectorConstruction::SetSourceHolderPos(G4ThreeVector val){sourceHolderPos = val; sourcePos = val;}
void DetectorConstruction::SetSourceHolderRot(G4ThreeVector val){sourceHolderRot = val; sourceRot = val;}

/**********************************************************************************************/

#include "G4RunManager.hh"

void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

/**********************************************************************************************/

