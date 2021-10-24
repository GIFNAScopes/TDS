#ifndef __ANA_TDS_H__
#define __ANA_TDS_H__

#include <string>
#include <TH1.h>

namespace AnaTDS {

void analizeTDS(const std::string &fileRaw, const std::string &fileRoot);
void saveSpc(const TH1 *h, const std::string &filename);

}

#endif
