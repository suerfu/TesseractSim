# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(TesseractSim)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Find ROOT package
#
find_package(ROOT REQUIRED)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include(${ROOT_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Setup Geant4, ROOT include directories and compile definitions

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4, ROOT libraries
#
add_executable(TesseractSim TesseractSim.cc ${sources} ${headers})
target_link_libraries(TesseractSim ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build TesseractSim. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
file(COPY ${PROJECT_SOURCE_DIR}/macros DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/config DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/muon_config DESTINATION ${PROJECT_BINARY_DIR})
#set(SCRIPTS
#  /macros/gui.mac
#  /macros/init_vis.mac
#  /macros/vis.mac
#  /macros/icons.mac
#  /macros/test.mac
#  /macros/rock.mac
#  /macros/geantino.mac
#  /macros/rock_gamma_K40.mac
#  /macros/rock_gamma_Th232.mac
#  /macros/rock_gamma_U238.mac
#  /macros/rock_neutron_Th232.mac
#  /macros/rock_neutron_Unat.mac
#  /macros/wall_gamma_K40.mac
#  /macros/wall_gamma_Th232.mac
#  /macros/wall_gamma_U238.mac
#  /macros/wall_neutron_Th232.mac
#  /macros/wall_neutron_Unat.mac
#  /macros/virtualDetector_WallGenerator.mac
#  /config/geometry/cryoPlates_Ricochet.dat
#  /config/geometry/cryostatBeams_Ricochet.dat
#  /config/geometry/cryostatWall_Ricochet.dat
#  /config/geometry/dimensions_Ricochet.dat
#  /config/geometry/cryoPlates_Nucleus.dat
#  /config/geometry/cryostatBeams_Nucleus.dat
#  /config/geometry/cryostatWall_Nucleus.dat
#  /config/geometry/dimensions_Nucleus.dat
#  /config/generator/rockGammaK40.root
#  /config/generator/rockGammaTh232.root
#  /config/generator/rockGammaU238.root
#  /config/generator/rockNeutronTh232.root
#  /config/generator/rockNeutronUnat.root
#  )
#
#foreach(_script ${SCRIPTS})
#  configure_file(
#    ${PROJECT_SOURCE_DIR}/${_script}
#    ${PROJECT_BINARY_DIR}/${_script}
#    COPYONLY
#    )
#endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS TesseractSim DESTINATION bin)
