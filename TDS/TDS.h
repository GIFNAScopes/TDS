
#ifndef __TDS_H__
#define __TDS_H__


#include "ANAStructs.h"

#include <visa.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <memory.h>
#include <time.h>
#include <signal.h>
#include <chrono>
#include <atomic>
#include <vector>

class TDSException : public std::exception {
  public:
    TDSException(const std::string& msg) : m_msg(msg){
    }

   ~TDSException(){ }

   virtual const char* what() const throw () {
        return m_msg.c_str();
   }
  private:
   const std::string m_msg;
};

class TDS{

  public:
  
  TDS(const std::string &cfgFile);
  ~TDS();
  void TDSConfig(const std::string &configFile);
  void TDSSetAcqParams(const std::string &configFile);
  void TDSInitFastFrame();
  void TDSAcquireFF();
  void TDSStopFF();
  std::vector<int8_t> TDSGetFF( int nChannels);
  std::vector<uint64_t> TDSGetTimes(int nFrames, bool first=false);
  void TDSWaitEvent();
  void TDSDisableEvent();
  void TDSExitFastFrame();
  void Flush();
  //ViStatus ServiceReqEventHandler (ViSession vi,ViEventType eventType,ViEvent event,ViAddr userHandle)
  double TranslateTimeStamp(const char* tStamp);

  static const std::string TDSError(ViSession &vi, ViStatus &status);
  void writeTDS(const std::string &cmd, ViUInt32 &retCnt);
  const std::string readTDS(const std::string &cmd, int length, ViUInt32 &retCnt );
  void Start(const int &maxEvents);

  static std::atomic<bool> abrt;

  private:
  
  int verbose =0;
  int fNFrames=100;
  int fNBytestoRead;
  int timeDimBuffer;
  int fDimBuffer;
  int fNTriggers=0;
  int fPulseDepth = 500;
  // Default values
  int fNSignals=1;      // Number of signals
  int fSamplingRate=250; // 250 MHz = 4ns/p
  int fRangeChannelA=200, fRangeChannelB=200;
  int fPolarityA=1, fPolarityB=0;
  int fPreTrigger=200;

  int fFileNumber=-1;     // Number of the file (FileNameFileNumber.raw.NFiles)
  int fNFiles=1;     // Number of the file (FileNameFileNumber.raw.NFiles)
  int fMaxBlocksPerFile=400;    //  Number of blocks per file (file ~ 100MB)
  int fNBlocksPerFile=0;

  double initTime=0;
  double deadTime=0;

  int fileSize =0;

  std::string FileName = "test";
  char fOutFileName [1024];
  std::string fAddress ="TCPIP::192.168.1.15::INSTR";

  ViSession fResourceManager = VI_NULL;
  ViSession fViSession = VI_NULL;
};

#endif


