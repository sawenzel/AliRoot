#ifndef ALIFLATEXTERNALTRACKPARAM_H
#define ALIFLATEXTERNALTRACKPARAM_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               *
 * Primary Authors : Sergey Gorbunov, Jochen Thaeder, Chiara Zampolli     */

/**
 * >> Flat structure representing parameters of an external track  <<
 */

#include "Rtypes.h"
#include "AliExternalTrackParam.h"

class AliFlatExternalTrackParam
{
 public:

  AliFlatExternalTrackParam();
  ~AliFlatExternalTrackParam() {}

  // constructor and method for reinitialisation of virtual table
  AliFlatExternalTrackParam( AliVConstructorReinitialisationFlag );
  void Reinitialize() { new (this) AliFlatExternalTrackParam( AliVReinitialize ); }
 
  void SetAlpha(Float_t alpha)             {fAlpha = alpha;}
  void SetX(Float_t x)                     {fX = x;}
  void SetY(Float_t y)                     {fY = y;}
  void SetZ(Float_t z)                     {fZ = z;}
  void SetSnp(Float_t snp)                 {fSnp = snp;}
  void SetTgl(Float_t tgl)                 {fTgl = tgl;}
  void SetSigned1Pt(Float_t signed1Pt)     {fSigned1Pt = signed1Pt;}
  void SetCovEntry(Int_t idx, Float_t cov) {(idx >= 0 && idx < 15) ? fC[idx] = cov : 0.;}

  Float_t  GetAlpha()             const {return fAlpha;}
  Float_t  GetX()                 const {return fX;}
  Float_t  GetY()                 const {return fY;}
  Float_t  GetZ()                 const {return fZ;}
  Float_t  GetSnp()               const {return fSnp;}
  Float_t  GetTgl()               const {return fTgl;}
  Float_t  GetSigned1Pt()         const {return fSigned1Pt;}
  Float_t* GetCov()               const {return const_cast<Float_t*>(fC);}
  Float_t  GetCovEntry(Int_t idx) const {return (idx >= 0 && idx < 15) ? fC[idx] : 0.;}
  Float_t  GetPt()                const {
    Double_t pt1 = fabs( fSigned1Pt );
    return (pt1>kAlmost0) ? 1./pt1 : kVeryBig;
  }
  void GetExternalTrackParam( AliExternalTrackParam &p ) const;
  void SetExternalTrackParam( const AliExternalTrackParam *p );

 private:

  Float_t fAlpha;     // azimuthal angle of reference frame
  Float_t fX;         // x: radial distance
  Float_t fY;         // local Y-coordinate of a track (cm)
  Float_t fZ;         // local Z-coordinate of a track (cm)
  Float_t fSnp;       // local sine of the track momentum azimuthal angle
  Float_t fTgl;       // tangent of the track momentum dip angle
  Float_t fSigned1Pt; // 1/pt (1/(GeV/c))
  Float_t fC[15];     // covariance matrix
};

inline AliFlatExternalTrackParam::AliFlatExternalTrackParam()
 :
 fAlpha(0),  
 fX(0),      				 
 fY(0),
 fZ(0),
 fSnp(0),
 fTgl(0),
 fSigned1Pt(0)
{
  for( int i=0; i<15; i++ ) fC[i]=0;
}

#pragma GCC diagnostic ignored "-Weffc++" 
inline AliFlatExternalTrackParam::AliFlatExternalTrackParam( AliVConstructorReinitialisationFlag ){}
#pragma GCC diagnostic warning "-Weffc++" 

inline void AliFlatExternalTrackParam::GetExternalTrackParam( AliExternalTrackParam &p ) const
{
 // Get external track parameters  
  Float_t par[5] = { fY, fZ, fSnp, fTgl, fSigned1Pt };
  p.Set( fX, fAlpha, par, fC );
}

inline void AliFlatExternalTrackParam::SetExternalTrackParam( const AliExternalTrackParam *p ) 
{
  // Set external track parameters
  if( !p ) return;
  fAlpha = p->GetAlpha();
  fX = p->GetX();
  fY = p->GetY();
  fZ = p->GetZ();
  fSnp = p->GetSnp();
  fTgl = p->GetTgl();
  fSigned1Pt = p->GetSigned1Pt();  
  for (Int_t idx = 0; idx <15; ++idx) fC[idx] = p->GetCovariance()[idx];
}

#endif
