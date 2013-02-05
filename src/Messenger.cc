#include "Messenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include "globals.hh"

G4bool Messenger::binaryOutput = 0;
G4bool Messenger::textOutput = 0;
G4double Messenger::detDistToMask = 34*mm;
G4double Messenger::maskPixSize  = 1.2*mm;
G4double Messenger::maskHeight = 2*mm;
G4bool Messenger::spectrumOn = 0;
G4bool Messenger::maskOn = 1;
G4bool Messenger::detectorOn = 1;
G4bool Messenger::inclboxOn = 1;
G4bool Messenger::AlBoxCoverOn = 0;
G4int Messenger::collimatorType = 0;
G4int Messenger::sourceHolderType = 0;
G4ThreeVector Messenger::sourceHolderPos = G4ThreeVector(0*mm,0*mm,30*mm);
G4ThreeVector Messenger::sourceHolderRot = G4ThreeVector(0,0,0);
G4ThreeVector Messenger::sourceRefPos = G4ThreeVector(0,0,30)*mm;

Messenger::Messenger()
{
  dir = new G4UIdirectory("/mask_sim/");
  dir->SetGuidance("UI commands specific to this example.");
  geoDir = new G4UIdirectory("/mask_sim/geom/");
  geoDir->SetGuidance("Geometry commands are here.");	  
  detDir = new G4UIdirectory("/mask_sim/geom/det/");
  detDir->SetGuidance("detector commands.");
  maskDir = new G4UIdirectory("/mask_sim/geom/mask/");
  maskDir->SetGuidance("mask commands.");
  sourceHolderDir = new G4UIdirectory("/mask_sim/geom/sourceHolder/");
  sourceHolderDir->SetGuidance("source holder commands.");
  collimatorDir = new G4UIdirectory("/mask_sim/geom/collimator/");
  collimatorDir->SetGuidance("collimator commands.");
  outputDir = new G4UIdirectory("/mask_sim/output/");
  outputDir->SetGuidance("output file commands.");

  spectrumOnCmd = new G4UIcmdWithABool("/mask_sim/spectrum",this);
  spectrumOnCmd->SetGuidance("Test Radioactive spectrum.");
  spectrumOnCmd->SetParameterName("choice",false);
  spectrumOnCmd->AvailableForStates(G4State_PreInit);

  maskHeightCmd = new G4UIcmdWithADoubleAndUnit("/mask_sim/geom/mask/height",this);
  maskHeightCmd->SetGuidance("Set the height of the mask.");
  maskHeightCmd->SetUnitCategory("Length");
  maskHeightCmd->SetParameterName("choice",false);
  maskHeightCmd->AvailableForStates(G4State_PreInit);

  maskPixSizeCmd = new G4UIcmdWithADoubleAndUnit("/mask_sim/geom/mask/pixSize",this);
  maskPixSizeCmd->SetGuidance("Set the height of the mask.");
  maskPixSizeCmd->SetUnitCategory("Length");
  maskPixSizeCmd->SetParameterName("choice",false);
  maskPixSizeCmd->AvailableForStates(G4State_PreInit);

  detDistToMaskCmd = new G4UIcmdWithADoubleAndUnit("/mask_sim/geom/det/distToMask",this);
  detDistToMaskCmd->SetGuidance("Set the height of the mask.");
  detDistToMaskCmd->SetUnitCategory("Length");
  detDistToMaskCmd->SetParameterName("choice",false);
  detDistToMaskCmd->AvailableForStates(G4State_PreInit);
  
  collimatorTypeCmd = new G4UIcmdWithAnInteger("/mask_sim/geom/collimator/type",this);
  collimatorTypeCmd->SetGuidance("Set the collimator type.");
  collimatorTypeCmd->SetParameterName("choice",false);
  collimatorTypeCmd->AvailableForStates(G4State_PreInit);

  sourceHolderTypeCmd = new G4UIcmdWithAnInteger("/mask_sim/geom/sourceHolder/type",this);
  sourceHolderTypeCmd->SetGuidance("Set the source holder type.");
  sourceHolderTypeCmd->SetParameterName("choice",false);
  sourceHolderTypeCmd->AvailableForStates(G4State_PreInit);

  sourceHolderPosCmd = new G4UIcmdWith3VectorAndUnit("/mask_sim/geom/sourceHolder/position",this);
  sourceHolderPosCmd->SetGuidance("Position of the source holder (and probably for the source).");
  sourceHolderPosCmd->SetUnitCategory("Length");
  sourceHolderPosCmd->AvailableForStates(G4State_PreInit);
  
  sourceHolderRotCmd = new G4UIcmdWith3Vector("/mask_sim/geom/sourceHolder/rotation",this);
  sourceHolderRotCmd->SetGuidance("Rotation of the source holder (and probably for the source).");
  sourceHolderRotCmd->AvailableForStates(G4State_PreInit);
  
  outputBinaryCmd = new G4UIcmdWithABool("/mask_sim/output/binary",this);
  outputBinaryCmd->SetGuidance("Writes binary output file for true");
  outputBinaryCmd->SetParameterName("choice",false);
  outputBinaryCmd->AvailableForStates(G4State_PreInit);

  outputTextCmd = new G4UIcmdWithABool("/mask_sim/output/text",this);
  outputTextCmd->SetGuidance("Writes text otuput file for true");
  outputTextCmd->SetParameterName("choice",false);
  outputTextCmd->AvailableForStates(G4State_PreInit);
}

Messenger::~Messenger()
{

}

void Messenger::SetNewValue(G4UIcommand* cmd, G4String val)
{
  if (cmd == maskHeightCmd)
	{
		maskHeight=(maskHeightCmd->GetNewDoubleValue(val));
	}
	else if (cmd == maskPixSizeCmd)
	{
		maskPixSize=(maskPixSizeCmd->GetNewDoubleValue(val));		
	}
	else if (cmd == detDistToMaskCmd)
	{
		detDistToMask=(detDistToMaskCmd->GetNewDoubleValue(val));		
	}
	else if (cmd == collimatorTypeCmd)
	{
		collimatorType=(collimatorTypeCmd->GetNewIntValue(val));		
	}
	else if (cmd == sourceHolderTypeCmd)
	{
		sourceHolderType=(sourceHolderTypeCmd->GetNewIntValue(val));		
	}
	else if (cmd == sourceHolderPosCmd)
	{
		sourceHolderPos=(sourceHolderPosCmd->GetNew3VectorValue(val));		
	}
	else if (cmd == sourceHolderRotCmd)
	{
		sourceHolderRot=(sourceHolderRotCmd->GetNew3VectorValue(val));		
	}
	else if (cmd == outputBinaryCmd)
	{
		binaryOutput=(outputBinaryCmd->GetNewBoolValue(val));		
	}
	else if (cmd == outputTextCmd)
	{
		textOutput=(outputTextCmd->GetNewBoolValue(val));		
	}
	else if (cmd == spectrumOnCmd)
	{
	  spectrumOn=(spectrumOnCmd->GetNewBoolValue(val));
	}
}
