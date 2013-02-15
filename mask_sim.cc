/*****************************************************************************
*Author  : Yigit Dallilar
*Date    : 21.01.2013
*Project : Sabanci University coded mask simulation
*Name    : mask_sim.cc
* - main file for the simulation
******************************************************************************/

/*****************************************************************************
  Notes : 
  (1) General messenger added and G4UImanager starts immediately.

  To Do :
  (1) New Random Engine
******************************************************************************/

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"

#include "Messenger.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

/*******************************************************************************/

int main(int argc,char** argv) {

  // (1)  
  Messenger* msgr = new Messenger();
  G4UImanager* UI = G4UImanager::GetUIpointer();  
  // mask_sim directory options are written over Messenger.cc which is general messenger for the simulation...
  // and use settings.mac to control this options...
  UI->ApplyCommand("/control/execute settings.mac");

  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  
  G4RunManager * runManager = new G4RunManager;
  DetectorConstruction* detCon = new DetectorConstruction();
  PhysicsList* physList = new PhysicsList();
  runManager->SetUserInitialization(detCon);
  runManager->SetUserInitialization(physList);  
  PrimaryGeneratorAction* prim  = new PrimaryGeneratorAction();        
  runManager->SetUserAction(prim);
	RunAction* runAct = new RunAction();
	runManager->SetUserAction(runAct);

  runManager->Initialize();
      
  if (argc!=1)   // batch mode  
    { 
     G4String command = "/control/execute ";
     G4String fileName = argv[1];
     UI->ApplyCommand(command+fileName);  
    }
    
  else           // define visualization and UI terminal for interactive mode 
    { 
#ifdef G4VIS_USE
     G4VisManager* visManager = new G4VisExecutive;
     visManager->Initialize();
     UI->ApplyCommand("/control/execute vis.mac");          
#endif

#ifdef G4UI_USE
      G4UIExecutive * ui = new G4UIExecutive(argc,argv);      
      ui->SessionStart();
      delete ui;
#endif
     
#ifdef G4VIS_USE
     delete visManager;
#endif     
    }

  delete runManager;

  return 0;
}

/***************************************************************************************/

