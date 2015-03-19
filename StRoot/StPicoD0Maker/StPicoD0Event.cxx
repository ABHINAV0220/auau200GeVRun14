#include "StPicoDstMaker/StPicoEvent.h"
#include "StPicoDstMaker/StPicoTrack.h"

#include "StPicoD0Event.h"
#include "StKaonPion.h"

ClassImp(StPicoD0Event)

TClonesArray *StPicoD0Event::fgKaonPion = 0;

//-----------------------------------------------------------------------
StPicoD0Event::StPicoD0Event()
{
  if (!fgKaonPion) fgKaonPion = new TClonesArray("StKaonPion");
  mKaonPion = fgKaonPion; 
  mNKaonPion = 0;
}

//-----------------------------------------------------------------------
void StPicoD0Event::addPicoEvent(StPicoEvent const & picoEvent)
{
  // StPicoEvent variables
  mRunId = picoEvent.runId();
  mEventId = picoEvent.eventId();
}

//-----------------------------------------------------------------------
void StPicoD0Event::clear(char const *option)
{
  mKaonPion->Clear(option);
  mNKaonPion = 0;
}
//---------------------------------------------------------------------
void StPicoD0Event::addKaonPion(StKaonPion* t)
{
  TClonesArray &KaonPion = *mKaonPion;
  new(KaonPion[mNKaonPion++]) StKaonPion(t);
}
