

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/TDS ${ROOT_INCLUDE_DIRS})

find_package(ROOT 6.24 CONFIG REQUIRED)

root_generate_dictionary(G__Hit Hit.h LINKDEF HitLinkDef.h)

add_library(TDSANA SHARED Hit.cxx G__Hit.cxx AnaTDS.cxx)
target_include_directories(TDSANA PUBLIC ${CMAKE_SOURCE_DIR}/TDS ${ROOT_INCLUDE_DIRS})

target_link_libraries(TDSANA)

install(TARGETS TDSANA DESTINATION lib)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libHit_rdict.pcm DESTINATION lib)

#Add execuatable and link to the libraries
add_executable(analyzeTDS analyzeTDS.cxx)
target_link_libraries(analyzeTDS LINK_PUBLIC TDSANA ${ROOT_LIBRARIES})

install(TARGETS analyzeTDS DESTINATION bin)

add_executable(TDSRoot TDSRoot.cxx)
target_link_libraries(TDSRoot LINK_PUBLIC TDSANA ${ROOT_LIBRARIES})

install(TARGETS TDSRoot DESTINATION bin)

#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/*.h
#        DESTINATION .
#        COMPONENT install
#        )

