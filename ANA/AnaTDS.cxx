
#include "AnaTDS.h"
#include "ANAStructs.h"
#include "Hit.h"

#include <iostream>
#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <Rtypes.h>

void AnaTDS::analizeTDS(const std::string &fileRaw, const std::string &fileRoot, int &index){

  std::ifstream fin (fileRaw, std::ios::in | std::ios::binary);
  int DCPOINTS = 50;

  int pulse_depth;
  double srate;

  TFile * fout = new TFile (fileRoot.c_str(), "RECREATE");
  TTree * tree = new TTree ("tree", "TDS data");

  uint64_t prevTime=0;
  uint8_t mVdiv[4];

  std::vector<Hit *> myHits;

  int nEvents=0, nHits=0, size=0;
  bool first = true;

  do{

    ANABlockHead blockhead;
      if(first){
        fin.read((char*)&blockhead,sizeof(ANABlockHead));
        if(fin.peek() == EOF)break;
        nEvents = blockhead.NEvents;
        nHits = blockhead.NHits/blockhead.NEvents;
        srate = blockhead.SRate;
	pulse_depth = blockhead.PSize;
        myHits.resize(nHits);
        std::cout<<"nEvents "<<nEvents<<" NHits "<<nHits<<std::endl;
        std::cout<< "SRATE: " << (double)srate << " PULSE DEPTH: " << pulse_depth<<" PRETRIGGER: "<< blockhead.Pretrigger << std::endl;
        for(int i=0; i<nHits;i++){
          std::cout<<"Polarity ch"<<i+1<<" "<<(int)blockhead.NegPolarity[i]<<std::endl;
          std::string hitName = "Hit"+std::to_string(i);
          tree->Branch(hitName.c_str(), "Hit", &myHits[i] /*, 32000, 99*/);
        }
        first=false;
      } else {
        ANABlockHead dummyBH;
        fin.read((char*)&dummyBH,sizeof(ANABlockHead));
        //std::cout<<"nEvents "<<dummyBH.NEvents<<" NHits "<<dummyBH.NHits/dummyBH.NHits<< " SRATE: " << (double)dummyBH.SRate << " PULSE DEPTH: " << dummyBH.PSize<<" PRETRIGGER: "<< dummyBH.Pretrigger << std::endl;
      }
      size += sizeof(ANABlockHead);
      //std::cout<<"File position "<<fin.tellg()<<" "<<size<<std::endl;
      if(fin.peek() == EOF)break;

      for (int nev=0; nev<nEvents; nev++){
        ANAEventHead eventhead;
        fin.read((char*)&eventhead,sizeof(ANAEventHead));
        size += sizeof(ANAEventHead);
        //std::cout<<"File position "<<fin.tellg()<<" "<<size<<std::endl;
        //if(c<nEvents)std::cout<<"Clocks "<<c<<" "<<eventhead.clockTicksLT<<" "<<eventhead.clockTicksRT<<std::endl;
        if(fin.peek() == EOF)break;

        std::vector <int8_t>buffer(pulse_depth);

          for(int i=0; i<nHits;i++){
            fin.read((char *)&buffer[0],pulse_depth);
            size += pulse_depth;
            if(fin.peek() == EOF)break;
            //std::transform(buffer.begin(), buffer.end(), std::back_inserter(sData), [](int8_t x) { return (Short_t)x;});
            Hit hit(index,pulse_depth,blockhead.mVdiv[i], srate, blockhead.Pretrigger,blockhead.NegPolarity[i], buffer);
            myHits[i] =&hit;
            myHits[i]->SetClockTickLT (eventhead.clockTicksLT);
            myHits[i]->SetClockTickRT (eventhead.clockTicksRT);
            // Set delta time (real time since previous event)
            myHits[i]->SetDeltaTime ( eventhead.clockTicksRT - prevTime);
            prevTime = myHits[0]->GetClockTickRT();
            myHits[i]->analyzeHit();
          }
        index++;
        tree->Fill();
      }

    if(index%500==0)std::cout << "Processed events: "<<index<< std::endl;
  } while(fin.peek() != EOF);

  std::cout<<"Done "<<index<<" event processed"<<std::endl;
  
  fout->Write();
  fout->Close();

}

void AnaTDS::saveSpc(const TH1 *h, const std::string &filename){

  std::ofstream fq (filename);
  for (int i = 0; i<h->GetNbinsX(); i++){
    fq << h->GetBinContent(i)<<"\n";
  }

}


