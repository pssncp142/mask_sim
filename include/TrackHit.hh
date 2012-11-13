#ifndef TrackHit_h
#define TrackHit_h 1

#include "G4VHit.hh" 
#include "G4THitsCollection.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4VPhysicalVolume.hh"

class TrackHit : public G4VHit
{

public :
	TrackHit();
	~TrackHit();
	TrackHit(const TrackHit &right);

	const TrackHit& operator=(const TrackHit&);
  G4int operator==(const TrackHit&) const;

  inline void * operator new(size_t);
  inline void operator delete(void *aHit);

  void Draw();
  void Print();

	/*************************************************************************************/	
	//GETS
	G4VPhysicalVolume * GetPrePV()        {return prePV;};
  G4String            GetPrePVName()    {return prePVName;};
  G4ThreeVector 			GetPrePos()       {return prePos;};
  G4ThreeVector 			GetPreMomDir()    {return preMomDir;};
  G4Material *  			GetPreMat()       {return preMat;};
  G4String      			GetPreMatName()   {return preMatName;};
  G4double      			GetPreTotEn()     {return preTotEn;};
  G4ThreeVector 			GetPreMom()       {return preMom;};
  G4double      			GetPreKinEn()     {return preKinEn;};

	G4VPhysicalVolume * GetPostPV()  		  {return postPV;};
  G4String            GetPostPVName()   {return postPVName;};
  G4ThreeVector 			GetPostPos()      {return postPos;};
  G4ThreeVector 			GetPostMomDir()   {return postMomDir;};
  G4Material *  			GetPostMat()      {return postMat;};
  G4String      			GetPostMatName()  {return postMatName;};
  G4double      			GetPostTotEn()    {return postTotEn;};
  G4ThreeVector 			GetPostMom()      {return postMom;};
  G4double      			GetPostKinEn()    {return postKinEn;};
	/*************************************************************************************/
  //SETS
	void			SetPrePV(G4VPhysicalVolume* val)        {prePV=val;};
  void			SetPrePVName(G4String val)    					{prePVName=val;};
  void			SetPrePos(G4ThreeVector val)       			{prePos=val;};
  void			SetPreMomDir(G4ThreeVector val)    			{preMomDir=val;};
  void			SetPreMat(G4Material* val)       				{preMat=val;};
  void			SetPreMatName(G4String val)   					{preMatName=val;};
  void			SetPreTotEn(G4double val)    						{preTotEn=val;};
  void			SetPreMom(G4ThreeVector val)       			{preMom=val;};
  void			SetPreKinEn(G4double val)     					{preKinEn=val;};

	void			SetPostPV(G4VPhysicalVolume* val)       {postPV=val;};
  void			SetPostPVName(G4String val)    					{postPVName=val;};
  void			SetPostPos(G4ThreeVector val)       		{postPos=val;};
  void			SetPostMomDir(G4ThreeVector val)    		{postMomDir=val;};
  void			SetPostMat(G4Material* val)       			{postMat=val;};
  void			SetPostMatName(G4String val)   					{postMatName=val;};
  void			SetPostTotEn(G4double val)    					{postTotEn=val;};
  void			SetPostMom(G4ThreeVector val)       		{postMom=val;};
  void			SetPostKinEn(G4double val)     					{postKinEn=val;};

	
	/*************************************************************************************/	
private :	
	/*************************************************************************************/
	//VARIABLES
	G4VPhysicalVolume * prePV  			 ;
  G4String            prePVName    ;
  G4ThreeVector 			prePos       ;
  G4ThreeVector 			preMomDir    ;
  G4Material *  			preMat       ;
  G4String      			preMatName   ;
  G4double      			preTotEn     ;
  G4ThreeVector 			preMom       ;
  G4double      			preKinEn     ;

	G4VPhysicalVolume * postPV  		 ;
  G4String            postPVName   ;
  G4ThreeVector 			postPos      ;
  G4ThreeVector 			postMomDir   ;
  G4Material *  			postMat      ;
  G4String      			postMatName  ;
  G4double      			postTotEn    ;
  G4ThreeVector 			postMom      ;
  G4double      			postKinEn    ;
	/*************************************************************************************/

};

typedef G4THitsCollection<TrackHit> TrackHitCollection;

extern G4Allocator<TrackHit> TrackHitAllocator;

void* TrackHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) TrackHitAllocator.MallocSingle();
  return aHit;
}

void TrackHit::operator delete(void *aHit)
{
  TrackHitAllocator.FreeSingle((TrackHit*) aHit);
}

#endif
