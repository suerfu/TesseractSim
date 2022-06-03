/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
//
/// \file RunAction.cc
/// \brief Implementation of the RunAction class

// history:
// 2021-11-18 B.Suerfu initial version
// 2022-04-28 B.Suerfu outputing material table as a TMacro
// 2022-05-06 B.Suerfu changed output macro name as runMacro for consistency; added version in macro

#include "RunAction.hh"
#include "RunActionMessenger.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalVolumeStore.hh"

#include "TFile.h"
#include "TTree.h"


RunAction::RunAction( CommandlineArguments* c) : G4UserRunAction(), fRunActionMessenger(0), fCmdlArgs( c ){

    version = "1.0.1";
        // Version number. Do not change.
        // Backward compatible should increment minor number
        // Bug fixes should increment patch number

    fRunActionMessenger = new RunActionMessenger( this );

    // Configure the random engine.
    // The seed is first set by the current time.
    // It will be updated by the commandline parameter if provided.
    //
    G4Random::setTheEngine(new CLHEP::RanecuEngine);

    G4long seeds[2];
    time_t systime = time(NULL);
    if( fCmdlArgs->Find("seed") ){
        seeds[0] = stol( fCmdlArgs->Get("seed"));
    }
    else{
        seeds[0] = (long) systime;
    }
    seeds[1] = (long) (seeds[0]*G4UniformRand());
    
    G4Random::setTheSeeds(seeds);

    // Aldo keep a record of randomSeeds for later output
    //
    randomSeeds.push_back( seeds[0] );
    randomSeeds.push_back( seeds[1] );

    G4cout << "Seeds for random generator are " << seeds[0] << ", " << seeds[1] << G4endl;


    // Configure output
    //
    outputName = fCmdlArgs->Get( "output" );
    if( outputName=="" ){
        outputName = fCmdlArgs->Get( "o" );
    }

    outputFile = 0;
    dataTree = 0;

    G4RunManager::GetRunManager()->SetPrintProgress( 1 );
}


RunAction::~RunAction(){

    // Moved from EndOfRun so that multiple runs can be recorded in a single file.
    //
    if( outputFile!=0 ) {

        TMacro ver( "version" );
        ver.AddLine( version.c_str() );
        ver.Write();

        // Write the macro used in this run as a ROOT macro
        //
        for( unsigned int i=0; i<macros.size(); i++){
            TMacro mac( "runMacro" );
            mac.ReadFile( macros[i] );
            mac.Write();    
        }

        std::stringstream ss;
        for( unsigned int i=0; i<randomSeeds.size(); i++)
            ss << randomSeeds[i] << '\t';

        // Also record the random seeds used in this run as a separate TMacro.
        //
        TMacro randm( "randomSeeds");
        randm.AddLine( ss.str().c_str());
        randm.Write();

        // New since April 28, 2022
        // Record the material table as well.
        //
        TMacro geomTable( "geometryTable");

        // Iterate over the vector of stored physical volumes and get their material & mass.
        auto volumeStore = G4PhysicalVolumeStore::GetInstance();
        for( auto itr = volumeStore->begin(); itr!=volumeStore->end(); itr++){
            ss.str( std::string() ); // clear the string stream
            ss << (*itr)->GetName() << ' ' << (*itr)->GetLogicalVolume()->GetMass( false, false )/CLHEP::kg << ' ' << (*itr)->GetLogicalVolume()->GetMaterial()->GetName();
            geomTable.AddLine( ss.str().c_str() );
        }
        geomTable.Write();

        outputFile->Write();
        outputFile->Close();
    }
}


CommandlineArguments* RunAction::GetCommandlineArguments(){
    return fCmdlArgs;
}


void RunAction::SetOutputFileName( G4String newname ){
    outputName = newname;
}


void RunAction::BeginOfRunAction(const G4Run* /*run*/){

    // If output name is specified, create a ROOT file and a TTree.
    //
    if( outputName!="" && outputFile==0 ){

        outputFile = new TFile(outputName, "NEW");
        G4cout << "ROOT file " << outputName << " created." << G4endl;

        if( outputFile->IsOpen() ){
            dataTree = new TTree("events", "Track-level info for the run");
            G4cout << "TTree object created." << G4endl;
        }
    }
}



void RunAction::EndOfRunAction( const G4Run* run ){}



TTree* RunAction::GetDataTree(){
    return dataTree;
}


void RunAction::AddRecordWhenHit( G4String a){ recordWhenHit.insert(a); }


bool RunAction::RecordWhenHit( G4String a ){
    if( recordWhenHit.empty()==true ){
        return true;
            // If no volume is specified, record everything.
    }
    else{
        return recordWhenHit.find( a )!=recordWhenHit.end();
    }
}


void RunAction::AddKillWhenHit( G4String a){ killWhenHit.insert(a); }

bool RunAction::KillWhenHit( G4String a ){
    return killWhenHit.find( a )!=killWhenHit.end();
}


void RunAction::AddExcludeParticle( G4String a){ excludeParticle.insert(a); }

bool RunAction::ExcludeParticle( G4String a ){
    return excludeParticle.find( a )!=excludeParticle.end();
}


void RunAction::AddExcludeVolume( G4String a){ excludeVolume.insert(a); }

bool RunAction::ExcludeVolume( G4String a ){
    return excludeVolume.find( a )!=excludeVolume.end();
}


void RunAction::AddExcludeProcess( G4String a){ excludeProcess.insert(a); }

bool RunAction::ExcludeProcess( G4String a ){
    return excludeProcess.find( a )!=excludeProcess.end();
}



