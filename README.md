# TDSDAQ
Data AcQuisition software for TDS Oscilloscopes, tested so far on TDS5034B, may need to be revisited for the other oscilloscope types. Based on a software developed for anais and it is meant to be used for DAQ . The computer talks to the oscilloscope via ethernet, although other communication protocols may work if you the proper oscilloscope address is provided.

##Pre-requisites

This project requires ROOT installation and NI-VISA drivers, the instructions to install NI-VISA drivers are at the end of this document.

##Installation

Checkout the repository:
```
cd $HOME
git clone https://github.com... TDSDaq
cd TDSDAQ
```
Compile and install:
```
cd ~/TDSDaq
mkdir build
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=../install/ 
make -j4 install
```
To avoid the compilation of TDS_DAQ (e.g. VISA libraries not present in your system), just append `-DTDS_DAQ=OFF`
```
cmake ../ -DCMAKE_INSTALL_PREFIX=../install/ -DTDS_DAQ=OFF
```
This will create an install directory from where you can source the repository:

```
source ../install/thisTDS.sh
```

Now you are ready to go....

Usefull commands:

Analyze data from binary file `analyzeTDS --f TFIIIxxx.raw`
Decode binary file and load it in TDSRoot `processdata TFIII0000.raw`

TDSRoot (load root, TDS libraries and macros ):
Load root (analyzed files) `readData("TFIIIxxx.raw")`
Draw amplitude (int) vs area `tree->Draw("Hit0.DC:Hit0.Area")`
Histogram with a given bin size and limits `tree->Draw("Hit0.Area>>h(1000,0,100000)")`
Save spectrum (histogram) `saveSpc(h,"Test.txt")`
Create a list after grafical cut `tree->Draw(">>list","CUTG")`
Draw pulses from the list `drawPulse(i++)`
Draw all pulses `drawAllPulses("OutName")`
Reset Event list `tree->SetEventList(0)`

##Instructions for installing NI-VISA drivers
Instructions for installations on Centos 8 (visa drivers are only supported on Ubuntu, CentOs, RedHat and SUSE, check this link for further details https://www.ni.com/es-es/support/downloads/drivers/download.ni-linux-device-drivers.html#409880)

Packages (from root dependencies, check https://root.cern/install/dependencies/)

sudo yum install git make cmake gcc-c++ gcc binutils \
libX11-devel libXpm-devel libXft-devel libXext-devel python3.8 openssl-devel

sudo yum install epel-release
sudo yum config-manager --set-enabled powertools
sudo yum install redhat-lsb-core gcc-gfortran pcre-devel \
mesa-libGL-devel mesa-libGLU-devel glew-devel ftgl-devel mysql-devel \
fftw-devel cfitsio-devel graphviz-devel libuuid-devel \
avahi-compat-libdns_sd-devel openldap-devel python3-numpy \
libxml2-devel gsl-devel readline-devel R-devel R-Rcpp-devel R-RInside-devel

Download NI packages:

https://www.ni.com/es-es/support/downloads/drivers/download.ni-linux-device-drivers.html#409880

Instructions to install NI-Drivers:
https://www.ni.com/es-es/support/documentation/supplemental/18/downloading-and-installing-ni-driver-software-on-linux-desktop.html

sudo yum install ni-rhel8centos8-drivers-2020.rpm
sudo yum install ni-visa

###Needs Kernel-devel package
sudo yum install "kernel-devel-uname-r == $(uname -r)"
sudo dkms autoinstall

###Important Start nipal service
sudo service nipal start



