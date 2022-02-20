cmake_minimum_required( VERSION 2.8 )

project( analysis )

SET( CMAKE_CXX_STANDARD 11)

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

include_directories( include )
file( GLOB SOURCES "src/*.cpp" )

add_executable( ProcessTrack ProcessTrack.cpp ${SOURCES} )
target_link_libraries( ProcessTrack ${ROOT_LIBRARIES} )

add_executable( RockSpecAnalyzer RockSpecAnalyzer.cpp )
target_link_libraries( RockSpecAnalyzer ${ROOT_LIBRARIES} )

add_executable( EnergyDepVSIntialEnergy EnergyDepVSIntialEnergy.cpp )
target_link_libraries( EnergyDepVSIntialEnergy ${ROOT_LIBRARIES} )
