
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <sys/stat.h>

#include "AnaTDS.h"

void help() {
    std::cout << " analyzeTDS options:" << std::endl;
    std::cout << "    --f       : Filename" << std::endl;
    std::cout << "    --h       : Print this help" << std::endl;
}

int main(int argc, char ** argv)
{
 
  std::string fileName="";

    for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
        if (arg == "--f") {
            i++;
            fileName = argv[i];
        } else if (arg == "--h") {
            help();
            return 0;
        } else {  // unmatched options
            std::cerr << "Warning invalid argument " << arg << std::endl;
            help();
            return -1;
        }
    }

    if(fileName.empty()){
      std::cerr<<"Please, provide a file name"<<std::endl;
      help();
      return -1;
    }

  struct stat fb;
  int index=0;
    if(stat (fileName.c_str(), &fb) == 0){
      std::string fileRoot = fileName +".root";
      AnaTDS::analizeTDS(fileName, fileRoot,index);
    } else {
      int nFiles=1;
      char outFileName[1024];
      sprintf(outFileName,"%s.%02d", fileName.c_str(), nFiles);
      std::cout<<outFileName<<std::endl;
        if(stat (outFileName, &fb) !=0){
          std::cerr<<"Filename "<<fileName<<" not found"<<std::endl;
          return -1;
        }
        while(stat (outFileName, &fb) == 0 ){
          std::string fName = outFileName;
          std::string fileRoot = fName +".root";
          AnaTDS::analizeTDS(fName, fileRoot,index);
          nFiles++;
          sprintf(outFileName,"%s.%02d", fileName.c_str(), nFiles);
        }
    
    
    }

  std::cout<<"Exiting analysis"<<std::endl;

}
