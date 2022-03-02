/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
// $Id: GeometryConstructionMessenger.cc $
//
/// \file GeometryConstructionMessenger.cc
/// \brief Definition of the GeometryConstructionMessenger class

#include "GeometryConstructionMessenger.hh"
#include "GeometryConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "GeometryManager.hh"

GeometryConstructionMessenger::GeometryConstructionMessenger(GeometryConstruction* det) : G4UImessenger(){

	detector = det;

    directory = new G4UIdirectory( "/geometry/" );
    directory->SetGuidance( "Set geometry type and path to dimension files" );

	fGeoTypeCmd = new G4UIcmdWithAnInteger( "/geometry/type", this);
	fGeoTypeCmd->SetGuidance( "Set the geometry type. \n"
							  "10X -> TESSERACT detector with shielding, cryostat, and inner detector.\n"
							  "       100 Dummy inner detector.\n"
							  "       101 HeRALD inner detector.\n"
							  "       102 SPICE inner detector.\n"
							  "     Further details are set by the choice of geometry dimension files.\n"
							  "200 -> Rock geometry used to sample alpha and neutrons from the cavern.");
	fGeoTypeCmd->SetParameterName("type", true);
	fGeoTypeCmd->SetDefaultValue(0);

	// All types
	fConstructGeoCmd = new G4UIcmdWithoutParameter( "/geometry/construct", this);
	fConstructGeoCmd->SetGuidance( "Construct the geometry after loading parameters" );

	fDimensionFileCmd = new G4UIcmdWithAString( "/geometry/dimensionFile", this);
	fDimensionFileCmd->SetGuidance( "Dimension file path and name. Use this file to include dimension name, value pairs.\n"
				"Consider including groups of dimensions for special geometries in seperate files.");

	// Type 10X 
	fCryostatWallFileCmd = new G4UIcmdWithAString( "/geometry/cryostatWallFile", this);
	fCryostatWallFileCmd->SetGuidance( "TESSERACT cryostat wall profile file path and name" );

	fCryoPlateFileCmd = new G4UIcmdWithAString( "/geometry/cryoPlateFile", this);
	fCryoPlateFileCmd->SetGuidance( "TESSERACT cryostat inner plates with holes, file path and name" );

	fCryoBeamFileCmd = new G4UIcmdWithAString( "/geometry/cryoBeamFile", this);
	fCryoBeamFileCmd->SetGuidance( "TESSERACT cryostat inner beams, file path and name" );

	fSetByVersionCmd = new G4UIcmdWithAString( "/geometry/setByVersion", this);
	fSetByVersionCmd->SetGuidance( "Set dimension, cryostatWall, cryoBeam, and cryoPlate files together by specify the version folder name" );

	
}



GeometryConstructionMessenger::~GeometryConstructionMessenger(){
//    delete FSDistanceCmd;
}


void GeometryConstructionMessenger::SetNewValue( G4UIcommand* command, G4String newValue) {

	G4cout<<"GeometryConstructionMessenger::SetNewValue "<<newValue<<G4endl;

	if( command == fGeoTypeCmd){
		GeometryManager::Get()->SetGeometryType(fGeoTypeCmd->ConvertToInt(newValue));
		G4cout<<"Detector configuration is " <<GeometryManager::Get()->GetGeometryType()<<G4endl;
	}
	else if( command==fDimensionFileCmd ){
		GeometryManager::Get()->SetFilePath("dimensionFile", newValue);
	}
	else if( command==fCryostatWallFileCmd ){
		GeometryManager::Get()->SetFilePath("cryostatWallFile", newValue);
	}
	else if( command==fCryoPlateFileCmd ){
		GeometryManager::Get()->SetFilePath("cryoPlateFile", newValue);
	}
	else if( command==fCryoBeamFileCmd ){
		GeometryManager::Get()->SetFilePath("cryoBeamFile", newValue);
	}
	else if( command==fSetByVersionCmd ){
		G4cout<<"Set geometry version "<<newValue<<G4endl;
		GeometryManager::Get()->SetFilePath("dimensionFile", "config/geometry/"+newValue+"/dimensions.dat");
		GeometryManager::Get()->SetFilePath("cryostatWallFile", "config/geometry/"+newValue+"/cryostatWall.dat");
		GeometryManager::Get()->SetFilePath("cryoPlateFile", "config/geometry/"+newValue+"/cryoPlate.dat");
		GeometryManager::Get()->SetFilePath("cryoBeamFile", "config/geometry/"+newValue+"/cryostatBeam.dat");
	}
	else if( command==fConstructGeoCmd ){
		// At this point GeometryManager::SetFilePath() has been called by GeometryConstructionMessenger 
		// Mark that we are ready to load dimensions!
		//
		GeometryManager::Get()->GeometryTypeAndFilesSet();

		// Load dimensions.
		// Note by Suerfu on 2022-Jan-17:
		// The types of dimension files should depend on geometry type.
		// Maybe consider loading different sets of files depending on the type of geometry?
		GeometryManager::Get()->LoadDimensions();
	}

}
