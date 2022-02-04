#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(RockSpecAnalyzer)
#----------------------------------------------------------------------------
# Find ROOT
#
find_package(ROOT 6.18 CONFIG REQUIRED)
#----------------------------------------------------------------------------
# Find RicochetSimuOuput
#
#find_package(RicochetSimuOutput REQUIRED)
#----------------------------------------------------------------------------
# Use RUNPATH (must be set before executable definition)
#
include(GNUInstallDirs)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
#----------------------------------------------------------------------------
# Executable
#
add_executable(${PROJECT_NAME} "${PROJECT_NAME}.cc")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)
#target_link_libraries(${PROJECT_NAME} ROOT::Rint ROOT::Hist Ricochet::SimuOutput)
target_link_libraries(${PROJECT_NAME} ROOT::Rint ROOT::Hist ROOT::Tree)
#----------------------------------------------------------------------------
# Install
#
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
