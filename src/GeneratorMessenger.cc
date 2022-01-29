/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
// $Id: GeneratorMessenger.cc $
//
/// \file GeneratorMessenger.cc
/// \brief Definition of the GeneratorMessenger class

#include "GeneratorMessenger.hh"

#include "GeneratorAction.hh"

GeneratorMessenger::GeneratorMessenger( GeneratorAction* generator )  : G4UImessenger(), primaryGenerator(generator), primaryGeneratorDir(0) {

	primaryGeneratorDir = new G4UIdirectory("/generator/");
	primaryGeneratorDir->SetGuidance("Generator position, momentum and energy spectrum control.");

    cmdSetSpectrum = new G4UIcmdWithAString("/generator/spectrum", this);
    cmdSetSpectrum->SetGuidance("Set the ROOT file from which to sample position, direction and energy.");
    cmdSetSpectrum->SetParameterName("foo.root", false);
    cmdSetSpectrum->AvailableForStates(G4State_PreInit, G4State_Idle);

		cmdSetParticle = new G4UIcmdWithAString("/generator/particle", this);
		cmdSetParticle->SetGuidance("Set the name of the particle.");
		cmdSetParticle->SetParameterName("geantino", false);
		cmdSetParticle->AvailableForStates(G4State_PreInit, G4State_Idle);


		cmdSetSurfaceName = new G4UIcmdWithAString("/generator/surfacename", this);
		cmdSetSurfaceName->SetGuidance("Set the surface name");
		cmdSetSurfaceName->SetParameterName("Top", false);
		cmdSetSurfaceName->AvailableForStates(G4State_PreInit, G4State_Idle);

		cmdSetWall_x = new G4UIcmdWithADoubleAndUnit("/generator/wall_x",this);
		cmdSetWall_x->SetGuidance("Set the X length of the Wall");
		cmdSetWall_x->SetParameterName("X_length",false);
		cmdSetWall_x->SetRange("X_length>0.");
		cmdSetWall_x->SetUnitCategory("Length");
		cmdSetWall_x->SetDefaultUnit("m");
		cmdSetWall_x->AvailableForStates(G4State_PreInit, G4State_Idle);

		cmdSetWall_y = new G4UIcmdWithADoubleAndUnit("/generator/wall_y",this);
		cmdSetWall_y->SetGuidance("Set the Y length of the Wall");
		cmdSetWall_y->SetParameterName("Y_length",false);
		cmdSetWall_y->SetRange("Y_length>0.");
		cmdSetWall_y->SetUnitCategory("Length");
		cmdSetWall_y->SetDefaultUnit("m");
		cmdSetWall_y->AvailableForStates(G4State_PreInit, G4State_Idle);

		cmdSetWall_z = new G4UIcmdWithADoubleAndUnit("/generator/wall_z",this);
		cmdSetWall_z->SetGuidance("Set the Z length of the Wall");
		cmdSetWall_z->SetParameterName("Z_length",false);
		cmdSetWall_z->SetRange("Z_length>0.");
		cmdSetWall_z->SetUnitCategory("Length");
		cmdSetWall_z->SetDefaultUnit("m");
		cmdSetWall_z->AvailableForStates(G4State_PreInit, G4State_Idle);

    //cmdSetPosition = new G4UIcmdWithoutParameter("/generator/setPos", this);
    //cmdSetPosition->SetGuidance("Set the position of next primary vertex from ROOT file.");
    //cmdSetPosition->AvailableForStates(G4State_PreInit, G4State_Idle);

    //cmdSetDir = new G4UIcmdWithoutParameter("/generator/setDir", this);
    //cmdSetDir->SetGuidance("Set the direction of next primary vertex from ROOT file.");
    //cmdSetDir->AvailableForStates(G4State_PreInit, G4State_Idle);

    //cmdSetEnergy = new G4UIcmdWithoutParameter("/generator/setEnergy", this);
    //cmdSetEnergy->SetGuidance("Set the energy of next primary vertex from ROOT file.");
    //cmdSetEnergy->AvailableForStates(G4State_PreInit, G4State_Idle);

    //cmdSetWall = new G4UIcmdWithoutParameter("/generator/confineOnWall", this);
    //cmdSetWall->SetGuidance("Assumes the source is on the wall. x,y,z in the ROOT file will be irrelevant and theta and phi will be w.r.t. the normal direction of the surface.");
    //cmdSetWall->AvailableForStates(G4State_PreInit, G4State_Idle);

}


GeneratorMessenger::~GeneratorMessenger(){

    delete cmdSetSpectrum;
		delete cmdSetParticle;
		delete cmdSetSurfaceName;
		delete cmdSetWall_x;
		delete cmdSetWall_y;
		delete cmdSetWall_z;
    //delete cmdSetPosition;
    //delete cmdSetDir;
    //delete cmdSetEnergy;
    //delete cmdSetWall;
}


void GeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue){

	if( command == cmdSetSpectrum )	{
		primaryGenerator->SetSpectrum( newValue);
	}

	if( command == cmdSetParticle){
				 primaryGenerator->SetParticleName( newValue);
		 }

	if( command == cmdSetSurfaceName){
 			   primaryGenerator->SetConversionSurfaceNameToIndex( newValue);
 		}

  if( command == cmdSetWall_x){
 			   primaryGenerator->SetWallX(cmdSetWall_x->GetNewDoubleValue(newValue));
 			 }
	if( command == cmdSetWall_y){
		  	 primaryGenerator->SetWallY(cmdSetWall_y->GetNewDoubleValue(newValue));
		  }
	if( command == cmdSetWall_z){
		 		 primaryGenerator->SetWallZ(cmdSetWall_z->GetNewDoubleValue(newValue));
		  }

// if( command == cmdSetEnergy ){
//				primaryGenerator->SetEnergy();
//		}


}
