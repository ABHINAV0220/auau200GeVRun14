#ifndef StKaonPion_hh
#define StKaonPion_hh
#ifdef __ROOT__

#include "TObject.h"
#include "TClonesArray.h"
#include "StLorentzVectorF.hh"

class StPicoTrack;
class StPicoEvent;

class StKaonPion : public TObject
{
 public:
  StKaonPion();
  StKaonPion(StKaonPion const *);
  StKaonPion(StPicoTrack const * kaon, StPicoTrack const * pion,unsigned short kIdx,unsigned short pIdx,
             StThreeVectorF const & vtx, float bField);
  ~StKaonPion() {}// please keep this non-virtual and NEVER inherit from this class 

  StLorentzVectorF const & lorentzVector() const { return mLorentzVector;}
  float m()    const;
  float pt()   const;
  float eta()  const;
  float phi()  const;
  float pointingAngle() const;
  float decayLength() const;
  float kaonDca() const;
  float pionDca() const;
  unsigned short   kaonIdx() const;
  unsigned short   pionIdx() const;
  float dcaDaughters() const;
  float cosThetaStar() const;
          
 private:
  // disable copy constructor and assignment operator by making them private (once C++11 is available in STAR you can use delete specifier instead)
  StKaonPion(StKaonPion const &);
  StKaonPion& operator=(StKaonPion const &);
  StLorentzVectorF mLorentzVector; // this owns four float only

  float mPointingAngle;
  float mDecayLength;
  float mKaonDca;
  float mPionDca;

  unsigned short  mKaonIdx; // index of track in StPicoDstEvent
  unsigned short  mPionIdx;

  unsigned short mDcaDaughters; // dcaDaughters * 10000. 1 μm precision
  char mCosThetaStar; // cosThetaStar * 100.

  ClassDef(StKaonPion,1)
};

inline float StKaonPion::m()    const { return mLorentzVector.m();}
inline float StKaonPion::pt()   const { return mLorentzVector.perp();}
inline float StKaonPion::eta()  const { return mLorentzVector.pseudoRapidity();}
inline float StKaonPion::phi()  const { return mLorentzVector.phi();}
inline float StKaonPion::pointingAngle() const { return mPointingAngle;}
inline float StKaonPion::decayLength() const { return mDecayLength;}
inline float StKaonPion::kaonDca() const { return mKaonDca;}
inline float StKaonPion::pionDca() const { return mPionDca;}
inline unsigned short   StKaonPion::kaonIdx() const { return mKaonIdx;}
inline unsigned short   StKaonPion::pionIdx() const { return mPionIdx;}
inline float StKaonPion::dcaDaughters() const { return static_cast<float>(mDcaDaughters)/10000.;}
inline float StKaonPion::cosThetaStar() const { return static_cast<float>(mCosThetaStar/100.);}

#endif
#endif

