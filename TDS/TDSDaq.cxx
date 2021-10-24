
#include "TDS.h"
#include <iostream>
#include <signal.h>

void signal_handler(int signum) {
    std::cout << " signal handler " << signum << std::endl;
    TDS::abrt=true;
}

void help() {
    std::cout << " TDSDaq options:" << std::endl;
    std::cout << "    --a       : Set device address" << std::endl;
    std::cout << "    --c       : Set configFile" << std::endl;
    std::cout << "    --e       : Max events" << std::endl;
    std::cout << "    --h       : Print this help" << std::endl;
}

int main(int argc, char* argv[])
{

  std::string cfgFile = "anais.cfg";
  std::string address = "TCPIP::192.168.1.15::INSTR";
  int maxEvents =0;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--c") {
            i++;
            cfgFile = argv[i];
        } else if (arg == "--a") {
            i++;
            address = argv[i];
        } else if (arg == "--e") {
            i++;
            maxEvents = std::atoi(argv[i]);
        } else if (arg == "--h") {
            help();
            return 0;
        } else {  // unmatched options
            std::cerr << "Warning invalid argument " << arg << std::endl;
            help();
            return -1;
        }
    }

  std::cout <<" Cfg file "<<cfgFile<<std::endl;
  std::cout << "OSC address "<<address << std::endl;
  std::cout << "Maximum number of events ";
  maxEvents==0 ? std::cout<<"Infinite" : std::cout<<maxEvents;
  std::cout<<std::endl;

  TDS *TDSDaq = nullptr;

  try {
    TDSDaq = new TDS(address);
    TDSDaq->TDSSetAcqParams(cfgFile);//Also configures the OSC
  } catch(const TDSException& e) {
    std::cerr<<"TDSException was thrown: "<<e.what()<<std::endl;
    return -1;
  } catch (const std::exception& e) {
    std::cerr<<"std::exception was thrown: "<<e.what()<<std::endl;
    return -1;
  }

  std::cout << "Press a key to start..." << std::endl;
  getchar();

    std::cout << "Installing signal handler" << std::endl;
    // Install signal handler
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGILL, signal_handler);
    signal(SIGTRAP, signal_handler);
    signal(SIGABRT, signal_handler);
    signal(SIGIOT, signal_handler);
    signal(SIGFPE, signal_handler);
    signal(SIGBUS, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGKILL, signal_handler);
    signal(SIGPIPE, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGSTKFLT, signal_handler);
    signal(SIGSYS, signal_handler);

  try {
    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    std::cout<<"Start time "<<std::ctime(&start_time)<<std::endl;

    TDSDaq->Start(maxEvents);

    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::cout<<"End time "<<std::ctime(&end_time)<<std::endl;
    std::cout<<"Duration "<<std::chrono::duration_cast<std::chrono::seconds>(end - start).count()<<" seconds"<<std::endl;
  } catch(const TDSException& e) {
    std::cerr<<"TDSException was thrown: "<<e.what()<<std::endl;
  } catch (const std::exception& e) {
    std::cerr<<"std::exception was thrown: "<<e.what()<<std::endl;
  }

  std::cout<<"Exiting..."<<std::endl;

}


