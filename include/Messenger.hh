#ifndef Messenger_h
#define Messenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"
#include "G4ThreeVector.hh"

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithABool;
class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;

class Messenger: public G4UImessenger
{
  public:

    Messenger();
   ~Messenger();
    static G4bool binaryOutput;
    static G4bool textOutput;
    static G4double detDistToMask;
    static G4double maskPixSize;
    static G4double maskHeight;
    static G4bool maskOn;
    static G4bool detectorOn;
    static G4bool inclboxOn;
    static G4bool AlBoxCoverOn;
    static G4int collimatorType;
    static G4int sourceHolderType;
    static G4ThreeVector sourceHolderPos;
    static G4ThreeVector sourceHolderRot;
    static G4ThreeVector sourceRefPos;
        
    void SetNewValue(G4UIcommand*, G4String);
    
  private: 
  
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
