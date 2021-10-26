
#include "Hit.h"
#include <TMath.h>

#include<iostream>

ClassImp(Hit);

void Hit::analyzeHit( ){

  auto smoothed = GetSignalSmoothed();

  int nPointsB = smoothed.size()*Pretrigger/200;//Baseline

  //std::cout<<"Pulse size "<<smoothed.size()<<" Points baseline "<<nPointsB<<" Pretrigger "<<Pretrigger<<std::endl;

  double baseLine = 0, baseLineSigma = 0;
  for (int i = 0; i < nPointsB; i++) {
    int val = smoothed[i];
    baseLine += val;
    baseLineSigma += val * val;
  }

  if (nPointsB > 0) {
    baseLine /= nPointsB;
    baseLineSigma = TMath::Sqrt(baseLineSigma / nPointsB - baseLine * baseLine);
  }

  //std::cout<<" Baseline "<<baseLine<<" "<<baseLineSigma<<std::endl;

  double max=-1;
  int maxBin=-1;
  double integral =0;

  for(int i=0;i<smoothed.size();i++){
     double val =0;
     if(negPolarity) val = baseLine - smoothed[i];
     else val = smoothed[i] - baseLine;
       if(val >max){
         max=val;
         maxBin=i;
       }
     integral += val;
     if(negPolarity && smoothed[i]<=-127)saturated=true;
     else if(!negPolarity && smoothed[i]>=127)saturated=true;
  }

   int rsStart=-1, rsEnd=-1,wdStart=-1, wdEnd=-1,pEnd=-1;
   int integ2=0;
    for(int i=0;i<smoothed.size();i++){
      double val =0;
      //Subtract baseline
      if(negPolarity) val = baseLine - smoothed[i];
      else val = smoothed[i] - baseLine;
      integ2 += val;
      if(rsStart == -1 && val >= max*0.15)rsStart = i;
      if(wdStart == -1 && rsStart !=-1 && val >= max*0.5) wdStart = i;
      if(rsEnd == -1 && wdStart != -1 && val >= max*0.85)rsEnd = i;
      if(wdEnd == -1 && rsEnd !=-1 && val <= max*0.5) wdEnd = i;
      if(pEnd == -1 && integ2 >= integral*0.95) pEnd = i;
    }

  Area = AreaToPhys(integral);
  Max = maxBin;
  High = TimeToPhys(max);
  DC = std::round(max);
  T0 = rsStart; // In points
  TEnd = pEnd; // In points
  RT = rsEnd - rsStart;  // In points
  Width = wdEnd - wdStart;  // In points
  Baseline = baseLine;
  BaselineSigma = baseLineSigma;

  //std::cout<<"Max "<<max<<" Integral "<<integral<<" Width "<<Width<<" Risetime "<<RT<<std::endl;

}

std::vector<double> Hit::GetSignalSmoothed(int neighbours) {


    std::vector<double> smoothed(pulse_depth);
    
    for (int i = 0; i < pulse_depth; i++){
    int nPoints=0;
      for (int j = - neighbours; j<=neighbours;j++){
        int index = i + j;
        if(index<0||index>= pulse_depth)continue;
        int val = Pulse->GetBinContent(index+1);
        smoothed[i] += val;
        nPoints++;
      }
      if(nPoints)smoothed[i] /= nPoints ;
    }

  return smoothed;
}

const TH1C *Hit::getHisto( ){

  return Pulse;
}


