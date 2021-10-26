

#ifndef __ANAstructs__h__
#define __ANAstructs__h__

#include <cstdint>

// RT -> Real time
// LT -> Live Time = real time - dead time

struct ANABlockHead {

  uint64_t    TimeStamp;    
  uint64_t    RTTicks;       
  uint64_t    LTTicks;       
  uint64_t    SRate;
  uint16_t    PSize;        
  uint16_t    Pretrigger;   
  uint16_t    mVdiv[4];     
  uint16_t    NEvents;      
  uint16_t    NHits;        
  uint8_t     NegPolarity[4];  
};

struct ANAEventHead {
  uint64_t  clockTicksLT;
  uint64_t  clockTicksRT;
};

#endif

