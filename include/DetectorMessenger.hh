#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class Run;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithABool;
class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;

class DetectorMessenger: public G4UImessenger
{
  public:

    DetectorMessenger(DetectorConstruction*);
   ~DetectorMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:

    DetectorConstruction* detector;
    
    G4UIdirectory* dir;
    G4UIdirectory* geoDir;
    G4UIdirectory* detDir;
    G4UIdirectory* maskDir;
    G4UIdirectory* sourceHolderDir;
    G4UIdirectory* collimatorDir;

    G4UIcmdWithADoubleAndUnit* maskPixSizeCmd;
    G4UIcmdWithADoubleAndUnit* maskHeightCmd;
    G4UIcmdWithADoubleAndUnit* detDistToMaskCmd;
    G4UIcmdWithAnInteger* sourceHolderTypeCmd;
    G4UIcmdWithAnInteger* collimatorTypeCmd;    
    G4UIcmdWith3VectorAndUnit* sourceHolderPosCmd;
    G4UIcmdWith3Vector* sourceHolderRotCmd;
    G4UIcmdWithoutParameter* updateCmd;    

    G4UIdirectory* outputDir;
    G4UIcmdWithABool* outputBinaryCmd;
    G4UIcmdWithABool* outputTextCmd;
    
};

#endif

