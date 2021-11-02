

#ifndef __ANAstructs__h__
#define __ANAstructs__h__

#include <cstdint>

#define TDS_MAX_SIGNALS 4

// RT -> Real time
// LT -> Live Time = real time - dead time

struct ANABlockHead {

  uint64_t    TimeStamp;    
  uint64_t    RTTicks;       
  uint64_t    LTTicks;       
  uint64_t    SRate;
  uint16_t    PSize;        
  uint16_t    Pretrigger;   
  uint16_t    mVdiv[TDS_MAX_SIGNALS];     
  uint16_t    NEvents;      
  uint16_t    NHits;        
  uint8_t     NegPolarity[TDS_MAX_SIGNALS];  
};

struct ANAEventHead {
  uint64_t  clockTicksLT;
  uint64_t  clockTicksRT;
};

#endif

