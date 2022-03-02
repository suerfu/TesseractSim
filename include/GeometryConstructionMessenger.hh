/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

/// \file GeometryConstructionMessenger.hh
/// \brief Definition of the GeometryConstructionMessenger class

#ifndef GEOMETRYCONSTRUCTIONMESSENGER_H
#define GEOMETRYCONSTRUCTIONMESSENGER_H 1

#include "globals.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "GeometryConstruction.hh"

class GeometryConstruction;
class G4UIdirectory;

class GeometryConstructionMessenger: public G4UImessenger{

public:

    GeometryConstructionMessenger(GeometryConstruction* det);

    virtual ~GeometryConstructionMessenger();

    virtual void SetNewValue(G4UIcommand*, G4String);

private:

    GeometryConstruction* detector;

    G4UIdirectory* directory;
	G4UIcmdWithAnInteger* fGeoTypeCmd;
	G4UIcmdWithAString*   fDimensionFileCmd;
	G4UIcmdWithAString*   fCryostatWallFileCmd;
	G4UIcmdWithAString*   fCryoPlateFileCmd;
	G4UIcmdWithAString*   fCryoBeamFileCmd;
	G4UIcmdWithAString*   fSetByVersionCmd;
	G4UIcmdWithoutParameter* fConstructGeoCmd;

   // G4UIcmdWith3VectorAndUnit* posCmd;
   //     // Command to specify position of the farside detector
   // G4UIcmdWithADoubleAndUnit* angCmd_x;
   // G4UIcmdWithADoubleAndUnit* angCmd_y;
   // G4UIcmdWithADoubleAndUnit* angCmd_z;
   //     // Command to specify angle of rotation the farside detector
   // G4UIcommand* place_detector;
};

#endif
