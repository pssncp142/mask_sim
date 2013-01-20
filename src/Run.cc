#include "Run.hh"
#include "TrackHit.hh"
#include "DetectorConstruction.hh"

#include "G4Run.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"

using namespace std;

Run::Run(const std::vector<G4String> SDName) : G4Run()
{
}

Run::~Run()
{

}

void Run::RecordEvent(const G4Event* aEvent)
{

  numberOfEvent++;  // This is an original line.  
  if (numberOfEvent <= 1) { startT = time(NULL); lastT = startT; }
  
  if (1)
  {
    G4long totEventNum = (G4long)GetNumberOfEventToBeProcessed();
    G4double fperc = numberOfEvent * 100.0 / totEventNum;
    G4int percent = (G4int)(fperc);
    
    if ( numberOfEvent == 1 || percent == fperc || time(NULL) - lastT >=1 )  
    {
      G4cout << "\rProgress: [";
      
      for (int i = 0; i < (percent+1)/2; i++)
      {
        G4cout << ":";
      }
      
      for (int i = (percent+1)/2; i < 50; i++)
      {
        G4cout << " ";
      }
      G4cout << "]" << percent << "%";
      
      time_t elpsT = time(NULL) - startT;
      time_t leftT = (G4double)elpsT / (G4double)numberOfEvent * (G4double)totEventNum;
      
      char buffer [100];
      sprintf(buffer, "  ==> TE: %02d:%02d:%02d / TR: %02d:%02d:%02d   ", (G4int)elpsT/3600, ((G4int)elpsT%3600)/60, (G4int)elpsT%60, (G4int)leftT/3600, ((G4int)leftT%3600)/60, (G4int)leftT%60);
      G4cout << buffer;
      
      lastT = time(NULL);
    }
    G4cout.flush();
    if ( numberOfEvent == totEventNum ) G4cout << G4endl;
  }
  
  G4HCofThisEvent* HCE = aEvent->GetHCofThisEvent();
  if (!HCE) return;
  
  G4SDManager* fSDM = G4SDManager::GetSDMpointer();
  G4int collectionID = fSDM->GetCollectionID("HitCollection");
	 
  TrackHitCollection* HC = (TrackHitCollection*)(HCE->GetHC(collectionID));
  
  G4int NbHits = HC->entries()-1;
  //variables to hold data
  string* parName = new string[NbHits];
  double* edep = new double[NbHits]; 
  double* gTime = new double[NbHits]; 
  double* xx = new double[NbHits]; 
  double* yy = new double[NbHits];
  double* zz = new double[NbHits];

   //getting data from thisHit
  for (int i=0;i<NbHits;i++)
  {
    TrackHit* thisHit = (*HC)[i];
    parName[i] = thisHit->GetParName();
    edep[i] = thisHit->GetEdep();
    xx[i] = thisHit->GetX();
    yy[i] = thisHit->GetY();
    zz[i] = thisHit->GetZ();
    gTime[i] = thisHit->GetGlobalTime();
  }
  
  //output files is started...
  for (int i=0;i<NbHits;i++)
  {
    //binary data file...
    if (DetectorConstruction::binaryOutput == 1)
    {
      std::ofstream ofs;
      ofs.open("output/data.bin",std::iostream::app | std::iostream::binary);
      if ((parName[i] == "gamma") or (parName[i]=="e-"))
      {
        ofs.write((char*)(&i), sizeof(int));
        ofs.write((char*)(&xx[i]), sizeof(double));
        ofs.write((char*)(&yy[i]), sizeof(double));
        ofs.write((char*)(&zz[i]), sizeof(double));
        ofs.write((char*)(&edep[i]), sizeof(double));
        ofs.write((char*)(&gTime[i]), sizeof(double));
      }
      ofs.close();
    }
    
    //text data file...
    if (DetectorConstruction::textOutput==1)
    {
      std::ofstream ofs;
      ofs.open("output/data.txt",std::iostream::app);
      if ((parName[i] == "gamma") or (parName[i]=="e-"))
      {
        ofs << xx[i] << "  " << yy[i] << "  " << zz[i] << "  " << 
        edep[i] << "  " << gTime[i] << endl; 
      }
      if(i==NbHits-1)
      { ofs << endl; }
      ofs.close();
    }
  } 
}
