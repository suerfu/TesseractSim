#ifndef TRACKREADER_H
#define TRACKREADER_H 1

#include <string>
#include <vector>
#include <map>

#include "MCPulseArray.h"
#include "StepInfo.h"

#include "TFile.h"
#include "TTree.h"
#include "TMacro.h"

using namespace std;

class TrackReader{

public:

    // Constructor
    TrackReader(){}

    // Destructor
    ~TrackReader(){;}

    void Reset(){}

    // Add an active volume
    void SetActiveVolume( vector<string> name){ arrayAV = name;}
        // Active volume is by which event clustering will be performed

    // Add a volume of interest.
    void SetVoI( vector<string> name ){ arrayVOI = name; }
        // A volume of interest is by definition volume which energy deposit will be summed and recorded

    // Sets the (hypothetical) DAQ window in microsecond.
    void SetDAQWindow( float win = 1){ daqWindow = win;}
        // Events ocurring within this window is regarded as the same event.

    float GetDAQWindow(){ return daqWindow; }

    // Sets the coincidence window in microseconds
    void SetCoinWindow( float win = 1){ coinWindow = win;}
        // Events ocurring within this window from an event in active volume will be recorded.
    
    float GetCoinWindow(){ return coinWindow; }

    // Process the tracks in input and write output as a ROOT TTree in output.
    void Process( string output, vector<string> inputs );

    // Get duration of simulation in seconds
    double GetTimeSimulated( TMacro run, TMacro geo );

    // Get the number of particle simulated in this run
    double GetNbParticleSimulated( TMacro mac );

private:

    static const unsigned int MAX_FILENAME_LEN = 128;

    double daqWindow;
    double coinWindow;

    vector<string> arrayAV;
        // This array holds the active volumes.
    vector<string> arrayVOI;
        // This array holds the volumes of interest.
        // If it's zero, will treat all volumes as volume of interest.

    vector<string> GetVOIFromFile( vector<string> inputName);
        // Iterate through given files to generate a list of volumes involved.


    void ConfigureTTree( TTree* tree);

    // Set branch variables to the default values.
    void DefaultBranchVariables(){
        map<string, double>::iterator itr;
        for( itr = energyDeposit.begin(); itr!=energyDeposit.end(); itr++ ){
            itr->second = 0;
        }
        timeStamp = -1;
    }

    char inputFileNameChar[ MAX_FILENAME_LEN ];
        // name of the file to which this event belongs to
    unsigned int ID;
        // global ID of the event processed
    unsigned int eventID;
        // event ID in its own run.
    unsigned int clusterIndex;
        // index of the event cluster when multiple event occurred in the DAQ window.
    double timeStamp;
        // timeStamp of the interaction.

    // Function to retrieve TMacro from the output file so that subsequent inputs are macros.
    TMacro GetMacro( string file, string name);

    double Nsimulated;
        // Total No. of particles simulated in this run

    double Tsimulated;
        // Total days simulated.
        // Assuming 1 Bq/kg for internal radioactivity
        // or 1 /m2/s for external gamma/neutron
        // Internal:
        //  If there is material/geometry table, use the correct mass.
        //  If not, assume mass is 1 kg.

    map<string, double> energyDeposit;

    void ProcessFile( TTree* tree, string s);

    void ProcessPulseArray( TTree* tree );

    bool NewEvent( char* name );

    StepInfo rdata;

    map<string, MCPulseArray> pulseArrayAV;
    //map<string, double> edepAVWrite;

    map<string, MCPulseArray> pulseArrayVOI;
    //map<string, double> edepVOIWrite;

    MCPulse ConvertToMCPulse( StepInfo );

    void FindEventTime( string& s, double& t);
};


#endif
