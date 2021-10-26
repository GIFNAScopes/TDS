#include <iostream>
#include <iomanip>
#include <sys/stat.h>

#include "Hit.h"

double entries=0;
int i=0;
int NHITS=0;

TFile * myFile=nullptr;
TChain * tree=nullptr;
std::vector<Hit *> myHits;
std::vector<TH1F *> pulseAll;
std::vector<TH1C *> histos;

///////////////////////////////////////////////////
// Read spectrum from ascii file and store it in h
///////////////////////////////////////////////////
void readSpc(TH1 * h, const std::string &filename)
{
  ifstream fq (filename, ios::in);
  double aux;

  for (int b = 1; b<=h->GetNbinsX(); b++) {
    fq >> aux;
    h->SetBinContent(b,aux);
  }
  fq.close();
}

///////////////////////////////////////////////////
// Read spectrum of nChannels from ascii file and returns
// an hitogram with the spectrum
///////////////////////////////////////////////////
TH1F * readSpc(const std::string &filename, int nChannels){

  TH1F * h=new TH1F (filename.c_str(), filename.c_str(), nChannels, 0, nChannels);
  ifstream fq (filename, ios::in);
  double aux;

  for (int b = 1; b<=h->GetNbinsX(); b++)
  {
    fq >> aux;
    h->SetBinContent(b,aux);
  }
  fq.close();
 
  return h;
}

///////////////////////////////////////////////////
// Save histogram h in ascii file filename
///////////////////////////////////////////////////
void saveSpc(TH1 * h, const std::string &filename)
{
  ofstream fq (filename, ios::out);
  // MARIA 04/03/06
  //for (int i = 0; i<h->GetNbinsX(); i++)
  for (int b = 1; b<=h->GetNbinsX(); b++)
  {
    fq << h->GetBinContent(b) << "\n";
  }
  fq.close();
}

// SOLO CARGA  LOS DATOS Y LANZA EL VIEWER, SIN REPRESENTAR NADA
// Usage: readData filename (optional)ini (optional)end
// Filename : FILENAMExxxx.raw
// if ini and end are not given, read all files of this run
// if only ini is given, read only that file
// if both are given, read from ini to end
void readData(const std::string &fileName)
{

  tree = new TChain("tree");

  struct stat fb;
   char outFileName[1024];
   sprintf(outFileName,"%s.root", fileName.c_str());
   if(fileName.find(".root") !=std::string::npos && stat (fileName.c_str(), &fb) == 0){
      std::cout<<fileName<<std::endl;
      tree->Add(fileName.c_str(), -1);
    } else if(stat (outFileName, &fb) == 0){
      std::cout<<outFileName<<std::endl;
      tree->Add(outFileName, -1);
    } else {
      int nFiles=1;
      sprintf(outFileName,"%s.%02d.root", fileName.c_str(), nFiles);
        if(stat (outFileName, &fb) !=0){
          std::cerr<<"Filename "<<outFileName<<" not found"<<std::endl;
          return;
        }
        while(stat (outFileName, &fb) == 0 ){
          std::cout<<outFileName<<std::endl;
          tree->Add(outFileName, -1);
          nFiles++;
          sprintf(outFileName,"%s.%02d.root", fileName.c_str(), nFiles);
        }
    }

  // Set stimation of the tree size 
  entries = tree->GetEntries();
  tree->SetEstimate(entries);

  //tree->StartViewer();

   for(NHITS=0;NHITS<4;NHITS++) {
    std::string brName = "Hit"+std::to_string(NHITS);
    if(!tree->GetBranch(brName.c_str()))break;
  }
  //--NHITS;
  std::cout<<"Number of hits "<<NHITS<<std::endl;
  myHits.resize(NHITS);

  for(int b=0;b<NHITS;b++){
    std::string brName = "Hit"+std::to_string(b);
    myHits[i] = nullptr;
    tree->SetBranchAddress(brName.c_str(), &myHits[i]);
    tree->GetEntry(0);
    std::cout<<"CH "<<b+1<< " "<<  myHits[i]->VScale<<" mv/Div "<<std::endl;
  }

  std::cout << "Sampling Rate: " << (myHits[0]->SRate) << " Hz  ( " << 1E9/(myHits[0]->SRate) << " ns/pt)"<< std::endl;
  std::cout << "Size Pulse: " << (myHits[0]->pulse_depth) << " points"<<std::endl;
  std::cout << "Pulse Length: "<< 1E9*float(myHits[0]->pulse_depth)/(myHits[0]->SRate)<< " ns"<< std::endl;
  std::cout << "PreTrigger: " << myHits[0]->Pretrigger << " %" << std::endl << std::endl;
  //gROOT->ProcessLine(".x  $ANAPIII/alias.C");

  // Maria 150307
  std::cout << "N Entries: " << entries << std::endl;
  tree->GetEntry(entries-1);
  double auxRTime = myHits[0]->GetClockTickRT()*1E-6;
  std::cout << "Real Time: " << auxRTime << std::endl;
  double auxLTime = myHits[0]->GetClockTickLT()*1E-6;
  std::cout << "Live Time: " << auxLTime << std::endl;
  tree->GetEntry(0);
  std::cout << "Inc Real Time: " << auxRTime - myHits[0]->GetClockTickRT()*1E-6 << std::endl;
  std::cout << "Inc Live Time: " << auxLTime - myHits[0]->GetClockTickLT()*1E-6 << std::endl;

}
///////////////////////////////////////////////////
// Add data to an existing tree
//  Be careful with RT and LT clocks, they are not arranged
///////////////////////////////////////////////////
void addData(const std::string &fileName)
{
     if(!tree){
       std::cout<<"Tree is not inilialized, use readData first "<<std::endl;
       return;
     }

   struct stat fb;
   char outFileName[1024];
   sprintf(outFileName,"%s.root", fileName.c_str());
   if(fileName.find(".root") !=std::string::npos && stat (fileName.c_str(), &fb) == 0){
      std::cout<<fileName<<std::endl;
      tree->Add(fileName.c_str(), -1);
    } else if(stat (outFileName, &fb) == 0){
      std::cout<<outFileName<<std::endl;
      tree->Add(outFileName, -1);
    } else {
      int nFiles=1;
      char outFileName[1024];
      sprintf(outFileName,"%s.%02d.root", fileName.c_str(), nFiles);
        if(stat (outFileName, &fb) !=0){
          std::cerr<<"Filename "<<fileName<<" not found"<<std::endl;
          return;
        }
        while(stat (outFileName, &fb) == 0 ){
          std::cout<<outFileName<<std::endl;
          tree->Add(outFileName, -1);
          nFiles++;
          sprintf(outFileName,"%s.%02d.root", fileName.c_str(), nFiles);
        }
    }

  // Set stimation of the tree size 
  entries = tree->GetEntries();
  tree->SetEstimate(entries);
  cout << "N Entries : " << entries << endl;
}

///////////////////////////////////////////
// Draw pulses corresponding to p event
// ALWAIS AFTER drawSpc !!!!
// p number corresponds to index in 'list' Event list
// if it exists
/////////////////////////////////////////
void drawPulse(int p){

  TEventList * list = (TEventList*)(gDirectory->Get("list"));
  tree->SetEventList(list);

  int ent = tree->GetEntryNumber(p);
  cout << ent << endl;
    // Maria 2/08/05
  if(ent<0 || ent>= tree->GetEntries()){
    std::cout<<"Entry "<<p <<" out of range 0-"<<tree->GetEntries()-1<<std::endl;
    return;
  }

   for(auto &h : histos){
     delete h;
   }
   histos.clear();

  tree->GetEntry(ent); 
 
  histos.resize(NHITS);
 
  for(int iii=0;iii<NHITS;iii++){
	  histos[iii] = (TH1C *)(myHits[iii]->getHisto()->Clone()) ;
  	  histos[iii]->SetStats(0);
	  histos[iii]->GetYaxis()->SetLabelSize(.08);
	  histos[iii]->GetXaxis()->SetLabelSize(.08);
	    if(iii==0)histos[iii]->Draw();
	    else histos[iii]->Draw("SAME");
	  (histos[iii]->GetYaxis())->SetRangeUser(-128,128);
  }

}

/////////////////////////////////////////////////////////
// Add together all pulses in 'list' event list
// and plot it
// ALWAYS AFTER drawSpc!!!!!
// The pulses are stored in pulse0All, pulse1All. 
// To access them, do
// TH1F * paux0 = (TH1F*)gDirectory->Get("pulse0All")
// TH1F * paux1 = (TH1F*)gDirectory->Get("pulse1All")
///////////////////////////////////////////////////////
void drawAllPulses(std::string fName ="")
{

 for(auto &p : pulseAll){
   delete p;
 }

 pulseAll.clear();

  TEventList * list = (TEventList*)(gDirectory->Get("list"));
  tree->SetEventList(list);

  // Maria 2/08/05
  int ent,cont;
  int max =tree->GetSelectedRows();

  if(max==0) max=tree->GetEntries(); //if no list selected use all pulses.

  pulseAll.resize(NHITS);
    for(int iii=0;iii<NHITS;iii++) {
      std::string pName = "Pulse"+std::to_string(NHITS+1);
      pulseAll[iii] = new TH1F (pName.c_str(), pName.c_str(),myHits[iii]->pulse_depth,0,myHits[iii]->pulse_depth);// 2500
    }

  for (cont = 0; cont<max; cont ++){
    ent = tree->GetEntryNumber(cont);
    tree->GetEntry(ent); 
      for(int iii=0;iii<NHITS;iii++){
        TH1C *h = (TH1C *)(myHits[iii]->getHisto()->Clone());
        pulseAll[iii]->Add(h);
        delete h;
      }
  }

  // Maria 260307
  for(int iii=0;iii<NHITS;iii++) {
    pulseAll[iii]->Scale(1./max);
    pulseAll[iii]->SetStats(0);
    pulseAll[iii]->Draw(); 

	if(!fName.empty()){
	  std::string name = "CH"+std::to_string(iii)+"_"+ fName;
	  saveSpc(pulseAll[iii], name);
	}
  }

}

