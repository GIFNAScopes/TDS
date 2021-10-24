

find_package(VISA REQUIRED)

add_library(TDSDAQ SHARED TDS.cxx)
target_include_directories(TDSDAQ PUBLIC ${VISA_INCLUDE_DIRS})

target_link_libraries(TDSDAQ ${VISA_LIBRARIES})

install(TARGETS TDSDAQ DESTINATION lib)

#Add execuatable and link to the libraries
add_executable(TDSDaq TDSDaq.cxx)
target_link_libraries(TDSDaq LINK_PUBLIC TDSDAQ ${VISA_LIBRARIES})

install(TARGETS TDSDaq DESTINATION bin)

#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#        DESTINATION .
#        COMPONENT install
#        )

