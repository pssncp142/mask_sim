/**********************************************************************************
* Programmer  : Yigit Dallilar
* Date        : 21.01.2013
* Project     : Sabanci University coded mask simulation
* File        : src/DetectorConstruction.cc
*  - Geometry of the simluation is defined here...
* 		
***********************************************************************************/

/**********************************************************************************
  Notes :
  **(1) Find a solution for rotation of the volumes.
  -G4Transform3D solves the problem, it rotates the problem in daughter volume coordinates 
  **(2) Collimator definitions should be added.
  (3) Maybe it should be useful to import some static values.
***********************************************************************************/

#include "Messenger.hh"
#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"

#include "G4NistManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
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
#include "G4Transform3D.hh"

/**********************************************************************************************/

DetectorConstruction::DetectorConstruction()
{
	//global options to be used in geometry
  worldSize = 200*CLHEP::m;
  detDistToMask = Messenger::detDistToMask;
  maskPixSize  = Messenger::maskPixSize;
  maskHeight = Messenger::maskHeight;
  spectrumOn = Messenger::spectrumOn;
  maskOn = Messenger::maskOn;
  detectorOn = Messenger::detectorOn;
  inclboxOn = Messenger::inclboxOn;
  AlBoxCoverOn = Messenger::AlBoxCoverOn;
  collimatorType = Messenger::collimatorType;
  sourceHolderType = Messenger::sourceHolderType;
  sourceHolderPos = Messenger::sourceHolderPos;
  sourceHolderRot = Messenger::sourceHolderRot;
  
  //information added to inside for IDL to create a suitable file name...
  if (true)//Messenger::binaryOutput)
  {
    double xx = sourceHolderPos.getX()*0.001,yy = sourceHolderPos.getY()*0.001,zz = sourceHolderPos.getZ()*0.001;
    std::ofstream ofs;
    ofs.open("output/runinfo.bin",std::iostream::app | std::iostream::binary);
    ofs.write((char*) &xx ,sizeof(double));
    ofs.write((char*) &yy ,sizeof(double));
    ofs.write((char*) &zz ,sizeof(double));
    if (Messenger::fillBlank)  {int x = 1; ofs.write((char*) &x, sizeof(int));}
    else  {int x = 0; ofs.write((char*) &x, sizeof(int));}
    ofs.close();
  }
  
  G4double sourceRefDist=0;  
  //shift of source holder due to existence of collimator...
  //And source distance is looked here...
  if (collimatorType == 0 or collimatorType == 2) 
  {  
    if (sourceHolderType == 1){sourceRefDist = 5.25*CLHEP::cm;} 
    if (sourceHolderType == 2 or sourceHolderType == 3){sourceRefDist = 3.75*CLHEP::cm;}
    shiftCollimator = 1.0*CLHEP::cm;
  } 
  else if (collimatorType == 1)
  {
    if (sourceHolderType == 1){sourceRefDist = 6.75*CLHEP::cm;} 
    if (sourceHolderType == 2 or sourceHolderType == 3){sourceRefDist = 5.25*CLHEP::cm;}
    shiftCollimator = 2.5*CLHEP::cm;
  }
  
  //Position vector of the source is sended to messenger...
  G4ThreeVector sourceRefPos = G4ThreeVector(0,0,sourceRefDist);
  rotm = *(new G4RotationMatrix());
  
  //First option is for source holder looking automatically to the dedector
  //Second option is for manual rotation
  if (Messenger::lookDedector)
  {
    G4ThreeVector pos_vec = sourceHolderPos + G4ThreeVector(0,0,detDistToMask);
    pos_vec_unit = pos_vec.unit();
    G4ThreeVector axis_vec = G4ThreeVector(0,0,1).cross(pos_vec);
    G4double rot_deg = std::acos(pos_vec.dot(G4ThreeVector(0,0,1))/(std::sqrt(pos_vec.dot(pos_vec))*
    std::sqrt(G4ThreeVector(0,0,1).dot(G4ThreeVector(0,0,1)))));
    G4cout << " " << G4endl;
    rotm.rotate(rot_deg,axis_vec);
    sourceRefPos = sourceRefDist*pos_vec_unit;
  }
  else
  {
    sourceRefPos.rotateX(sourceHolderRot.getX()*CLHEP::degree);
    sourceRefPos.rotateY(sourceHolderRot.getY()*CLHEP::degree);
    sourceRefPos.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);    
    rotm.rotateX(sourceHolderRot.getX()*CLHEP::degree);
    rotm.rotateY(sourceHolderRot.getY()*CLHEP::degree);
    rotm.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);        
  }

  Messenger::sourceRefPos = sourceRefPos;
}

/**********************************************************************************************/

DetectorConstruction::~DetectorConstruction()
{
}

/**********************************************************************************************/

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Function that defines materials...
  DefineMaterials();
  
  // Defining world wolume...
  World_sol = new G4Box("World",worldSize/2,worldSize/2,worldSize/2);		   
  World_log = new G4LogicalVolume(World_sol,Air,"World");		
  World_phys = new G4PVPlacement(0,G4ThreeVector(0,0,0),World_log,"World",0,false,0);  
  World_log->SetVisAttributes(G4VisAttributes::Invisible);
    
  sensDet = new SensitiveDetector("/SensDetector");
  
  // Geometry is building...
	if (spectrumOn)
	{
	  G4cout <<  "*********** WARNING!!! RADIOACTIVE SPECTRUM GEOMETRY ********" << G4endl;
	  G4cout <<  " This means that actual simulation is not working, this is   " << G4endl <<
	             " only for looking spectrum of given atoms. If it is not what " << G4endl <<
	             " you are looking for you should disable spectrumOn option... " << G4endl;
	  ConstructSpectrumMode();
	  G4cout <<  "******************** END OF GEOMETRY ************************" << G4endl << G4endl;
	}
	else
	{
	  G4cout <<  "******************* GEOMETRY OPTIONS ************************" << G4endl;
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
	  G4cout << "******************** END OF GEOMETRY ************************" << G4endl << G4endl;
  }
  
  G4cout << "*********************** OUTPUT OPTIONS *************************" << G4endl;
  if (Messenger::binaryOutput)
  {G4cout << "- Binary output will be written to data/data.bin" << G4endl;}
  else {G4cout << "- Binary output is inactive" << G4endl;}
  if (Messenger::textOutput)
  {G4cout << "- Text output will be written to data/data.txt" << G4endl;}
  else {G4cout << "- Text output is inactive" << G4endl;}
  G4cout << "- Initial values will be written to data/runinfo.bin" << G4endl;
  G4cout << "NOTE : readgeantdata.pro reads binaries directly from output location..." << G4endl;
  G4cout << "******************** END OF OUTPUT INFO ************************" << G4endl << G4endl;
   
  return World_phys;
} 

/**********************************************************************************************/
// Cosntructs radioactive spectrum test geometry... Nothing but a big sphere in the middle...
void DetectorConstruction::ConstructSpectrumMode()
{
  G4VSolid* sphere_sol = new G4Sphere("sphere_sol",10*CLHEP::cm,11*CLHEP::cm,0,360,0,180);
  G4LogicalVolume* sphere_log = new G4LogicalVolume(sphere_sol,CdZnTe,"sphere_log",0,0,0);
  G4VPhysicalVolume *sphere_phys = new G4PVPlacement(0,G4ThreeVector(0,0,0),sphere_log,"sphere_phys",World_log,false,0);
  sphere_log->SetVisAttributes(G4Color::Red());

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  sphere_log->SetSensitiveDetector(sensDet);	
  SDman->AddNewDetector(sensDet);
}
/**********************************************************************************************/
// Constructs Aliminum Cover...
void DetectorConstruction::ConstructAlBoxCover()
{
//!!! Just little bit changes over the old simulation...
    G4double AlBox1_x = 9.0*CLHEP::cm/2.0;
    G4double AlBox1_y = 11.5*CLHEP::cm/2.0;
    G4double AlBox1_z = 3.0*CLHEP::mm/2.0;
    
    G4double AlBox2_x = 3.5*CLHEP::cm/2.0;
    G4double AlBox2_y = 4.0*CLHEP::cm/2.0;
    G4double AlBox2_z = 3.01*CLHEP::mm/2.0;
    
    G4VSolid* AlBoxCover1_sol = new G4Box("AlBoxCover1_sol",AlBox1_x,AlBox1_y,AlBox1_z);
    G4VSolid* AlBoxCover2_sol = new G4Box("AlBoxCover2_sol",AlBox2_x,AlBox2_y,AlBox2_z);
    
    G4ThreeVector zTrans(0,AlBox2_y-AlBox1_y+0.6*CLHEP::cm,0);

    G4SubtractionSolid*  AlBoxCover_sol = new G4SubtractionSolid("AlBoxCover_sol", AlBoxCover1_sol, AlBoxCover2_sol, 0, zTrans);
 
    G4ThreeVector AlBoxCoverPos = G4ThreeVector(0,3.1*CLHEP::cm,300*CLHEP::mm);
    
    G4LogicalVolume* AlBoxCover_log = new G4LogicalVolume(AlBoxCover_sol, Al,"AlBoxCover_log",0,0,0);
    AlBoxCover_phys = new G4PVPlacement(0, AlBoxCoverPos,AlBoxCover_log,"AlBoxCover_phys",World_log,false,0);
    
    AlBoxCover_log->SetVisAttributes(G4Color::Blue());
		{G4cout << "- Aluminum Box Cover is build..." << G4endl;}
}

/**********************************************************************************************/
// Constructs Collimators... 
// New collimators can be added...
void DetectorConstruction::ConstructCollimator(G4int type)
{
  switch (type)
  {
    case 0 :  
    G4cout << "- No Collimator" << G4endl;		
    break;
    case 1 :
    {
      // Define dimensions.
    G4double innerRadiusOfTheTubeAlC = 0.7*CLHEP::mm;
    G4double outerRadiusOfTheTubeAlC = 22.5*CLHEP::mm;
    G4double heightOfTheTubeAlC = 0.8*CLHEP::cm/2.0;
    G4double startAngleOfTheTube = 0*CLHEP::deg;
    G4double spanningAngleOfTheTube = 360*CLHEP::deg;
    
    // Define the volume.
    G4Tubs* collAluminumCover = new G4Tubs("Coll_AluminumCover", innerRadiusOfTheTubeAlC,
        outerRadiusOfTheTubeAlC, heightOfTheTubeAlC, startAngleOfTheTube, spanningAngleOfTheTube);
    
    // Position vector.
    G4ThreeVector refFrame = G4ThreeVector(0,0,heightOfTheTubeAlC);
    if (Messenger::lookDedector)
    {
      refFrame = heightOfTheTubeAlC*pos_vec_unit;
    }
    else
    {
      refFrame.rotateX(sourceHolderRot.getX()*CLHEP::degree);
      refFrame.rotateY(sourceHolderRot.getY()*CLHEP::degree);
      refFrame.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);      
    }
    G4ThreeVector collAluminumCoverPos = sourceHolderPos + refFrame;
    
    // Definition of logical and physical volume.
    G4LogicalVolume* collAluminumCoverLogic = new G4LogicalVolume(collAluminumCover, Al, "collAluminumCover",0,0,0);
    collAluminumCoverPhys = new G4PVPlacement(G4Transform3D(rotm,collAluminumCoverPos), collAluminumCoverLogic,
        "collAluminumCoverPhys", World_log, false, 0);
 
    // Is volume visible?
    G4VisAttributes* visibleAluminumCover = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
    collAluminumCoverLogic->SetVisAttributes(visibleAluminumCover);
    //collAluminumCoverLogic->SetVisAttributes(G4VisAttributes::Invisible);
    // ------------------------------------------------------ //
    // Tungsten part of the collimator.
    // Define dimensions.
    G4double innerRadiusOfTheTube = 0.35*CLHEP::mm;
    G4double outerRadiusOfTheTube = 1.15*CLHEP::mm;
    G4double heightOfTheTube = 0.5*CLHEP::cm/2.0;

    // Define the volume.
    G4Tubs* collTungsten = new G4Tubs("Coll_Tungsten", innerRadiusOfTheTube,
        outerRadiusOfTheTube, heightOfTheTube, startAngleOfTheTube, spanningAngleOfTheTube);

    // Position of tungsten.
    G4ThreeVector refFrame1 = G4ThreeVector(0,0,heightOfTheTubeAlC+heightOfTheTube);
    if (Messenger::lookDedector)
    {
      refFrame1 = (heightOfTheTubeAlC+heightOfTheTube)*pos_vec_unit;
    }
    else
    {
      refFrame1.rotateX(sourceHolderRot.getX()*CLHEP::degree);
      refFrame1.rotateY(sourceHolderRot.getY()*CLHEP::degree);
      refFrame1.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);      
    }
    G4ThreeVector collTungstenPos = sourceHolderPos + refFrame + refFrame1;

    // Definition of logical and physical volume.
    G4LogicalVolume* collTungstenLogic = new G4LogicalVolume(collTungsten, W, "collTungsten",0,0,0);
    collTungstenPhys = new G4PVPlacement(G4Transform3D(rotm,collTungstenPos), collTungstenLogic,
        "collTungstenPhys", World_log, false, 0);
   
    // Is volume visible?
    G4VisAttributes* visibleTungsten = new G4VisAttributes(G4Colour(1.0,0.0,1.0));
    collTungstenLogic->SetVisAttributes(visibleTungsten);
    //collTungstenLogic->SetVisAttributes(G4VisAttributes::Invisible);
    // ------------------------------------------------------ //
    
    // ------------------------------------------------------ //
    // Lead part of the collimator.
    // Define dimensions.
    G4double innerRadiusOfTheTubeL = 1.15*CLHEP::mm;
    G4double outerRadiusOfTheTubeL = 10.0*CLHEP::mm;
    
    // Define the volume.
    G4Tubs* collLead = new G4Tubs("Coll_Lead", innerRadiusOfTheTubeL,
        outerRadiusOfTheTubeL, heightOfTheTube, startAngleOfTheTube, spanningAngleOfTheTube);
    
    // Position of lead.
    G4ThreeVector collLeadPos = collTungstenPos;
    
    // Definition of logical and physical volume.
    G4LogicalVolume* collLeadLogic = new G4LogicalVolume(collLead, Pb, "collLead",0,0,0);
    collLeadPhys = new G4PVPlacement(G4Transform3D(rotm,collLeadPos), collLeadLogic,
        "collLeadPhys", World_log, false, 0);

    // Is volume visible?
    G4VisAttributes* visibleLead = new G4VisAttributes(G4Colour(144,144,144));
    collLeadLogic->SetVisAttributes(visibleLead);
    //collLeadLogic->SetVisAttributes(G4VisAttributes::Invisible);
    // ------------------------------------------------------ //
    
    // ------------------------------------------------------ //
    // Aluminum Cover.
    // Covers lead and tungsten parts.
    // Define dimensions.
    G4double innerRadiusOfTheTubeAlB = 10.0*CLHEP::mm;
    G4double outerRadiusOfTheTubeAlB = 22.5*CLHEP::mm;
    G4double heightOfTheTubeAlB = 0.5*CLHEP::cm/2.0;
    
    // Define the volume.
    G4Tubs* collAluminumBig = new G4Tubs("Coll_AluminumBig", innerRadiusOfTheTubeAlB,
        outerRadiusOfTheTubeAlB, heightOfTheTubeAlB, startAngleOfTheTube, spanningAngleOfTheTube);
    
    // Position of the cover.
    refFrame1 = G4ThreeVector(0,0,heightOfTheTubeAlC+heightOfTheTubeAlB);
    if (Messenger::lookDedector)
    {
      refFrame1 = (heightOfTheTubeAlC+heightOfTheTubeAlB)*pos_vec_unit;
    }
    else
    {
      refFrame1.rotateX(sourceHolderRot.getX()*CLHEP::degree);
      refFrame1.rotateY(sourceHolderRot.getY()*CLHEP::degree);
      refFrame1.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);      
    }
    G4ThreeVector collAluminumBigPos = sourceHolderPos + refFrame + refFrame1;
    
    // Definition of logical and physical volume.
    G4LogicalVolume* collAluminumBigLogic = new G4LogicalVolume(collAluminumBig, Al, "collAluminumBig",0,0,0);
    collAluminumBigPhys = new G4PVPlacement(G4Transform3D(rotm,collAluminumBigPos), collAluminumBigLogic,
        "collAluminumBigPhys", World_log, false, 0);
    
    // Is volume visible?
    G4VisAttributes* visibleAluminumBig = new G4VisAttributes(G4Colour(248,248,248));
    collAluminumBigLogic->SetVisAttributes(visibleAluminumBig);
    //collAluminumBigLogic->SetVisAttributes(G4VisAttributes::Invisible);
    // ------------------------------------------------------ //
    
    // ------------------------------------------------------ //
    // Aluminum cap.
    G4double innerRadiusOfTheTubeAlS = 1.0*CLHEP::mm;
    G4double outerRadiusOfTheTubeAlS = 22.5*CLHEP::mm;
    G4double heightOfTheTubeAlS = 1.2*CLHEP::cm/2.0;
   
    // Define the volumes.
    G4Tubs* subtractVolume = new G4Tubs("subtractVolume",0.75*CLHEP::cm,outerRadiusOfTheTubeAlS+0.1*CLHEP::mm,1.0*CLHEP::cm/2.0,
        startAngleOfTheTube, spanningAngleOfTheTube);
    G4Tubs* collAluminumSmallBefore = new G4Tubs("Coll_AluminumSmallBefore", innerRadiusOfTheTubeAlS,
        outerRadiusOfTheTubeAlS, heightOfTheTubeAlS, startAngleOfTheTube, spanningAngleOfTheTube);

    // Needed for volume subtraction.
    G4ThreeVector zTrans(0,0,+0.2*CLHEP::cm/2.0);
    
    // This is the remaining volume to be used.
    G4SubtractionSolid* collAluminumSmall = new G4SubtractionSolid("Coll_AluminumSmall", collAluminumSmallBefore, subtractVolume, 0, zTrans);
    
    // Position of the aluminum cap.
    refFrame1 = G4ThreeVector(0,0,heightOfTheTubeAlC+heightOfTheTubeAlB+2*CLHEP::mm);
    if (Messenger::lookDedector)
    {
      refFrame1 = (heightOfTheTubeAlC+heightOfTheTubeAlB+2*CLHEP::mm)*pos_vec_unit;
    }
    else
    {
      refFrame1.rotateX(sourceHolderRot.getX()*CLHEP::degree);
      refFrame1.rotateY(sourceHolderRot.getY()*CLHEP::degree);
      refFrame1.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);      
    }
    G4ThreeVector collAluminumSmallPos = sourceHolderPos + refFrame + refFrame1;

    // Definition of logical and physical volume.
    G4LogicalVolume* collAluminumSmallLogic = new G4LogicalVolume(collAluminumSmall, Al, "collAluminumSmall",0,0,0);
    collAluminumSmallPhys = new G4PVPlacement(G4Transform3D(rotm,collAluminumSmallPos), collAluminumSmallLogic,
        "collAluminumSmallPhys", World_log, false, 0);
    
    // Is volume visible?
    G4VisAttributes* visibleAluminumSmall = new G4VisAttributes(G4Colour(248,248,248));
    collAluminumSmallLogic->SetVisAttributes(visibleAluminumSmall);
   // collAluminumSmallLogic->SetVisAttributes(G4VisAttributes::Invisible);
    // ------------------------------------------------------ //      
    }
    G4cout << "- Collimator Type 1 is build..." << G4endl;
    break;  
  }	
}
/**********************************************************************************************/
// Constructs Source Holders...
// Way to do here...
void DetectorConstruction::ConstructSourceHolder(G4int type)
{
  
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
      G4double innerRadiusOfTheHolderCs137 = 0.2*CLHEP::cm;
      G4double outerRadiusOfTheHolderCs137 = 10.5*CLHEP::cm/2.0;
      G4double heightOfTheHolderCs137 = 8.4*CLHEP::cm/2.0;
      G4double startAngleOfTheHolder = 0*CLHEP::deg;
      G4double spanningAngleOfTheHolder = 360*CLHEP::deg;

      // Inner cut.
      G4double innerRadiusOfInnerCutCs137 = 0*CLHEP::cm;
      G4double outerRadiusOfInnerCutCs137 = 6.8*CLHEP::cm/2.0;
      G4double heightOfInnerCutCs137 = 7.3*CLHEP::cm/2.0;

      // Outer cut.
      G4double innerRadiusOfOuterCutCs137 = 8.0*CLHEP::cm/2.0;
      G4double outerRadiusOfOuterCutCs137 = 10.6*CLHEP::cm/2.0;
      G4double heightOfOuterCutCs137 = 7.3*CLHEP::cm/2.0;
         
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
      G4ThreeVector refFrame1 = G4ThreeVector(0,0,heightOfTheHolderCs137+shiftCollimator);
      if (Messenger::lookDedector)
      {
        refFrame1 = (heightOfTheHolderCs137+shiftCollimator)*pos_vec_unit;
      }
      else
      {
        refFrame1.rotateX(sourceHolderRot.getX()*CLHEP::degree);
        refFrame1.rotateY(sourceHolderRot.getY()*CLHEP::degree);
        refFrame1.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);      
      }
      G4ThreeVector AlCs137HolderPos = sourceHolderPos + refFrame1;

         // Define physical and logical volume.   
      G4LogicalVolume* AlCs137HolderLogic = new G4LogicalVolume(AlCs137Holder, Al, "AlCs137HolderLogic",0,0,0);        
      G4VPhysicalVolume* AlCs137HolderPhys = new G4PVPlacement(G4Transform3D(rotm,AlCs137HolderPos), AlCs137HolderLogic,
             "AlCs137HolderPhys", World_log, false, 0);
         
      // Is volume visible?
      G4VisAttributes* visibleAlCs137Holder = new G4VisAttributes(G4Colour(248,248,248)); 
      AlCs137HolderLogic->SetVisAttributes(visibleAlCs137Holder);
      // ------------------------------------------------------ //
   
   
      // ------------------------------------------------------ //
      // If collimator is disabled add extension (small cylindirical volume in front of the source holder).
      if (Messenger::collimatorType == 2)
      {
        // Define dimensions.
        G4double innerRadiusOfTheHolderExtensionCs137 = 0.3*CLHEP::cm;
        G4double outerRadiusOfTheHolderExtensionCs137 = 1.5*CLHEP::cm;
        G4double heightOfTheHolderExtensionCs137 = 1.0*CLHEP::cm/2.0;
           
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
        G4ThreeVector refFrame2 = G4ThreeVector(0,0,shiftCollimator-heightOfTheHolderExtensionCs137);
        if (Messenger::lookDedector)
        {
          refFrame2 = (shiftCollimator-heightOfTheHolderExtensionCs137)*pos_vec_unit;
        }
        else
        {
          refFrame2.rotateX(sourceHolderRot.getX()*CLHEP::degree);
          refFrame2.rotateY(sourceHolderRot.getY()*CLHEP::degree);
          refFrame2.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);      
        }
        G4ThreeVector sourceHolderExtensionCs137Pos = sourceHolderPos + refFrame2;
                   
        // Define physical and logical volume.
        G4LogicalVolume* sourceHolderExtensionCs137Logic = new G4LogicalVolume(sourceHolderExtensionCs137, Al, "sourceHolderExtensionCs137",0,0,0);
        G4VPhysicalVolume* sourceHolderExtensionCs137Phys = new G4PVPlacement(G4Transform3D(rotm, sourceHolderExtensionCs137Pos)
        , sourceHolderExtensionCs137Logic,"sourceHolderExtensionCs137Phys", World_log, false, 0);
                
        // Is volume visible?
        G4VisAttributes* visiblesourceHolderExtensionCs137 = new G4VisAttributes(G4Colour(248,248,248)); 
        sourceHolderExtensionCs137Logic->SetVisAttributes(visiblesourceHolderExtensionCs137);
      }
         // ------------------------------------------------------ //
      
         
         // ------------------------------------------------------ //
         // Lead inside the source holder Cs 137.
         // Define dimensions.
         G4double innerRadiusOfTheLeadInside1 = 0.0*CLHEP::mm;
         G4double outerRadiusOfTheLeadInside = 6.78*CLHEP::cm/2.0;
         G4double heightOfTheLeadInside1 = 6.0*CLHEP::cm/2.0;
         
         G4double outerRadiusOfTheLeadInside2 = 0.4*CLHEP::cm/2.0;
         G4double heightOfTheLeadInside2 = 2.5*CLHEP::cm/2.0;
         
         G4double outerRadiusOfTheLeadInside3 = 1.25*CLHEP::cm;
         G4double heightOfTheLeadInside3 = 0.5*CLHEP::cm/2.0;
         
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
         G4ThreeVector zTransLeadCs1371(0,0,-(heightOfTheLeadInside1-heightOfTheLeadInside2)-0.01*CLHEP::mm);
         G4SubtractionSolid*  firstSubtractCs137Volume = new G4SubtractionSolid("firstSubtractCs137Volume", firstLeadInsideCs137, secondLeadInsideCs137, 0, zTransLeadCs1371);
         
         // Subtract third lead from the remaining.
         G4ThreeVector zTransLeadCs1372(0,0,-heightOfTheLeadInside3);
         G4SubtractionSolid*  secondSubtractCs137Volume = new G4SubtractionSolid("secondSubtractCs137Volume", firstSubtractCs137Volume, thirdLeadInsideCs137, 0, zTransLeadCs1372);
         
         // Position of the lead inside the source holder.
         G4ThreeVector LeadInsideCs137Pos = sourceHolderPos + refFrame1;
         
         // Define physical and logical volumes.
         G4LogicalVolume* LeadInsideCs137Logic = new G4LogicalVolume(secondSubtractCs137Volume, Pb, "LeadInsideCs137Logic",0,0,0);
         G4VPhysicalVolume* LeadInsideCs137Phys = new G4PVPlacement(G4Transform3D(rotm, LeadInsideCs137Pos), LeadInsideCs137Logic,
             "LeadInsideCs137Phys", World_log, false, 0);
         
         // Is volume visible?
         G4VisAttributes* visibleLeadInsideCs137 = new G4VisAttributes(G4Colour(1,0,0)); 
         LeadInsideCs137Logic->SetVisAttributes(visibleLeadInsideCs137);
         // ------------------------------------------------------ //
      
      
         // ------------------------------------------------------ //
         // Cap of the source holder for Cs 137.
         // Define dimensions.
         G4double innerRadiusOfTheSourceHolderCoverCs1371 = 6.8*CLHEP::cm/2.0;
         G4double outerRadiusOfTheSourceHolderCoverCs1371 = 8.02*CLHEP::cm/2.0;
         G4double heightOfTheSourceHolderCoverCs1371 = 2.5*CLHEP::cm/2.0;
         
         G4double innerRadiusOfTheSourceHolderCoverCs1372 = 0.0*CLHEP::mm;
         G4double outerRadiusOfTheSourceHolderCoverCs1372 = 8.0*CLHEP::cm/2.0;
         G4double heightOfTheSourceHolderCoverCs1372 = 3.8*CLHEP::cm/2.0;
         
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
         G4ThreeVector zTransCoverCs137(0,0,+heightOfTheSourceHolderCoverCs1371-heightOfTheSourceHolderCoverCs1372-0.01*CLHEP::mm);
         // This is the subtracted volume to be used ad a cap for the source holder.
         G4SubtractionSolid* sourceHolderCoverCs137 = new G4SubtractionSolid("sourceHolderCoverCs137", firstSourceHolderCs137Cover,
             firstSourceHolderCs137Sub, 0, zTransCoverCs137);
      
         // Position of the cap.
         G4ThreeVector refFrame2 = G4ThreeVector(0,0,heightOfTheLeadInside1+heightOfTheSourceHolderCoverCs1372+heightOfTheLeadInside3/2.0);
         if (Messenger::lookDedector)
         {
          refFrame2 = (heightOfTheLeadInside1+heightOfTheSourceHolderCoverCs1372+heightOfTheLeadInside3/2.0)*pos_vec_unit;
         }
         else
         {
          refFrame2.rotateX(sourceHolderRot.getX()*CLHEP::degree);
          refFrame2.rotateY(sourceHolderRot.getY()*CLHEP::degree);
          refFrame2.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);
         }
         G4ThreeVector sourceHolderCoverCs137Pos = sourceHolderPos + refFrame1 + refFrame2;
        
         // Define physical and logical volume.
         G4LogicalVolume* sourceHolderCoverCs137Logic = new G4LogicalVolume(sourceHolderCoverCs137, Al, "sourceHolderCoverCs137",0,0,0); 
         G4VPhysicalVolume* sourceHolderCoverCs137Phys = new G4PVPlacement(G4Transform3D(rotm, sourceHolderCoverCs137Pos), sourceHolderCoverCs137Logic,
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
    case 3 :
    {
      // Define dimensions.
         G4double innerRadiusOfTheHolderFirst = 0.2*CLHEP::cm;
         G4double outerRadiusOfTheHolder= 22.5*CLHEP::mm;
         G4double heightOfTheHolderFirst = 4.5*CLHEP::cm/2.0;
         G4double startAngleOfTheHolder = 0*CLHEP::deg;
         G4double spanningAngleOfTheHolder = 360*CLHEP::deg;
         
         // This is the part that will be subtracted.
         G4double innerRadiusOfTheHolderSecond = 0.0*CLHEP::mm;
         G4double outerRadiusOfTheHolderSecond = 1.75*CLHEP::cm;
         G4double heightOfTheHolderSecond = 3.0005*CLHEP::cm/2.0;
             
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
         G4ThreeVector refFrame = G4ThreeVector(0,0,heightOfTheHolderFirst+shiftCollimator);
         if(Messenger::lookDedector)
         {
          refFrame = (heightOfTheHolderFirst+shiftCollimator)*pos_vec_unit;
         }
         else
         {
          refFrame.rotateX(sourceHolderRot.getX()*CLHEP::degree);      
          refFrame.rotateY(sourceHolderRot.getY()*CLHEP::degree);
          refFrame.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);         
         }
         G4ThreeVector holderUnionVolumePos = sourceHolderPos + refFrame;
         
         // Define logical and physical volume.
         G4LogicalVolume* holderUnionVolumeLogic = new G4LogicalVolume(holderUnionVolume, Al, "holderUnionVolume",0,0,0);
         G4VPhysicalVolume* holderUnionVolumePhys = new G4PVPlacement(G4Transform3D(rotm, holderUnionVolumePos), holderUnionVolumeLogic,
             "holderUnionVolumePhys", World_log, false, 0);
         
         // Is volume visible?            
         G4VisAttributes* visibleholderUnionVolume = new G4VisAttributes(G4Colour(248,248,248)); 
         holderUnionVolumeLogic->SetVisAttributes(visibleholderUnionVolume);
         // holderUnionVolumeLogic->SetVisAttributes(G4VisAttributes::Invisible);//
         // ------------------------------------------------------ //
         
         // ------------------------------------------------------ //
         // Lead inside the source holder.
         // Define dimensions.
         G4double innerRadiusOfTheLeadInside1 = 0.0*CLHEP::mm;
         G4double outerRadiusOfTheLeadInside = 1.75*CLHEP::cm;
         G4double heightOfTheLeadInside1 = 1.984*CLHEP::cm/2.0;

         G4double outerRadiusOfTheLeadInside2 = 0.2*CLHEP::cm;
         G4double heightOfTheLeadInside2 = 0.992*CLHEP::cm/2.0;
         
         G4double outerRadiusOfTheLeadInside3 = 3.96*CLHEP::mm; // (For the old source 1.25 cm, for new source 3.96*mm)
         G4double heightOfTheLeadInside3 = 0.496*CLHEP::cm/2.0; 

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
         if (Messenger::lookDedector)
         {
          refFrame1 = pos_vec_unit*heightOfTheLeadInside3;
         }
         else
         {         
          refFrame1.rotateX(sourceHolderRot.getX()*CLHEP::degree);      
          refFrame1.rotateY(sourceHolderRot.getY()*CLHEP::degree); 
          refFrame1.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);
         }         
         G4ThreeVector firstSubtractPos = sourceHolderPos + refFrame + refFrame1;
          
         // Definition of logical and physical volume.
         G4LogicalVolume* firstSubtractLogic = new G4LogicalVolume(secondSubtractVolume, Pb, "firstSubtractVolume",0,0,0);
         G4VPhysicalVolume* firstSubtractPhys = new G4PVPlacement(G4Transform3D(rotm, firstSubtractPos), firstSubtractLogic,
             "firstSubtractPhys", World_log, false, 0);
                
         // Is volume visible?          
         G4VisAttributes* visiblefirstSubtract = new G4VisAttributes(G4Colour(144,144,144)); 
         firstSubtractLogic->SetVisAttributes(visiblefirstSubtract);
	//firstSubtractLogic->SetVisAttributes(G4VisAttributes::Invisible);
         // ------------------------------------------------------ //

         
         // ------------------------------------------------------ //
         // Cap of the source holder.
         // Define dimensions.
         G4double innerRadiusOfTheSourceHolderCover1 = 1.75*CLHEP::cm;
         G4double outerRadiusOfTheSourceHolderCover1 = 2.344*CLHEP::cm;
         G4double heightOfTheSourceHolderCover1 = 1.19*CLHEP::cm/2.0;

         G4double innerRadiusOfTheSourceHolderCover2 = 0.0*CLHEP::mm;
         G4double outerRadiusOfTheSourceHolderCover2 = 2.304*CLHEP::cm;
         G4double heightOfTheSourceHolderCover2 = 1.84*CLHEP::cm/2.0;
         
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
         G4ThreeVector zTransCover(0,0,+heightOfTheSourceHolderCover1-heightOfTheSourceHolderCover2-0.01*CLHEP::mm);
    
         // This is the volume to be used.
         G4SubtractionSolid* sourceHolderCover = new G4SubtractionSolid("sourceHolderCover", firstSourceHolderCover, firstSourceHolderSub,0, zTransCover);
         
         // Define logical volume of the cover.
         G4LogicalVolume* sourceHolderCoverLogic = new G4LogicalVolume(sourceHolderCover, Al, "sourceHolderCover",0,0,0);         
         
         if (Messenger::sourceHolderType == 2)
         {
           
           // Lead piece for Co 57 closer to the source holder cap.
           // Define dimensions.
           G4double innerRadiusOfTheLeadCo57 = 0.0*CLHEP::mm;
           G4double outerRadiusOfTheLeadCo57 = 1.75*CLHEP::cm;
           G4double heightOfTheLeadCo57 = 0.496*CLHEP::cm/2.0;
           G4double startAngleOfTheHolder = 0*CLHEP::deg;
           G4double spanningAngleOfTheHolder = 360*CLHEP::deg;

           // Define volumes.
           G4Tubs* sourceHolderLeadCo57 = new G4Tubs("sourceHolderLeadCo57",
               innerRadiusOfTheLeadCo57,
               outerRadiusOfTheLeadCo57,
               heightOfTheLeadCo57,
               startAngleOfTheHolder,
               spanningAngleOfTheHolder);

           // Position of the extra lead for Co 57.
           G4ThreeVector refFrame2 = G4ThreeVector(0,0,shiftCollimator+heightOfTheLeadInside1+2.0*heightOfTheLeadCo57);
           if (Messenger::lookDedector)
           {
             refFrame2 = pos_vec_unit*(shiftCollimator+heightOfTheLeadInside1+2.0*heightOfTheLeadCo57);
           }
           else
           {         
             refFrame2.rotateX(sourceHolderRot.getX()*CLHEP::degree);      
             refFrame2.rotateY(sourceHolderRot.getY()*CLHEP::degree); 
             refFrame2.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);
           }         
           G4ThreeVector sourceHolderCo57LeadPos = sourceHolderPos + refFrame + refFrame2;
         
           // Define logical and physical volume.
           G4LogicalVolume* sourceHolderCo57LeadLogic = new G4LogicalVolume(sourceHolderLeadCo57, Pb, "sourceHolderLeadCo57",0,0,0);         
           G4VPhysicalVolume* sourceHolderCo57LeadPhys = new G4PVPlacement(G4Transform3D(rotm, sourceHolderCo57LeadPos), sourceHolderCo57LeadLogic,
               "sourceHolderCo57LeadPhys", World_log, false, 0);
           
           // Is volume visible?
           G4VisAttributes* visiblesourceHolderCo57Lead = new G4VisAttributes(G4Colour(144,144,144));
           sourceHolderCo57LeadLogic->SetVisAttributes(visiblesourceHolderCo57Lead);
           
           // Position of the lead cover if the source holder is Co 57.
           refFrame2 = G4ThreeVector(0,0,heightOfTheLeadInside1+heightOfTheSourceHolderCover2+heightOfTheHolderSecond/2.0);
           if (Messenger::lookDedector)
           {
             refFrame2 = pos_vec_unit*(heightOfTheLeadInside1+heightOfTheSourceHolderCover2+heightOfTheHolderSecond/2.0);
           }
           else
           {         
             refFrame2.rotateX(sourceHolderRot.getX()*CLHEP::degree);      
             refFrame2.rotateY(sourceHolderRot.getY()*CLHEP::degree); 
             refFrame2.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);
           }         
           G4ThreeVector sourceHolderCoverPos = sourceHolderPos + refFrame + refFrame2;
           // Physical volume of the cover if the source holder is Co 57.
           G4VPhysicalVolume* sourceHolderCoverPhys = new G4PVPlacement(G4Transform3D(rotm, sourceHolderCoverPos), sourceHolderCoverLogic,
               "sourceHolderCoverPhys", World_log, false, 0);
         } 
         else 
         {           
           // Position of the lead cover if the source holder is Am 241 or Cd 109.
           refFrame1 = G4ThreeVector(0,0,heightOfTheLeadInside1+
           heightOfTheSourceHolderCover2+heightOfTheHolderSecond/2.0-2.0*heightOfTheLeadInside3);
           if (Messenger::lookDedector)
           {
              refFrame1 = pos_vec_unit*(heightOfTheLeadInside1+
              heightOfTheSourceHolderCover2+heightOfTheHolderSecond/2.0-2.0*heightOfTheLeadInside3);
           }
           else
           {
             refFrame1.rotateX(sourceHolderRot.getX()*CLHEP::degree);
             refFrame1.rotateY(sourceHolderRot.getY()*CLHEP::degree);
             refFrame1.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);      
           }
           G4ThreeVector sourceHolderCoverPos = sourceHolderPos + refFrame1 + refFrame;         

           // Physical volume of the cover id the source holder is Am 241 or Cd 109.
           G4VPhysicalVolume* sourceHolderCoverPhys = new G4PVPlacement(G4Transform3D(rotm, sourceHolderCoverPos), sourceHolderCoverLogic,
               "sourceHolderCoverPhys", World_log, false, 0);
             
           // Is volume visible?
           G4VisAttributes* sourceHolderCoverVolume = new G4VisAttributes(G4Colour(248,248,248)); 
           sourceHolderCoverLogic->SetVisAttributes(sourceHolderCoverVolume);
         }
             
         if (Messenger::collimatorType == 2)
         {
           // Define dimensions.
           G4double innerRadiusOfTheHolderExtension = 0.4*CLHEP::cm;
           G4double outerRadiusOfTheHolderExtension = 0.75*CLHEP::cm;
           G4double heightOfTheHolderExtension = 1.0*CLHEP::cm/2.0;
           
           // Eliminate the shift in position due to the existence of the collimator.
           shiftCollimator = 2.0*heightOfTheHolderExtension;

           // Define volume
           G4Tubs* sourceHolderExtension = new G4Tubs("sourceHolderExtension",
               innerRadiusOfTheHolderExtension,
               outerRadiusOfTheHolderExtension,
               heightOfTheHolderExtension,
               startAngleOfTheHolder,
               spanningAngleOfTheHolder);
           
           // Position of the source holder extension.
           G4ThreeVector refFrame2 = G4ThreeVector(0,0,shiftCollimator-heightOfTheHolderExtension);
           if (Messenger::lookDedector)
           {
             refFrame2 = pos_vec_unit*(shiftCollimator-heightOfTheHolderExtension);
           }
           else
           {
             refFrame2.rotateX(sourceHolderRot.getX()*CLHEP::degree);
             refFrame2.rotateY(sourceHolderRot.getY()*CLHEP::degree);
             refFrame2.rotateZ(sourceHolderRot.getZ()*CLHEP::degree);      
           }
           G4ThreeVector sourceHolderExtensionPos = sourceHolderPos + refFrame2;         
   
           // Define physical and logical volume.
           G4LogicalVolume* sourceHolderExtensionLogic = new G4LogicalVolume(sourceHolderExtension, Al, "sourceHolderExtension",0,0,0);
           G4VPhysicalVolume* sourceHolderExtensionPhys = new G4PVPlacement(G4Transform3D(rotm,sourceHolderExtensionPos), sourceHolderExtensionLogic,
               "sourceHolderExtensionPhys", World_log, false, 0);
           
           // Is volume visible?
           G4VisAttributes* visiblesourceHolderExtension = new G4VisAttributes(G4Colour(248,248,248));
           sourceHolderExtensionLogic->SetVisAttributes(visiblesourceHolderExtension);
           //sourceHolderExtensionLogic->SetVisAttributes(G4VisAttributes::Invisible);
         }
        
    }
    G4cout << "- Source Holder Type 2 is build..." << G4endl;    
  }
  if (Messenger::lookDedector)
  {
    G4cout << "  Source Holder Position  : " << sourceHolderPos << G4endl;
    G4cout << "  Source Holder Rotation  : looks directly to the dedector" << G4endl;  
  }
  else
  {
    G4cout << "  Source Holder Position  : " << sourceHolderPos << G4endl;
    G4cout << "  Source Holder Rotation  : " << sourceHolderRot << G4endl;  
  }
}

/**********************************************************************************************/
// Constructs Inclined box ...(holder for the mask)
void DetectorConstruction::ConstructInclinedBox()
{
	G4double wallThick = 3*CLHEP::mm;
	G4double width1 = 95*0.5*CLHEP::mm;
	G4double width2 = 45*0.5*CLHEP::mm;
	G4VSolid* inclbox1_sol = new G4Trd("inclbox1_sol",width2+wallThick,width1+wallThick,width2+wallThick,width1+wallThick,detDistToMask*0.55);
	G4VSolid* inclbox2_sol = new G4Trd("inclbox2_sol",width2,width1,width2,width1,detDistToMask*0.551);
	G4VSolid* inclbox_sol = new G4SubtractionSolid("inclbox_sol",inclbox1_sol,inclbox2_sol);
	G4LogicalVolume* inclbox_log = new G4LogicalVolume(inclbox_sol,Al,"inclbox_log");
	inclbox_log->SetVisAttributes(G4Color::Red());
	inclbox_phys = new G4PVPlacement(0,G4ThreeVector(0,0,-detDistToMask*0.5)*CLHEP::mm,inclbox_log,"inclbox_phys",World_log,false,0);
	G4cout << "- Inclined Box is build..." << G4endl;
}

/**********************************************************************************************/
// Constructs detectors ...
void DetectorConstruction::ConstructDetector()
{
  G4double width = 19.54*CLHEP::mm;
  G4double height = 5*CLHEP::mm;
  G4double distover2 = 1.2*CLHEP::mm; //1.2*mm;
  G4VSolid* detect_sol = new G4Box("detect_sol",width*0.5*CLHEP::mm,width*0.5*CLHEP::mm,height*0.5*CLHEP::mm);
  detect_log = new G4LogicalVolume(detect_sol,CdZnTe,"detect_log");
  detect_log->SetVisAttributes(G4Color::Yellow());  
  
  if (Messenger::fillBlank)
  { 
    G4RotationMatrix* rotm = new G4RotationMatrix();
    rotm->rotateZ(90*CLHEP::degree);
    G4VSolid* blank1_sol = new G4Box("blank_sol1",width*0.5*CLHEP::mm,1.2*CLHEP::mm,height*0.5*CLHEP::mm);
    G4VSolid* blank2_sol = new G4Box("blank_sol2",1.2*CLHEP::mm,1.2*CLHEP::mm,height*0.5*CLHEP::mm);
    blank1_log = new G4LogicalVolume(blank1_sol,CdZnTe,"blank1_log");
    blank2_log = new G4LogicalVolume(blank2_sol,CdZnTe,"blank2_log");
    G4VPhysicalVolume * blank2_phys = new G4PVPlacement(0,G4ThreeVector(0,0,-detDistToMask+2.5*CLHEP::mm),
    blank2_log,"blank2_phys",World_log,false,0); 
    G4VPhysicalVolume* blank1_phys = new G4PVPlacement(0,G4ThreeVector((width*0.5+distover2),0,-detDistToMask+2.5*CLHEP::mm),
    blank1_log,"blank1_phys",World_log,false,0);
    blank1_phys = new G4PVPlacement(0,G4ThreeVector(-(width*0.5+distover2),0,-detDistToMask+2.5*CLHEP::mm),
    blank1_log,"blank1_phys",World_log,false,0);
    blank1_phys = new G4PVPlacement(rotm,G4ThreeVector(0,(width*0.5+distover2),-detDistToMask+2.5*CLHEP::mm),
    blank1_log,"blank1_phys",World_log,false,0);
    blank1_phys = new G4PVPlacement(rotm,G4ThreeVector(0,-(width*0.5+distover2),-detDistToMask+2.5*CLHEP::mm),
    blank1_log,"blank1_phys",World_log,false,0);
    blank1_log->SetVisAttributes(G4Color::Yellow());  
    blank2_log->SetVisAttributes(G4Color::Yellow());  
  }
  
  detect_phys = new G4PVPlacement(0,G4ThreeVector((width*0.5+distover2),(width*0.5+distover2),-detDistToMask+2.5*CLHEP::mm)
  ,detect_log,"detect_phys",World_log,false,0);
  detect_phys = new G4PVPlacement(0,G4ThreeVector(-(width*0.5+distover2),-(width*0.5+distover2),-detDistToMask+2.5*CLHEP::mm)
  ,detect_log,"detect_phys",World_log,false,0);
  detect_phys = new G4PVPlacement(0,G4ThreeVector(-(width*0.5+distover2),(width*0.5+distover2),-detDistToMask+2.5*CLHEP::mm)
  ,detect_log,"detect_phys",World_log,false,0);
  detect_phys = new G4PVPlacement(0,G4ThreeVector((width*0.5+distover2),-(width*0.5+distover2),-detDistToMask+2.5*CLHEP::mm)
  ,detect_log,"detect_phys",World_log,false,0);
	
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
	if (Messenger::fillBlank)
	{
	  blank1_log->SetSensitiveDetector(sensDet);
	  blank2_log->SetSensitiveDetector(sensDet);
	}
  detect_log->SetSensitiveDetector(sensDet);	
  SDman->AddNewDetector(sensDet);

	G4cout << "- Detectors are build..." << G4endl;
	if (Messenger::fillBlank)
	{
	  G4cout << "  " << width << "mm x " <<  width << "mm x " << height << "mm CdZnTe detector 2 by 2.\n  " << 
    distover2*2 << "mm space gap filled with CdZnTe also.." << G4endl;
	}
	else
	{
    G4cout << "  " << width << "mm x " <<  width << "mm x " << height << "mm CdZnTe detector 2 by 2 with " << 
    distover2*2 << "mm space gap between them.." << G4endl;
  }
}

/**********************************************************************************************/
// Constructs mask ...
void DetectorConstruction::ConstructMask()
{
  G4VSolid* pixel_sol = new G4Box("pixel_sol",maskPixSize*0.5*CLHEP::mm,maskPixSize*0.5*CLHEP::mm,maskHeight*0.5*CLHEP::mm);
  G4VSolid* strip_sol = new G4Box("strip_sol",(36+0.5)*maskPixSize*CLHEP::mm,maskPixSize*0.5*CLHEP::mm,maskHeight*0.5*CLHEP::mm);
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
		  mask_phys = new G4PVPlacement(0,G4ThreeVector(-(xpos[k]+i),(ypos[k]-j),0)*maskPixSize*CLHEP::mm,
						pixel_log,"mask_phys",World_log,false,0);						
		}
	    }
	}
    }

  mask_phys = new G4PVPlacement(0,G4ThreeVector(0,0,0),strip_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(0,G4ThreeVector(0,37,0)*maskPixSize*CLHEP::mm,strip_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(0,G4ThreeVector(0,-37,0)*maskPixSize*CLHEP::mm,strip_log,"mask_phys",World_log,false,0);
  
  G4RotationMatrix* rotm = new G4RotationMatrix();
  rotm->rotateZ(90*CLHEP::degree);
  mask_phys = new G4PVPlacement(rotm,G4ThreeVector(37,0,0)*maskPixSize*CLHEP::mm,strip_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(rotm,G4ThreeVector(-37,0,0)*maskPixSize*CLHEP::mm,strip_log,"mask_phys",World_log,false,0);
  
  mask_phys = new G4PVPlacement(0,G4ThreeVector(37,37,0)*maskPixSize*CLHEP::mm,pixel_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(0,G4ThreeVector(-37,-37,0)*maskPixSize*CLHEP::mm,pixel_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(0,G4ThreeVector(-37,37,0)*maskPixSize*CLHEP::mm,pixel_log,"mask_phys",World_log,false,0);
  mask_phys = new G4PVPlacement(0,G4ThreeVector(37,-37,0)*maskPixSize*CLHEP::mm,pixel_log,"mask_phys",World_log,false,0);	

	G4cout << "- Coded Mask is build..." << G4endl;
}

/**********************************************************************************************/
// Defines materials...
void DetectorConstruction::DefineMaterials()
{
  G4String symbol;
  G4double a,density;
  G4int z,natoms,ncomponents;
#define GET_MATERIAL G4NistManager::Instance()->FindOrBuildMaterial
  Air = GET_MATERIAL("G4_AIR"); 
  Pb = GET_MATERIAL("G4_Pb");
  Al = GET_MATERIAL("G4_Al");
  W  = GET_MATERIAL("G4_W");
  G4Element* Cd = new G4Element("Cadmium", symbol="Cd", z=48, a=112.411*CLHEP::g/CLHEP::mole);
  G4Element* Zn = new G4Element("Zinc", symbol="Zn", z=30, a=65.39*CLHEP::g/CLHEP::mole);
  G4Element* Te = new G4Element("Tellurium", symbol="Te", z=52, a=127.6*CLHEP::g/CLHEP::mole);
  CdZnTe = new G4Material("CdZnTe",density=5.78*CLHEP::g/CLHEP::cm3,ncomponents=3);
  CdZnTe->AddElement(Cd,natoms=9);
  CdZnTe->AddElement(Zn,natoms=1);
  CdZnTe->AddElement(Te,natoms=10);
#undef GET_MATERIAL
}

/**********************************************************************************************/

