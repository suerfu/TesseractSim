#include "MuonGeneratorMessenger.hh"
#include "MuonGenerator.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"

MuonGeneratorMessenger::MuonGeneratorMessenger(MuonGenerator *generator) :
	myMuonGenerator(generator) {

	/*Base directory--------------------------------------------------------------*/
	//Create for the current detector a subdirectory within the base directory of "/Tesseract/"
	G4String baseDirectory = "/generator/";
	dirBaseDirectory = new G4UIdirectory(baseDirectory);

	G4String strMuonGenerator = baseDirectory.append("MuonGenerator/");
	dirInterface = new G4UIdirectory(strMuonGenerator);
	dirInterface->SetGuidance("Muon generator according to Markus Horn.");

	//Macro command to set the verbosity level of neutron counter related information
	G4String strVerbose = strMuonGenerator;
	strVerbose.append("Verbose");
	cmdVerbose = new G4UIcmdWithAnInteger(strVerbose, this);
	cmdVerbose->SetGuidance("Set the verbose level of the muon generator.");
	cmdVerbose->SetGuidance("Default value is 1");
	cmdVerbose->SetDefaultValue(1);

	G4String strMuonPhysic = strMuonGenerator;
	strMuonPhysic.append("PathToProfileFile");
	cmdProfileFile = new G4UIcmdWithAString(strMuonPhysic, this);
	cmdProfileFile->SetGuidance("The path to the file with the profile of the mountain range. Default value is 'profile.dat'.");
	cmdProfileFile->SetDefaultValue("profile.dat");
	cmdProfileFile->AvailableForStates(G4State_PreInit, G4State_Idle);

	G4String strNbOfZenithBins = strMuonGenerator;
	strNbOfZenithBins.append("NbOfZenithBins");
	cmdNbOfZenithBins = new G4UIcmdWithAnInteger(strNbOfZenithBins, this);
	cmdNbOfZenithBins->SetGuidance("Number of bins along the zenith direction [0,Pi/2].");
	cmdNbOfZenithBins->SetGuidance("Default value is 90.");
	cmdNbOfZenithBins->SetDefaultValue(90);
	cmdNbOfZenithBins->AvailableForStates(G4State_PreInit, G4State_Idle);

	G4String strNbOfAzimuthBins = strMuonGenerator;
	strNbOfAzimuthBins.append("NbOfAzimuthBins");
	cmdNbOfAzimuthBins = new G4UIcmdWithAnInteger(strNbOfAzimuthBins, this);
	cmdNbOfAzimuthBins->SetGuidance("Number of bins along the azimuth direction [0,2*Pi).");
	cmdNbOfAzimuthBins->SetGuidance("Default value is 360.");
	cmdNbOfAzimuthBins->SetDefaultValue(360);
	cmdNbOfAzimuthBins->AvailableForStates(G4State_PreInit, G4State_Idle);

	G4String strNbOfEnergyBins = strMuonGenerator;
	strNbOfEnergyBins.append("NbOfEnergyBins");
	cmdNbOfEnergyBins = new G4UIcmdWithAnInteger(strNbOfEnergyBins, this);
	cmdNbOfEnergyBins->SetGuidance("Number of bins along the logarithmic energy axis [Log(E_min),Log(E_max)].");
	cmdNbOfEnergyBins->SetGuidance("Default value is 100.");
	cmdNbOfEnergyBins->SetDefaultValue(100);
	cmdNbOfEnergyBins->AvailableForStates(G4State_PreInit, G4State_Idle);

	G4String strEnergyMin = strMuonGenerator;
	strEnergyMin.append("EnergyMin");
	cmdEnergyMin = new G4UIcmdWithADoubleAndUnit(strEnergyMin, this);
	cmdEnergyMin->SetGuidance("Lower bound E_min of the muon energy range.");
	cmdEnergyMin->SetGuidance("Default value is 200 GeV.");
	cmdEnergyMin->SetDefaultValue(200.0*GeV);
	cmdEnergyMin->AvailableForStates(G4State_PreInit, G4State_Idle);

	G4String strEnergyMax = strMuonGenerator;
	strEnergyMax.append("EnergyMax");
	cmdEnergyMax = new G4UIcmdWithADoubleAndUnit(strEnergyMax, this);
	cmdEnergyMax->SetGuidance("Upper bound E_max of the muon energy range.");
	cmdEnergyMax->SetGuidance("Default value is 2 TeV.");
	cmdEnergyMax->SetDefaultValue(2.0*TeV);
	cmdEnergyMax->AvailableForStates(G4State_PreInit, G4State_Idle);

	G4String strRadiusSphere = strMuonGenerator;
	strRadiusSphere.append("RadiusSphere");
	cmdRadiusSphere = new G4UIcmdWithADoubleAndUnit(strRadiusSphere, this);
	cmdRadiusSphere->SetGuidance("Radius of the sphere which is homogenous illuminated with muons.");
	cmdRadiusSphere->SetGuidance("Default value is 5 m.");
	cmdRadiusSphere->SetDefaultValue(5.0*m);
	cmdRadiusSphere->AvailableForStates(G4State_PreInit, G4State_Idle);

	G4String strRadiusHemisphere = strMuonGenerator;
	strRadiusHemisphere.append("RadiusHemisphere");
	cmdRadiusHemisphere = new G4UIcmdWithADoubleAndUnit(strRadiusHemisphere, this);
	cmdRadiusHemisphere->SetGuidance("Radius of the hemipshere on which the muon starts.");
	cmdRadiusHemisphere->SetGuidance("Default value is 30 m.");
	cmdRadiusHemisphere->SetDefaultValue(30.0*m);
	cmdRadiusHemisphere->AvailableForStates(G4State_Idle, G4State_PreInit);

	G4String strBuildLUT = strMuonGenerator;
	strBuildLUT.append("BuildLookUpTable");
	cmdBuildLookUpTable = new G4UIcmdWithoutParameter(strBuildLUT, this);
	cmdBuildLookUpTable->SetGuidance("Build the look-up table for the probabilities.");
	cmdBuildLookUpTable->SetGuidance("Call this command after you set all properties of the muon generator.");
	cmdBuildLookUpTable->AvailableForStates(G4State_PreInit, G4State_Idle);
}

MuonGeneratorMessenger::~MuonGeneratorMessenger() {
	delete cmdVerbose;
	delete cmdProfileFile;
	delete cmdNbOfZenithBins;
	delete cmdNbOfAzimuthBins;
	delete cmdNbOfEnergyBins;
	delete cmdEnergyMin;
	delete cmdEnergyMax;
	delete cmdRadiusSphere;
	delete cmdRadiusHemisphere;
	delete cmdBuildLookUpTable;
	delete dirInterface;
	delete dirBaseDirectory;
}

void MuonGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
	if (command == cmdProfileFile) {
		myMuonGenerator->SetPathToProfileFile(newValue);
	}
	if (command == cmdVerbose) {
		myMuonGenerator->SetVerboseLevel(cmdVerbose->GetNewIntValue(newValue));
	}
	if (command == cmdNbOfZenithBins) {
	  myMuonGenerator->SetNbOfZenithBins(cmdNbOfZenithBins->GetNewIntValue(newValue));
	}
	if (command == cmdNbOfAzimuthBins){
	  myMuonGenerator->SetNbOfAzimuthBins(cmdNbOfAzimuthBins->GetNewIntValue(newValue));
	}
	if(command==cmdNbOfEnergyBins){
	  myMuonGenerator->SetNbOfEnergyBins(cmdNbOfEnergyBins->GetNewIntValue(newValue));
	}
	if(command==cmdEnergyMin){
	  myMuonGenerator->SetEnergyMin(cmdEnergyMin->GetNewDoubleValue(newValue));
	}
	if(command==cmdEnergyMax){
	  myMuonGenerator->SetEnergyMax(cmdEnergyMax->GetNewDoubleValue(newValue));
	}
	if(command==cmdRadiusSphere){
	  myMuonGenerator->SetRadiusSphere(cmdRadiusSphere->GetNewDoubleValue(newValue));
	}
	if(command==cmdRadiusHemisphere){
	  myMuonGenerator->SetRadiusHemisphere(cmdRadiusHemisphere->GetNewDoubleValue(newValue));
	}
	if(command==cmdBuildLookUpTable){
	  myMuonGenerator->InitSource();
	}
}
