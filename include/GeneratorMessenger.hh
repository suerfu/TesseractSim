/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

// $Id: GeneratorMessenger.hh $
//
/// \file GeneratorMessenger.hh
/// \brief Definition of the GeneratorMessenger class

#ifndef GeneratorMessenger_h
#define GeneratorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

class GeneratorAction;
class G4UIdirectory;

class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;

class GeneratorMessenger : public G4UImessenger {

public:

	GeneratorMessenger(GeneratorAction*);
	virtual ~GeneratorMessenger();

	virtual void SetNewValue(G4UIcommand*, G4String);

private:

	GeneratorAction* primaryGenerator;

	G4UIdirectory* primaryGeneratorDir;

	G4UIcmdWithAString* cmdSetSpectrum;

  G4UIcmdWithAString* cmdSetParticle;

	G4UIcmdWithAString* cmdSetSurfaceName;
  //G4UIcmdWithoutParameter* cmdSetPosition;
  //G4UIcmdWithoutParameter* cmdSetDir;
  //G4UIcmdWithoutParameter* cmdSetEnergy;
  //G4UIcmdWithoutParameter* cmdSetWall;
	G4UIcmdWithADoubleAndUnit* cmdSetWall_x;
	G4UIcmdWithADoubleAndUnit* cmdSetWall_y;
	G4UIcmdWithADoubleAndUnit* cmdSetWall_z;


};

#endif
