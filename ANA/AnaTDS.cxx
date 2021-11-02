
#include "AnaTDS.h"
#include "ANAStructs.h"
#include "Hit.h"

#include <iostream>
#include <fstream>

#include <TFile.h>
#include <TTree.h>

void AnaTDS::analizeTDS(const std::string &fileRaw, const std::string &fileRoot, int &index){

  std::ifstream fin (fileRaw, std::ios::in | std::ios::binary);

  int pulse_depth;
  double srate;

  TFile * fout = new TFile (fileRoot.c_str(), "RECREATE");
  TTree * tree = new TTree ("tree", "TDS data");

  uint64_t prevTime=0;
  uint8_t mVdiv[TDS_MAX_SIGNALS];

  Hit * myHits[TDS_MAX_SIGNALS];

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
        std::cout<<"nEvents "<<nEvents<<" NHits "<<nHits<<std::endl;
        std::cout<< "SRATE: " << (double)srate << " PULSE DEPTH: " << pulse_depth<<" PRETRIGGER: "<< blockhead.Pretrigger << std::endl;
        assert(nHits < TDS_MAX_SIGNALS);
        for(int i=0; i<nHits;i++){
          myHits[i] = nullptr;
          std::cout<<"mVdiv "<<(int)blockhead.mVdiv[i]<<std::endl;
          std::cout<<"Polarity ch"<<i+1<<" "<<(int)blockhead.NegPolarity[i]<<std::endl;
          std::string hitName = "Hit"+std::to_string(i);
          tree->Branch(hitName.c_str(), &myHits[i] /*, 32000, 99*/);
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
        //if(index%nEvents ==0)std::cout<<"Clocks "<<index<<" "<<(double)eventhead.clockTicksLT<<" "<<(double)eventhead.clockTicksRT<<std::endl;
        if(fin.peek() == EOF)break;

        std::vector <Char_t>buffer(pulse_depth);

          for(int i=0; i<nHits;i++){
            fin.read((char *)&buffer[0],pulse_depth);
            size += pulse_depth;
            if(fin.peek() == EOF)break;

              if(myHits[i]){
                delete myHits[i];
                myHits[i] = nullptr;
              }

            myHits[i] = new Hit(i,index,pulse_depth,blockhead.mVdiv[i], srate, blockhead.Pretrigger,blockhead.NegPolarity[i], buffer);

            myHits[i]->SetClockTickLT (eventhead.clockTicksLT);
            myHits[i]->SetClockTickRT (eventhead.clockTicksRT);
            // Set delta time (real time since previous event)
              if(index==0)myHits[i]->SetDeltaTime(0);
              else myHits[i]->SetDeltaTime ( eventhead.clockTicksRT - prevTime);

            if(i==nHits-1)prevTime = eventhead.clockTicksRT;
            myHits[i]->analyzeHit();

            //if(index%nEvents ==0)std::cout<<"Clocks "<<index<<" "<<(double)myHits[i]->GetClockTickLT()<<" "<<(double)myHits[i]->GetClockTickLT()<<std::endl;
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


