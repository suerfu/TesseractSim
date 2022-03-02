# A Geant4 template for detector simulation

## Introduction
This repository is a template and a framework for writing Geant4 code for detector simulation. The framework provides some basic functions such as parsing of commandline arguments, messenger classes to set parameters for geometries, GeneratorAction and RunAction. The program also outputs a ROOT file detailing stepwise information of all the particles simulated.

## Installation
The code can be installed following the basic Geant4 application installation procedures. At the root of the project directory `TesseractSim/`, execute
```
mkdir ../build
cd ../build
cmake ../TesseractSim
make
```
Note that one needs ROOT to run this application.
*Add micros and config file names and pathes to CMakeLists.txt `set(SCRIPTS ...)`*

## Running the program
The basic usage of this program is: executable [-option [argument(s)] ]
- -m/--macro,       used to spefify the macro file to execute.
- -u/--interactive, enter interactive session.
- --vis,            enable visualization. (disabled by default)
- --seed,           the random seed to be used. (default current time)
- -o/--output       specify the output file name to which trajectories will be recorded.

Unlike many Geant4 examples, the program will do nothing by default. The user is responsible for specifying a macro to execute, or to enter interactive session. In the interactive mode, *init_vis.mac* will be executed by default.

To accomodate situations when the program is run in a cluster, visualization has to be turned on manually with *--vis* option.

By default, the program uses current time. This behavior can be altered with *--seed* option so that a user-specified random seed is used.

ROOT output is specified with *-o/--output* option. If the file already exists, it WILL NOT be overwritten.

## Output Format

The output ROOT file has three entries:
- macros used
- the random seeds
- a TTree entry called *events* recording all the steps of all particles in the simulation (except neutrinos)

The *events* TTree records the event ID, track ID, step ID, parent ID, particle name, kinetic information (xyz, momentum, energy, time) and the process that defined the step. There are three special *flag* processes:
* initStep: marks the beginning of a new track
* newEvent: marks the beginning of a new event
* timeReset: when a radioactive decay ocurrs, the timescale can exceed float precision. To preserve all information, all radioactive decays are treated as a new sub-event. User can always merge them later in the offline analysis.

The default units are mm for length, ns for time and keV for energy.

## Custom Commands

### Geometry
***Set geometry in the mac file before `/run/initialize`***
General dimensions and there names are stored in `config/geometry/dimensions_XX.dat`
Set files with command
```
/geometry/dimensionFile config/geometry/dimensions_XX.dat
```
Structually different geometries can be set by 
```
/geometry/type int
```
Implemented types include
- 10X : TESSERACT abstract geometry. Including shield, cryostat, and inner detector.
Dimensions of the geometries are stored under `config/geometry/`. New geometries should be add to new config files, follow the formats in each file. Dimensions to be implemented in a run are set by command
```
/geometry/cryostatWallFile config/geometry/cryostatWall_XX.dat
/geometry/cryoPlateFile config/geometry/cryoPlates_XX.dat
/geometry/cryoBeamFile config/geometry/cryoBeams_XX.dat
```
`X` indicate the type of inner detector. `100` is virtual, `101` is HeRALD, `102` is SPICE.
To set geometry by versions (https://docs.google.com/spreadsheets/d/1Rh030C20SL-vMcX-TB1AyDCaPTAWi5ll98lAL_tP6po/edit?usp=sharing)
```
/geometry/setByVersion Ricochet_V1_1
```
- 200 : Rock geometry.
Creates a 10m x 10m x 5m Rock with Chemical composition of Homestake cavern and density of 3.26 g/cm3. The rock material is placed at the bottom half of the world and top half is called a virtualDetector, which is basically vacuum that does not block any particle.
- other : "Cubic cow". The mock-up geometry is a cube target named *target* in the center of the world. The cube has an empty center, and is surrounded by a few farside detectors called *fsN* where N is a number starting from 0.

Available commands for `type 1` include 
```
/target/material materialName
/target/pos x y z unit
/target/rotateX(Y,Z) N unit
```
```
/farside/pos x y z unit
/farside/rotateX(Y,Z) N unit
/farside/set Name （apply the above parameters to farside detector called Name）
```

### Generator Action
```
/generator/spectrum config/generator/rockGammaU238.root
/generator/particle gamma
#can Specify:Top,Bottom,SideRight,SideRight,Front,Back, All
/generator/surfacename SideRight
/generator/wall_x 3 m
/generator/wall_y 3 m
/generator/wall_z 3 m
/generator/wall_center_position 0 0 0 m
```
Can be used to sample Energy and Azimuthal angle from ROOT file, that has a T2HF file with Energy on the X axis and Azimuthal angle on Yaxis.
One needs to specify the dimensions of cubic Wall, from where the events would be generated randomly. The user also needs to specify the Surface name, for eg: Top, Bottom, SideRight, SideLeft, Front, Back and All(case sensitive), from which the events would generated. Also needs to specify the particle name, and its user responsibily to match the particle and their ROOT file. Also the user needs to input the position and dimensions of the box.

### Filtering
```
/filter/recordWhenHit bar
```
Record the entire tracks when there is a hit in detector *bar* (physical volume name).
```
/filter/killWhenHit bar
```
Kill any particles that makes their way to detector named *bar* (physical volume name). Useful when you want to sample flux on a surface.
```
/filter/excludeParticle bar
/filter/excludeProcess bar
/filter/excludeVolume bar
```
Excludes certain particles, processes and volumes. When the condition matches, the step is not recorded. In addition to these explicit conditions, neutrinos are by default ignored.
