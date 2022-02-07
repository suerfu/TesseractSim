
#include "TrackReader.h"

#include "TTree.h"
#include "TFile.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <set>

using namespace std;


void TrackReader::ConfigureTTree( TTree* tree ){

    // Set global-level information
    //
    stringstream ss;
    ss << "file[" << MAX_FILENAME_LEN << "]/C";
    tree->Branch( "file", &inputFileNameChar, ss.str().c_str() );

    tree->Branch( "ID", &ID, "ID/I");
    tree->Branch( "eventID", &eventID, "eventID/I");
    tree->Branch( "clusterIndex", &clusterIndex, "clusterIndex/I");

    timeStamp = -1;
    tree->Branch( "timeStamp", &timeStamp, "timeStamp/D" );
        // Time stamp is by default -1.


    // Generate energy deposition array for active volumes.
    //
    for( vector<string>::iterator itr1 = arrayAV.begin(); itr1!=arrayAV.end(); itr1++ ){

        string volumeName = *itr1;
        cout << "Creating pulse array for Active Volume " << volumeName << endl;
        string edepName = string("edep_") + *itr1;
        string rootName = edepName + "/D";
            // rootName is name + ROOT data format.

        pulseArrayAV[ volumeName ] = MCPulseArray();

        energyDeposit[ volumeName ] = 0;
        tree->Branch( edepName.c_str(), &energyDeposit[volumeName], rootName.c_str() );
            // Set address for ROOT tree branch
        
        // If a volume name also exists in volume-of-interest, remove them all.
        //
        vector<string>::iterator tmp = find(arrayVOI.begin(), arrayVOI.end(), *itr1);
        while( tmp!=arrayVOI.end() ){
            arrayVOI.erase( tmp );
            tmp = find(arrayVOI.begin(), arrayVOI.end(), *itr1);
        }
    }


    // Generate energy deposition array for volumes of interest.
    //
    for( vector<string>::iterator itr1 = arrayVOI.begin(); itr1!=arrayVOI.end(); itr1++ ){
        
        string volumeName = *itr1;
        cout << "Creating pulse array for VOI " << volumeName << endl;
        string edepName = string("edep_") + *itr1;
        string rootName = edepName + "/D";
            // rootName is name + ROOT data format.

        pulseArrayVOI[ volumeName ] = MCPulseArray();

        energyDeposit[ volumeName ] = 0;
        tree->Branch( edepName.c_str(), &energyDeposit[volumeName], rootName.c_str() );
            // Set address for ROOT tree branch
    }


}


bool TrackReader::NewEvent( char* name ){
    if (strncmp( name, "newEvent", 8)==0 )
        return true;
    else if( strncmp( name, "timeReset", 9)==0 )
        return true;
    return false;
}



void TrackReader::Process( string outputStr, vector<string> inputStr ){

    // ==================================================
    // Prepare VOI and PSD for writing trees
    // ==================================================
    //
    // If no volume-of-interest is specified, iterate over the file to get all the volumes involved.
    if( arrayVOI.empty()==true ){
        cout << "Volume-of-Interest is empty. All volumes treated as VOI.\n"; 
        arrayVOI = GetVOIFromFile( inputStr );
    }

    // ==================================================
    // Create ROOT file
    // ==================================================
    //
    TFile* outputFile = new TFile( outputStr.c_str(), "NEW" );

    if( !outputFile ){
        cerr << "Error creating output file " << outputStr << endl;
        return;
    }

    // ==================================================
    // Create ROOT TTree and initialize branches.
    // ==================================================
    //
    TTree* tree = new TTree("events", "Event-level information for MC simulated tracks.");
    ConfigureTTree( tree );
        // Set the addresses of global-level variables.

    ID = 0;

    for( vector<string>::iterator itr = inputStr.begin(); itr!=inputStr.end(); itr++ ){
        ProcessFile( tree, *itr );
    }

//    tree->Write();
    outputFile->Write();

}



void TrackReader::ProcessFile( TTree* tree, string input ){


    // ==================================================
    // Open the ROOT file to read and set branch variables
    // ==================================================
    //
    TFile* inputFile = TFile::Open( input.c_str(), "READ");
    if( !inputFile ){
        cerr << "ERROR reading file " << input << endl;
        cerr << "Skipping...\n";
        return;
    }

    strncpy( inputFileNameChar, input.c_str(), MAX_FILENAME_LEN-1 );

    // ==================================================
    // Retrieve the tree and set branch address of variables for readout
    // ==================================================
    //
    TTree* inputTree = (TTree*)inputFile->Get("events");

    StepInfo rdata;

    inputTree -> SetBranchAddress( "eventID",  &rdata.eventID);
    inputTree -> SetBranchAddress( "trackID",  &rdata.trackID);
    inputTree -> SetBranchAddress( "parentID", &rdata.parentID);
    inputTree -> SetBranchAddress( "particle", &rdata.particle_name);
    inputTree -> SetBranchAddress( "volume",   &rdata.volume_name);
    inputTree -> SetBranchAddress( "Eki",      &rdata.Eki);
    inputTree -> SetBranchAddress( "Ekf",      &rdata.Ekf);
    inputTree -> SetBranchAddress( "Edep",     &rdata.Edep);
    inputTree -> SetBranchAddress( "t",        &rdata.time);
    inputTree -> SetBranchAddress( "process",  &rdata.proc_name);

    // ==================================================
    // Loop over the tree and process the events.
    // ==================================================
    //
    unsigned int nEntries = inputTree->GetEntries();

    // Here initial index is starting with 1 since in any valid file, first entry is newEvent.
    // If file is empty, the for-loop condition is not satisfied, thus won't be executed.
    //
    for( unsigned int n = 1; n<nEntries; n++ ){

        inputTree->GetEntry(n);

        // If current event is a new event or the last event, fill the previous event and initialize.
        //
        if( NewEvent( rdata.proc_name )==true || n==nEntries-1 ){

            ProcessPulseArray( tree );

            map<string, MCPulseArray>::iterator clr;
            for( clr=pulseArrayAV.begin(); clr!=pulseArrayAV.end(); clr++ ){
                (clr->second).Clear();
            }
            for( clr=pulseArrayVOI.begin(); clr!=pulseArrayVOI.end(); clr++ ){
                (clr->second).Clear();
            }
        }

        // If not new event flag, then it's a regular step.
        // Check if it falls in one of the active volumes.
        // If yes, then put it in the energy deposit array to be processed later.
        //
        else{

            eventID = rdata.eventID;

            if( rdata.Edep>1e-9 ){

                string name = string( rdata.volume_name );

                if( find( arrayAV.begin(), arrayAV.end(), name ) != arrayAV.end() ){
                    pulseArrayAV[name].PushBack( ConvertToMCPulse(rdata ) );
                }

                else if( find( arrayVOI.begin(), arrayVOI.end(), name ) != arrayVOI.end() ){
                    pulseArrayVOI[name].PushBack( ConvertToMCPulse( rdata ) );
                }
            }
        }
    }
            
    //tree->Fill();
}



void TrackReader::ProcessPulseArray( TTree* tree ){

    clusterIndex = 0;

    map<string, MCPulseArray>::iterator itr;
    for( itr=pulseArrayAV.begin(); itr!=pulseArrayAV.end(); itr++ ){
        (itr->second).Sort();
    }
    for( itr=pulseArrayVOI.begin(); itr!=pulseArrayVOI.end(); itr++ ){
        (itr->second).Sort();
    }

    // Find the first interaction event.
    //
    double eventTime = -1;
    string volName;
    FindEventTime( volName, eventTime );
    
    while( eventTime > 0 ){

        // Now eventTime is the time of interaction.
        // volName is the name of the volume where this event happened.
        // Next, find coincidence.
        //
        for( itr=pulseArrayAV.begin(); itr!=pulseArrayAV.end(); itr++){
            if( itr->first==volName ){
                MCPulse p = (itr->second).FindPrimaryInteraction( eventTime, daqWindow );
                energyDeposit[ itr->first ] = p.GetEnergy()[0]+p.GetEnergy()[1]+p.GetEnergy()[2];
            }
            else{
                MCPulse p = (itr->second).FindCoincidentInteraction( eventTime, coinWindow, daqWindow );
                energyDeposit[ itr->first ] = p.GetEnergy()[0]+p.GetEnergy()[1]+p.GetEnergy()[2];
            }
        }

        for( itr=pulseArrayVOI.begin(); itr!=pulseArrayVOI.end(); itr++){
            MCPulse p = (itr->second).FindCoincidentInteraction( eventTime, coinWindow, daqWindow );
            energyDeposit[ itr->first ] = p.GetEnergy()[0]+p.GetEnergy()[1]+p.GetEnergy()[2];
        }

        timeStamp = eventTime;

        tree -> Fill();
        ID++;
            // Since the tree may have to be filled multiple times, it has to be done in this function.
        
        DefaultBranchVariables();
        clusterIndex++;

        FindEventTime( volName, eventTime );
    }

}



MCPulse TrackReader::ConvertToMCPulse( StepInfo step ){

    MCPulse val;

    string name( step.particle_name );
    if( name=="e-" || name=="e+" || name=="gamma" ){
        val.SetEnergy( 0, step.Edep );
    }
    else if( name=="mu-" || name=="mu+ "){
        val.SetEnergy( 2, step.Edep);
    }
    else{
        val.SetEnergy( 1, step.Edep);
    }
    val.SetTime( step.time );

    return val;
}



void TrackReader::FindEventTime( string& volName, double& eventTime ){

    map<string, MCPulseArray>::iterator itr;

    for( itr=pulseArrayAV.begin(); itr!=pulseArrayAV.end(); itr++){

        if( itr==pulseArrayAV.begin() ){
            volName = itr->first;
            eventTime = (itr->second).GetNextInteractionTime();
        }
        else{
            if( eventTime > (itr->second).GetNextInteractionTime() ){
                volName = itr->first;
                eventTime = (itr->second).GetNextInteractionTime();
            }
        }
    }
}



vector<string> TrackReader::GetVOIFromFile( vector<string> inputName){

    cout << "Reading over the input files to check all volumes involved..." << endl;

    set<string> container;
    vector<string> return_val;

    for( vector<string>::iterator itr = inputName.begin(); itr!=inputName.end(); itr++ ){

        // Open the ROOT file to read and set branch variables
        //
        TFile* inputFile = TFile::Open( itr->c_str(), "READ");
        if( !inputFile ){
            cerr << "ERROR reading file " << *itr << endl;
            cerr << "Skipping...\n";
            continue;
        }

        // Retrieve the tree and set branch address of variables for readout
        //
        TTree* inputTree = (TTree*)inputFile->Get("events");

        StepInfo rdata;
        inputTree -> SetBranchAddress( "volume",   &rdata.volume_name);

        // Loop over the tree and process the events.
        //
        unsigned int nEntries = inputTree->GetEntries();

        for( unsigned int n = 1; n<nEntries; n++ ){

            inputTree->GetEntry(n);

            string name = rdata.volume_name;
            if( name!="" ){
                container.insert( rdata.volume_name);
            }
        }
        inputFile->Close();
    }

    cout << "Adding ";
    for( auto itr = container.begin(); itr!=container.end(); itr++ ){
        return_val.push_back( *itr );
        cout << *itr << ' ';
    }

    cout << "as volumes of interest." << endl;

    return return_val;
}
