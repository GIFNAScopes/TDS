#ifndef __HIT_H__
#define __HIT_H__

//**************************
// Class for store the hits
//**************************

#include <Rtypes.h>
#include <TH1C.h>

class Hit 
{
  protected:
    ULong64_t ClockTickLT;
    ULong64_t ClockTickRT;
    ULong64_t DeltaTime; // Time since previous event
    TH1C *Pulse = nullptr;
    Float_t High;
    Short_t Max;
    Float_t Area;
    Float_t T0; // In points
    Float_t TEnd; // In points
    Float_t RT;  // In points
    Float_t Width;  // In points
    Int_t DC;
    Float_t Baseline;
    Float_t BaselineSigma;
    Bool_t saturated = false;
    Int_t ident;

public:
    const int pulse_depth {2500};
    const float VScale {0};
    const float SRate {1};
    const int Pretrigger {0};
    const bool negPolarity{true};

    Float_t AreaToPhys(Float_t a)
    {
	//Phys -> mV *ns
	return a*VScale*10/256.*1E9/SRate;
    }
    Float_t HighToPhys(Float_t h)
    {
	//Phys -> mV 
	return h*VScale*10/256.;
    }
    Float_t TimeToPhys(Float_t t)
    {
	//Phys -> ns
	return t*1E9/SRate;
    }

    void SetClockTickRT (ULong64_t clock) {ClockTickRT=clock;};
    void SetClockTickLT (ULong64_t clock) {ClockTickLT=clock;};
    void SetDeltaTime (ULong64_t time) {DeltaTime=time;};
    void SetHigh (Short_t h) {High=HighToPhys(h);};
    void SetMax (Short_t h) {Max=HighToPhys(h);};
    void SetArea  (Float_t a) {Area=AreaToPhys(a);};
    void SetT0 (Float_t t0) {T0=TimeToPhys(t0);};
    void SetTEnd  (Float_t te) {TEnd=te ;};

    ULong64_t GetClockTickRT () {return ClockTickRT;};
    ULong64_t GetClockTickLT () {return ClockTickLT;};
    ULong64_t GetDeltaTime () {return DeltaTime;};
    Short_t GetHigh() {return High;};
    Short_t GetMax() {return Max;};
    Float_t GetArea() {return Area;};

    Hit(int id, int size, int hscale, float srate, int pretrigger,bool nPol, std::vector<int8_t> &sData) : ident(id), pulse_depth(size),VScale(hscale),SRate(srate),Pretrigger(pretrigger), negPolarity(nPol) {
      std::string pName = "Pulse"+std::to_string(ident);
      Pulse = new TH1C (pName.c_str(),pName.c_str(), sData.size(), 0, sData.size());
      for(int i=0;i<sData.size();i++)Pulse->SetBinContent(i+1,sData[i]);
    }

    Hit(){
      Pulse = nullptr;
    }

    ~Hit(){
      if(Pulse)delete Pulse;
      Pulse = nullptr;
    }

    void analyzeHit( );
    std::vector<double> GetSignalSmoothed(int neighbours = 5);
    const TH1C *getHisto( );

   ClassDef(Hit, 1)

};


#endif

