//-----------------------------------------------------------------------------
//  $Header$
//
//
//  Origin:   Marian Ivanov, Uni. of Bratislava, ivanov@fmph.uniba.sk
//
//  Declaration of class AliTPCRF1D
//
//-----------------------------------------------------------------------------
#include "TMath.h"
#include "AliTPCRF1D.h"
#include "TF2.h"
#include <iostream.h>
#include "TCanvas.h"
#include "TPad.h"
#include "TStyle.h"
#include "TH1.h"

extern TStyle * gStyle;

static Double_t funGauss(Double_t *x, Double_t * par)
{
  return TMath::Exp(-(x[0]*x[0])/(2*par[0]*par[0]));
}

static Double_t funCosh(Double_t *x, Double_t * par)
{
  return 1/TMath::CosH(3.14159*x[0]/(2*par[0]));  
}    

static Double_t funGati(Double_t *x, Double_t * par)
{
  //par[1] = is equal to k3
  //par[0] is equal to pad wire distance
  Float_t K3=par[1];
  Float_t K3R=TMath::Sqrt(K3);
  Float_t K2=(TMath::Pi()/2)*(1-K3R/2.);
  Float_t K1=K2*K3R/(4*TMath::ATan(K3R));
  Float_t l=x[0]/par[0];
  Float_t tan2=TMath::TanH(K2*l);
  tan2*=tan2;
  Float_t res = K1*(1-tan2)/(1+K3*tan2);  
  return res;  
}    




///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

AliTPCRF1D * gRF1D;
ClassImp(AliTPCRF1D)


AliTPCRF1D::AliTPCRF1D(Bool_t direct,Int_t np,Float_t step)
{
  fDirect=direct;
  fNRF = np;
  fcharge = new Float_t[fNRF];
  fDSTEPM1=1./step;
  fSigma = 0;
  gRF1D = this;
  fGRF = 0;
  fkNorm = 0.5;
  fpadWidth = 3.5;
  forigsigma=0.;
  fOffset = 0.;
}


AliTPCRF1D::~AliTPCRF1D()
{
  if (fcharge!=0) delete fcharge;
  if (fGRF !=0 ) fGRF->Delete();
}

Float_t AliTPCRF1D::GetRF(Float_t xin)
{
  //x xin DSTEP unit
  //return linear aproximation of RF
  Float_t x = TMath::Abs((xin-fOffset)*fDSTEPM1)+fNRF/2;
  Int_t i1=Int_t(x);
  if (x<0) i1-=1;
  Float_t res=0;
  if (i1+1<fNRF)
    res = fcharge[i1]*(Float_t(i1+1)-x)+fcharge[i1+1]*(x-Float_t(i1));    
  return res;
}

Float_t  AliTPCRF1D::GetGRF(Float_t xin)
{  
  if (fGRF != 0 ) 
    return fkNorm*fGRF->Eval(xin)/fInteg;
      else
    return 0.;
}

   
void AliTPCRF1D::SetParam( TF1 * GRF,Float_t padwidth,
		       Float_t kNorm, Float_t sigma)
{
   fpadWidth = padwidth;
   fGRF = GRF;
   fkNorm = kNorm;
   if (sigma==0) sigma= fpadWidth/TMath::Sqrt(12.);
   forigsigma=sigma;
   fDSTEPM1 = 10/TMath::Sqrt(sigma*sigma+fpadWidth*fpadWidth/12); 
   sprintf(fType,"User");
   //   Update();   
}
  

void AliTPCRF1D::SetGauss(Float_t sigma, Float_t padWidth,
		      Float_t kNorm)
{
  // char s[120];
  fpadWidth = padWidth;
  fkNorm = kNorm;
  if (fGRF !=0 ) fGRF->Delete();
  fGRF = new TF1("fun",funGauss,-5,5,2);
  funParam[0]=sigma;
  forigsigma=sigma;
  fGRF->SetParameters(funParam);
   fDSTEPM1 = 10./TMath::Sqrt(sigma*sigma+fpadWidth*fpadWidth/12); 
  //by default I set the step as one tenth of sigma
   //  Update(); 
  sprintf(fType,"Gauss");
}

void AliTPCRF1D::SetCosh(Float_t sigma, Float_t padWidth,
		     Float_t kNorm)
{
  //  char s[120];
  fpadWidth = padWidth;
  fkNorm = kNorm;
  if (fGRF !=0 ) fGRF->Delete();
  fGRF = new TF1("fun",	funCosh, -5.,5.,2);   
  funParam[0]=sigma;
  fGRF->SetParameters(funParam);
  forigsigma=sigma;
  fDSTEPM1 = 10./TMath::Sqrt(sigma*sigma+fpadWidth*fpadWidth/12); 
  //by default I set the step as one tenth of sigma
  //  Update();
  sprintf(fType,"Cosh");
}

void AliTPCRF1D::SetGati(Float_t K3, Float_t padDistance, Float_t padWidth,
		     Float_t kNorm)
{
  //  char s[120];
  fpadWidth = padWidth;
  fkNorm = kNorm;
  if (fGRF !=0 ) fGRF->Delete();
  fGRF = new TF1("fun",	funGati, -5.,5.,2);   
  funParam[0]=padDistance;
  funParam[1]=K3;  
  fGRF->SetParameters(funParam);
  forigsigma=padDistance;
  fDSTEPM1 = 10./TMath::Sqrt(padDistance*padDistance+fpadWidth*fpadWidth/12); 
  //by default I set the step as one tenth of sigma
  //  Update(); 
  sprintf(fType,"Gati");
}

void AliTPCRF1D::Draw(Float_t x1,Float_t x2,Int_t N)
{ 
  char s[100];
  TCanvas  * c1 = new TCanvas("canRF","Pad response function",700,900);
  c1->cd();
  TPad * pad1 = new TPad("pad1RF","",0.05,0.55,0.95,0.95,21);
  pad1->Draw();
  TPad * pad2 = new TPad("pad2RF","",0.05,0.05,0.95,0.45,21);
  pad2->Draw();

  sprintf(s,"RF response function for %1.2f cm pad width",
	  fpadWidth);  
  pad1->cd();
  TH1F * hRFo = new TH1F("hRFo","Original charge distribution",N+1,x1,x2);
  pad2->cd();
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0); 
  TH1F * hRFc = new TH1F("hRFc",s,N+1,x1,x2);
  Float_t x=x1;
  Float_t y1;
  Float_t y2;

  for (Float_t i = 0;i<N+1;i++)
    {
      x+=(x2-x1)/Float_t(N);
      y1 = GetRF(x);
      hRFc->Fill(x,y1);
      y2 = GetGRF(x);
      hRFo->Fill(x,y2);      
    };
  pad1->cd();
  hRFo->Fit("gaus");
  pad2->cd();
  hRFc->Fit("gaus");
}

void AliTPCRF1D::Update()
{
  //initialize to 0
  for (Int_t i =0; i<fNRF;i++)  fcharge[i] = 0;
  if ( fGRF == 0 ) return;
  fInteg  = fGRF->Integral(-5*forigsigma,5*forigsigma,funParam,0.00001);
  if ( fInteg == 0 ) fInteg = 1; 
  if (fDirect==kFALSE){
  //integrate charge over pad for different distance of pad
  for (Int_t i =0; i<fNRF;i++)
    {      //x in cm fpadWidth in cm
      Float_t x = (Float_t)(i-fNRF/2)/fDSTEPM1;
      Float_t x1=TMath::Max(x-fpadWidth/2,-5*forigsigma);
      Float_t x2=TMath::Min(x+fpadWidth/2,5*forigsigma);
      fcharge[i] = 
	fkNorm*fGRF->Integral(x1,x2,funParam,0.0001)/fInteg;
    };   
  }
  else{
    for (Int_t i =0; i<fNRF;i++)
      {      //x in cm fpadWidth in cm
	Float_t x = (Float_t)(i-fNRF/2)/fDSTEPM1;
	fcharge[i] = fkNorm*fGRF->Eval(x);
      };   
  }  
  fSigma = 0; 
  Float_t sum =0;
  Float_t mean=0;
  for (Float_t  x =-fNRF/fDSTEPM1; x<fNRF/fDSTEPM1;x+=1/fDSTEPM1)
    {      //x in cm fpadWidth in cm
      Float_t weight = GetRF(x+fOffset);
      fSigma+=x*x*weight; 
      mean+=x*weight;
      sum+=weight;
    };  
  if (sum>0){
    mean/=sum;
    fSigma = TMath::Sqrt(fSigma/sum-mean*mean);   
  }
  else fSigma=0; 
}

void AliTPCRF1D::Streamer(TBuffer &R__b)
{
   // Stream an object of class AliTPC.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TObject::Streamer(R__b);     
      //read pad parameters
      R__b >> fpadWidth;
      //read charge parameters
      R__b >> fType[0];
      R__b >> fType[1];
      R__b >> fType[2];
      R__b >> fType[3];
      R__b >> fType[4];
      R__b >> forigsigma;
      R__b >> fkNorm;
      R__b >> fK3X;
      R__b >> fPadDistance; 
      R__b >> fInteg;
      R__b >> fOffset;
      //read functions
      if (fGRF!=0) { 
	delete fGRF;  
	fGRF=0;
      }
      if (strncmp(fType,"User",3)==0){
	fGRF= new TF1;
	R__b>>fGRF;   
      }
 
      if (strncmp(fType,"Gauss",3)==0) 
	fGRF = new TF1("fun",funGauss,-5.,5.,4);
      if (strncmp(fType,"Cosh",3)==0) 
	fGRF = new TF1("fun",funCosh,-5.,5.,4);
      if (strncmp(fType,"Gati",3)==0) 
	fGRF = new TF1("fun",funGati,-5.,5.,4);   
      R__b >>fDSTEPM1;  
      R__b >>fNRF;
      R__b.ReadFastArray(fcharge,fNRF); 
      R__b.ReadFastArray(funParam,5); 
      if (fGRF!=0) fGRF->SetParameters(funParam);     

   } else {
      R__b.WriteVersion(AliTPCRF1D::IsA());
      TObject::Streamer(R__b);   
      //write pad width
      R__b << fpadWidth;
      //write charge parameters
      R__b << fType[0];
      R__b << fType[1];
      R__b << fType[2];
      R__b << fType[3];
      R__b << fType[4];
      R__b << forigsigma;
      R__b << fkNorm;
      R__b << fK3X;
      R__b << fPadDistance;
      R__b << fInteg;
      R__b << fOffset;
      //write interpolation parameters
      if (strncmp(fType,"User",3)==0) R__b <<fGRF;   
      R__b <<fDSTEPM1;
      R__b <<fNRF;    
      R__b.WriteFastArray(fcharge,fNRF); 
      R__b.WriteFastArray(funParam,5); 
       
      

   }
}
 
