#ifndef MuonGeneratorMessenger_h
#define MuonGeneratorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class MuonGenerator;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

class MuonGeneratorMessenger: public G4UImessenger {
public:
	MuonGeneratorMessenger(MuonGenerator *generator);
	~MuonGeneratorMessenger();

	void SetNewValue(G4UIcommand *command, G4String newValue);

private:
  MuonGenerator *myMuonGenerator;
  
  G4UIdirectory *dirBaseDirectory;
  G4UIdirectory *dirInterface;
  G4UIcmdWithAnInteger *cmdVerbose;
  G4UIcmdWithAString *cmdProfileFile;
  G4UIcmdWithAnInteger *cmdNbOfZenithBins;
  G4UIcmdWithAnInteger *cmdNbOfAzimuthBins;
  G4UIcmdWithAnInteger *cmdNbOfEnergyBins;
  G4UIcmdWithADoubleAndUnit *cmdEnergyMin;
  G4UIcmdWithADoubleAndUnit *cmdEnergyMax;
  G4UIcmdWithADoubleAndUnit *cmdRadiusSphere;
  G4UIcmdWithADoubleAndUnit *cmdRadiusHemisphere;
  G4UIcmdWithoutParameter *cmdBuildLookUpTable;
};

#endif
