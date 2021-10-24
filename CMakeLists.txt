cmake_minimum_required(VERSION 3.10)

# set the project name
project(TDSDAQ VERSION 1.0)

message("===============  ${PROJECT_NAME}  ==============")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(ROOT REQUIRED)

execute_process(COMMAND root-config --has-cxx20 OUTPUT_VARIABLE HASCXX20)
execute_process(COMMAND root-config --has-cxx17 OUTPUT_VARIABLE HASCXX17)
execute_process(COMMAND root-config --has-cxx14 OUTPUT_VARIABLE HASCXX14)
execute_process(COMMAND root-config --has-cxx11 OUTPUT_VARIABLE HASCXX11)

set(CMAKE_CXX_STANDARD_REQUIRED YES)

if (${HASCXX20} MATCHES "yes")
    set(CMAKE_CXX_STANDARD 20)
elseif (${HASCXX17} MATCHES "yes")
    set(CMAKE_CXX_STANDARD 17)
elseif (${HASCXX14} MATCHES "yes")
    set(CMAKE_CXX_STANDARD 14)
elseif (${HASCXX11} MATCHES "yes")
    set(CMAKE_CXX_STANDARD 11)
endif ()


message("")
message("TDSDAQ will be installed in : ${CMAKE_INSTALL_PREFIX}")
message("===========================================================")

if( ${TDS_DAQ} MATCHES "OFF" )
else ()
    add_subdirectory(TDS)
endif ()

add_subdirectory(ANA)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config
       DESTINATION .
       COMPONENT install
       )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/macros
       DESTINATION .
       COMPONENT install
       )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
       DESTINATION .
       COMPONENT install
       )


#Create thisTDS.sh
include(thisTDS)


