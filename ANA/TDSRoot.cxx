#include <TApplication.h>
#include <TROOT.h>
#include <TRint.h>
#include <TSystem.h>
#include <TStyle.h>


int main(int argc, char* argv[]) {

  printf("---------------------Wellcome to TDSRoot--------------------\n");

  gSystem->Load("libTDSANA.so");
  gSystem->AddIncludePath(" -I$TDS_INCLUDE_PATH");
  gROOT->ProcessLine(".L  $TDS_PATH/macros/utilTDS.C");

  gStyle->SetPalette(1);
  gStyle->SetTimeOffset(0);
  // display root's command line
  TRint theApp("App", &argc, argv);
  theApp.Run();

  return 0;
}
