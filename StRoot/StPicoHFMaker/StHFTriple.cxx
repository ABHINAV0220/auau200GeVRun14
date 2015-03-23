#include <limits>
#include <cmath>

#ifdef __ROOT__
#include "StHFTriple.h"

#include "StLorentzVectorF.hh"
#include "StThreeVectorF.hh"
#include "StDcaGeometry.h" // remove in official production
#include "StPhysicalHelixD.hh"
#include "phys_constants.h"
#include "SystemOfUnits.h"
#include "StPicoDstMaker/StPicoTrack.h"

ClassImp(StHFTriple)


StHFTriple::StHFTriple(): mLorentzVector(StLorentzVectorF()),
   mParticle1MassHypo(std::numeric_limits<float>::quiet_NaN()), mParticle2MassHypo(std::numeric_limits<float>::quiet_NaN()),
   mPointingAngle(std::numeric_limits<float>::quiet_NaN()), mDecayLength(std::numeric_limits<float>::quiet_NaN()),
   mParticle1Dca(std::numeric_limits<float>::quiet_NaN()), mParticle2Dca(std::numeric_limits<float>::quiet_NaN()),
   mParticle1Idx(std::numeric_limits<unsigned short>::max()), mParticle2Idx(std::numeric_limits<unsigned short>::max()),
   mDcaDaughters(std::numeric_limits<unsigned short>::max()), mCosThetaStar(std::numeric_limits<char>::min())

{
}
//------------------------------------
StHFTriple::StHFTriple(StHFTriple const * t) : mLorentzVector(t->mLorentzVector),
   mParticle1MassHypo(t->mParticle1MassHypo), mParticle2MassHypo(t->mParticle2MassHypo),
   mPointingAngle(t->mPointingAngle), mDecayLength(t->mDecayLength),
   mParticle1Dca(t->mParticle1Dca), mParticle2Dca(t->mParticle2Dca),
   mParticle1Idx(t->mParticle1Idx), mParticle2Idx(t->mParticle2Idx),
   mDcaDaughters(t->mDcaDaughters), mCosThetaStar(t->mCosThetaStar)
{
}
//------------------------------------
StHFTriple::StHFTriple(StPicoTrack const * const particle1, StPicoTrack const * const particle2,
		   float particle1MassHypo, float particle2MassHypo,
		   unsigned short const p1Idx, unsigned short const p2Idx,
		   StThreeVectorF const & vtx, float const bField)  : 
  mLorentzVector(StLorentzVectorF()),
  mParticle1MassHypo(particle1MassHypo), mParticle2MassHypo(particle2MassHypo),
  mPointingAngle(std::numeric_limits<float>::quiet_NaN()), mDecayLength(std::numeric_limits<float>::quiet_NaN()),
  mParticle1Dca(std::numeric_limits<float>::quiet_NaN()), mParticle2Dca(std::numeric_limits<float>::quiet_NaN()),
  mParticle1Idx(p1Idx), mParticle2Idx(p2Idx),
  mDcaDaughters(std::numeric_limits<unsigned short>::max()), mCosThetaStar(std::numeric_limits<char>::min())
{
   if ((!particle1 || !particle2) || (particle1->id() == particle2->id()))
   {
      mParticle1Idx = std::numeric_limits<unsigned short>::max();
      mParticle2Idx = std::numeric_limits<unsigned short>::max();
      return;
   }

   /// prefixes code:
   ///   k means kaon  -> p1
   ///   p means pion  -> p2
   ///   kp means kaon-pion pair -> pair


   // to be used for testing with preview II pico production
   StDcaGeometry p1DcaG, p2DcaG;
   p1DcaG.set(particle1->params(), particle1->errMatrix());
   p2DcaG.set(particle2->params(), particle2->errMatrix());
   StPhysicalHelixD p1Helix = p1DcaG.helix();
   StPhysicalHelixD p2Helix = p2DcaG.helix();
   // to be used in official production
   //StPhysicalHelixD p1Helix = particle1->dcaGeometry().helix();
   //StPhysicalHelixD p2Helix = particle2->dcaGeometry().helix();

   // move origins of helices to the primary vertex origin
   p1Helix.moveOrigin(p1Helix.pathLength(vtx));
   p2Helix.moveOrigin(p2Helix.pathLength(vtx));

   // use straight lines approximation to get point of DCA of particle1-particle2 pair
   StThreeVectorF const p1Mom = p1Helix.momentum(bField * kilogauss);
   StThreeVectorF const p2Mom = p2Helix.momentum(bField * kilogauss);
   StPhysicalHelixD const p1StraightLine(p1Mom, p1Helix.origin(), 0, particle1->charge());
   StPhysicalHelixD const p2StraightLine(p2Mom, p2Helix.origin(), 0, particle2->charge());

   pair<double, double> const ss = p1StraightLine.pathLengths(p2StraightLine);
   StThreeVectorF const p1AtDcaToP2 = p1StraightLine.at(ss.first);
   StThreeVectorF const p2AtDcaToP1 = p2StraightLine.at(ss.second);

   // calculate DCA of particle1 to particl2 at their DCA
   float const dcaDaughters = (p1AtDcaToP2 - p2AtDcaToP1).mag();
   mDcaDaughters = (dcaDaughters * 10000.) > std::numeric_limits<unsigned short>::max() ?
                   std::numeric_limits<unsigned short>::max() : static_cast<unsigned short>(std::round(dcaDaughters * 10000.));

   // calculate Lorentz vector of particle1-particle2 pair
   StThreeVectorF const p1MomAtDca = p1Helix.momentumAt(ss.first,  bField * kilogauss);
   StThreeVectorF const p2MomAtDca = p2Helix.momentumAt(ss.second, bField * kilogauss);

   StLorentzVectorF const p1FourMom(p1MomAtDca, p1MomAtDca.massHypothesis(mParticle1MassHypo));
   StLorentzVectorF const p2FourMom(p2MomAtDca, p2MomAtDca.massHypothesis(mParticle2MassHypo));

   mLorentzVector = p1FourMom + p2FourMom;

   // calculate cosThetaStar
   StLorentzVectorF const pairFourMomReverse(-mLorentzVector.px(), -mLorentzVector.py(), -mLorentzVector.pz(), mLorentzVector.e());
   StLorentzVectorF const p1FourMomStar = p1FourMom.boost(pairFourMomReverse);
   mCosThetaStar = static_cast<char>(std::round(std::cos(p1FourMomStar.vect().angle(mLorentzVector.vect())) * 100.));

   // calculate pointing angle and decay length
   StThreeVectorF const vtxToV0 = (p1AtDcaToP2 + p2AtDcaToP1) * 0.5 - vtx;
   mPointingAngle = vtxToV0.angle(mLorentzVector.vect());
   mDecayLength = vtxToV0.mag();

   // calculate DCA of tracks to primary vertex
   mParticle1Dca = (p1Helix.origin() - vtx).mag();
   mParticle2Dca = (p2Helix.origin() - vtx).mag();
}
#endif // __ROOT__
