
#include "TDS.h"
#include <vector>
#include <sys/stat.h>
#include <thread>
#include <chrono>
#include <ctime>

std::atomic<bool> TDS::abrt(false);

TDS::TDS(const std::string &address){

  ViStatus status;
  // Open a default Session
  status = viOpenDefaultRM(&fResourceManager);
  if (status < VI_SUCCESS) { throw(TDSException(TDSError(fViSession, status)));}

  // Open the gpib device
  status = viOpen(fResourceManager, (ViChar*)address.c_str(), VI_NULL, VI_NULL, &fViSession);
  if (status < VI_SUCCESS) { throw(TDSException(TDSError(fViSession, status)));}

  ViUInt32 retCnt;
  std::cout<<readTDS("*IDN?",128, retCnt)<<std::endl;

}

TDS::~TDS(){

  if (fViSession != VI_NULL) {viClear(fViSession); viClose(fViSession);}
  if (fResourceManager != VI_NULL) viClose(fResourceManager);

}

void TDS::TDSConfig(const std::string &configFile){

  ViUInt32 retCnt;

  writeTDS("ACQUIRE:STATE STOP", retCnt);

  // Open configuration file
  std::ifstream fCfg (configFile);
  if(!fCfg.is_open())throw TDSException("Cannot open config file "+configFile);

  std::string line;

  while (getline(fCfg,line)){
    // Skip commentaries
    if (line[0] == '#' || line.length() < 2)continue;
    // Send command to scope
    writeTDS(line, retCnt);
  }

  writeTDS("ACQUIRE:STATE STOP", retCnt);

}

void TDS::TDSSetAcqParams(const std::string &configFile){

  // Open configuration file
  std::ifstream fCfg (configFile);
   if(!fCfg.is_open())throw TDSException("Cannot open config file "+configFile);
  std::string line;

  std::cout<<"Opening cfg file "<<configFile<<std::endl;

  std::string TDSConfigFile= "TDS.cfg";

  while (getline(fCfg,line)){
    // Skip commentaries
    if (line[0] == '#' || line.length() < 2)continue;
    std::stringstream ss(line);
    std::string first;
    ss>>first;
      if(first=="NSIGNALS")ss >> fNSignals ;
      else if (first == "SAMPLING_RATE")ss >> fSamplingRate ;
      else if (first == "RANGE_CHANNEL_A") ss >> fRangeChannelA;
      else if (first == "RANGE_CHANNEL_B") ss >> fRangeChannelB;
      else if (first == "PRETRIGGER") ss >> fPreTrigger;
      else if (first == "BUFFER_DIMENSION") ss >>fNFrames;
      else if (first == "BLOCKS_PER_FILE") ss >>fMaxBlocksPerFile;
      else if (first == "PULSE_DEPTH") ss >>fPulseDepth;
      else if (first == "TDS_CONFIGURATION_FILE") ss >>TDSConfigFile;
      else if (first == "FILE_NAME" ) ss >>FileName;
      else if (first == "VERBOSE_LEVEL" ) ss >>verbose;
      else if (first == "NEGATIVE_POLARITY_CH_A") ss >> fPolarityA;
      else if (first == "NEGATIVE_POLARITY_CH_B") ss >> fPolarityB;
  }

  fFileNumber=-1,fNFiles=1;
  struct stat fb;
    do{
      fFileNumber++;
      sprintf(fOutFileName,"%s%04d.raw.%02d", FileName.c_str(), fFileNumber,fNFiles);
      //std::cout<<fOutFileName<<std::endl;
    }while(stat (fOutFileName, &fb) == 0);

  std::cout << "\nConfiguration parameters: " << std::endl;
  std::cout << "N SIGNALS: " << fNSignals << std::endl;
  std::cout << "SAMPLING RATE: " << fSamplingRate << std::endl; 
  std::cout << "RANGE_CHANNEL_A: " << fRangeChannelA << std::endl;
  std::cout << "RANGE_CHANNEL_B: " << fRangeChannelB << std::endl;
  std::cout << "PRETRIGGER: " << fPreTrigger << std::endl;
  std::cout << "BUFFER_DIMENSION: " <<fNFrames << std::endl;
  std::cout << "PULSE DEPTH: " << fPulseDepth << std::endl;
  std::cout << "BLOCKS_PER_FILE: " <<fMaxBlocksPerFile << std::endl;
  std::cout << "TDS_CONFIGURATION_FILE: " << TDSConfigFile << std::endl;
  std::cout << "File Name: " << fOutFileName << std::endl;

  ViUInt32 retCnt;

  //Configure acquisition paramenters
  TDSConfig(TDSConfigFile);
  std::string cmd = "HORIZONTAL:MAIN:SAMPLERATE "+std::to_string(fSamplingRate)+"E6";
  writeTDS(cmd, retCnt);
  cmd = "CH1:SCALE " + std::to_string(fRangeChannelA/1000.);
  writeTDS(cmd, retCnt);
  cmd = "CH2:SCALE " + std::to_string(fRangeChannelB/1000.);
  writeTDS(cmd, retCnt);
  cmd = "HORIZONTAL:RECORDLENGTH "+std::to_string(fPulseDepth);
  writeTDS(cmd, retCnt);
  cmd = "HORIZONTAL:TRIGGER:POSITION "+std::to_string(fPreTrigger);
  writeTDS(cmd, retCnt);
  writeTDS("DATA:START 1", retCnt);
  cmd = "DATA:STOP "+std::to_string(fPulseDepth);
  writeTDS(cmd, retCnt);
  cmd ="HORIZONTAL:FASTFRAME:COUNT "+std::to_string(fNFrames);
  writeTDS(cmd, retCnt);

  fDimBuffer = fNFrames*fPulseDepth;

  // Set NBytesToRead
  cmd = std::to_string(fDimBuffer);
  fNBytestoRead = cmd.length()+2;
}

void TDS::TDSInitFastFrame(){
  ViUInt32 retCnt;

  writeTDS("ACQUIRE:STOPAFTER SEQUENCE", retCnt);
  writeTDS("HORIZONTAL:FASTFRAME:STATE ON", retCnt);
  writeTDS("DESE 1;*ESE 1;*SRE 32", retCnt);

}

void TDS::TDSAcquireFF(){

  ViStatus status;
  ViUInt32 retCnt;

  // Clear registers and enable OPC event
  writeTDS("*CLS", retCnt);

  writeTDS("ACQUIRE:STATE RUN", retCnt);

  //Setting timeout to 10 min
  status = viSetAttribute(fViSession,VI_ATTR_TMO_VALUE,600000);
  if (status < VI_SUCCESS) {throw(TDSException(TDSError(fViSession, status)));}

  // Ask for an OPC event
  writeTDS("*OPC", retCnt);
  writeTDS("*WAI", retCnt);
}

void TDS::TDSWaitEvent()
{
  ViStatus status;
  ViUInt32 retCnt;

  int busy=0;
  do{
    std::string busyString = readTDS("BUSY?",1, retCnt);
    busy = std::atoi(busyString.c_str());
  } while (busy>0);

   //Setting timeout to default
   status = viSetAttribute(fViSession,VI_ATTR_TMO_VALUE,1000);
   if (status < VI_SUCCESS) {throw(TDSException(TDSError(fViSession, status)));}


}

void TDS::TDSDisableEvent()
{
  ViStatus status;
  ViEvent context=0;
  ViUInt32 retCnt;

  // Disable event queue untill the acquisition is launched again
  status = viDisableEvent( fViSession, VI_EVENT_SERVICE_REQ, VI_QUEUE);
  if (status < VI_SUCCESS) {throw(TDSException(TDSError(fViSession, status)));}

  int busy=0;
   do{
    std::string busyString = readTDS("BUSY?",1, retCnt);
    busy = std::atoi(busyString.c_str());
  } while (busy>0 );

  viClose(context);

}

void TDS::TDSStopFF()
{

  ViUInt32 retCnt;
  writeTDS("ACQUIRE:STATE STOP", retCnt);

}

int8_t* TDS::TDSGetFF( int nChannels)
{
  ViStatus status;
  ViUInt32 retCnt=0;
  char cAux;

  std::vector<int8_t> buffer(fDimBuffer*nChannels);

  for (int nch=0;nch< nChannels;nch++){
    std::string cmd = "DATA:SOURCE CH"+ std::to_string(nch+1);
    writeTDS(cmd, retCnt);
    writeTDS("curve?",retCnt);
    // FORMAT TO READ: #<x> <yyy> <n><n>....
    // <x>: Number of bytes for <yyy>
    // <yyy>: Number of bytes to transfer (record length)
    // Number of bytes computed in TDSConfigAcqParam and
    // Stored in NBytesToRead
    status = viRead(fViSession, (ViByte*)cmd.c_str(), fNBytestoRead , &retCnt);

    int acc=0;
      while (fDimBuffer != acc){
        status = viRead(fViSession,(ViBuf)&buffer[fDimBuffer*nch], fDimBuffer, &retCnt);
        if (status < VI_SUCCESS) {throw(TDSException(TDSError(fViSession, status)));}
        acc+=retCnt;
      }
  }

  return buffer.data();
}


uint64_t* TDS::TDSGetTimes(int nFrames, bool first){

  ViStatus status;
  ViUInt32 retCnt=0;
  timeDimBuffer = 50*nFrames;
  std::vector<char> timeBuffer (timeDimBuffer);

  std::string cmd = "HORIZONTAL:FASTFRAME:TIMESTAMP:ALL:CH1? 1 , "+std::to_string(nFrames);
  writeTDS(cmd,retCnt);

  status = viRead(fViSession,(ViBuf)&timeBuffer[0], timeDimBuffer, &retCnt);
  if (status < VI_SUCCESS) {throw(TDSException(TDSError(fViSession, status)));}

  char *ini = timeBuffer.data();
  std::vector <uint64_t>tickBuffer(nFrames);

    for(int i=0;i<nFrames;i++){
      ini = strchr(ini,':')+2;
      char timeStamp[256];
      strncpy(timeStamp, ini, 36);
      if (first && i==0){
        initTime = (uint64_t)TranslateTimeStamp(timeStamp);
        tickBuffer[i]=0;
      } else {
        double tf = TranslateTimeStamp(timeStamp);
        tickBuffer[i] = (uint64_t)(tf - initTime);
        //std::cout<<i<<" "<<(double)tf<<" "<<(double)tf-initTime<<std::endl;
      }
      ini += 36;
    }

  return tickBuffer.data();

}

double TDS::TranslateTimeStamp(const char* tStamp){//Return timeStamp in microseconds

  char months [][12] = {"Jan","Feb","Mar","Apr","May","Jun",
                       "Jul","Aug","Sep","Oct","Nov","Dec"};
  char theMonth [3];
  char auxTime [12];
  char hour[2]; char min[2]; char sec[2];
  struct tm tS;
  int hora, minuto, segundo, milis,micros;

  //Take time for ini timeStamp
  std::stringstream saux (tStamp);
  saux >> tS.tm_mday;
  saux >> theMonth;
  int m=0;
  for (m=0; m<12; m++)
    if (!strcmp(theMonth, months[m])) break;

  tS.tm_mon = m;
  saux >> tS.tm_year;
  tS.tm_year -= 1900;
  saux >> auxTime ;
  sscanf(auxTime, "%2d:%2d:%2d.%3d",&hora, &minuto, &segundo, &milis);
  tS.tm_hour = hora;
  tS.tm_min  = minuto;
  tS.tm_sec = segundo;
  time_t timeS = mktime(&tS);
  saux >> micros;
  
  double t = timeS*1E6 + milis*1E3 + micros;

  return t;
}

void TDS::TDSExitFastFrame()
{
  ViStatus status;
  ViUInt32 retCnt;

  // Set off fastframe mode
  writeTDS("HORIZONTAL:FASTFRAME:STATE OFF", retCnt);

  //Disable event queue
  status = viDisableEvent( fViSession, VI_EVENT_SERVICE_REQ, VI_QUEUE);
  if (status < VI_SUCCESS) {throw(TDSException(TDSError(fViSession, status)));}

}

void TDS::Flush(){

  auto initT = std::chrono::high_resolution_clock::now();

  fNTriggers += fNFrames;
  int8_t *pulses = TDSGetFF(fNSignals);
  uint64_t *ticks = TDSGetTimes(fNFrames,fNTriggers==fNFrames);

  ANABlockHead BlockHead;
  // RT Ticks
  BlockHead.RTTicks = ticks[fNFrames-1];
  // LT Ticks
  BlockHead.LTTicks = BlockHead.RTTicks-deadTime;
  
  std::cout<<ticks[0]<<" "<<(double)BlockHead.RTTicks<<std::endl;

  BlockHead.TimeStamp = (uint64_t)std::chrono::seconds(std::time(NULL)).count();

  // NEvents
  BlockHead.NEvents = fNFrames;
  // NHits
  BlockHead.NHits = fNFrames*fNSignals;//->from *2.

  BlockHead.mVdiv[0] = fRangeChannelA;BlockHead.mVdiv[1] = fRangeChannelB;
  BlockHead.mVdiv[2] = 0;BlockHead.mVdiv[3] = 0;
  BlockHead.NegPolarity[0] =fPolarityA;BlockHead.NegPolarity[1] =fPolarityB;
  BlockHead.Pretrigger = fPreTrigger;
  BlockHead.PSize = fPulseDepth;
  BlockHead.SRate = (uint64_t) (fSamplingRate*1000000);


  std::ofstream FOut (fOutFileName, std::ios::binary | std::ios::app);
  std::cout<<"File "<<fOutFileName<<" opened"<<std::endl;
  FOut.write((char*)&BlockHead, sizeof(BlockHead));

    for (int ev=0; ev<fNFrames; ev++){
      ANAEventHead EventHead;
      EventHead.clockTicksRT  = ticks[ev];
      EventHead.clockTicksLT  = EventHead.clockTicksRT - (uint64_t)deadTime;
      FOut.write((char*)&EventHead, sizeof(EventHead));
        for(int j=0;j<fNSignals;j++){
            int index = j*fNFrames*fPulseDepth+ev*fPulseDepth;
            //std::cout<<"Pulses index "<<index<<" "<<(int)pulses[index]<<std::endl;
	    FOut.write((char*)&pulses[index], fPulseDepth);
        }
    }

  std::cout<<"Pulses written in file "<<std::endl;

  fNBlocksPerFile++;

    if(fNBlocksPerFile>=fMaxBlocksPerFile){
      fNFiles++;
      sprintf(fOutFileName,"%s%04d.raw.%02d", FileName.c_str(), fFileNumber,fNFiles);
      fNBlocksPerFile=0;
    }

  // Refresh some information in screen.  !
  fprintf(stdout,"N Events in buffer: %d %d \nReal time: %lf Live time: %lf \nNFiles: %d\n", fNFrames, fNTriggers,(double)BlockHead.RTTicks/1000000.,(double)BlockHead.LTTicks/1000000.,fNFiles);

  auto endTime = std::chrono::high_resolution_clock::now();
  deadTime += std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - initT).count() * 1E-3;
  if(verbose>0)std::cout << "DEAD: " << deadTime << std::endl;

}

void TDS::Start(const int &maxEvents){

  TDSInitFastFrame();
  std::this_thread::sleep_for (std::chrono::seconds(2));

  while(!abrt && (fNTriggers <maxEvents || maxEvents==0) ){
    TDSAcquireFF();
    TDSWaitEvent();
    Flush();
    TDSStopFF();
  }

  TDSExitFastFrame();

}

void TDS::writeTDS(const std::string &cmd, ViUInt32 &retCnt){

  ViStatus status;

  if(verbose>0)std::cout<<cmd<<std::endl;

  std::string cm = cmd +"\n";//looks like is needed while using TCPIP
  status = viWrite(fViSession, (ViBuf) cm.c_str(), cm.length(), &retCnt);
  if (status < VI_SUCCESS) {throw(TDSException(TDSError(fViSession, status)));}


}

const std::string TDS::readTDS(const std::string &cmd, int length, ViUInt32 &retCnt){

  ViStatus status;

  writeTDS(cmd,retCnt);
  
  char cAux[1024];sprintf(cAux,"\0");
  status= viRead(fViSession, (ViByte*) cAux, length , &retCnt);
  if (status < VI_SUCCESS) {throw(TDSException(TDSError(fViSession, status)));}
  if(verbose>0)std::cout<<cAux<<std::endl;
  return std::string(cAux);
}

const std::string TDS::TDSError(ViSession &vi, ViStatus &status){

  ViChar buffer[256];
  viStatusDesc(vi, status, buffer);
  return std::string(buffer);

}


